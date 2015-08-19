
void panLogo();
void do_converts();
void panFdata();
void writeSettings();
void readPanelSettings();
static int checkPAL(int line);
void updateSettings(byte panelu, byte panel_x, byte panel_y, byte panel_s );
void unplugSlaves();
void startPanels();
void writePanels();
void readSettings();
void OnMavlinkTimer();
void read_mavlink();
void panWaitMAVBeats(int first_col, int first_line);
void panBatt_A(int first_col, int first_line);
void panRose(int first_col, int first_line);
void panGPSats(int first_col, int first_line);
void panGPS(int first_col, int first_line);
void panHeading(int first_col, int first_line);

void panPitch(int first_col, int first_line);
void panRoll(int first_col, int first_line);
void panThr(int first_col, int first_line);
void panBatteryPercent(int first_col, int first_line);
void panTime(int first_col, int first_line);
void panWarn(int first_col, int first_line);
void panCOG(int first_col, int first_line);
void panHomeDis(int first_col, int first_line);
void panHorizon(int first_col, int first_line);
void panWPDis(int first_col, int first_line);
void panAlt(int first_col, int first_line);
void panCur_A(int first_col, int first_line);
void panCur_A(int first_col, int first_line);
void showArrow(uint8_t rotate_arrow);  
void timers();
void panCALLSIGN(int first_col, int first_line);
void panDistance(int first_col, int first_line);
void panTemp(int first_col, int first_line);
void panEff(int first_col, int first_line);
void panRSSI(int first_col, int first_line);
void panWindSpeed(int first_col, int first_line);
void panHomeDir(int first_col, int first_line);
void panHomeAlt(int first_col, int first_line);
void panVel(int first_col, int first_line);
void panAirSpeed(int first_col, int first_line);
void panFlightMode(int first_col, int first_line);
void panClimb(int first_col, int first_line);
void panTune(int first_col, int first_line);
void showHorizon(int start_col, int start_row);
void panCamPos(int first_col, int first_line);
void showBatteryBVolt(uint8_t x, uint8_t y, uint8_t flags);
void showClimbEfficiency(uint8_t x, uint8_t y, uint8_t flags);

void uploadFont();
void load_mavlink_settings(void);
