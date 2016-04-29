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
 а "\\x41"
 б "\\x61"
 в "\\x42"
 г "\\x62"
 д "\\x63"
 е "\\x45"
 ё "\\x64"
 ж "\\x65"
 з "\\x66"
 и "\\x67"
 й "\\x68"
 к "\\x4b"
 л "\\x69"
 м "\\x4d"
 н "\\x48"
 о "\\x4f"
 п "\\x6a"
 р "\\x50"
 с "\\x43"
 т "\\x54"
 у "\\x6b"
 ф "\\x6c"
 х "\\x58"
 ц "\\x6d"
 ч "\\x6e"
 ш "\\x6f"
 щ "\\x70"
 ъ "\\x71"
 ы "\\x72"
 ь "\\x73"
 э "\\x74"
 ю "\\x75"
 я "\\x76"
 А "\\x41"
 Б "\\x61"
 В "\\x42"
 Г "\\x62"
 Д "\\x63"
 Е "\\x45"
 Ё "\\x64"
 Ж "\\x65"
 З "\\x66"
 И "\\x67"
 Й "\\x68"
 К "\\x4b"
 Л "\\x69"
 М "\\x4d"
 Н "\\x48"
 О "\\x4f"
 П "\\x6a"
 Р "\\x50"
 С "\\x43"
 Т "\\x54"
 У "\\x6b"
 Ф "\\x6c"
 Х "\\x58"
 Ц "\\x6d"
 Ч "\\x6e"
 Ш "\\x6f"
 Щ "\\x70"
 Ъ "\\x71"
 Ы "\\x72"
 Ь "\\x73"
 Э "\\x74"
 Ю "\\x75"
 Я "\\x76"
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
