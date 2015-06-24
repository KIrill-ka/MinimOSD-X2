#!/usr/bin/tclsh

array set xlat2 {
 0 "\\x30"
 1 "\\x31"
 2 "\\x32"
 3 "\\x33"
 4 "\\x34"
 5 "\\x35"
 6 "\\x36"
 7 "\\x37"
 8 "\\x38"
 9 "\\x39"
}
array set xlat {
 а "\\x61"
 б "\\x62"
 в "\\x63"
 г "\\x64"
 д "\\x65"
 е "\\x66"
 ё "\\x67"
 ж "\\x68"
 з "\\x69"
 и "\\x6a"
 й "\\x6b"
 к "\\x6c"
 л "\\x6d"
 м "\\x6e"
 н "\\x6f"
 о "\\x70"
 п "\\x71"
 р "\\x72"
 с "\\x73"
 т "\\x74"
 у "\\x75"
 ф "\\x76"
 х "\\x77"
 ц "\\x78"
 ч "\\x79"
 ш "\\x7a"
 щ "\\x7b"
 ъ "\\x7c"
 ы "\\x7d"
 ь "\\x7e"
 э "\\x7f"
 ю "\\x80"
 я "\\x81"
 А "\\x40"
 Б "\\x41"
 В "\\x42"
 Г "\\x43"
 Д "\\x44"
 Е "\\x45"
 Ё "\\x46"
 Ж "\\x47"
 З "\\x48"
 И "\\x49"
 Й "\\x4a"
 К "\\x4b"
 Л "\\x4c"
 М "\\x4d"
 Н "\\x4e"
 О "\\x4f"
 П "\\x50"
 Р "\\x51"
 С "\\x52"
 Т "\\x53"
 У "\\x54"
 Ф "\\x55"
 Х "\\x56"
 Ц "\\x57"
 Ч "\\x58"
 Ш "\\x59"
 Щ "\\x5a"
 Ъ "\\x5b"
 Ы "\\x5c"
 Ь "\\x5d"
 Э "\\x5e"
 Ю "\\x5f"
 Я "\\x60"
}

while {![eof stdin]} {
set s [gets stdin]
set s1 ""
set x 0
for {set i 0} {$i < [string length $s]} {incr i} {
 set ch [string index $s $i]
 if {[info exists xlat($ch)]} {
  append s1 $xlat($ch)
  set x 1
 } elseif {$x && [info exists xlat2($ch)]} {
  append s1 $xlat2($ch)
 } else {
  append s1 $ch
 }
}
puts $s1
}
