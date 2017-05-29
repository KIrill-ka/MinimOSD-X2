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

static void convert_cfg(uint8_t prev_ver)
{
    uint8_t i, j;
    if(prev_ver < EEP_VER_START || prev_ver > EEP_VER_CURRENT) {
     eeprom_write_byte((uint8_t*)FONT_LOADER_ON_ADDR, 1);
     eeprom_write_byte((uint8_t*)VOFFSET_ADDR, 16);
     eeprom_write_byte((uint8_t*)HOFFSET_ADDR, 32);
     eeprom_write_byte((uint8_t*)MAV_BAUD_ADDR, 57);
     eeprom_write_byte((uint8_t*)MOTOR_WARN_THR_ADDR, 255);
     eeprom_write_byte((uint8_t*)PANELS_NUM_ADDR, 2);
     if(eeprom_read_byte((uint8_t*)AUTO_SCREEN_SWITC_ADD) == 3)
      eeprom_write_byte((uint8_t*)AUTO_SCREEN_SWITC_ADD, 4);
     for(i = 0; i < npanels; i++) {
      for(j = EEP_EF_CLIMB; j < PANEL_CONFIG_SIZE; j++)
       eeprom_write_byte((uint8_t*) (j+i*PANEL_CONFIG_SIZE), 0);
      eeprom_write_byte((uint8_t*) (panCamPos_en_ADDR+i*PANEL_CONFIG_SIZE), 0);
     }
    } else if(prev_ver < 6) {
     for(i = 0; i < npanels; i++) {
      eeprom_write_word((uint16_t*) (EEP_GPS_REL_ALT+i*PANEL_CONFIG_SIZE), 0);
      eeprom_write_word((uint16_t*) (EEP_MAV_STATUSTEXT+i*PANEL_CONFIG_SIZE), 0);
      eeprom_write_word((uint16_t*) (EEP_WIND_ARROW+i*PANEL_CONFIG_SIZE), 0);
     }
     eeprom_write_byte((uint8_t*)PANELS_NUM_ADDR, 3);
    } else if(prev_ver < 7) {
     for(i = 0; i < npanels; i++)
      eeprom_write_word((uint16_t*) (EEP_WIND_ARROW+i*PANEL_CONFIG_SIZE), 0);
    }
    eeprom_write_byte((uint8_t*)VER_NEW_ADDR, EEP_VER_CURRENT);
}

void setup() 
{
    uint8_t ver;
    ver = eeprom_read_byte((uint8_t*)VER_NEW_ADDR);
	if(ver != EEP_VER_CURRENT) convert_cfg(ver);
    if(eeprom_read_byte((uint8_t*)MAV_BAUD_ADDR) == 115)
     Serial.begin(115200);
    else Serial.begin(57600);
    // setup mavlink port
    mavlink_comm_0_port = &Serial;

    // Prepare OSD for displaying 
    osd.init();

    // Start 
    startPanels();
    delay(500);

    // Get correct panel settings from EEPROM
    readSettings();
    for(panel = 0; panel < npanels; panel++) readPanelSettings();
    panel = 0; //set panel to 0 to start in the first navigation screen
    if (eeprom_read_byte((uint8_t*)MODELL_TYPE_ADD) != 0)
     eeprom_write_byte((uint8_t*)MODELL_TYPE_ADD, 0);
    if (eeprom_read_byte((uint8_t*)FW_VERSION1_ADDR) != FW_VER_HI)
     eeprom_write_byte((uint8_t*)FW_VERSION1_ADDR, FW_VER_HI);
    if (eeprom_read_byte((uint8_t*)FW_VERSION2_ADDR) != FW_VER_MID)
     eeprom_write_byte((uint8_t*)FW_VERSION2_ADDR, FW_VER_MID);
    if (eeprom_read_byte((uint8_t*)FW_VERSION3_ADDR) != FW_VER_LO)
     eeprom_write_byte((uint8_t*)FW_VERSION3_ADDR, FW_VER_LO);
    load_mavlink_settings();
	delay(2000);
	Serial.flush(); 
}

void loop() 
{
    uint32_t ms = millis();
    uint8_t f;

    f = osd_statf;

    if((f & (NEW_DATA_F|SCREEN_UP_F|NO_VSYNC_F)) == NEW_DATA_F || ms > mavLinkTimer + 120) {
      mavLinkTimer = ms;
      setHeadingPatern();  // generate the heading patern
      setHomeVars();   // calculate and set Distance from home and Direction to home
      setFdataVars();
      writePanels();

      f = osd_statf;
      if((f & SCREEN_UP_F) != 0) f |= NO_VSYNC_F; /* no vsync in 120ms, force update */
      f |= SCREEN_UP_F;
      f &= ~NEW_DATA_F;
      osd_statf = f;
    }

    if((f & NO_VSYNC_F) != 0 && osd.checkVsync()) {
     f &= ~NO_VSYNC_F;
     osd_statf = f;
    }

    if(f & SCREEN_UP_F) {
      if(osd.checkVsync() || (f & NO_VSYNC_F) != 0) {
        osd.update();
        f &= ~SCREEN_UP_F;
        osd_statf = f;
      }
    }

    read_mavlink();
}

#include "Panels.h"
#include "MAVLink.h"
#include "Font.h"
