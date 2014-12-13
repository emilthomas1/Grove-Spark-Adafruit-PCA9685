/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
  
  Adapted for Spark Core by Paul Kourany, Sept. 15, 2014
  Adapted for Grove Labs by Chad Bean, Nov. 13, 2014
  Adapted for Grove Labs by Louis DeScioli, Dec 12, 2014
 ****************************************************/

#include "I2C_PWM_Driver.h"
#include <math.h>


// Set to true to print some debug messages, or false to disable them.
#define ENABLE_DEBUG_OUTPUT true

/**
 * Initializes the driver with an I2C address. This address must match
 * the address that is set by the physical jumpers on the driver. If the
 * address is not being set by hardware, leave no parameters and it will
 * use the default as specified in the header file
 */
I2C_PWM_Driver::I2C_PWM_Driver(uint8_t addr) {
  _i2caddr = addr;
}

/**
 * [I2C_PWM_Driver::begin description]
 */
void I2C_PWM_Driver::begin(void) {
 Wire.begin();
 reset();
}

/**
 * [I2C_PWM_Driver::reset description]
 */
void I2C_PWM_Driver::reset(void) {
 write8(PCA9685_MODE1, 0x0);
}

/**
 * [I2C_PWM_Driver::setPWMFreq description]
 * @param freq [description]
 */
void I2C_PWM_Driver::setPWMFreq(float freq) {
  if (ENABLE_DEBUG_OUTPUT) {
    Serial.print("Attempting to set freq "); Serial.println(freq);
  }
  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  if (ENABLE_DEBUG_OUTPUT) {
    Serial.print("Estimated pre-scale: "); Serial.println(prescaleval);
  }
  uint8_t prescale = floor(prescaleval + 0.5);
  if (ENABLE_DEBUG_OUTPUT) {
    Serial.print("Final pre-scale: "); Serial.println(prescale);
  }
  
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                          // This is why the beginTransmission below was not working.
  //  Serial.print("Mode now 0x"); Serial.println(read8(PCA9685_MODE1), HEX);
}

/**
 * [I2C_PWM_Driver::setPWM description]
 * @param num [description]
 * @param on  [description]
 * @param off [description]
 */
void I2C_PWM_Driver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
  if (ENABLE_DEBUG_OUTPUT) {
   Serial.print("Setting PWM "); Serial.print(num); Serial.print(": "); Serial.print(on); Serial.print("->"); Serial.println(off);
  }

  Wire.beginTransmission(_i2caddr);
  Wire.write(LED0_ON_L+4*num);
  Wire.write(on);
  Wire.write(on>>8);
  Wire.write(off);
  Wire.write(off>>8);
  Wire.endTransmission();
}

/**
 * [I2C_PWM_Driver::readPWM description]
 * @param  num [description]
 * @return     [description]
 */
uint16_t I2C_PWM_Driver::readPWM(uint8_t num) {
  int toReturn =  (read8(num*4+LED0_OFF_H)<<8);
  toReturn += read8(num*4+LED0_OFF_L);
  return toReturn;
}

/**
 * Sets pin without having to deal with on/off tick placement and properly handles
 * a zero value as completely off.  Optional invert parameter supports inverting
 * the pulse for sinking to ground.  
 * @param num    [description]
 * @param val    Should be from 0 to 4095 inclusive, will be clamped if not within range
 * @param invert [description]
 */
void I2C_PWM_Driver::setPin(uint8_t num, uint16_t val, bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  val = min(val, 4095);
  val = max(0, val);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, 4095-val);
    }
  }
  else {
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
  }
}

/**
 * [I2C_PWM_Driver::read8 description]
 * @param  addr [description]
 * @return      [description]
 */
uint8_t I2C_PWM_Driver::read8(uint8_t addr) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return Wire.read();
}

/**
 * [I2C_PWM_Driver::write8 description]
 * @param addr [description]
 * @param d    [description]
 */
void I2C_PWM_Driver::write8(uint8_t addr, uint8_t d) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(addr);
  Wire.write(d);
  Wire.endTransmission();
}
