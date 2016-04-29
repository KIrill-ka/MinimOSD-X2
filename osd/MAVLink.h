#include <avr/wdt.h>
#include "../GCS_MAVLink/include/mavlink/v1.0/mavlink_types.h"
#include "../GCS_MAVLink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

static int8_t lf_count = 0; // -1 when we have received at least 1 MAVLink packet

#define MAV_BEAT_INTERVAL 5000
static uint32_t mav_beat_timer = 0;

void load_mavlink_settings(void)
{
 if(!eeprom_read_byte((uint8_t*)FONT_LOADER_ON_ADDR))
  lf_count = -1;
}

static void process_command(mavlink_message_t *msg)
{
 mavlink_command_long_t *c;
 uint8_t *pdata;
                 
 pdata = (uint8_t*)_MAV_PAYLOAD_NON_CONST(msg);
 c = (mavlink_command_long_t*)pdata;

 if(c->target_system != mavlink_system.sysid
       || c->target_component != mavlink_system.compid) return;

 switch(c->command) {
         case 246: 
                 if((uint8_t)c->param3 == 1) {
                  delay(1000);
                  uploadFont();
                  break;
                 }
                 cli();
                 UCSR0B = 0;
                 UCSR0A = 0; /* bootloader doesn't clear U2X0 flag, we might use this feature
                                to run bootloader @115200 */
                 asm volatile ("jmp 0x7800"); /* call bootloader */
                 break;
 }
}

void read_mavlink()
{
    mavlink_message_t msg; 
    mavlink_status_t status;
    int8_t n_bytes;
    uint32_t ms;

    n_bytes = Serial.available();
    while(n_bytes > 0) {
        uint8_t c = Serial.read();

        ms = millis();
        if(lf_count >= 0) {
         if(ms > 20000) lf_count = -1;
         else if(ms > 5000) {
            if(c == '\n') lf_count++;
            else if(c != '\r') lf_count = 0;

            if (lf_count == 3) {
             lf_count = -1;
             uploadFont();
            }
         }
        }

        if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
         
            uint8_t new_data = 1;

            lf_count = -1;

            switch(msg.msgid) {
            case MAVLINK_MSG_ID_HEARTBEAT:
                {
                    uint8_t type = mavlink_msg_heartbeat_get_autopilot(&msg);
                    if(type == MAV_AUTOPILOT_INVALID) break;
                    if(mavlink_system.sysid == 97) mavlink_system.sysid = msg.sysid;
                    osd_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
                    if(mavlink_msg_heartbeat_get_base_mode(&msg)& MAV_MODE_FLAG_SAFETY_ARMED)
                     osd_statf |= ARMED_F;
                    else osd_statf &= ~ARMED_F;
                }
                break;
            case MAVLINK_MSG_ID_BATTERY2:
                osd_battb_volt = mavlink_msg_battery2_get_voltage(&msg);
                break;
            case MAVLINK_MSG_ID_SYS_STATUS:
                {
                    osd_vbat_A = (mavlink_msg_sys_status_get_voltage_battery(&msg) / 1000.0f); //Battery voltage, in millivolts (1 = 1 millivolt)
                    osd_curr_A = mavlink_msg_sys_status_get_current_battery(&msg); //Battery current, in 10*milliamperes (1 = 10 milliampere)         
                    osd_battery_remaining_A = mavlink_msg_sys_status_get_battery_remaining(&msg); //Remaining battery energy: (0%: 0, 100%: 100)
                }
                break;

            case MAVLINK_MSG_ID_GPS_RAW_INT:
                {
                    osd_lat = mavlink_msg_gps_raw_int_get_lat(&msg) / 10000000.0f;
                    osd_lon = mavlink_msg_gps_raw_int_get_lon(&msg) / 10000000.0f;
                    osd_fix_type = mavlink_msg_gps_raw_int_get_fix_type(&msg);
                    osd_satellites_visible = mavlink_msg_gps_raw_int_get_satellites_visible(&msg);
                    osd_cog = mavlink_msg_gps_raw_int_get_cog(&msg);
                    eph = mavlink_msg_gps_raw_int_get_eph(&msg);
                }
                break; 
            case MAVLINK_MSG_ID_VFR_HUD:
                {
                    osd_airspeed = mavlink_msg_vfr_hud_get_airspeed(&msg);
                    osd_groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);
                    osd_heading = mavlink_msg_vfr_hud_get_heading(&msg); // 0..360 deg, 0=north
                    osd_throttle = (uint8_t)mavlink_msg_vfr_hud_get_throttle(&msg);
                    osd_climb = mavlink_msg_vfr_hud_get_climb(&msg);
                }
                break;
            case MAVLINK_MSG_ID_ATTITUDE:
                {
                    osd_pitch = ToDeg(mavlink_msg_attitude_get_pitch(&msg));
                    osd_roll = ToDeg(mavlink_msg_attitude_get_roll(&msg));
                }
                break;
            case MAVLINK_MSG_ID_MOUNT_STATUS:
                {
                 int32_t yaw = mavlink_msg_mount_status_get_pointing_c(&msg);
                 cam_pan_angle = yaw/100;
                }
                break;
            case MAVLINK_MSG_ID_STATUSTEXT:
                {
                 uint8_t i;
                 uint8_t sev = mavlink_msg_statustext_get_severity(&msg);
                 if(statustext_timer != 0 && sev >= statustext_sev) break;
                 statustext_sev = sev;
                 statustext_timer = 107; /* 10.7 sec */
                 statustext_pos = 0;
                 mavlink_msg_statustext_get_text(&msg, statustext);
                 for(i = 0; i < sizeof(statustext); i++) {
                  char c = statustext[i];
                  if(c >= 'a' && c <= 'z') statustext[i] = c-('a'-'A');
                 }
                }
                break;
                
            case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
                {
                  wp_target_bearing = mavlink_msg_nav_controller_output_get_target_bearing(&msg);
                  wp_dist = mavlink_msg_nav_controller_output_get_wp_dist(&msg);
                  alt_error = mavlink_msg_nav_controller_output_get_alt_error(&msg);
                  aspd_error = mavlink_msg_nav_controller_output_get_aspd_error(&msg);
                  xtrack_error = mavlink_msg_nav_controller_output_get_xtrack_error(&msg);
                }
                break;
            case MAVLINK_MSG_ID_MISSION_CURRENT:
                {
                    wp_number = (uint8_t)mavlink_msg_mission_current_get_seq(&msg);
                }
                break;
            case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
                {
//                    chan1_raw = mavlink_msg_rc_channels_raw_get_chan1_raw(&msg);
//                    chan2_raw = mavlink_msg_rc_channels_raw_get_chan2_raw(&msg);
//                    chan3_raw = mavlink_msg_rc_channels_raw_get_chan3_raw(&msg);
//                    chan4_raw = mavlink_msg_rc_channels_raw_get_chan4_raw(&msg);
                    chan5_raw = mavlink_msg_rc_channels_raw_get_chan5_raw(&msg);
                    chan6_raw = mavlink_msg_rc_channels_raw_get_chan6_raw(&msg);
                    chan7_raw = mavlink_msg_rc_channels_raw_get_chan7_raw(&msg);
                    chan8_raw = mavlink_msg_rc_channels_raw_get_chan8_raw(&msg);
                    osd_rssi = mavlink_msg_rc_channels_raw_get_rssi(&msg);
                }
                break;           
            case MAVLINK_MSG_ID_WIND:
                {
                  osd_winddirection = mavlink_msg_wind_get_direction(&msg); // 0..360 deg, 0=north
                  osd_windspeed = mavlink_msg_wind_get_speed(&msg); //m/s
                }
                break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE:
                {
                    temperature = mavlink_msg_scaled_pressure_get_temperature(&msg);
                }
                break;
            case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                {
                    osd_alt_to_home = mavlink_msg_global_position_int_get_relative_alt(&msg)*0.001;
                    osd_alt = mavlink_msg_global_position_int_get_alt(&msg)*0.001;
                }
                break;
            case MAVLINK_MSG_ID_COMMAND_LONG:
                new_data = 0;
                process_command(&msg);
                break;
            default:
                new_data = 0;
                break;
            }

            if(new_data) {
             osd_statf |= NEW_DATA_F;
             last_mav_data_ts = ms;
            }
        }

        ms = millis();
        if(ms > mav_beat_timer + MAV_BEAT_INTERVAL && last_mav_data_ts != 0) {
         mav_beat_timer = ms;
         mavlink_msg_heartbeat_send(MAVLINK_COMM_0, 24 /*dev type MAV_TYPE*/, 0 /* AP class MAV_AUTOPILOT */, 
             0 /* MAV_MODE_FLAGS */, 
             (uint32_t)osd_statf | ((uint32_t)eeprom_read_byte((uint8_t*)MAV_BAUD_ADDR)<<16) /* custom mode */,
             0 /* MAV_STATE */);
        }

        n_bytes = Serial.available();
        if(n_bytes > 0) continue;
        delayMicroseconds(138);
        n_bytes = Serial.available();
    }
}
