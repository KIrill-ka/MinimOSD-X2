/* ******************************************************************/
/* *********************** GENERAL FUNCTIONS ********************** */

//Extract functions (get bits from the positioning bytes
#define ISa(panel,whichBit) getBit(panA_REG[panel], whichBit)
#define ISb(panel,whichBit) getBit(panB_REG[panel], whichBit)
#define ISc(panel,whichBit) getBit(panC_REG[panel], whichBit)
#define ISd(panel,whichBit) getBit(panD_REG[panel], whichBit)
#define ISe(panel,whichBit) getBit(panE_REG[panel], whichBit)

boolean getBit(byte Reg, byte whichBit) {
    boolean State;
    State = Reg & (1 << whichBit);
    return State;
}

byte setBit(byte &Reg, byte whichBit, boolean stat) {
    if (stat) {
        Reg = Reg | (1 << whichBit);
    } 
    else {
        Reg = Reg & ~(1 << whichBit);
    }
    return Reg;
}

void readSettings() 
{
    overspeed = eeprom_read_byte((uint8_t*)overspeed_ADDR);
    stall = eeprom_read_byte((uint8_t*)stall_ADDR);
    battv = eeprom_read_byte((uint8_t*)battv_ADDR);
    switch_mode = eeprom_read_byte((uint8_t*)switch_mode_ADDR);
    panel_auto_switch = eeprom_read_byte((uint8_t*)AUTO_SCREEN_SWITC_ADD);
    ch_toggle = eeprom_read_byte((uint8_t*)ch_toggle_ADDR);
    rssical = eeprom_read_byte((uint8_t*)OSD_RSSI_HIGH_ADDR);
    rssipersent = eeprom_read_byte((uint8_t*)OSD_RSSI_LOW_ADDR);
    rssiraw_on = eeprom_read_byte((uint8_t*)OSD_RSSI_RAW_ADDR);

    batt_warn_level = eeprom_read_byte((uint8_t*)OSD_BATT_WARN_ADDR);
    rssi_warn_level = eeprom_read_byte((uint8_t*)OSD_RSSI_WARN_ADDR);
    
    uint8_t i;
    for(i=0; i < OSD_CALL_SIGN_TOTAL; i++) 
        char_call[i] = eeprom_read_byte((uint8_t*)OSD_CALL_SIGN_ADDR + i);
    
    motor_warn_thr = eeprom_read_byte((uint8_t*)MOTOR_WARN_THR_ADDR);
    motor_warn_curr = eeprom_read_byte((uint8_t*)MOTOR_WARN_CURR_ADDR);
    npanels_conf = eeprom_read_byte((uint8_t*)PANELS_NUM_ADDR);
    if(npanels_conf > npanels) npanels_conf = npanels;
    else if(npanels_conf < 1) npanels_conf = 1;
}

void readPanelSettings() {

    uint16_t offset = PANEL_CONFIG_SIZE * panel;
 //   setBit(panA_REG[panel], Cen_BIT, eeprom_read_byte((uint8_t*)panCenter_en_ADDR + offset));
 //   panCenter_XY[0][panel] = eeprom_read_byte((uint8_t*)panCenter_x_ADDR + offset);
 //   panCenter_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCenter_y_ADDR + offset));

    setBit(panA_REG[panel], Bp_BIT, eeprom_read_byte((uint8_t*)panBatteryPercent_en_ADDR + offset));
    panBatteryPercent_XY[0][panel] = eeprom_read_byte((uint8_t*)panBatteryPercent_x_ADDR + offset);
    panBatteryPercent_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panBatteryPercent_y_ADDR + offset));

    setBit(panA_REG[panel], Pit_BIT, eeprom_read_byte((uint8_t*)panPitch_en_ADDR + offset));
    panPitch_XY[0][panel] = eeprom_read_byte((uint8_t*)panPitch_x_ADDR + offset);
    panPitch_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panPitch_y_ADDR + offset));

    setBit(panA_REG[panel], Rol_BIT, eeprom_read_byte((uint8_t*)panRoll_en_ADDR + offset));
    panRoll_XY[0][panel] = eeprom_read_byte((uint8_t*)panRoll_x_ADDR + offset);
    panRoll_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panRoll_y_ADDR + offset));

    setBit(panA_REG[panel], BatA_BIT, eeprom_read_byte((uint8_t*)panBatt_A_en_ADDR + offset));
    panBatt_A_XY[0][panel] = eeprom_read_byte((uint8_t*)panBatt_A_x_ADDR + offset);
    panBatt_A_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panBatt_A_y_ADDR + offset));

    //setBit(panA_REG, BatB_BIT, eeprom_read_byte((uint8_t*)panBatt_B_en_ADDR));
    //panBatt_B_XY[0] = eeprom_read_byte((uint8_t*)panBatt_B_x_ADDR);
    //panBatt_B_XY[1] = checkPAL(eeprom_read_byte((uint8_t*)panBatt_B_y_ADDR));

    setBit(panA_REG[panel], GPSats_BIT, eeprom_read_byte((uint8_t*)panGPSats_en_ADDR + offset));
    panGPSats_XY[0][panel] = eeprom_read_byte((uint8_t*)panGPSats_x_ADDR + offset);
    panGPSats_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panGPSats_y_ADDR + offset));

    setBit(panA_REG[panel], COG_BIT, eeprom_read_byte((uint8_t*)panCOG_en_ADDR + offset));
    panCOG_XY[0][panel] = eeprom_read_byte((uint8_t*)panCOG_x_ADDR + offset);
    panCOG_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCOG_y_ADDR + offset));

    setBit(panA_REG[panel], GPS_BIT, eeprom_read_byte((uint8_t*)panGPS_en_ADDR + offset));
    panGPS_XY[0][panel] = eeprom_read_byte((uint8_t*)panGPS_x_ADDR + offset);
    panGPS_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panGPS_y_ADDR + offset));

    //****** Second set of 8 Panels ******

    setBit(panB_REG[panel], Rose_BIT, eeprom_read_byte((uint8_t*)panRose_en_ADDR + offset));
    panRose_XY[0][panel] = eeprom_read_byte((uint8_t*)panRose_x_ADDR + offset);
    panRose_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panRose_y_ADDR + offset));

    setBit(panB_REG[panel], Head_BIT, eeprom_read_byte((uint8_t*)panHeading_en_ADDR + offset));
    panHeading_XY[0][panel] = eeprom_read_byte((uint8_t*)panHeading_x_ADDR + offset);
    panHeading_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panHeading_y_ADDR + offset));

//    setBit(panB_REG[panel], MavB_BIT, eeprom_read_byte((uint8_t*)panMavBeat_en_ADDR + offset));
//    panMavBeat_XY[0][panel] = eeprom_read_byte((uint8_t*)panMavBeat_x_ADDR + offset);
//    panMavBeat_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panMavBeat_y_ADDR + offset));

    setBit(panB_REG[panel], HDis_BIT, eeprom_read_byte((uint8_t*)panHomeDis_en_ADDR + offset));
    panHomeDis_XY[0][panel] = eeprom_read_byte((uint8_t*)panHomeDis_x_ADDR + offset);
    panHomeDis_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panHomeDis_y_ADDR + offset));

    setBit(panB_REG[panel], HDir_BIT, eeprom_read_byte((uint8_t*)panHomeDir_en_ADDR + offset));
    panHomeDir_XY[0][panel] = eeprom_read_byte((uint8_t*)panHomeDir_x_ADDR + offset);
    panHomeDir_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panHomeDir_y_ADDR + offset));

//    setBit(panB_REG[panel], WDir_BIT, eeprom_read_byte((uint8_t*)panWPDir_en_ADDR + offset));
//    panWPDir_XY[0][panel] = eeprom_read_byte((uint8_t*)panWPDir_x_ADDR + offset);
//    panWPDir_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panWPDir_y_ADDR + offset));

    setBit(panB_REG[panel], WDis_BIT, eeprom_read_byte((uint8_t*)panWPDis_en_ADDR + offset));
    panWPDis_XY[0][panel] = eeprom_read_byte((uint8_t*)panWPDis_x_ADDR + offset);
    panWPDis_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panWPDis_y_ADDR + offset));

    setBit(panB_REG[panel], Time_BIT, eeprom_read_byte((uint8_t*)panTime_en_ADDR + offset));
    panTime_XY[0][panel] = eeprom_read_byte((uint8_t*)panTime_x_ADDR + offset);
    panTime_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panTime_y_ADDR + offset));

    //setBit(panB_REG, RSSI_BIT, eeprom_read_byte((uint8_t*)panRSSI_en_ADDR));
    //panRSSI_XY[0] = eeprom_read_byte((uint8_t*)panRSSI_x_ADDR);
    //panRSSI_XY[1] = checkPAL(eeprom_read_byte((uint8_t*)panRSSI_y_ADDR));

    //****** Third set of 8 Panels ******

    setBit(panC_REG[panel], CurA_BIT, eeprom_read_byte((uint8_t*)panCur_A_en_ADDR + offset));
    panCur_A_XY[0][panel] = eeprom_read_byte((uint8_t*)panCur_A_x_ADDR + offset);
    panCur_A_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCur_A_y_ADDR + offset));

    //setBit(panC_REG, CurB_BIT, eeprom_read_byte((uint8_t*)panCur_B_en_ADDR));
    //panCur_B_XY[0] = eeprom_read_byte((uint8_t*)panCur_B_x_ADDR);
    //panCur_B_XY[1] = checkPAL(eeprom_read_byte((uint8_t*)panCur_B_y_ADDR));

    setBit(panC_REG[panel], Alt_BIT, eeprom_read_byte((uint8_t*)panAlt_en_ADDR + offset));
    panAlt_XY[0][panel] = eeprom_read_byte((uint8_t*)panAlt_x_ADDR + offset);
    panAlt_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panAlt_y_ADDR + offset));

    setBit(panC_REG[panel], Halt_BIT, eeprom_read_byte((uint8_t*)panHomeAlt_en_ADDR + offset));
    panHomeAlt_XY[0][panel] = eeprom_read_byte((uint8_t*)panHomeAlt_x_ADDR + offset);
    panHomeAlt_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panHomeAlt_y_ADDR + offset));

    setBit(panC_REG[panel], As_BIT, eeprom_read_byte((uint8_t*)panAirSpeed_en_ADDR + offset));
    panAirSpeed_XY[0][panel] = eeprom_read_byte((uint8_t*)panAirSpeed_x_ADDR + offset);
    panAirSpeed_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panAirSpeed_y_ADDR + offset));

    setBit(panC_REG[panel], Vel_BIT, eeprom_read_byte((uint8_t*)panVel_en_ADDR + offset));
    panVel_XY[0][panel] = eeprom_read_byte((uint8_t*)panVel_x_ADDR + offset);
    panVel_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panVel_y_ADDR + offset));

    setBit(panC_REG[panel], Thr_BIT, eeprom_read_byte((uint8_t*)panThr_en_ADDR + offset));
    panThr_XY[0][panel] = eeprom_read_byte((uint8_t*)panThr_x_ADDR + offset);
    panThr_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panThr_y_ADDR + offset));

    setBit(panC_REG[panel], FMod_BIT, eeprom_read_byte((uint8_t*)panFMod_en_ADDR + offset));
    panFMod_XY[0][panel] = eeprom_read_byte((uint8_t*)panFMod_x_ADDR + offset);
    panFMod_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panFMod_y_ADDR + offset));

    setBit(panC_REG[panel], Hor_BIT, eeprom_read_byte((uint8_t*)panHorizon_en_ADDR + offset));
    panHorizon_XY[0][panel] = eeprom_read_byte((uint8_t*)panHorizon_x_ADDR + offset);
    panHorizon_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panHorizon_y_ADDR + offset));

    setBit(panD_REG[panel], Warn_BIT, eeprom_read_byte((uint8_t*)panWarn_en_ADDR + offset));
    panWarn_XY[0][panel] = eeprom_read_byte((uint8_t*)panWarn_x_ADDR + offset);
    panWarn_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panWarn_y_ADDR + offset));

    setBit(panD_REG[panel], WindS_BIT, eeprom_read_byte((uint8_t*)panWindSpeed_en_ADDR + offset));
    panWindSpeed_XY[0][panel] = eeprom_read_byte((uint8_t*)panWindSpeed_x_ADDR + offset);
    panWindSpeed_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panWindSpeed_y_ADDR + offset));

    setBit(panD_REG[panel], Climb_BIT, eeprom_read_byte((uint8_t*)panClimb_en_ADDR + offset));
    panClimb_XY[0][panel] = eeprom_read_byte((uint8_t*)panClimb_x_ADDR + offset);
    panClimb_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panClimb_y_ADDR + offset));

    setBit(panD_REG[panel], Tune_BIT, eeprom_read_byte((uint8_t*)panTune_en_ADDR + offset));
    panTune_XY[0][panel] = eeprom_read_byte((uint8_t*)panTune_x_ADDR + offset);
    panTune_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panTune_y_ADDR + offset));

    setBit(panD_REG[panel], RSSI_BIT, eeprom_read_byte((uint8_t*)panRSSI_en_ADDR + offset));
    panRSSI_XY[0][panel] = eeprom_read_byte((uint8_t*)panRSSI_x_ADDR + offset);
    panRSSI_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panRSSI_y_ADDR + offset));
    
    setBit(panD_REG[panel], Eff_BIT, eeprom_read_byte((uint8_t*)panEff_en_ADDR + offset));
    panEff_XY[0][panel] = eeprom_read_byte((uint8_t*)panEff_x_ADDR + offset);
    panEff_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panEff_y_ADDR + offset));

    setBit(panD_REG[panel], CALLSIGN_BIT, eeprom_read_byte((uint8_t*)panCALLSIGN_en_ADDR + offset));
    panCALLSIGN_XY[0][panel] = eeprom_read_byte((uint8_t*)panCALLSIGN_x_ADDR + offset);
    panCALLSIGN_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCALLSIGN_y_ADDR + offset));

//   setBit(panE_REG[panel], Ch_BIT, eeprom_read_byte((uint8_t*)panCh_en_ADDR + offset));
//    panCh_XY[0][panel] = eeprom_read_byte((uint8_t*)panCh_x_ADDR + offset);
//    panCh_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCh_y_ADDR + offset));

    setBit(panE_REG[panel], TEMP_BIT, eeprom_read_byte((uint8_t*)panTemp_en_ADDR + offset));
    panTemp_XY[0][panel] = eeprom_read_byte((uint8_t*)panTemp_x_ADDR + offset);
    panTemp_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panTemp_y_ADDR + offset));

    setBit(panE_REG[panel], DIST_BIT, eeprom_read_byte((uint8_t*)panDistance_en_ADDR + offset));
    panDistance_XY[0][panel] = eeprom_read_byte((uint8_t*)panDistance_x_ADDR + offset);
    panDistance_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panDistance_y_ADDR + offset));

    setBit(panE_REG[panel], CAM_POS_BIT, eeprom_read_byte((uint8_t*)panCamPos_en_ADDR + offset));
    panCameraPos_XY[0][panel] = eeprom_read_byte((uint8_t*)panCamPos_x_ADDR + offset);
    panCameraPos_XY[1][panel] = checkPAL(eeprom_read_byte((uint8_t*)panCamPos_y_ADDR + offset));
}

static int checkPAL(int line){
    if(line >= MAX7456_CENTER_NTSC && osd.getMode() == 0) line -= 3;
    return line;
}

static bool 
get_item_config(uint16_t addr, uint8_t panel, uint8_t *xr, uint8_t *yr, uint8_t *flag_bits)
{
 uint8_t x, y, f;

 addr += panel * PANEL_CONFIG_SIZE;
 x = eeprom_read_byte((uint8_t*)addr);
 if ((x & 0x80) == 0) return false;
 y = eeprom_read_byte((uint8_t*)addr+1);
 f = (y >> 4) | ((x & 0x60)>>1);
 y &= 0x0f;
 x &= 0x1f;
 *yr = checkPAL(y);
 *xr = x;
 *flag_bits = f;
 return true;
}
