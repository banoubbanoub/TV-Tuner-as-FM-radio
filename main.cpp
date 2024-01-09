#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// I2C device address
#define DEVICE_ADDRESS 0x10

// Reference Divider
#define REFERENCE_DIVIDER 640
// Step Size
#define STEP_SIZE 50000 // 50 kHz

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change the address if needed, and adjust the dimensions

// Rotary Encoder Configuration
Encoder encoder(2, 3); // Adjust the pin numbers based on your setup

// Switch Input Configuration
const int switchPin = 4;  // Adjust the pin number based on your setup
bool switchState = LOW;
bool lastSwitchState = LOW;
unsigned long lastSwitchTime = 0;
unsigned long switchDebounceTime = 50;  // Adjust the debounce time as needed

// Frequency variables for each band with minimum and maximum values
uint32_t minFrequencyBandI = 54.0e6;
uint32_t maxFrequencyBandI = 88.0e6;
uint32_t rfFrequencyBandI =  minFrequencyBandI;  // Starting frequency for Band I (VHF-LO)

uint32_t minFrequencyBandII = 174.0e6;
uint32_t maxFrequencyBandII = 216.0e6;
uint32_t rfFrequencyBandII = minFrequencyBandII;  // Starting frequency for Band II (VHF-HI)

uint32_t minFrequencyBandIII = 470.0e6;
uint32_t maxFrequencyBandIII = 806.0e6;
uint32_t rfFrequencyBandIII = minFrequencyBandIII;  // Starting frequency for Band III (UHF)

uint32_t minFrequencyBandIV = 807.0e6;
uint32_t maxFrequencyBandIV = 1.4e9;
uint32_t rfFrequencyBandIV = minFrequencyBandIV;  // Starting frequency for Band IV

uint8_t dividerByte1, dividerByte2;
uint8_t slaveAddress = 0x20; // Replace with your actual slave address
uint8_t controlByte = 0x3C; // Replace with your actual control byte
uint8_t portControlBandI = 0x01;  // Port control value for Band I
uint8_t portControlBandII = 0x02;  // Port control value for Band II
uint8_t portControlBandIII = 0x03;  // Port control value for Band III
uint8_t portControlBandIV = 0x04;  // Port control value for Band IV

void setDeviceConfiguration(uint8_t slaveAddress, uint8_t dividerByte1, uint8_t dividerByte2, uint8_t controlByte, uint8_t portControl) {
  Wire.beginTransmission(DEVICE_ADDRESS);
  Wire.write(dividerByte1);
  Wire.write(dividerByte2);
  Wire.write(controlByte);
  Wire.write(portControl);
  Wire.endTransmission();
}

void calculateDividerBytes(uint32_t rfFrequency, uint8_t &dividerByte1, uint8_t &dividerByte2) {
  uint16_t dividerValue = rfFrequency / (REFERENCE_DIVIDER * STEP_SIZE);
  dividerByte1 = (dividerValue >> 8) & 0xFF;
  dividerByte2 = dividerValue & 0xFF;
}

void displayConfigurationOnLCD(uint32_t rfFrequency) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RF Frequency:");

  lcd.setCursor(0, 1);
  lcd.print(rfFrequency / 1e6, 1);  // Display frequency in MHz with one decimal place
  lcd.print(" MHz");
}

void switchBand() {
  // Switch to the next band
  if (portControlBandI == 0x01) {
    // Switch from Band I to Band II
    portControlBandI = 0x00;
    portControlBandII = 0x01;
    rfFrequencyBandI = minFrequencyBandII;
    calculateDividerBytes(rfFrequencyBandII, dividerByte1, dividerByte2);
    setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandII);
    displayConfigurationOnLCD(rfFrequencyBandII);
  } else if (portControlBandII == 0x02) {
    // Switch from Band II to Band III
    portControlBandII = 0x00;
    portControlBandIII = 0x01;
    rfFrequencyBandII = minFrequencyBandIII;
    calculateDividerBytes(rfFrequencyBandIII, dividerByte1, dividerByte2);
    setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandIII);
    displayConfigurationOnLCD(rfFrequencyBandIII);
  } else if (portControlBandIII == 0x03) {
    // Switch from Band III to Band IV
    portControlBandIII = 0x00;
    portControlBandIV = 0x01;
    rfFrequencyBandIII = minFrequencyBandIV;
    calculateDividerBytes(rfFrequencyBandIV, dividerByte1, dividerByte2);
    setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandIV);
    displayConfigurationOnLCD(rfFrequencyBandIV);
  } else if (portControlBandIV == 0x04) {
    // Switch from Band IV back to Band I
    portControlBandIV = 0x00;
    portControlBandI = 0x01;
    rfFrequencyBandIV = minFrequencyBandI;
    calculateDividerBytes(rfFrequencyBandI, dividerByte1, dividerByte2);
    setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandI);
    displayConfigurationOnLCD(rfFrequencyBandI);
  }
}



void setup() {
  Wire.begin();
  lcd.begin(16, 2);  // Adjust these values based on your LCD module

  pinMode(switchPin, INPUT_PULLUP);  // Internal pull-up resistor for the switch

  // Set the initial configuration for Band I (VHF-LO)
  calculateDividerBytes(rfFrequencyBandI, dividerByte1, dividerByte2);
  setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandI);

  // Display the initial configuration on the LCD for Band I
  displayConfigurationOnLCD(rfFrequencyBandI);
}

void loop() {
  static long lastEncoded = 0;
  long encoderValue = encoder.read();

  // Read the switch input and debounce
  int reading = digitalRead(switchPin);
  if (reading != lastSwitchState) {
    lastSwitchTime = millis();
  }

  if ((millis() - lastSwitchTime) > switchDebounceTime) {
    if (reading != switchState) {
      switchState = reading;

      // Toggle between bands on switch press
      if (switchState == HIGH) {
        switchBand();
      }
    }
  }

  if (encoderValue != lastEncoded) {
    // Adjust the frequency based on the encoder rotation (for simplicity, using a fixed step)
    if (encoderValue > lastEncoded) {
      if (rfFrequencyBandI + 0.1e6 <= maxFrequencyBandI) {
        rfFrequencyBandI += 0.1e6;  // Increase frequency by 0.1 MHz
      }
    } else {
      if (rfFrequencyBandI - 0.1e6 >= minFrequencyBandI) {
        rfFrequencyBandI -= 0.1e6;  // Decrease frequency by 0.1 MHz
      }
    }

    // Calculate new divider bytes and set the device configuration
    calculateDividerBytes(rfFrequencyBandI, dividerByte1, dividerByte2);
    setDeviceConfiguration(slaveAddress, dividerByte1, dividerByte2, controlByte, portControlBandI);

    // Update the LCD display
    displayConfigurationOnLCD(rfFrequencyBandI);

    lastEncoded = encoderValue;
  }

  // Your other loop code, if needed
}



 

