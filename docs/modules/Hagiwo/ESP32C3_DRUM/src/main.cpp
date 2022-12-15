//https://note.com/solder_state/n/n950ed088c3cb

#define  ENCODER_OPTIMIZE_INTERRUPTS //rotary encoder
#include "sample.h"//sample file
#include <Encoder.h>//rotary encoder
#include <EEPROM.h>

Encoder myEnc(D10, D9);//rotary encoder
float oldPosition  = -999;//rotary encoder
float newPosition = -999;//rotary encoder

float i; //sample play progress
float freq = 1;//sample frequency
bool trig1, old_trig1, done_trig1;
int sound_out;//sound out PWM rate
byte sample_no = 1;//select sample number

long timer = 0;//timer count for eeprom write
bool eeprom_write = 0; //0=no write,1=write

//-------------------------timer interrupt for sound----------------------------------
hw_timer_t *timer0 = NULL;
portMUX_TYPE timerMux0 = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t ledstat = 0;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux0) ; // enter critical range
  if (done_trig1 == 1) {//when trigger in
    i = i + freq;
    if (i >= 28800) {//when sample playd all ,28800 = 48KHz sampling * 0.6sec 
      i = 0;
      done_trig1 = 0;
    }
  }
  sound_out = (((pgm_read_byte(&(smpl[sample_no][(int)i * 2]))) | (pgm_read_byte(&(smpl[sample_no][(int)i * 2 + 1]))) << 8) >> 6) ;//16bit to 10bit
  ledcWrite(1, sound_out+ 511); //PWM output
  portEXIT_CRITICAL_ISR(&timerMux0) ; // exit critical range
}

void setup() {
  EEPROM.begin(1);  //1byte memory space
  EEPROM.get(0, sample_no);//callback saved sample number
  sample_no++;//countermeasure rotary encoder error
  if (sample_no >= 48) {//countermeasure rotary encoder error
    sample_no = 0;
  }

  pinMode(D7, INPUT); //trigger in
  pinMode(D9, INPUT_PULLUP); //rotary encoder
  pinMode(D10, INPUT_PULLUP); //rotary encoder
  pinMode(D5, OUTPUT);//sound_out PWM
  timer = millis();//for eeprom write
  analogReadResolution(10);

  ledcSetup(1, 39000, 10);//PWM frequency and resolution
  ledcAttachPin(D5, 1);//(LED_PIN, LEDC_CHANNEL_0);//timer ch1 , apply D5 output

  timer0 = timerBegin(0, 1666, true);  // timer0, 12.5ns*1666 = 20.83usec(48kHz), count-up
  timerAttachInterrupt(timer0, &onTimer, true); // edge-triggered
  timerAlarmWrite(timer0, 1, true); // 1*20.83usec = 20.83usec, auto-reload
  timerAlarmEnable(timer0); // enable timer0
}

void loop() {
    //-------------------------trigger----------------------------------
  old_trig1 = trig1;
  trig1 = digitalRead(D7);
  if (trig1  == 1 && old_trig1 == 0 ) { //detect trigger signal low to high , before sample play was done
    done_trig1 = 1;
    i = 0;
  }
  
    //-------------------------pitch setting----------------------------------
  freq = analogRead(A3) * 0.002 + analogRead(A0) *  0.002;
  
  //-------------------------sample change----------------------------------
  newPosition = myEnc.read();
  if ( (newPosition - 3) / 4  > oldPosition / 4) { 
    oldPosition = newPosition;
    sample_no = sample_no - 1;
    if (sample_no < 0 || sample_no > 200) {//>200 is overflow countermeasure
      sample_no = 47;
    }
    done_trig1 = 1;//1 shot play when sample changed
    i = 0;
    timer = millis();
    eeprom_write = 1;//eeprom update flug on
  }

  else if ( (newPosition + 3) / 4  < oldPosition / 4 ) { 
    oldPosition = newPosition;
    sample_no = sample_no + 1;
    if (sample_no >= 48) {
      sample_no = 0;
    }
    done_trig1 = 1;//1 shot play when sample changed
    i = 0;
    timer = millis();
    eeprom_write = 1;//eeprom update flug on
  }

  //-------------------------save to eeprom----------------------------------
  if (timer + 5000 <= millis() && eeprom_write == 1) {//Memorized 5 seconds after sample number change
    eeprom_write = 0;
    eeprom_update();
  }
}

void eeprom_update() {
  EEPROM.put(0, sample_no);
  EEPROM.commit();
}