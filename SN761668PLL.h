#include <Arduino.h>
#include<Wire.h>
//#include<I2CSwitch.h>
void SNpoweronreset(){
     //TCA9548A(5);
  Wire.beginTransmission(0x60);
   Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0xC9);
  Wire.write(0x00);
  Wire.write(0x20);
  Wire.endTransmission();
}
void SNTVTuner(byte data ,uint8_t osci){
  // TCA9548A(0);
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << 0);          // send byte to select bus
  Wire.endTransmission();
  uint16_t fpd =0;
  fpd = (data + 107);
  Wire.beginTransmission(0x60);//b01100000
 
  switch (osci)
 {
 case 1 :
   Wire.write(0x28);//fpd&0xff//0b10010000
  Wire.write(data);//fpd>>8//0b00101000
  Wire.write(0x8B);
  Wire.write(0x38);
  Wire.write(0xE3);
   break;
 case 2:
  Wire.write(0x27);
   Wire.write(data);
  Wire.write(0x8B);
  Wire.write(0x38);
   Wire.write(0xE3);

   break;
case 3:
  Wire.write(0x07);
  Wire.write(data);
  Wire.write(0x8B);
  Wire.write(0x31);
   Wire.write(0xE3);
   break;
   case 4:
  fpd = (data + 107);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0x00ff);
  Wire.write(0x8B);
  Wire.write(0x31);
  Wire.write(0xE3);
   break;
    case 5:
  fpd = (data + 107);
  Wire.write(fpd >> 8);
   Wire.write(fpd & 0x00ff);
  Wire.write(0x8B);
  Wire.write(0x32);
   Wire.write(0xE3);
   break;
      case 6:
  fpd = (data + 107);
  Wire.write(fpd >> 8 );
   Wire.write(fpd & 0x00ff );
  Wire.write(0x8B);
  Wire.write(0x38);
   Wire.write(0xE3);
   break;
   
 }
 Wire.endTransmission();
}