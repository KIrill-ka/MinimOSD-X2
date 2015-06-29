
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

OSD::OSD()
{
}

//------------------ init ---------------------------------------------------

void OSD::init(bool new_cfg)
{
  uint8_t offset;

  PORTD |= _BV(PD6);
  pinMode(MAX7456_SELECT,OUTPUT);
  pinMode(MAX7456_VSYNC, INPUT);
  digitalWrite(MAX7456_VSYNC,HIGH); //enabling pull-up resistor

  detectMode();

  digitalWrite(MAX7456_SELECT,LOW);
  //read black level register
  Spi.transfer(MAX7456_OSDBL_reg_read);//black level read register
  byte osdbl_r = Spi.transfer(0xff);
  Spi.transfer(MAX7456_VM0_reg);
  Spi.transfer(MAX7456_RESET | video_mode);
  delay(50);
  //set black level
  byte osdbl_w = (osdbl_r & 0xef); //Set bit 4 to zero 11101111
  Spi.transfer(MAX7456_OSDBL_reg); //black level write register
  Spi.transfer(osdbl_w);

  setBrightness();

  // define sync (auto,int,ext) and
  // making sure the Max7456 is enabled
  control(1);

  if(new_cfg) {
   offset = EEPROM.read(VOFFSET_ADDR);
   PORTD &= ~_BV(PD6);
   Spi.transfer(MAX7456_VOS_reg); // 5 valid bits
   Spi.transfer(offset);
   PORTD |= _BV(PD6);
   offset = EEPROM.read(HOFFSET_ADDR);
   PORTD &= ~_BV(PD6);
   Spi.transfer(MAX7456_HOS_reg); // 6 valid bits
   Spi.transfer(offset);
   PORTD |= _BV(PD6);
  }
  memset(osdbuf, ' ', sizeof(osdbuf)-1);
  osdbuf[sizeof(osdbuf)-1] = MAX7456_END_string;
}

//------------------ Detect Mode (PAL/NTSC) ---------------------------------

void OSD::detectMode()
{
  //read STAT and auto detect Mode PAL/NTSC
  digitalWrite(MAX7456_SELECT,LOW);
  Spi.transfer(MAX7456_STAT_reg_read);//status register
  byte osdstat_r = Spi.transfer(0xff);

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
      digitalWrite(MAX7456_SELECT,LOW);
  }
}

//------------------ Set Brightness  ---------------------------------
void OSD::setBrightness()
{

    uint8_t blevel = EEPROM.read(OSD_BRIGHTNESS_ADDR);
    uint8_t x;

    if(blevel == 0) //low brightness
        blevel = MAX7456_WHITE_level_80;
    else if(blevel == 1) 
        blevel = MAX7456_WHITE_level_90;
    else if(blevel == 2)
        blevel = MAX7456_WHITE_level_100;
    else if(blevel == 3) //high brightness
        blevel = MAX7456_WHITE_level_120;
    else 
        blevel = MAX7456_WHITE_level_80; //low brightness if bad value
    
    // set all rows to same charactor white level, 90%
    for (x = 0x0; x < 0x10; x++)
    {
        Spi.transfer(x + 0x10);
        Spi.transfer(blevel);
    }
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

//------------------ plug ---------------------------------------------------

void OSD::plug()
{
  digitalWrite(MAX7456_SELECT,LOW);
}

//------------------ clear ---------------------------------------------------

void OSD::clear()
{
  // clear the screen
  digitalWrite(MAX7456_SELECT,LOW);
  Spi.transfer(MAX7456_DMM_reg);
  Spi.transfer(MAX7456_CLEAR_display);
  digitalWrite(MAX7456_SELECT,HIGH);
  // FIXME: wait for clear to complete
}

//------------------ set panel -----------------------------------------------

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
OSD::control(uint8_t ctrl){
  digitalWrite(MAX7456_SELECT,LOW);
  Spi.transfer(MAX7456_VM0_reg);
  switch(ctrl){
    case 0:
      Spi.transfer(video_mode); // disable display
      break;
    case 1:
      //Spi.transfer((MAX7456_ENABLE_display_vert | video_mode) | MAX7456_SYNC_internal);
      //Spi.transfer((MAX7456_ENABLE_display_vert | video_mode) | MAX7456_SYNC_external);
      Spi.transfer(MAX7456_ENABLE_display_vert | video_mode | MAX7456_SYNC_autosync); 
      break;
  }
  digitalWrite(MAX7456_SELECT,HIGH);
}

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

void OSD::char_write_start(uint8_t char_n)
{
 write_reg(MAX7456_VM0_reg, 0);
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
 write_reg(MAX7456_VM0_reg, MAX7456_ENABLE_display_vert|MAX7456_SYNC_autosync|video_mode);
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

