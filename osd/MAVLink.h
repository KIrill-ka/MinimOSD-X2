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
 uint8_t i;
 uint8_t *addr, *buf;
 uint8_t len;
                 
 pdata = (uint8_t*)_MAV_PAYLOAD_NON_CONST(msg);
 c = (mavlink_command_long_t*)pdata;

 if(c->target_system != mavlink_system.sysid
       || c->target_component != mavlink_system.compid) return;
 switch(c->command) {
         case 30400: /* reboot */
                 break;
         case 30401: /* read eeprom */
         case 30402: /* write eeprom */
                 {
                  addr = (uint8_t*)*(uint16_t*)pdata;
                  len = *(uint8_t*)(pdata+2);
                  buf = pdata+4; /* 6 floats = 24 bytes */
                  if(len > 24) break;
                  if(c->command == 30401) {
                   for(i = 0; i < len; i++) 
                    buf[i] = eeprom_read_byte(addr+i);
                   c->command = 30403;
                   c->target_component = msg->compid;
                   c->target_system = msg->sysid;
                   _mav_finalize_message_chan_send(MAVLINK_COMM_0, MAVLINK_MSG_ID_COMMAND_LONG, (const char *)c, MAVLINK_MSG_ID_COMMAND_LONG_LEN, 152);
                  } else {
                   for(i = 0; i < len; i++) 
                    eeprom_write_byte(addr+i, buf[i]);
                  }
                 }
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
        if (lf_count >= 0 && ms < 20000 && ms > 5000) {
            if (c == '\n') lf_count++;
            else if (c != '\r')  lf_count = 0;

            if (lf_count == 3) {
             lf_count = -1;
             uploadFont();
            }
        }

        if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
         
            uint8_t new_data = 1;

            lf_count = -1;

            switch(msg.msgid) {
            case MAVLINK_MSG_ID_HEARTBEAT:
                {
                    osd_mode = (uint8_t)mavlink_msg_heartbeat_get_custom_mode(&msg);
                }
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
                    osd_alt = mavlink_msg_vfr_hud_get_alt(&msg);
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
                }
                break;
            case MAVLINK_MSG_ID_COMMAND_LONG:
                new_data = 0;
                process_command(&msg);
                break;
#if 0
            case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
                new_data = 0;
                {
                 uint8_t i;
                 mavlink_param_value_t *pval;
                 mavlink_param_request_read_t *req;
                 int16_t idx;
                 uint8_t sz;
                 req = (mavlink_param_request_read_t*)_MAV_PAYLOAD_NON_CONST(&msg);

                 if(req->target_system != mavlink_system.sysid
                                || req->target_component != mavlink_system.compid) break;
                 idx = req->param_index;
                 if(req->param_id[0] == 'R'
                          && req->param_id[1] == 'A'
                          && req->param_id[2] == 'W') {
                  sz = (req->param_id[3]-'0')*10+(req->param_id[4]-'0');
                  if(sz > 16) break;
                 } else break;
                 pval = (mavlink_param_value_t*)req;
                 memset(pval, 0, sizeof(*pval));
                 for(i = 0; i < sz; i++, idx++) 
                   pval->param_id[i] = eeprom_read_byte((const uint8_t*)idx);
                 pval->param_count = 1024; /* eeprom size */
                 pval->param_index = idx;
                 /* packet.param_type = MAVLINK_TYPE; */
                 _mav_finalize_message_chan_send(MAVLINK_COMM_0, MAVLINK_MSG_ID_PARAM_VALUE, (const char *)pval, 25, 220);
                }
                break;
            case MAVLINK_MSG_ID_PARAM_SET:
                new_data = 0;
                {
                 uint8_t i;
                 int16_t idx;
                 uint8_t sz;
                 mavlink_param_set_t *req;

                 req = (mavlink_param_set_t*)_MAV_PAYLOAD_NON_CONST(&msg);

                 if(req->target_system != mavlink_system.sysid
                         || req->target_component != mavlink_system.compid) break;
                  idx = req->param_value;
                  sz = req->param_type;
                  if(sz > 16 || sz == 0) break;
                  for(i = 0; i < sz; i++, idx++) 
                    eeprom_write_byte((uint8_t*)idx, req->param_id[i]);
                 }
                break;
#endif
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
        if(ms > mav_beat_timer + MAV_BEAT_INTERVAL && ms - MAV_BEAT_INTERVAL > mav_beat_timer) {
         mav_beat_timer = ms;
         mavlink_msg_heartbeat_send(MAVLINK_COMM_0, 24 /*dev type MAV_TYPE*/, 0 /* AP class MAV_AUTOPILOT */, 
                         0 /* MAV_MODE_FLAGS */, osd_statf /* custom mode */, 0 /* MAV_STATE */);
        }

        n_bytes = Serial.available();
        if(n_bytes > 0) continue;
        delayMicroseconds(138);
        n_bytes = Serial.available();
    }
}
