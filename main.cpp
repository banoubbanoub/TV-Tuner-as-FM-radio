#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
int RECV_PIN = 5;  // IR Receiver pin
IRrecv irrecv(RECV_PIN);
decode_results results;
LiquidCrystal_I2C  lcd(0x27, 16, 2);
#define clk 4
#define dta 3
#define sw 5
#define s0 8
#define s1 9
#define en 11
#define cnt1 6
#define cnt2 7
int cureentclock =0;
int previceClock =0;
int TVIF = 57;
int FMradioIF = 107;
int FM915IF = 26;
int CBIF = 27;
uint16_t CBFraquency = 1500;
uint16_t FM915Frequency = 17465;
uint16_t TVFrequency = 2100;
uint16_t FMFrequency = 2400;
uint16_t FrequencyVariable=0;
int tvSelect=0;
int chSelect =0;

void videoSwitchOutput(int p){
switch (p)
{
case 6:
digitalWrite(s0,HIGH);
digitalWrite(s1,LOW);
digitalWrite(en,LOW);
  break;
  case 7:
digitalWrite(s0,LOW);
digitalWrite(s1,LOW);
digitalWrite(en,LOW);
  break;
case 8:
digitalWrite(s0,HIGH);
digitalWrite(s1,HIGH);
digitalWrite(en,LOW);
  break;
}

}
// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

void videoProcessors(){
  TCA9548A(3);
  Wire.beginTransmission(0B01001101);//b01001101
 
  Wire.write(0x00);
  Wire.write(0x06);
  Wire.endTransmission();
 Wire.beginTransmission(0B01001101);//b01001101
 Wire.write(0x01);
  Wire.write(0xA4);
  Wire.endTransmission();
Wire.beginTransmission(0B01001101);//b01001101
  Wire.write(0x02);
  Wire.write(0xC3f);
    Wire.beginTransmission(0B01001101);//b01001101
    Wire.endTransmission();
  Wire.write(0x03);
  Wire.write(0x83f);
  Wire.endTransmission();
    Wire.beginTransmission(0B01001101);//b01001101
  Wire.write(0x04);
  Wire.write(0x01);
  Wire.endTransmission();
    Wire.beginTransmission(0B01001101);//b01001101
  Wire.write(0x05);
  Wire.write(0xFF);
  Wire.endTransmission();
 Wire.beginTransmission(0B01001101);//b01001101
  Wire.write(0x06);
Wire.write(0x3D);
Wire.endTransmission();
  Wire.beginTransmission(0B01001101);//b01001101
Wire.write(0x07);
Wire.write(0x00);
Wire.endTransmission();
Wire.beginTransmission(0B01001101);//b01001101
Wire.write(0x08);
Wire.write(0x00);
Wire.endTransmission();

 Wire.beginTransmission(0B01001101);//b01001101
  Wire.write(0x09);
  switch (chSelect)
  {
  case 6:
    Wire.write(0x01);
    break;
  case 7:
  Wire.write(0x03);
  break;
  case 8:
  Wire.write(0x00);
  break;
  }
  
 // Wire.write(0x0A);
  //Wire.write(0x01);
 // Wire.write(0x0D);
 // Wire.write(0x11);
  Wire.endTransmission();
}

void TVtuner(uint16_t data,int source){
  uint16_t fpd =0;
  //Wire.beginTransmission(0x61); //TDA6508
  TCA9548A(tvSelect);
 Wire.beginTransmission(0x61);//UTA6039
 switch (source)
 {
 case 1 :
 fpd = (data + TVIF);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0xff);
  Wire.write(0xc0);
  Wire.write(0x01);
  Wire.write(0x00);
   break;
 case 2:
  fpd = (data + FMradioIF);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0xff);
  Wire.write(0xc0);
  Wire.write(0x01);
   Wire.write(0x00);

   break;
case 3:
 fpd = (data + FM915IF);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0xff);
  Wire.write(0xc0);
  Wire.write(0x00);
  // Wire.write(0x00);
   break;
   case 4:
 fpd = (data + CBIF);
  Wire.write(fpd >> 8);
  Wire.write(fpd & 0xff);
  Wire.write(0xc0);
  Wire.write(0x0A);
   Wire.write(0x00);
   break;
 }
 
  Wire.endTransmission();
}
void videoSwitch(){
    TCA9548A(4);
Wire.beginTransmission(0x48);
Wire.write(0x00);
Wire.write(0x00);
Wire.write(0x0c);
Wire.write(0x00);
Wire.endTransmission();
}
void AudioSelector(int p){
  switch (p)
  {
  case 1:
    digitalWrite(cnt1,HIGH);
    digitalWrite(cnt2,LOW);
    break;
    case 2:
    digitalWrite(cnt1,HIGH);
    digitalWrite(cnt2,HIGH);
    break;
  }
}
void DTVTuner(uint16_t data){
  TCA9548A(5);
 uint16_t fpd =0;
 fpd = (data   );
  Wire.beginTransmission(0B01100000);//b01001101
   Wire.write(0x27);//fpd&0xff//0b10010000
  Wire.write(0x10);//fpd>>8//0b00101000
  Wire.write(0x8B);
  Wire.write(0x38);
  Wire.write(0xe3);
 Wire.endTransmission();
}
void setup() {
  // put your setup code here, to run once:
    irrecv.enableIRIn();
    Wire.begin();
Serial.begin(9600);
  lcd.init(); 
   lcd.begin(16,2);
  lcd.backlight();
  pinMode(clk,INPUT);
  pinMode(dta,INPUT);
  pinMode(sw,INPUT);
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
  pinMode(en,OUTPUT);
 pinMode(cnt1,OUTPUT);
 pinMode(cnt2,OUTPUT);
  previceClock = digitalRead(clk);
  videoSwitch();
videoProcessors();
lcd.setCursor(0,0);
lcd.print("DTV Tuner");
//AudioSelector(2);
}
int count=0;

void loop() {
  // put your main code here, to run repeatedly:
   cureentclock = digitalRead(clk);
  
  if(cureentclock != previceClock){
   if(cureentclock != digitalRead(dta)){
    FrequencyVariable--;
     TVtuner(FrequencyVariable,count);
      DTVTuner(FrequencyVariable);
    switch (count)
    {
     
    case 1:
      Serial.print("TV: ");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TV:");
      break;
     case 2:
      Serial.print("FM: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FM:");
      break;
       case 3:
      Serial.print("FMVH: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FMVH:");
      break;
       case 4:
      Serial.print("CB: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CB:");
      break;
   
    }
    Serial.println(FrequencyVariable);
     lcd.setCursor(3,0);
    lcd.print(FrequencyVariable);
    }
   else{
    FrequencyVariable++;
      TVtuner(FrequencyVariable,count);
       switch (count)
    {
   
    case 1:
      Serial.print("TV: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TV:");
      break;
     case 2:
      Serial.print("FM: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FM:");
      break;
       case 3:
      Serial.print("FM915: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FMVH:");
      break;
       case 4:
      Serial.print("CB: ");
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CB:");
      break;
   
    }
    lcd.setCursor(3,0);
    lcd.print(FrequencyVariable);
      Serial.println(FrequencyVariable);
    }
  }
  previceClock = cureentclock;
  if(irrecv.decode(&results)){
   //Serial.println(results.value);
   switch(results.value){
     case 3772784863:
     FrequencyVariable = TVFrequency;
     count =1;
     break;
     case 3772817503:
     FrequencyVariable = FMFrequency;
     count =2;
     break;
     case 3772801183:
     FrequencyVariable = FM915Frequency;
     count =3;
     break;
   case 3772780783:
   FrequencyVariable = CBFraquency;
   count =4;
   break;
   case 3772790473:
    AudioSelector(1);
   tvSelect =1;
   lcd.setCursor(0,1);
   lcd.print("TV 1");
   break;
   case 3772819543:
    AudioSelector(2);
   tvSelect =2;
    lcd.setCursor(0,1);
   lcd.print("TV 2");
   break;
   case 3772797103:
   chSelect =6;
   videoProcessors();
   AudioSelector(1);
   break; 
   case 3772788943:
   chSelect =7;
   videoProcessors();
   AudioSelector(2);
   break;
   case 3772821583:
   chSelect =8;
   videoProcessors();
   break;
   case 3772778233:
   AudioSelector(1);
   break;
   case 3772810873:
   AudioSelector(2);
   break;
   }
   videoSwitchOutput(chSelect);
  }
  irrecv.resume();
}