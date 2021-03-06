proc ihex_read {fname} {
 set fd [open $fname r]
 set chunk {}
 set chunk_list {}
 set chunk_addr 0
 set chunk_addr_cur 0
 while {![eof $fd]} {
  set s [gets $fd]
  if {[string length $s] < 11} continue
  if {[string index $s 0] ne ":"} { close $fd; return {}} 
  set len 0x[string range $s 1 2]
  set addr 0x[string range $s 3 6]
  set type 0x[string range $s 7 8]
  set csum_len [expr {[string length $s]-1}]
  set cs 0
  for {set i 1} {$i < $csum_len} {incr i 2} {
   set byte 0x[string range $s $i $i+1]
   set cs [expr {$byte + $cs}]
  }
  if {($cs & 0xff) != 0} {
    close $fd
    return {}
  }
  set data [string range $s 9 end-2]
  if {[string length $data] != $len*2} {
    close $fd
    return {}
  }
  if {$type == 1} break
  if {$type != 0} {
    close $fd
    return {}
  }
  if {$chunk_addr_cur != $addr} {
   if {$chunk ne {}} {
    lappend chunk_list $chunk_addr $chunk
    set chunk_addr $addr
    set chunk_addr_cur $addr
    set chunk {}
   }
  }
  incr chunk_addr_cur $len
  append chunk [binary format H* $data]
 }
 if {$chunk ne {}} {
    lappend chunk_list $chunk_addr $chunk
 }
 close $fd
 return $chunk_list
}

#set h [ihex_read [lindex $argv 0]]
#foreach {i j} $h {
# puts $i
# set d 
#}
