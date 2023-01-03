#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"  //touch sensor library

Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t touch_data = 0;  //use for touch sensor library
const int LDAC = 8;                       //spi LDAC pin number
bool touch[4] = {0, 0, 0, 0}; //detect touching
long touch_chat[4] = {0, 0, 0, 0}; //chattering countermeasure
long burst_timer[4] = {0, 0, 0, 0}; //use for burst mode
bool old_touch[4] = {0, 0, 0, 0};
bool gate[4] = {0, 0, 0, 0};
int CV[5] = {0, 0, 0, 0, 0};
byte mode = 0; //0=Momentary,1=Alternate,2=burst,3=CV setting
int CVmode_count = 0;//CV mode enter count
long CVmode_timer = 0;//CV mode enter count
byte CVmode = 0; //0=quantize set,1=A set,2=B set,3=C set,4=D set
long LED_timer = 0;
long timer;
int LED_LFO = 0;
bool quant = 0;//0= no quantize , 1 = pre quantize
bool output_mode = 0;//gate out impedance , 0=high impedance low out, 1=low impedance low out

const long cv_qnt[61] = {//10bit pre quantizer table
 0,  17, 34, 51, 68, 85, 102,  119,  136,  153,  170,  187,  204,  221,  239,  256,
 273,  290,  307,  324,  341,  358,  375,  392,  409,  426,  443,  460,  477,  495,  512,
 529,  546,  563,  580,  597,  614,  631,  648,  665,  682,  699,  716,  733,  751,  768,
 785,  802,  819,  836,  853,  870,  887,  904,  921,  938,  955,  972,  989,  1007, 1023
};

void setup() {
 pinMode(LDAC, OUTPUT) ;//spi
 pinMode(SS, OUTPUT) ;//spi
 pinMode(9, OUTPUT) ;//gateA
 pinMode(7, OUTPUT) ;//gateB
 pinMode(6, OUTPUT) ;//gateC
 pinMode(3, OUTPUT) ;//gateD
 pinMode(5, OUTPUT) ;//LED
 pinMode(2, INPUT_PULLUP) ;//toggle SW
 pinMode(4, INPUT_PULLUP) ;//toggle SW
 cap.begin(0x5A);//start mpr121
 timer = millis();//
 EEPROM.get(0x00, CV);

 SPI.begin();
 SPI.setBitOrder(MSBFIRST) ;
 SPI.setClockDivider(SPI_CLOCK_DIV8) ;
 SPI.setDataMode(SPI_MODE0) ;
 delay(50);
}

void loop() {
 for (int i = 0; i < 4; i++) {
   old_touch[i] =  touch[i];
 }

 if (mode != 3) {
   mode_select();  //read toggle SW

   if (analogRead(0) <= 512) {
     output_mode = 0;
   }
   else if (analogRead(0) > 512) {
     output_mode = 1;
   }
 }

 if (CV[0] <= 512) {
   quant = 0;
 }
 else if (CV[0] > 512) {
   quant = 1;
 }

 //  ---------------------------read touch sensor------------------------------------------
 touch_data = cap.touched();
 for (int i = 0; i < 4; i++) {
   if (mode != 2) {
     if (touch_chat[i] + 30 <= millis()) {//chattering countermeasure , 30msec interval
       touch[i] =  bitRead(touch_data, i);
       touch_chat[i] = millis();
     }
   }
   else if (mode == 2) {//burst interval
     touch[i] =  bitRead(touch_data, i);
   }
 }

 //  ---------------------------mode=0 Momentary------------------------------------------
 if (mode == 0) {
   for (int i = 0; i < 4; i++) {
     if (old_touch[i] == 0 &&  touch[i] == 1) {
       DACout(CV[i + 1]);
     }
   }
   trig_out();
 }

 //  ---------------------------mode=1 Alternate------------------------------------------
 if (mode == 1) {
   for (int i = 0; i < 4; i++) {
     if (old_touch[i] == 0 &&  touch[i] == 1) {
       DACout(CV[i + 1]);
     }
   }
   for (int i = 0; i < 4; i++) {
     if (old_touch[i] == 0 &&  touch[i] == 1) {
       gate[i] = !gate[i];
     }
   }

   if (gate[0] == 1) {
     pinMode(9, OUTPUT) ;
     digitalWrite(9, HIGH);
   }
   else if (gate[0] == 0) {
     if (output_mode == 0) {
       pinMode(9, INPUT) ;
     }
     else if (output_mode == 1) {
       digitalWrite(9, LOW);
     }
   }
   if (gate[1] == 1) {
     pinMode(7, OUTPUT) ;
     digitalWrite(7, HIGH);
   }
   else if (gate[1] == 0) {
     if (output_mode == 0) {
       pinMode(7, INPUT) ;
     }
     else if (output_mode == 1) {
       digitalWrite(7, LOW);
     }
   }
   if (gate[2] == 1) {
     pinMode(6, OUTPUT) ;
     digitalWrite(6, HIGH);
   }
   else if (gate[2] == 0) {
     if (output_mode == 0) {
       pinMode(6, INPUT) ;
     }
     else if (output_mode == 1) {
       digitalWrite(6, LOW);
     }
   }
   if (gate[3] == 1) {
     pinMode(3, OUTPUT) ;
     digitalWrite(3, HIGH);
   }
   else if (gate[3] == 0) {
     if (output_mode == 0) {
       pinMode(3, INPUT) ;
     }
     else if (output_mode == 1) {
       digitalWrite(3, LOW);
     }
   }
 }

 //  ---------------------------mode=2 burst------------------------------------------
 if (mode == 2) {
   for (int i = 0; i < 4; i++) {
     if (old_touch[i] == 0 &&  touch[i] == 1) {
       DACout(CV[i + 1]);
     }
   }

   for (int i = 0; i < 5; i++) {
     if (burst_timer[i] + CV[i + 1] / 3 + 3  <= millis()) {
       touch[i] = 0;
       burst_timer[i] = millis();
     }
   }
   trig_out();
 }

 //  ---------------------------CV setting mode------------------------------------------

 if (mode != 3) {
   if (touch_data == 15 && CVmode_timer + 100 <= millis()) {
     CVmode_count++;
     CVmode_timer = millis();
   }
   else if (touch_data = !15) {
     CVmode_count = 0;
     CVmode_timer = millis();
   }

   if (CVmode_count >= 10) {
     mode = 3;
     LED_timer = millis();
     digitalWrite(9, LOW);//all gate off
     digitalWrite(7, LOW);
     digitalWrite(6, LOW);
     digitalWrite(3, LOW);
     LEDflash(3);//flash 3times
     delay(500);
   }
 }

 if (mode == 3) {
   switch (CVmode) {
     case 0:
       CV[CVmode] = analogRead(0);
       if (old_touch[0] == 0 && touch[0] == 1) {
         CVmode++;
         LEDflash(CVmode + 1);
       }
       break;

     case 1:
       CV[CVmode] = analogRead(0);
       DACout(CV[CVmode]);
       if (old_touch[0] == 0 && touch[0] == 1) {
         CVmode++;
         LEDflash(CVmode + 1);
       }
       break;

     case 2:
       CV[CVmode] = analogRead(0);
       DACout(CV[CVmode]);
       if (old_touch[0] == 0 && touch[0] == 1) {
         CVmode++;
         LEDflash(CVmode + 1);
       }
       break;

     case 3:
       CV[CVmode] = analogRead(0);
       DACout(CV[CVmode]);
       if (old_touch[0] == 0 && touch[0] == 1) {
         CVmode++;
         LEDflash(CVmode + 1);
       }
       break;

     case 4:
       CV[CVmode] = analogRead(0);
       DACout(CV[CVmode]);
       if (old_touch[0] == 0 && touch[0] == 1) {
         CVmode++;
         LEDflash(CVmode + 1);
         CVmode = 0;
         mode_select();
         CVmode_count = 0;
         eeprom(); //save setting data
       }
       break;
   }
 }

 //  ---------------------------LED turn on/off------------------------------------------

 if (mode != 3) {
   if (touch[0] == 1 || touch[1] == 1 || touch[2] == 1 || touch[3] == 1) {
     LED(touch[0] * 63 + touch[1] * 63 + touch[2] * 63 + touch[3] * 63);
   }
   else if (touch[0] == 0 && touch[1] == 0 && touch[2] == 0 && touch[3] == 0) {
     LED(0) ;
   }
 }

 else if (mode == 3) {
   if (LED_timer + 1 <= millis()) {
     LED_LFO = LED_LFO+1;
     if (LED_LFO >= 255) {
       LED_LFO = 0;
       if (CVmode != 0) {//gate on for confirm CV setting output
         touch[CVmode - 1] = 1;
         trig_out();
         delay(10);
         touch[CVmode - 1] = 0;
         trig_out();
       }
     }
     LED(LED_LFO);
     LED_timer = millis();
   }
 }
}

void mode_select() {
 if (digitalRead(2) == 0 && digitalRead(4) == 1) {
   mode = 2;
 }
 if (digitalRead(2) == 1 && digitalRead(4) == 1) {
   mode = 1;
 }
 if (digitalRead(2) == 1 && digitalRead(4) == 0) {
   mode = 0;
 }
}

void LEDflash(byte times) {
 for (int i = 0; i < times; i++) {
   LED(0);
   delay(50);
   LED(255);
   delay(50);
 }
}

void trig_out() {
 if (touch[0] == 1) {
   pinMode(9, OUTPUT) ;
   digitalWrite(9, HIGH);
 }
 else if (touch[0] == 0) {
   if (output_mode == 0) {
     pinMode(9, INPUT) ;
   }
   else if (output_mode == 1) {
     digitalWrite(9, LOW);
   }
 }
 if (touch[1] == 1) {
   pinMode(7, OUTPUT) ;
   digitalWrite(7, HIGH);
 }
 else if (touch[1] == 0) {
   if (output_mode == 0) {
     pinMode(7, INPUT) ;
   }
   else if (output_mode == 1) {
     digitalWrite(7, LOW);
   }
 }
 if (touch[2] == 1) {
   pinMode(6, OUTPUT) ;
   digitalWrite(6, HIGH);
 }
 else if (touch[2] == 0) {
   if (output_mode == 0) {
     pinMode(6, INPUT) ;
   }
   else if (output_mode == 1) {
     digitalWrite(6, LOW);
   }
 }
 if (touch[3] == 1) {
   pinMode(3, OUTPUT) ;
   digitalWrite(3, HIGH);
 }
 else if (touch[3] == 0) {
   if (output_mode == 0) {
     pinMode(3, INPUT) ;
   }
   else if (output_mode == 1) {
     digitalWrite(3, LOW);
   }
 }
}

void DACout(int dat) {
 if (quant == 1) {
   dat = cv_qnt[map(dat, 0, 1023, 0, 60)];//quantize
 }
 digitalWrite(LDAC, HIGH) ;//spi communication
 digitalWrite(SS, LOW) ;
 SPI.transfer((dat >> 6) | 0x30) ;
 SPI.transfer((dat << 2) & 0xff) ;
 digitalWrite(SS, HIGH) ;
 digitalWrite(LDAC, LOW) ;
}

void LED(byte R) {
 analogWrite(5, R);//PWM output
}

void eeprom() {
 EEPROM.put(0x00, CV);//save setting data
}