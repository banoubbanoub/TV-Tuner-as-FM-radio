#include <Arduino.h>
#include<Wire.h>

void TDApoweronreset(){
  //TCA9548A(1);
  Wire.beginTransmission(0x61);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0xC8);
  Wire.write(0x00);
  Wire.endTransmission();
}
void TDATVtuner(uint16_t data,uint8_t osci){
 // TCA9548A(1);
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << 1);          // send byte to select bus
  Wire.endTransmission();
  uint16_t fpd =0;
 Wire.beginTransmission(0x61);
 switch (osci)
 {
 case 1 : // tv osil
 fpd = (data + 57);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0XFF );
  Wire.write(0xc0);
  Wire.write(0x01);
   break;
 case 2: // radio osi
  fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0X00FF);
  Wire.write(0xc0);
  Wire.write(0x01);
  
   break;
case 3: // 915 mhz osil
 fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0X00FF);//fpd & 0x00ff
  Wire.write(0xc0);
  Wire.write(0x00);
   break;
   case 4: //
 fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0X00FF );
  Wire.write(0xc0);
  Wire.write(0x02);
   break;
     case 5:
 fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0x00ff);
  Wire.write(0xc0);
  Wire.write(0x06);
   break;
     case 6:
 fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0x00ff);
  Wire.write(0xc0);
  Wire.write(0x0A);
   break;
 }
 
  Wire.endTransmission();
}