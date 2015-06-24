# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

# if you have placed the alternate core in your sketchbook directory, then you can just mention the core name alone.
#ALTERNATE_CORE = atmega
# If not, you might have to include the full path.
#ALTERNATE_CORE_PATH = /home/sudar/Dropbox/code/arduino-sketches/hardware/attiny/

BOARD_TAG    = atmega328

ARDUINO_HEADER = dummy_header.h
USER_LIB_PATH =  ../libraries
include /usr/share/arduino/Arduino.mk
OSD_LANG ?= OSD_ENGLISH
CPPFLAGS += -Wno-deprecated-declarations -D$(OSD_LANG) 

# !!! Important. You have to use make ispload to upload when using ISP programmer
