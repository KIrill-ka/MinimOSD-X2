/*
   This project is focused on optimization and redesign of firmware for MinimOSD 
   board and it's clones. Project is based on MinimOSD Extra. User interface
   concept, look and feel are planned not change from MinimOSD Extra. Firmware
   will retain compatibility with minimosdextra's Windows configuration tool
   (OSD_Config) for some time.
   
   June 2015
   Kirill A. Kornilov

   Keywords: MAX7456 ATMEGA328P MAVLINK
   Original copyrights found in MinimOSD_Extra_Plane_Pre_release_Beta.ino 
   are retained below.
 */

/*

Copyright (c) 2011.  All rights reserved.
An Open Source Arduino based OSD and Camera Control project.

Program  : ArduCAM-OSD (Supports the variant: minimOSD)
Version  : V2.1, 24 September 2012
Author(s): Sandro Benigno
Coauthor(s):
Jani Hirvinen   (All the EEPROM routines)
Michael Oborne  (OSD Configutator)
Mike Smith      (BetterStream and Fast Serial libraries)
Gábor Zoltán
Pedro Santos
Special Contribuitor:
Andrew Tridgell by all the support on MAVLink
Doug Weibel by his great orientation since the start of this project
Contributors: James Goppert, Max Levine, Burt Green, Eddie Furey
and all other members of DIY Drones Dev team
Thanks to: Chris Anderson, Jordi Munoz
*/

/*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>

*/

#define isPAL 1

#include <FastSerial.h>
#include <AP_Common.h>
#include <math.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "wiring.h"
#endif
#include <EEPROM.h>
#include <GCS_MAVLink.h>

#ifdef OSD_RUSSIAN
#include "strings_ru.h"
#else 
#include "strings_en.h"
#endif

#include "Config.h"
#include "Max7456_osd.h"
#include "Vars.h"
#include "Func.h"

#include "prototypes.h"


FastSerialPort0(Serial);
OSD osd;

void setup() 
{
	if(eeprom_read_byte((uint8_t*)VER_NEW_ADDR) == VER_NEW)
     osd_statf |= NEW_CFG_F;
    if((osd_statf & NEW_CFG_F) != 0
       && eeprom_read_byte((uint8_t*)MAV_BAUD_ADDR) == 115)
     Serial.begin(115200);
    else Serial.begin(57600);
    // setup mavlink port
    mavlink_comm_0_port = &Serial;

    // Prepare OSD for displaying 
    osd.init((osd_statf & NEW_CFG_F) != 0);

    // Start 
    startPanels();
    delay(500);

    // Get correct panel settings from EEPROM
    readSettings();
    for(panel = 0; panel < npanels; panel++) readPanelSettings();
    panel = 0; //set panel to 0 to start in the first navigation screen
    checkModellType();
    load_mavlink_settings();
	delay(2000);
	Serial.flush(); 
}

void loop() 
{

    if((osd_statf & (NEW_DATA_F|SCREEN_UP_F)) == NEW_DATA_F || millis() > mavLinkTimer + 120) {
      mavLinkTimer = millis();
      setHeadingPatern();  // generate the heading patern
      setHomeVars();   // calculate and set Distance from home and Direction to home
      setFdataVars();
      writePanels();
      osd_statf |= SCREEN_UP_F;
      osd_statf &= ~NEW_DATA_F;
    }

    if(osd_statf & SCREEN_UP_F) {
      if(osd.checkVsync()) {
        osd.update();
        osd_statf &= ~SCREEN_UP_F;
      }
    }

    read_mavlink();
}

#include "Panels.h"
#include "MAVLink.h"
#include "Font.h"
