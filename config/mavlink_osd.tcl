namespace eval osd {

set sysid 97
set compid 94

proc detect {fd {timeout 10000}} {
 set end [clock milliseconds]
 incr end $timeout
 while {1} {
  set w [expr {$end - [clock milliseconds]}]
  if {$w <= 0} {return 0}
  mav::send_hb $fd
  set hb [mav::wait_for_msg $fd 0 $w]
  if {$hb eq {}} continue
  binary scan $hb "@10cu" type
  if {$type == 24} break
 }
 binary scan $hb "@3cucu" osd::sysid osd::compid
 return 1
}

proc read_eeprom {fd addr len {timeout 1000}} {
 set end [clock milliseconds]
 incr end $timeout
 while {1} {
  set w [expr {($end - [clock milliseconds])/4}]
  if {$w <= 0} {return {}}
  mav::send_eeprom_read $fd $osd::sysid $osd::compid $addr $len
  set repl [mav::wait_for_msg $fd 76 $w]
  if {$repl ne {}} break
 }
 binary scan $repl "@10a$len" data
 return $data
}

proc write_eeprom {fd addr data {verify 1} {timeout 1000}} {
 set end [clock milliseconds]
 incr end $timeout
 while {1} {
  mav::send_eeprom_write $fd $osd::sysid $osd::compid $addr $data
  if {!$verify} {return 1}
  set w [expr {$end - [clock milliseconds]}]
  if {$w <= 0} {return 0}
  set x [read_eeprom $fd $addr [string length $data] $w]
  if {$x eq $data} {return 1}
 }
}

proc read_eeprom_all {fd {timeout 1000}} {
 set buf {}
 for {set i 0} {$i < 1024} {incr i 24} {
  set rl [expr {1024-$i >= 24 ? 24 : 1024-$i}]
  set x [read_eeprom $fd $i $rl $timeout]
  if {$x eq {}} {return {}}
  append buf $x
 }
 return $buf
}

proc write_eeprom_all {fd data {verify 1} {timeout 1000}} {
 for {set i 0} {$i < 1024} {incr i 24} {
  set rl [expr {1024-$i >= 24 ? 24 : 1024-$i}]
  binary scan $data "@${i}a${rl}" blk
  if {![write_eeprom $fd $i $blk $verify $timeout]} {return 0}
 }
 return 1
}

proc test {} {
set fd [mav::open_serial "/dev/ttyACM0"]
osd::detect $fd
puts "osd detected: ${osd::sysid}:${osd::compid}"
set buf [osd::read_eeprom_all $fd 2000]
binary scan $buf "H*" x
puts $x
set r [osd::write_eeprom_all $fd $buf 1 2000]
puts "write eeprom: $r"
}
  
}
