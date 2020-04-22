#pragma once

#include <Arduino.h>
#include "output.h"

#include <Wire.h>
//#include <Adafruit_PWMServoDriver.h>

#define PCA9685_I2C_ADDRESS 0x40      /**< Default PCA9685 I2C Slave Address */
#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define PCA9685_MODE2 0x01      /**< Mode Register 2 */
#define MODE1_RESTART 0x80 /**< Restart enabled */
#define PCA9685_LED0_ON_L 0x06  /**< LED0 on tick, low byte*/
#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40  /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80 /**< Restart enabled */
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */

const byte PWM_LED_ORDER[] = {8,9,10,11,1,0,5,4,3,7,6,2};

//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

class PWMOutput : public Output {
  public:
  
    PWMOutput() : _i2c(&Wire) {

    }
    
    void SetPixelColor(int index, uint8_t r,uint8_t g,uint8_t b) override 
    { 
       if (index<12)
          this->values[PWM_LED_ORDER[index]] = (r+g+b)/3;
    }

    boolean Ready(){
      return !this->busy;
    }
    
    void Show() 
    { 
      while(this->busy); //wait for the device to be ready. (is there another way to do this?)
      
      this->busy=true;
      memcpy(this->valuesBuf,this->values,sizeof(this->values));
      xTaskCreate(SendAsync,"SendAsync",10000,this,1,NULL);

      //this->busy=false;
    }
    
    void Begin() override 
    {
      while(this->busy);
      _i2c->begin();
      this->reset();

      //this->setPWMFreq(1500);
      this->setPWMFreq(100);
    }
    
    void Clear() 
    {
      for (int i=0;i<12;i++)
        this->values[i]=0;
    }

    void SetLength(int len) override {

    }
  private: 
    uint8_t values[12];
    uint8_t valuesBuf[12];
    TwoWire *_i2c;


    void reset() {
      write8(PCA9685_MODE1, MODE1_RESTART);
      delay(10);
    }
    
    void write8(uint8_t addr, uint8_t d) {
      _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
      _i2c->write(addr);
      _i2c->write(d);
      _i2c->endTransmission();
    }

    uint8_t read8(uint8_t addr) {
      _i2c->beginTransmission(PCA9685_I2C_ADDRESS);
      _i2c->write(addr);
      _i2c->endTransmission();
    
      _i2c->requestFrom((uint8_t)PCA9685_I2C_ADDRESS, (uint8_t)1);
      return _i2c->read();
    }


    void setPWMFreq(float freq) {
    
      // Range output modulation frequency is dependant on oscillator
      if (freq < 1)
        freq = 1;
      if (freq > 3500)
        freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)
    
      float prescaleval = ((25000000 / (freq * 4096.0)) + 0.5) - 1;
      if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
      if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
      uint8_t prescale = (uint8_t)prescaleval;
    
      uint8_t oldmode = read8(PCA9685_MODE1);
      uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
      write8(PCA9685_MODE1, newmode);                             // go to sleep
      write8(PCA9685_PRESCALE, prescale); // set the prescaler
      write8(PCA9685_MODE1, oldmode);
      delay(5);
      // This sets the MODE1 register to turn on auto increment.
      write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
    
    }


    volatile boolean busy=false;

    static void SendAsync(void* param)
    { //TODO semaphore instead of this busy param
      PWMOutput* this2 = (PWMOutput*) param;
      this2->_i2c->beginTransmission(PCA9685_I2C_ADDRESS);
      this2->_i2c->write(PCA9685_LED0_ON_L);

      int edgepos=0;
      bool cascadedPWm=false;
      bool pcaCascaded = false;

      unsigned int on, off;
      
      for (int i=0;i<12;i++){

        int gammaCorrected = this2->gammaCurve[this2->valuesBuf[i]];
        if (gammaCorrected==4096) {
          //this led is fully on, no pwm
          on = 1<<4;
          off = 0;
        } else if (cascadedPWm) {
          //we start at the off-end of the previous led, to flatten out current
          on = edgepos;
          off = (edgepos+gammaCorrected) % 4096; 
          edgepos = off;
        } else if (pcaCascaded) {
          on = edgepos;
          off = (on+gammaCorrected) % 4096; 
          edgepos += 40;          
        } else {
          //we always start at the start of each pwm cycle, so we are sure to update the value when the led is off
          on = 0;
          off = gammaCorrected; 
        }
        this2->_i2c->write(on);
        this2->_i2c->write(on >> 8);
        this2->_i2c->write(off);
        this2->_i2c->write(off >> 8);
      }
      this2->_i2c->endTransmission();

      this2->busy=false;
      vTaskDelete( NULL );
    }
};