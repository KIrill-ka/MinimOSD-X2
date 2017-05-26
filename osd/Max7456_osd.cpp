
#include <FastSerial.h>

#include "Max7456_osd.h"
// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "wiring.h"
#endif
#include "Spi.h"
#include "Config.h"
#include <EEPROM.h>

#define HW_VSYNC

static void write_reg(uint8_t addr, uint8_t val)
{
 PORTD &= ~_BV(PD6);
 Spi.transfer(addr);
 Spi.transfer(val);
 PORTD |= _BV(PD6);
}

static uint8_t read_reg(uint8_t addr)
{
 uint8_t v;
 PORTD &= ~_BV(PD6);
 Spi.transfer(addr);
 v = Spi.transfer(0xff);
 PORTD |= _BV(PD6);
 return v;
}
  
OSD::OSD()
{
}

//------------------ init ---------------------------------------------------

void OSD::init()
{
  uint8_t offset;

  PORTD |= _BV(PD6);
  pinMode(MAX7456_SELECT,OUTPUT);
  pinMode(MAX7456_VSYNC, INPUT);
  digitalWrite(MAX7456_VSYNC,HIGH); //enabling pull-up resistor

  detectMode();

  write_reg(MAX7456_VM0_reg, MAX7456_RESET | video_mode);
  delay(50);
  setBrightness();

  // define sync (auto,int,ext) and
  // making sure the Max7456 is enabled
  control(1);

  offset = EEPROM.read(VOFFSET_ADDR);
  write_reg(MAX7456_VOS_reg, offset); // 5 valid bits
  offset = EEPROM.read(HOFFSET_ADDR);
  write_reg(MAX7456_HOS_reg, offset); // 6 valid bits
  
  memset(osdbuf, ' ', sizeof(osdbuf)-1);
  osdbuf[sizeof(osdbuf)-1] = MAX7456_END_string;
}

//------------------ Detect Mode (PAL/NTSC) ---------------------------------

void OSD::detectMode()
{
  //read STAT and auto detect Mode PAL/NTSC
  byte osdstat_r = read_reg(MAX7456_STAT_reg_read);

  if ((B00000001 & osdstat_r) == 1){ //PAL
      setMode(1);  
  }
  else if((B00000010 & osdstat_r) == 1){ //NTSC
      setMode(0);
  }
  //If no signal was detected so it uses EEPROM config
  else{
      if (EEPROM.read(PAL_NTSC_ADDR) == 0){ //NTSC
          setMode(0);
      } 
      else { //PAL
          setMode(1);
      }
  }
}

//------------------ Set Brightness  ---------------------------------
void OSD::setBrightness()
{

    uint8_t blevel = EEPROM.read(OSD_BRIGHTNESS_ADDR);
    uint8_t x;
    uint8_t osdbl;
    uint8_t osd_level;

    //read black level register
    osdbl = read_reg(MAX7456_OSDBL_reg_read);
    if(blevel >= 100) osdbl |= 1<<4;
    else osdbl &= ~(1<<4);
    write_reg(MAX7456_OSDBL_reg, osdbl);


    switch(blevel%10) {
            case 1:  osd_level = MAX7456_WHITE_level_90; break;
            case 2:  osd_level = MAX7456_WHITE_level_100; break;
            case 3:  osd_level = MAX7456_WHITE_level_120; break;
            case 0:  
            default:
                     osd_level = MAX7456_WHITE_level_80; break;
    }
    switch((blevel/10)%10) {
            case 1:  osd_level |= 1<<2; /* 10% */ break;
            case 2:  osd_level |= 2<<2; /* 20% */ break;
            case 3:  osd_level |= 3<<2; /* 30% */ break;
            case 0:  
            default: /* no change, level 0% */ break;
    }
    
    // set all rows to the same brightness level
    for (x = 0x0; x < 0x10; x++) 
        write_reg(x+0x10, osd_level);
}

//------------------ Set Mode (PAL/NTSC) ------------------------------------

void OSD::setMode(int themode)
{
  switch(themode){
    case 0:
      video_mode = MAX7456_MODE_MASK_NTCS;
      video_center = MAX7456_CENTER_NTSC;
      break;
    case 1:
      video_mode = MAX7456_MODE_MASK_PAL;
      video_center = MAX7456_CENTER_PAL;
      break;
  }
}

//------------------ Get Mode (PAL 0/NTSC 1) --------------------------------

int OSD::getMode()
{
  switch(video_mode){
    case MAX7456_MODE_MASK_NTCS:
      return 0;
      break;
    case MAX7456_MODE_MASK_PAL:
      return 1;
      break;
  }
  return 0;
}

//------------------ Get Center (PAL/NTSC) ----------------------------------

int OSD::getCenter()
{
  return video_center; //first line for center panel
}


void OSD::clear()
{
  write_reg(MAX7456_DMM_reg, MAX7456_CLEAR_display);
  // FIXME: wait for clear to complete
}

void
OSD::setPanel(uint8_t st_col, uint8_t st_row){
  col = st_col;
  row = st_row;
  bufpos = st_row*30+st_col;
}


//------------------ write single char ---------------------------------------------

void
OSD::openSingle(uint8_t x, uint8_t y){
  bufpos = y*30+x;
}

//------------------ write ---------------------------------------------------

size_t
OSD::write(uint8_t c){
  
  if(c == '|'){
   row++;
   bufpos = row*30+col;
  } else {
   uint16_t bp = bufpos;
   if(bp < 30*16) {
    osdbuf[bp] = c;
    bufpos = bp+1;
   }
  }
  return 1;
}

void
OSD::update() {
 uint8_t *b;
 uint8_t ch;

 b = osdbuf;

 PORTD &= ~_BV(PD6);
 Spi.transfer(MAX7456_DMAH_reg);
 Spi.transfer(0);
 Spi.transfer(MAX7456_DMAL_reg);
 Spi.transfer(0);
 Spi.transfer(MAX7456_DMM_reg);
 Spi.transfer(1);
 PORTD |= _BV(PD6);

 ch = *b;
 while(1) {
  PORTD &= ~_BV(PD6);
  SPDR = ch;
  if(ch == MAX7456_END_string) break;
  *b = ' '; b++; ch = *b;
  while (!(SPSR & (1<<SPIF))) ;
  PORTD |= _BV(PD6);
 }
 while (!(SPSR & (1<<SPIF))) ;
 PORTD |= _BV(PD6);
 //memset(osdbuf, ' ', sizeof(osdbuf));
}

uint8_t OSD::checkVsync() {
#ifndef HW_VSYNC
 uint8_t s;
 PORTD &= ~_BV(PD6);
 Spi.transfer(MAX7456_STAT_reg_read);
 s = Spi.transfer(0xff);
 PORTD |= _BV(PD6);
 return (s & 0x10) == 0;
#else
 return !(PIND & _BV(PD2));
#endif
}

void
OSD::control(uint8_t ctrl)
{
  uint8_t val;

  val = video_mode;
  if(ctrl) val |= MAX7456_ENABLE_display_vert | MAX7456_SYNC_autosync;
  write_reg(MAX7456_VM0_reg, val);
}


void OSD::char_write_start(uint8_t char_n)
{
 write_reg(MAX7456_CMAH_reg, char_n);
}

void OSD::char_write_byte(uint8_t byte_n, uint8_t data)
{
 write_reg(MAX7456_CMAL_reg, byte_n);
 write_reg(MAX7456_CMDI_reg, data);
}

void OSD::char_write_end()
{
 write_reg(MAX7456_CMM_reg, WRITE_nvr);
 while(read_reg(MAX7456_STAT_reg_read) & STATUS_reg_nvr_busy);
}

//------------------ pure virtual ones (just overriding) ---------------------

int  OSD::available(void){
	return 0;
}
int  OSD::read(void){
	return 0;
}
int  OSD::peek(void){
	return 0;
}
void OSD::flush(void){
}

