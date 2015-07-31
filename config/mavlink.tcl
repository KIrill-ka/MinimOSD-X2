
namespace eval mav {

array set msg_info {
 74 {VFR_HUD 20 20}
 76 {COMMAND_LONG 33 152}
  0 {HEARTBEAT 9 50}
 20 {PARAM_REQUEST_READ 20 214}
 22 {PARAM_VALUE 25 220}
}


proc crc_accumulate {sum data} {
 set tmp [expr {($data ^ $sum) & 0xff}]
 set tmp [expr {(($tmp<<4) ^ $tmp) & 0xff}]
 return [expr {($sum>>8) ^ ($tmp<<8) ^ ($tmp<<3) ^ ($tmp>>4)}]
}

proc crc_calculate {b} {
 binary scan $b cu* x
 set sum 0xffff
 foreach byte $x {
  set sum [crc_accumulate $sum $byte]
 }
 return $sum
}

proc open_serial {path {baud 115200}} {
 set ofd [open $path r+]
 fconfigure $ofd -translation binary
 fconfigure $ofd -buffering none
 fconfigure $ofd -timeout 500
 fconfigure $ofd -mode $baud,n,8,1
 fconfigure $ofd -handshake none
 return $ofd
}

proc close_serial {fd} {
 close $fd
}

set my_sysid 253
set my_compid 1


# u8 magic u8 len u8 seq u8 sysid u8 compid u8 msgid
#f7sccc{0 0 0 0 0 0 0} 12 7 94 0

set seq 1
proc send {ofd id buf} {
 set len [lindex $mav::msg_info($id) 1]
 set msg [binary format "cccccca*" 254 $len $mav::seq $mav::my_sysid $mav::my_compid $id $buf]
 binary scan $msg "@1a*" csum_data
 set cs [crc_calculate $csum_data]
 set cs [crc_accumulate $cs [lindex $mav::msg_info($id) 2]]
 puts -nonewline $ofd $msg
 puts -nonewline $ofd [binary format "s" $cs]
 #binary scan $msg cu* b1
 #binary scan [binary format "s" $cs] cu* b2
 #puts "$b1:$b2"
 incr mav::seq
}

proc send_cmd {ofd dest_sys dest_comp cmd_id buf} {
 set body [binary format "a28sccc" $buf $cmd_id $dest_sys $dest_comp 0]
 send $ofd 76 $body
}

proc send_hb {ofd} {
 set body [binary format "iccccc" 0 0 0 0 0 0]
 send $ofd 0 $body
}

#proc send_paramreq {ofd dest_sys dest_comp idx name} {
# set name [binary format "a*i4" $name {0 0 0 0}]
# set body [binary format "scca16" $idx $dest_sys $dest_comp $name]
# send $ofd 20 $body
#}

proc send_eeprom_read {ofd dest_sys dest_comp addr sz} {
 set body [binary format "sc@27c" $addr $sz 0]
 send_cmd $ofd $dest_sys $dest_comp 30401 $body
}

proc send_eeprom_write {ofd dest_sys dest_comp addr data} {
 set sz  [string length $data]
 if {$sz > 24} {error "send_eeprom_write: maximal chunk length is 24"}
 set body [binary format "@27c@0scca*" 0 $addr $sz 0 $data]
 send_cmd $ofd $dest_sys $dest_comp 30402 $body
}

proc check_head {b} {
 set n [binary scan $b "cucucucucucu" magic len seq sysid compid msgid]
 if {$n < 6} {return -1}
 if {$magic != 254} {return 0}
 if {![info exists mav::msg_info($msgid)]} {return 0}
 if {[lindex $mav::msg_info($msgid) 1] != $len} {return 0} 
 return 1
}

proc check_message {b} {
 binary scan $b "cucucucucucu" magic len seq sysid compid msgid
 if {[string length $b] < $len+6+2} {return -1}
 set csum_off [expr {$len+6}]
 set csum_data_len [expr {$len+5}]
 binary scan $b @1a${csum_data_len}@${csum_off}su csum_data csum
 set csum1 [crc_calculate $csum_data]
 set csum_extra [lindex $mav::msg_info($msgid) 2]
 set csum1 [crc_accumulate $csum1 $csum_extra]
 if {$csum1 != $csum} {return 0}
 return 1
}

proc skip_message {b} {
 return {}
}
proc print_message {b} {
 binary scan $b "cucucucucucu" magic len seq sysid compid msgid
 puts "[lindex $mav::msg_info($msgid) 0] $sysid:$compid"
}

proc wait_for_msg {fd id {timeout -1}} {
 set buf {}
 if {$timeout < 0} {
  set end 1e100
 } else {
  set end [expr {[clock milliseconds]+$timeout}]
 }
 while {[clock milliseconds] < $end} {
  set c [read $fd 1]
  set buf [binary format "a*a" $buf $c]
  set cont 0
  while {$buf ne "" && !$cont} {
   binary scan $buf cu magic
   set cont [check_head $buf] 
   if {$cont > 0} {
    set cont [check_message $buf]
   }
   if {!$cont} {
    binary scan $buf @1a* buf
    continue
   }
   if {$cont > 0} {
    binary scan $buf "@5cu" id1
    #print_message $buf
    if {$id == $id1} {return $buf}
    set buf [skip_message $buf]
    set cont 0
   }
  }
 }
 return {}
}
}
