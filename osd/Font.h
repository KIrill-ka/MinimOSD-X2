// download font in mcm format
void uploadFont()
{
    uint8_t char_num = 0;
    uint8_t char_byte = 0;
    uint8_t bit_cnt = 0;
    uint8_t char_data = 0;
    char b;

    osd.control(0);
    Serial.printf_P(PSTR("Ready for Font\n"));

    while(1) { 
        b = Serial.read();
        switch(b) {
        case 0x0d: // carridge return, end of line
            if(bit_cnt == 0) break;
            if(char_byte == 0) {
             osd.char_write_start(char_num);
            }
            if(char_byte < 54) osd.char_write_byte(char_byte, char_data);
            char_byte++;
            bit_cnt = 0;
            if(char_byte == 54) osd.char_write_end();
            else if(char_byte == 64) {
             Serial.printf_P(PSTR("Char Done\n"));
             if(char_num == 255) {
              osd.control(1);
              return;
             }
             char_num++;
             char_byte = 0;
            }
            break;
        case 0x0a: // line feed, ignore
            break;
        case '0':
        case '1':
            char_data <<= 1;
            char_data |= b&1;
            bit_cnt++;
            break;
        default:
            break;
        }
    }
}
