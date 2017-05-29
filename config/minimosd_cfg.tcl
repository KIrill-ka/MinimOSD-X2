
if {[info exists env(HOME)]} {
 set def_settings $env(HOME)/.minimosd_cfg.tcl
 if {[file exists $def_settings]} {
  source $def_settings
 }
}


set eeprom_version 7
set npanels 3
set fw_version "0.6.1"

array set name2addr {
Pitch_en 6 Pitch_x 8 Pitch_y 10
Roll_en 12 Roll_x 14 Roll_y 16
Batt_A_en 18 Batt_A_x 20 Batt_A_y 22
GPSats_en 30 GPSats_x 32 GPSats_y 34
COG_en 36 COG_x 38 COG_y 40
GPS_en 42 GPS_x 44 GPS_y 46
Rose_en 48 Rose_x 50 Rose_y 52
Heading_en 54 Heading_x 56 Heading_y 58
HomeDir_en 66 HomeDir_x 68 HomeDir_y 70
HomeDis_en 72 HomeDis_x 74 HomeDis_y 76
WPDis_en 86 WPDis_x 88  WPDis_y 90  
RSSI_en 92 RSSI_x 94  RSSI_y 96  
Cur_A_en 98 Cur_A_x 100 Cur_A_y 102 
Alt_en 110 Alt_x 112 Alt_y 114
Vel_en 116 Vel_x 118 Vel_y 120
Thr_en 122 Thr_x 124 Thr_y 126
FMod_en 128 FMod_x 130 FMod_y 132
Horizon_en 134 Horizon_x 136 Horizon_y 138
HomeAlt_en 140 HomeAlt_x 142 HomeAlt_y 144
AirSpeed_en 146 AirSpeed_x 148 AirSpeed_y 150
BatteryPercent_en 152 BatteryPercent_x 154 BatteryPercent_y 156
Time_en 158 Time_x 160 Time_y 162
Warn_en 164 Warn_x 166 Warn_y 168
WindSpeed_en 176 WindSpeed_x 178 WindSpeed_y 180
Climb_en 182 Climb_x 184 Climb_y 186
Tune_en 188 Tune_x 190 Tune_y 192
Eff_en 194 Eff_x 196 Eff_y 198
CALLSIGN_en 200 CALLSIGN_x 202 CALLSIGN_y 204
Temp_en 212 Temp_x 214 Temp_y 216
Distance_en 224 Distance_x 226 Distance_y 228
CamPos_en 230 CamPos_x 231 CamPos_y 232

EF_CLIMB_panel_item 233
BATT_B_VOLT_panel_item 235
GPS_REL_ALT_panel_item 237
MAV_MSG_panel_item 239
WIND_ARROW_panel_item 241

SIGN_MSL_ON 876
SIGN_HA_ON 878
SIGN_GS_ON 880
SIGN_AS_ON 882 
MODEL_TYPE 884 
AUTO_SCREEN_SWITCH 886
BATT_SHOW_PERCENT 888
measure 890
overspeed 892
stall 894
battv 896
RSSI_HIGH 900
RSSI_LOW 902
ch_toggle 906
RSSI_RAW 908
switch_mode 910
PAL_NTSC 912
BATT_WARN 914
RSSI_WARN 916
BRIGHTNESS 918
CALL_SIGN 920
FW_VERSION1 930
FW_VERSION2 932
FW_VERSION3 934
CS_VERSION1 936
CS_VERSION2 937
CS_VERSION3 938
MOTOR_WARN_THR 944
MOTOR_WARN_CURR 945
VOFFSET 946
HOFFSET 947
MAV_BAUD 948
FONT_LOADER_ON 949
PANELS_NUM 950
EEPROM_OLD_VER 1010
EEPROM_NEW_VER 1014
}

array set var_default {
SIGN_MSL_ON 1
SIGN_HA_ON 1
SIGN_GS_ON 1
SIGN_AS_ON 1
AUTO_SCREEN_SWITCH 4
BATT_SHOW_PERCENT 0
measure 0
overspeed 100
stall 25
battv 101
RSSI_HIGH 255
RSSI_LOW 0
ch_toggle 0
RSSI_RAW 0
PAL_NTSC 1
BATT_WARN 10
RSSI_WARN 10
BRIGHTNESS 0
CALL_SIGN "abcdefgh"
MOTOR_WARN_THR 120
MOTOR_WARN_CURR 50
VOFFSET 0
HOFFSET 0
MAV_BAUD 57
PANELS_NUM 3
}

set write_ignore_vars {
  EEPROM_OLD_VER EEPROM_NEW_VER  FW_VERSION1 FW_VERSION2 FW_VERSION3
}

set cfg_new_vars {
 VOFFSET HOFFSET MAV_BAUD
 MOTOR_WARN_CURR MOTOR_WARN_THR
 CamPos
 BATT_B_VOLT EF_CLIMB GPS_REL_ALT MAV_MSG WIND_ARROW
 PANELS_NUM 
}

proc cfg_var_valid {var {panel -1}} {
 if {$::cfg_ver >= 4 && $::cfg_ver != 255} {return 1}
 if {$panel == 2} {return 0}
 if {[lsearch -exact $::cfg_new_vars $var] != -1} {return 0}
 return 1
}

proc cfg_var_cvt {var val} {
 if {$::cfg_ver >= 4 && $::cfg_ver != 255} {return $val}
 if {$var eq "AUTO_SCREEN_SWITCH" && $val == 3} {return 4}
 return $val
}

set ops {
 "write" "read" "mcm2fnt" "fnt2mcm" "writefont" "writefw"
}

proc usage {code} {
 if {$code == 0} {set f stdout} else {set f stderr}
 set me minimosd_cfg
 puts $f "$me: usage"
 puts $f "\t$me write \[connection_options\] \[-eep <eeprom_file>] \[-cf <config_file>]"
 puts $f "\t$me read \[connection_options\] \[-eep <eeprom_file>] \[-cf <config_file>]"
 puts $f "\t$me mcm2fnt \[-mcm <mcm_file>] \[-fnt <fnt_file>]"
 puts $f "\t$me fnt2mcm \[-mcm <mcm_file>] \[-fnt <fnt_file>]"
 puts $f "\t$me writefont \[-P <serial_port>\] \[{-mcm <mcm_file> | -fnt <fnt_file>}]"
 puts $f "\t$me writefw \[connection_options\] -fw <hex_file>"
 puts $f ""
 puts $f "connection options:"
 puts $f "\t-P <serial_port>\tserial port device name for bootloader/mavlink connection"
 puts $f "\t-mav\tattempt to use mavlink serial port forwarding for bootloader connection"
 puts $f "\t-mav_baud <br>\tport speed for mavlink connection to autopilot"
 puts $f "\t-mav_osd_baud <br>\tport speed for connection between autopilot and OSD"
 puts $f "\t-mav_sysid <id>\tmavlink system id assigned to autopilot/OSD"
 puts $f "\t-mav_chan <chan_id>\tautopilot mavlink channel id where OSD is connected"
 exit $code
}

proc my_error {msg} {
 puts stderr "minimosd_cfg: error: $msg"
 exit 2
}

set op ""
if {![info exists mav_chan]} {set mav_chan 0}
if {![info exists verbose]} {set verbose 0}
if {![info exists noclear]} {set noclear 0}

for {set i 0} {$i < [llength $argv]} {incr i} {
 switch [lindex $argv $i] {
  "-eep" {incr i; set fn [lindex $argv $i]}
  "-cf" {incr i; set ofn [lindex $argv $i]}
  "-P" {incr i; set serial_port [lindex $argv $i]}
  "-mcm" {incr i; set mcm_file [lindex $argv $i]}
  "-fnt" {incr i; set fnt_file [lindex $argv $i]}
  "-fw" {incr i; set fwfn [lindex $argv $i]}
  "-noclear" {set noclear 1}
  "-verbose" {set verbose 1; set mav::verbose 1}
  "-mav" {set serial_over_mav 1}
  "-mav_baud" {incr i; set mav_baud [lindex $argv $i]}
  "-mav_chan" {incr i; set mav_chan [lindex $argv $i]}
  "-mav_sysid" {incr i; set mav_sysid [lindex $argv $i]}
  "-mav_osd_baud" {incr i; set mav_osd_baud [lindex $argv $i]}
  "help" {usage 0}
  "write" -
  "read" -
  "mcm2fnt" -
  "fnt2mcm" -
  "writefont" -
  "writefw" {set op [lindex $argv $i]}
  default {
   my_error "unknown option [lindex $argv $i]"
  }
 }
}

if {$op eq ""} {usage 1}

if {![info exists serial_over_mav]} {
 set serial_over_mav 0
}

if {![info exists serial_port]} {
 if {$serial_over_mav} {
  set serial_port "/dev/ttyACM0"
 } else {
  set serial_port "/dev/ttyUSB0"
 }
}

if {![info exists mav_baud]} {
 if {$serial_over_mav} {
  set mav_baud 115200
 } else {
  if {[info exists mav_osd_baud]} {
   set mav_baud $mav_osd_baud
  } else {
   set mav_baud 57600
  }
 }
}

proc bl_connect {} {
 set ofd [open $::serial_port r+]
 fconfigure $ofd -translation binary
 fconfigure $ofd -buffering none
 fconfigure $ofd -timeout 500
 fconfigure $ofd -mode 57600,n,8,1
 fconfigure $ofd -handshake none
 
 fconfigure $ofd -ttycontrol {DTR 1}
 fconfigure $ofd -ttycontrol {DTR 0}
 after 500
 fconfigure $ofd -blocking 0
 read $ofd
 fconfigure $ofd -blocking 1
 set ::bl_cmd bl_cmd_serial
 return $ofd
}

proc bl_cmd_serial {fd data resp_len} {
  puts -nonewline $fd $data
  if {$resp_len == 0} {return {}}
  return [read $fd $resp_len]
}

array set mem2cmd {
 EEPROM E
 FLASH F
}

proc bl_read_mem1 {ofd memtype addr len} {
 set n $len
 if {$addr & 1} {
  incr n
  set trim 2
 } else {
  set trim 1
 }
 set a [expr {$addr/2}]
 set resp [$::bl_cmd $ofd [binary format "asa" U $a " "] 2]
 binary scan $resp "H*" r
 if {$r ne "1410"} {
  verbose_msg "bootloader: reading $memtype @$addr: bad responce for address set command ($r)"
  return {}
 }
 set resp [$::bl_cmd $ofd [binary format "aSaa" t $n $::mem2cmd($memtype) " "] [expr {$n+2}]]
#puts "got [string length $resp] bytes"
#binary scan $resp H* xx
#puts $xx
 set nf [binary scan $resp "H2@${trim}a${len}H2" r14 data r10]
 if {$nf != 3} {
  if {$nf == 0} {
   verbose_msg "bootloader: reading $memtype @$addr: no response"
  } else {
   set last_b [string length $resp]
   incr last_b -1
   binary scan $resp "@${last_b}H2" r10
   verbose_msg "bootloader: reading $memtype @$addr: bad length [string length $resp], $n+2 expected, header=$r14, trailer=$r10"
  }
  return {}
 }
 if {$r14 ne "14" || $r10 ne "10"} {
  verbose_msg "bootloader: reading $memtype bad responce: header $r14 trailer $r10" 
  return {}
 }
 return $data
}

proc bl_read_mem {ofd t addr len} {
 for {set i 0} {$i < 5} {incr i} {
  set r [bl_read_mem1 $ofd $t $addr $len]
  if {$r ne {}} {return $r}
 }
 return {}
}

proc bl_read_eep_num {ofd addr} {
 set r [bl_read_mem $ofd EEPROM $addr 1]
 if {$r eq {}} {return $r}
 binary scan $r cu ret
 return $ret
}

proc bl_write_mem {ofd memtype addr data {verify 0}} {
 verbose_msg "bootloader: write $memtype @$addr"
 if {$addr & 1} {
  if {$memtype eq "FLASH"} {
   error "can't write to odd FLASH address"
  }
  set b [bl_read_mem $ofd $memtype [expr {$addr-1}] 1]
  if {$b eq ""} {return 0}
  set data [binary format aa* $b $data]
 }
 set addr [expr {$addr/2}]
 set n [string length $data]
 set resp [$::bl_cmd $ofd [binary format "asa" U $addr " "] 2]
 binary scan $resp "H*" r
 if {$r ne "1410"} {
  verbose_msg "bootloader: $memtype address set error"
  return 0
 }
 set resp [$::bl_cmd $ofd [binary format "aSaa*a" d $n $::mem2cmd($memtype) $data " "] 2]
 binary scan $resp "H*" r
 if {$r ne "1410"} {
  verbose_msg "bootloader: $memtype write error: response=\"$r\" len=$n datalen=[string length $data]"
  return 0
 }
 if {$verify} {
  set v [bl_read_mem $ofd $memtype [expr {$addr*2}] $n]
  if {$v ne $data} {verbose_msg "bootloader: verify failed after $memtype write"; return 0}
 }
 return 1
}

proc bl_write_eep_num {fd addr byte {verify 0}} {
 set d [binary format c $byte]
 return [bl_write_mem $fd EEPROM $addr $d $verify]
}

proc bl_write_flash_page {fd addr data} {
 if {[string length $data] != 128} {
  error "bl_write_flash_page: wrong data size [string length $data]"
 }
 if {($addr % 128) != 0} {
  error "bl_write_flash_page: address is not on page boundary $addr"
 }
 return [bl_write_mem $fd FLASH $addr $data 1]
}

proc bl_write_bl_jump {fd} {
 set jump_page [binary format @127c@0H8 0 0C94003C]
 return [bl_write_flash_page $fd 0 $jump_page]
}

proc bl_write_flash {fd firmware} {

 if {![bl_write_bl_jump $fd]} {
  my_error "can't program first flash page"
 }
 set end [string length $firmware]
 set off 128
 while {1} {
  binary scan $firmware "@${off}a128" data
  for {set i 0} {$i < 5} {incr i} {
   if {[bl_write_flash_page $fd $off $data]} break
   verbose_msg "bootloader: retrying flash page write at @$off"
  }
  if {$i == 5} {
   my_error "flash write failed at @$off"
  }
  if {$off == 0} break
  incr off 128
  if {$off > $end} {set off 0}
 }
}

proc ihex_read_and_check {fname} {
 set x [ihex_read $fname]
 if {[llength $x] != 2} {
  my_error "ihex has more than one chunk"
 }
 if {[lindex $x 0] != 0} {
  my_error "firmware is not starting from address 0"
 }
 set data [lindex $x 1]
 set len [string length $data]
 if {$len > 32768-2048} {
  my_error "firmware is too big"
 }
 if {$len < 128} {
  my_error "firmware is too small"
 }
 set pagepad [expr {(($len-1)/128+1)*128-1}]
 set data [binary format "@${pagepad}c@0a*" 0 $data]
 return $data
}

proc bl_exit {fd} {
 # send some trash to force application start
 $::bl_cmd $fd [binary format a10 ??????????] 0

 if {$::eeprom_access eq "bl"} {
  close $fd
 } else {
  osd::serial_fwd_exit $fd
  mav::close_serial $fd
 }
}

proc verbose_msg {m} {
 if {!$::verbose} return
 puts $m
}

proc bl_check_sig {fd} {
 for {set i 0} {$i < 5} {incr i} {
  set resp [$::bl_cmd $fd [binary format "aa" u " "] 5]
  if {[binary scan $resp "H10" r] == 1} { 
   if {$r eq "141e950f10"} {return 1}
   binary scan $resp H* rr
   verbose_msg "bootloader: device signature doesn't match (got $rr)"
  }
  after 50
 }
 return 0
}

proc bit_flag_get {flag_v str pos} {
 return $flag_v
}

proc bit_flag_set {flag_v ch f} {
 if {$flag_v & $f} {return $ch}
 return ""
}

proc dec_flag_get {flag_v str pos} {
 upvar $pos i
 if {[string is digit [string index $str $i+2]]} {
  set end 2
 } else {
  set end 1
 }
 set val [string range $str $i+1 $i+$end]
 if {![string is integer $val]} {
  my_error "expected integer value in \"$str\""
 }
 if {$val & ~$flag_v} {
  puts [format %x [expr  ~$flag_v]]
  my_error "value is to large in \"$str\""
 }

 incr i $end
 return $val
}

proc dec_flag_set {flag_v ch f} {
 if {$flag_v & $f} {return $ch[expr {$f & $flag_v}]}
 return ""
}

array set flags_info {
  GPS_REL_ALT {s {bit_flag 0x20} u {bit_flag 0x10} l {bit_flag 0x8}}
  BATT_B_VOLT {u {bit_flag 0x10}}
  MAV_MSG {w {dec_flag 31}}
  WIND_ARROW {s {bit_flag 0x20}}
}
proc str2flags {var s} {
 set f 0
 array set fi $::flags_info($var)
 for {set i 0} {$i < [string length $s]} {incr i} {
   set ch [string index $s $i]
   lset fi($ch) 0 [lindex $fi($ch) 0]_get
   set ff [eval [concat $fi($ch) $s i]]
   set f [expr {$ff | $f}]
 }
 return $f
}
#   set f [expr {$::str2f($ch) | $f}]
#   if {$ch eq "x"} {
#    set ff 0x[string range $s $i+1 $i+2]
#    if {[catch {expr {$ff}}] != 0} {puts err}
#    set f [expr {$ff | $f}]
#    incr i 2
#   }

proc flags2str {var f} {
 array set fi $::flags_info($var)
 set s ""
 foreach i [array names fi] {
  lset fi($i) 0 [lindex $fi($i) 0]_set
  append s [eval [concat $fi($i) $i $f]]
 }
 return $s
}

proc var_type {name} {
  if {$::name2addr($name) < 250} {
   if {[string match "*_en" $name]} {return panel_item_old}
   if {[string match "*_panel_item" $name]} {return panel_item}
   return unknown
  }
  return var
}

proc var_name {full_name} {
 set t [var_type $full_name]
 if {$t eq {panel_item_old}} {return [string range $full_name 0 end-3]}
 if {$t eq {panel_item}} {return [string range $full_name 0 end-11]}
 return $full_name
}

proc panel_var {panel var} {
   if {[var_type $var] eq {panel_item_old}} {
     set ret {}
     set v [var_name $var]
     foreach i {en x y} {
      set addr [expr {$::name2addr(${v}_${i}) + 250*$panel}]
      binary scan $::eep @${addr}cu val
      lappend ret $i $val
     }
     lappend ret flags 0
     return $ret
   }

   set addr [expr {$::name2addr($var) + 250*$panel}]
   binary scan $::eep @${addr}Su val
   set en [expr {($val & 0x8000) != 0}]
   set ret [list en $en]
   lappend ret x [expr {($val>>8)&0x1f}]
   lappend ret y [expr {$val&0x0f}]
   lappend ret flags [expr {(($val>>9)&0x30) | (($val>>4)&0x0f)}]

   return $ret
}

array set special_get {
 CALL_SIGN read_callsign
 HOFFSET read_vhoffset
 VOFFSET read_vhoffset
}
array set special_set {
 CALL_SIGN write_callsign
 HOFFSET write_vhoffset
 VOFFSET write_vhoffset
}

proc read_callsign {var} {
  set addr $::name2addr($var)
  binary scan $::eep @${addr}a8 val
  return [string trimright $val "\xff\x00"]
}

proc write_callsign {fd var val} {
 set addr $::name2addr($var)
 set val [binary format @7c@0a* 0 $val]
 $::write_var $fd $addr $val
}

proc read_vhoffset {var} {
   set addr $::name2addr($var)
   binary scan $::eep @${addr}cu x
   if {$var eq "HOFFSET"} {return [expr {($x & 0x3f)-32}]}
   return [expr {($x & 0x1f)-16}]
}

proc write_vhoffset {fd var val} {
   set addr $::name2addr($var)
   if {$var eq "HOFFSET"} {
    set val [expr {($val+32) & 0x3f}]
   } else {
    set val [expr {($val+16) & 0x1f}]
   }
   $::write_var $fd $addr [binary format c $val]
}

proc cfg_var {var} {
   if {[info exists ::special_get($var)]} {
    return [$::special_get($var) $var]
   }
   set addr $::name2addr(${var})
   binary scan $::eep @${addr}cu val
   return $val
}

proc dump_panel_cfg {fd var_name} {
 set var [var_name $var_name]

 set en "-"
 #set flags 0x3f
 for {set panel 0} {$panel < $::npanels} {incr panel} {
  array set v [panel_var $panel $var_name]
  if {$v(en)} {set en "+"}
  #set flags [expr {$flags & $v(flags)}]
  array unset v
 }
 puts -nonewline $fd [format "%s%-15s" $en $var]
 for {set panel 0} {$panel < $::npanels} {incr panel} {
  if {![cfg_var_valid $var $panel]} {
   array set v {x 0 y 0 en - flags 0}
  } else {
   array set v [panel_var $panel $var_name]
  }
  if {$v(en)} {set en "+"} else {set en "-"}
  #set f $en[flags2str [expr {$v(flags) & ~$flags}]]
  set f $en
  if {$v(flags) != 0} {
   append f [flags2str $var $v(flags)]
  } 
  if {$panel == 0} {set sp " "} else {set sp "     "}
  puts -nonewline $fd [format "$sp%-3s %3d %3d" $f $v(x) $v(y)]
  array unset v
 }
 puts $fd ""
}

proc dump_var {fd var} {
 set sps ""
 for {set i [expr {25-[string length $var]}]} {$i != 0} {incr i -1} {
    append sps " "
 }
 if {[cfg_var_valid $var]} {
  set val [cfg_var_cvt $var [cfg_var $var]]
 } else {
  set val $::var_default($var)
 }
 puts $fd [format "%s%s %s" $var $sps $val]
}

array set plusminus {
 + 1
 - 0
}

proc write_var_file {f addr val} {
 seek $f $addr start
 puts -nonewline $f [binary format "a*" $val]
}

proc write_var_buf {f addr val} {
 set ::eep [binary format "a*@${addr}a*" $::eep $val]
}

proc write_var_bl {f addr val} {
 if {![bl_write_mem $f EEPROM $addr $val 1]} {
  my_error "write to eeprom @$addr failed"
 }
}

proc bl_write_eep_all {fd eep} {
  for {set a 0} {$a < 1024} {incr a 64} {
   binary scan $eep @${a}a64 blkl
   if {![bl_write_mem $fd EEPROM $a $blkl 1]} {
    return 0
   }
  }
  return 1
}

proc set_var {f var val} {
 if {[info exists ::special_set($var)]} {
   $::special_set($var) $f $var $val
   return
 }
 set addr $::name2addr($var)
 $::write_var $f $addr [binary format c $val]
}

proc set_panel_var {f p var val} {
 if {[info exists ::name2addr(${var}_en)]} {
  # old style format
  foreach {i j} $val {
   if {$i eq "flags"} continue
   set addr $::name2addr(${var}_${i})
   set addr [expr {$addr + $p*250}]
   $::write_var $f $addr [binary format c $j]
  }
  return
 }
 array set v $val
 set vv [expr {($v(en)<<15) | $v(y) | ($v(x)<<8) | (($v(flags)&0xf)<<4) | (($v(flags)&0x30)<<9)}]
 set addr $::name2addr(${var}_panel_item)
 set addr [expr {$addr + $p*250}]
 $::write_var $f $addr [binary format S $vv]
}

if {[info exists fn] && $op ne "writefw"} {
  set eeprom_access file
} elseif {$serial_over_mav} {
  set eeprom_access mav
} else {
  set eeprom_access bl
}

proc read_eeprom {fd} {
 set retry 0
 set ret {}
 for {set a 0} {$a < 1024} {incr a 64} {
  set r [bl_read_mem $fd EEPROM $a 64]
  if {$r eq {}} {
   if {$retry == 10} {return {}}
   incr retry
   verbose_msg "bootloader: retry eeprom read from $a"
   incr a -64
  } else {
   append ret $r
  }
 }
 return $ret
}

if {[info exists mav_sysid]} {
  set osd::sysid $::mav_sysid
  set bl_mav_needs_detect 0
  if {[info exists mav_osd_baud]} {set osd::restore_baud $mav_osd_baud}
} else {
 set bl_mav_needs_detect 1
}

proc osd_open_mav {} {
 set fd [mav::open_serial $::serial_port $::mav_baud]
 if {$::bl_mav_needs_detect} {
  if {![osd::detect $fd]} {
   my_error "no heartbeats from osd"
  }
  verbose_msg "mavlink: detected osd with sysid $osd::sysid, baudrate $osd::restore_baud"
  set ::bl_mav_needs_detect 0
 }
 return $fd
}

proc bl_open_mav {timeout {min_timeout 20}} {
 set ::bl_cmd osd::serial_fwd_data
 set fd [osd_open_mav]
 osd::reboot $fd

 # Let it transmit the command before changing speed.
 # Command ack might work better, but then we'd have
 # to ignore missing ack in case of interrupted firmware flash.
 after 100

 osd::serial_fwd_config [list chan $::mav_chan baud 57600 timeout_max 500 timeout_min 20]
 if {![bl_check_sig $fd]} {
   osd::serial_fwd_exit $fd
   my_error "failed to contact bootloader"
 }
 osd::serial_fwd_config [list chan $::mav_chan baud 57600 timeout_max $timeout timeout_min $min_timeout]
 return $fd
}

if {$op eq "read"} {
 
 if {$eeprom_access eq "bl"} {
  set fd [bl_connect]
  set eep [read_eeprom $fd]
  bl_exit $fd
 } elseif {$eeprom_access eq "mav"} {
  set fd [bl_open_mav 250]
  set eep [read_eeprom $fd]
  bl_exit $fd
 } else {
  set f [open $fn r]
  fconfigure $f -translation binary
  set eep [read $f 1024]
  close $f
 }
 if {[string length $eep] != 1024} {
  my_error "failed to read eeprom"
 }

 set fw_ver [cfg_var FW_VERSION1].[cfg_var FW_VERSION2].[cfg_var FW_VERSION3]
 if {$fw_ver ne $fw_version} {
  puts stderr "minimosd_cfg: warning: firmware version $fw_ver doesn't match this utility ($fw_version)"
  puts stderr "Please, upgrade minimosd_cfg to be sure settings are read correctly."
 }
 set cfg_ver [cfg_var EEPROM_NEW_VER]

 if {[info exists ofn]} {
  set ofd [open $ofn w]
 } else {
  set ofd stdout
 }
 foreach i [lsort [array names name2addr]] {
  set vt [var_type $i]
  if {$vt ne {panel_item} && $vt ne {panel_item_old}} continue
  dump_panel_cfg $ofd $i
 }
 foreach i [lsort [array names name2addr]] {
   set vt [var_type $i]
   if {$vt ne {var}} continue
   dump_var $ofd $i
 }
 if {$ofd ne "stdout"} {close $ofd}
}

if {$op eq "write"} {
 if {[info exists ofn]} {
  set ifd [open $ofn r]
 } else {
  set ifd stdin
 }
 if {$noclear} {
  if {$eeprom_access eq "bl"} {
   set f [bl_connect]
   set close_eep_fd bl_exit
   set write_var write_var_bl
  } elseif {$eeprom_access eq "mav"} {
   set f [bl_open_mav 1000 100]
   set close_eep_fd bl_exit
   set write_var write_var_bl
  } else {
   set write_var write_var_file
   set f [open $fn r+]
   fconfigure $f -translation binary
   set close_eep_fd close
  }
 } else {
  set write_var write_var_buf
  set eep [binary format @1023c 0]
  set f {}
 }

 while {![eof $ifd]} {
  set s [gets $ifd]
  if {[string index [lindex $s 0] 0] eq "#"} continue
  if {[llength $s] == 0} continue
  if {[llength $s] == 2} {
   if {[lsearch -exact $write_ignore_vars [lindex $s 0]] != -1} continue
   set var [lindex $s 0]
   set_var $f $var  [lindex $s 1]
   set v_set($var) 1
   continue
  }
  if {[llength $s] >= 4} {
   set np [expr {([llength $s]-1)/3}]
   if {$np > $npanels} {set np $npanels}
   set var [string range [lindex $s 0] 1 end]
   set off 1
   for {set p 0} {$p < $np} {incr p} {
    set v {}
    set f [lindex $s $off]
    lappend v en $plusminus([string index $f 0])
    if {[string range $f 1 end] ne ""} {
     lappend v flags [str2flags $var [string range $f 1 end]]
    } else {
     lappend v flags 0
    }
    incr off
    lappend v x [lindex $s $off] 
    incr off
    lappend v y [lindex $s $off]
    incr off
    set_panel_var $f $p $var $v
    if {[info exists name2addr(${var}_en)]} {
     set pv_set(${var}_en) 1
    } else {
     set pv_set(${var}_panel_item) 1
    }
   }
   if {!$noclear} {
    for {} {$p < $npanels} {incr p} {
     set_panel_var $f $p $var {en 0 x 0 y 0 flags 0}
    }
   }
   continue
  }
  my_error "unexpected config string: $s"
 }
 if {$ifd ne "stdin"} {close $ifd}
 if {!$noclear} {
   # set defaults
   foreach i [array names name2addr] {
    if {[info exists v_set($i)] || [info exists pv_set($i)]} continue
    set vt [var_type $i]
    if {$vt eq "panel_item_old" || $vt eq "panel_item"} {
     # disable all panel displays by default
     set var [var_name $i]
     for {set p 0} {$p < $npanels} {incr p} {
      set_panel_var $f $p $var {en 0 x 0 y 0 flags 0}
     }
    } elseif {[info exists var_default($i)]} {
     set_var $f $i $var_default($i)
    }
   }
   set_var $f EEPROM_NEW_VER $eeprom_version
   set_var $f EEPROM_OLD_VER 76
   set fw_ver [string map {. " "} $fw_version]
   set_var $f FW_VERSION1 [lindex $fw_ver 0]
   set_var $f FW_VERSION2 [lindex $fw_ver 1]
   set_var $f FW_VERSION3 [lindex $fw_ver 2]
   if {$eeprom_access eq "bl"} {
    set bl_fd [bl_connect]
    set r [bl_write_eep_all $bl_fd $eep]
    bl_exit $bl_fd
    if {!$r} {
     my_error "eeprom write failed"
    }
   } elseif {$eeprom_access eq "mav"} {
    set bl_fd [bl_open_mav 1000 100]
    set r [bl_write_eep_all $bl_fd $eep]
    bl_exit $bl_fd
    if {!$r} {
     my_error "eeprom write failed"
    }
   } elseif {$eeprom_access eq "file"} {
    set f [open $fn w]
    fconfigure $f -translation binary
    puts -nonewline $f $eep
    close $f
   }
 } else {
   $close_eep_fd $f
 }
}

proc read_bit {fd} {
 while {1} {
  set b [read $fd 1]
  if {$b eq "0"} {return $b}
  if {$b eq "1"} {return $b}
 }
}
if {$op eq "mcm2fnt"} {
 array set bit2char {
  0,0 *
  1,0 O
  0,1 _
  1,1 _
 }
 if {[info exists fnt_file]} {
  set ofd [open $fnt_file w]
 } else {
  set ofd stdout
 }
 if {[info exists mcm_file]} {
  set ifd [open $mcm_file r]
 } else {
  set ifd stdin
 }
 for {set char_cnt 0} {$char_cnt < 256} {incr char_cnt} {
  puts $ofd [format 0x%02x $char_cnt]
  for {set y_cnt 0} {$y_cnt < 18} {incr y_cnt} {
   for {set x_cnt 0} {$x_cnt < 12} {incr x_cnt} {
    set b1 [read_bit $ifd]
    set b2 [read_bit $ifd]
    puts -nonewline $ofd $bit2char($b1,$b2)
   }
   puts $ofd ""
  }
  for {set i 0} {$i < 80} {incr i} {read_bit $ifd}
 }
 if {$ofd ne "stdout"} {close $ofd}
 if {$ifd ne "stdin"} {close $ifd}
}

array set char2bit {
  * "00"
  O "10"
  _ "01"
}

set linepos 0
proc read_fnt_px {fd} {
 while {1} {
  set b [read $fd 1]
  if {[info exists ::char2bit($b)]} {
   incr ::linepos
   if {$::linepos == 12} {
    gets $fd
    set ::linepos 0
   }
   return $::char2bit($b)
  }
  if {$b eq "\r" || $b eq "\n"} {
   my_error "unexpected end of string"
  }
  if {[eof $fd]} {
   my_error "premature end of file"
  }
 }
}

proc fnt2mcm {ifd} {
 set ch {}
 for {set y_cnt 0} {$y_cnt < 54} {incr y_cnt} {
  set b ""
  for {set x_cnt 0} {$x_cnt < 4} {incr x_cnt} {
   append b [read_fnt_px $ifd]
  }
  lappend ch $b
 }
 for {set i 0} {$i < 10} {incr i} {
   lappend ch "01010101"
 }
 return $ch
}

proc read_fnt {ifd} {
 array set fnt {}
 while {[array size fnt] != 256} {
  set cn [expr {[gets $ifd]}]
  if {[eof $ifd]} {
   set g {}
   for {set i 0} {$i < 256} {incr i} {
    if {![info exists fnt($i)]} {
     lappend g $i
    }
   }
   my_error "premature end of file: more glyphs expected: $g"
  }
  set ch [fnt2mcm $ifd]
  if {[info exists fnt($cn)]} {puts stderr "warning: duplicate glyph $cn"}
  set fnt($cn) $ch
 }
 set l {}
 for {set i 0} {$i < 256} {incr i} {
  lappend l $fnt($i)
 }
 return $l
}

if {$op eq "fnt2mcm"} {
 if {[info exists fnt_file]} {
  set ifd [open $fnt_file r]
 } else {
  set ifd stdin
 }
 if {[info exists mcm_file]} {
  set ofd [open $mcm_file w]
 } else {
  set ofd stdout
 }
 set l [read_fnt $ifd]
 fconfigure $ofd -translation crlf
 puts -nonewline $ofd "MAX7456"
 for {set char_cnt 0} {$char_cnt < 256} {incr char_cnt} {
  set ch [lindex $l $char_cnt]
  for {set y_cnt 0} {$y_cnt < 64} {incr y_cnt} {
   puts $ofd ""
   puts -nonewline $ofd [lindex $ch $y_cnt]
  }
 }
 if {$ofd ne "stdout"} {close $ofd}
 if {$ifd ne "stdin"} {close $ifd}
}

proc get_font_loader_resp {fd buf resp skip_start} {
 upvar $buf mybuf
 #set b [read $fd 1]
 set b [$::fl_read $fd [string length $resp]]
 if {$b eq ""} {return "timeout"}
 append mybuf $b
 #binary scan $mybuf H* b1
 #binary scan $resp H* b2
 #puts "cmp $mybuf $b1 $resp $b2"
 if {[string first $resp $mybuf] < 0} {
  if {!$skip_start && [string length $mybuf] == [string length $resp]} {
   my_error "unexpected response from font loader"
  } else {
   return "nextch"
  }
 }
 return "ok"
}

proc fl_read_serial {fd max_len} {
 return [read $fd 1]
}

proc fl_write_serial {fd data} {
 puts -nonewline $fd $data
}

proc fl_read_mav {fd max_len} {
 set x [osd::serial_fwd_data $fd "" $max_len]
 #puts "resp $x"
 return $x
}

proc fl_write_mav {fd data} {
 #puts "fl_write_mav: $data"
 return [osd::serial_fwd_data $fd $data 0]
}

if {$op eq "writefont"} {
 if {[info exists fnt_file]} {
  set ifd [open $fnt_file r]
  set font_format "fnt"
  set fnt [read_fnt $ifd]
  close $ifd
 } else {
  if {![info exists mcm_file]} {
   set ifd stdin
  }
  set ifd [open $mcm_file r]
  set header [gets $ifd]
  if {$header ne "MAX7456"} {
   my_error "$mcm_file doesn't look like mcm"
  }
  set fnt {}
  for {set char 0} {$char < 256} {incr char} {
   set cx {}
   for {set byte 0} {$byte < 64} {incr byte} {
    lappend cx [gets $ifd]
   }
   lappend fnt $cx
  }
  if {$ifd ne "stdin"} {close $ifd}
  set font_format "mcm"
 }
 if {!$serial_over_mav} {
  
  if {![info exists mav_osd_baud]} {
   set ofd [bl_connect]
   set mav_baud [bl_read_eep_num $ofd $name2addr(MAV_BAUD)]
   if {$mav_baud eq ""} {
    my_error "no response from bootloader"
   }
   bl_exit $ofd
   if {$mav_baud == 115} {set mav_baud 115200} else {set mav_baud 57600}
   verbose_msg "font update: baud rate is $mav_baud"
  }
  if {!$bl_mav_needs_detect} {
    # open resets the OSD, wait for it to pass bootloader before osd::reset
  	set delay 4000
  } else {
    # osd_open_mav will wait for osd to initialize
    set delay 0
  }
  set ofd [osd_open_mav]
  after $delay
  osd::reboot $ofd fl

  set fl_read fl_read_serial
  set fl_write fl_write_serial
 } else {
  set ofd [osd_open_mav]
  osd::reboot $ofd fl
  osd::serial_fwd_config [list chan $::mav_chan baud 0 timeout_max 500 timeout_min 20]
  set fl_read fl_read_mav
  set fl_write fl_write_mav
 }
 set resp ""
 set timeou 25
 while {1} {
  set r [get_font_loader_resp $ofd resp "Ready for Font\r\n" 1]
  if {$r eq "ok"} break
  if {$r eq "nextch"} continue
  incr timeou -1
  if {!$timeou} {
   if {$serial_over_mav} {osd::serial_fwd_exit $ofd}
   mav::close_serial $ofd
   my_error "timeout waiting for responce from font loader"
  }
 }
 if {!$verbose} {
  puts -nonewline "Font loader is ready, writing chars: "
 } else {
  puts "Font loader is ready, writing chars: "
 }
 flush stdout
 for {set char 0} {$char < 256} {incr char} {
  set ch [lindex $fnt $char]
  set d ""
  set n 0
  for {set byte 0} {$byte < 64} {incr byte} {
    append d [lindex $ch $byte]
    append d "\r\n"
    incr n
    if {$n == 6} {
     $fl_write $ofd $d
     if {$serial_over_mav} {after 140}
     set n 0
     set d ""
    }
  }
  if {$n > 0} {
    $fl_write $ofd $d
    if {$serial_over_mav} {after 140}
  }
  set resp ""
  set timeo 10
  while {1} {
   set r [get_font_loader_resp $ofd resp "Char Done\r\n" 0]
   if {$r eq "ok"} break
   if {$r eq "nextch"} continue
   incr timeo -1
   if {!$timeo} {
    my_error "timeout waiting for responce after char $char"
   }
  }
  if {!$verbose} {
   if {$char % 10 == 0} {puts -nonewline .; flush stdout}
  } else {
   puts "wrote char $char"
  }
 }
 puts "done."
 if {$serial_over_mav} {
  osd::serial_fwd_exit $ofd
 }
 mav::close_serial $ofd
}

if {$op eq "writefw"} {
 set firmware [ihex_read_and_check $fwfn]
 if {!$serial_over_mav} {
  # exec -ignorestderr avrdude -patmega328p -carduino -P $serial_port -b57600 -D -U "flash:w:$fwfn:i"
  set bl_fd [bl_connect]
 } else {
  set bl_fd [bl_open_mav 1000 100]
 }
 bl_write_flash $bl_fd $firmware
 bl_exit $bl_fd
}
