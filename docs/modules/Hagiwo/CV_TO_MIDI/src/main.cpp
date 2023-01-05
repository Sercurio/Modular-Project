#include <EEPROM.h>
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#include <Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#define OLED_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//rotary encoder setting
#define  ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
Encoder myEnc(6, 3);//rotary encoder pin
int oldPosition  = -999;
int newPosition = -999;
int i = 0;
bool old_sw, sw;
int select = 0; //config select
bool select_set = 0;

byte set_ntch[6] = {1, 1, 1, 1, 2, 2};//midi ch
byte set_ntofs[6] = {24, 24, 24, 24, 24, 24};//offset notes
byte set_ntchd[6] = {0, 0, 0, 0, 0, 0}; //0=no chord,1=maj chord,2=minor chord
byte set_ccch[6] = {1, 1, 1, 1, 2, 2};//midi ch
byte set_cccc[6] = {74, 71, 93, 94, 13, 16};//midi control change
byte set_ccmax[6] = {127, 127, 127, 127, 127, 127};// CC max value
byte set_prcch[6] = {10, 10, 10, 10, 10, 10};//midi ch
byte set_prcnt[6] = {36, 38, 42, 44, 39, 45}; //BD,SD,CH,OH,HC,TM
byte set_prcvl[6] = {127, 127, 127, 127, 127, 127}; //volume

//CVsetting
int CV1, CV2, CV3, CV4, CV5, CV6;
int old_CV1, old_CV2, old_CV3, old_CV4, old_CV5, old_CV6;
bool note_on1, note_on2, note_on3, note_on4, note_on5, note_on6;
bool note_off1, note_off2, note_off3, note_off4, note_off5, note_off6;
int note_out1, note_out2, note_out3, note_out4, note_out5, note_out6;

int qnt[62] = {//detect touching key number
 0, 9,  26, 43, 60, 77, 94, 111,  128,  145,  162,  179,  196,  213,  230,  247,  264,  281,  298,  315,  332,  349,  366,  383,  400,  417,  434,  451,  468,  485,  502,  519,  536,  553,  570,  587,  604,  621,  638,  655,  672,  689,  706,  723,  740,  757,  774,  791,  808,  825,  842,  859,  876,  893,  910,  927,  944,  961,  978,  995,  1012, 1024
};
byte chord_maj[12][4] = {
 {0, 4, 7, 11},
 {0, 4, 7, 0},
 {2, 5, 9, 0},
 {2, 5, 9, 2},
 {4, 7, 11, 2},
 {0, 5, 9.4},
 {0, 5, 9.0},
 {2, 5, 7, 11},
 {2, 7, 7, 11},
 {4, 7, 9, 12},
 {4, 7, 9, 11},
 {0, 2, 4, 7}
};
byte chord_min[12][4] = {
 {0, 3, 7, 10},
 {0, 3, 7, 0},
 {2, 5, 8, 0},
 {2, 5, 8, 2},
 {3, 7, 10, 2},
 {0, 5, 8.4},
 {0, 5, 8.0},
 {2, 5, 7, 10},
 {2, 7, 7, 10},
 {3, 7, 8, 12},
 {3, 8, 8, 12},
 {0, 2, 3, 7}
};
int note_no1 = 0;
int note_no2 = 0;
int note_no3 = 0;
int k = 0;
int cmp1, cmp2;
byte mode = 5; //1=1NOTE4CC,2=2NOTE2CC,3=3NOTE,4=6CC,5=PRC
byte set_ch = 0;//0=no setting , 1=CH1 setting ,2=CH2 ...
bool disp_lat = 0;
long timer = 0;

void setup() {
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 display.setRotation(3);//dispray rotation 90deg
 display.clearDisplay();

 MIDI.begin();
 pinMode(10, INPUT_PULLUP);//push sw
 timer = millis();//Send MIDI CC regularly

 //read eeprom store data
 EEPROM.get(0x00, set_ntch);
 EEPROM.get(0x10, set_ntofs);
 EEPROM.get(0x20, set_ntchd);
 EEPROM.get(0x30, set_ccch);
 EEPROM.get(0x40, set_cccc);
 EEPROM.get(0x50, set_ccmax);
 EEPROM.get(0x60, set_prcch);
 EEPROM.get(0x70, set_prcnt);
 EEPROM.get(0x80, set_prcvl);
}

void loop() {
 old_sw = sw;
 old_CV1 = CV1;
 old_CV2 = CV2;
 old_CV3 = CV3;
 old_CV4 = CV4;
 old_CV5 = CV5;
 old_CV6 = CV6;

 //-------------------------------rotary encoder------------------------------------
 newPosition = myEnc.read();
 if ( (newPosition - 3) / 4  > oldPosition / 4) {
   all_notes_off();
   disp_lat = 1;//Update OLED only once when manipulated
   oldPosition = newPosition;
   if (set_ch == 0) {
     i = i - 1;
     if ( i <= -1) {
       i = 7;
     }
   }
   else if (set_ch >= 1) {
     switch (select_set) {
       case 0:
         select = select - 1;
         if ( select <= -1) {
           select = 9;
         }
         break;

       case 1:
         switch (select) {
           case 1:
             if (set_ntch[set_ch - 1] > 1) {
               set_ntch[set_ch - 1]--;
             }
             break;

           case 2:
             if (set_ntofs[set_ch - 1] > 0) {
               set_ntofs[set_ch - 1]--;
             }
             break;

           case 3:
             if (set_ntchd[set_ch - 1] > 0) {
               set_ntchd[set_ch - 1]--;
             }
             break;

           case 4:
             if (set_ccch[set_ch - 1] > 1) {
               set_ccch[set_ch - 1]--;
             }
             break;

           case 5:
             if (set_cccc[set_ch - 1] > 0) {
               set_cccc[set_ch - 1]--;
             }
             break;

           case 6:
             if (set_ccmax[set_ch - 1] > 0) {
               set_ccmax[set_ch - 1]--;
             }
             break;

           case 7:
             if (set_prcch[set_ch - 1] > 0) {
               set_prcch[set_ch - 1]--;
             }
             break;

           case 8:
             if (set_prcnt[set_ch - 1] > 0) {
               set_prcnt[set_ch - 1]--;
             }
             break;

           case 9:
             if (set_prcvl[set_ch - 1] > 0) {
               set_prcvl[set_ch - 1]--;
             }
             break;
         }
         break;
     }
   }
 }

 else if ( (newPosition + 3) / 4  < oldPosition / 4 ) {
   all_notes_off();
   disp_lat = 1;//Update OLED only once when manipulated
   oldPosition = newPosition;
   if (set_ch == 0) {
     i = i + 1;
     if ( i >= 8) {
       i = 0;
     }
   }

   else if (set_ch >= 1) {
     switch (select_set) {
       case 0:
         select = select + 1;
         if ( select >= 10) {
           select = 0;
         }
         break;

       case 1:
         switch (select) {
           case 1:
             if (set_ntch[set_ch - 1] < 10) {
               set_ntch[set_ch - 1]++;
             }
             break;

           case 2:
             if (set_ntofs[set_ch - 1] < 48) {
               set_ntofs[set_ch - 1]++;
             }
             break;

           case 3:
             if (set_ntchd[set_ch - 1] < 2) {
               set_ntchd[set_ch - 1]++;
               all_notes_off();
             }
             break;

           case 4:
             if (set_ccch[set_ch - 1] < 10) {
               set_ccch[set_ch - 1]++;
             }
             break;

           case 5:
             if (set_cccc[set_ch - 1] < 127) {
               set_cccc[set_ch - 1]++;
             }
             break;

           case 6:
             if (set_ccmax[set_ch - 1] < 127) {
               set_ccmax[set_ch - 1]++;
             }
             break;

           case 7:
             if (set_prcch[set_ch - 1] < 11) {
               set_prcch[set_ch - 1]++;
             }
             break;

           case 8:
             if (set_prcnt[set_ch - 1] < 127) {
               set_prcnt[set_ch - 1]++;
             }
             break;

           case 9:
             if (set_prcvl[set_ch - 1] < 127) {
               set_prcvl[set_ch - 1]++;
             }
             break;
         }
         break;
     }
   }
 }
 mode = constrain(i, 1, 5);
 select = constrain(select, 0, 9);

 //-------------------------------push SW------------------------------------

 sw = digitalRead(10);

 if (old_sw == 1 && sw == 0) {
   disp_lat = 1;//Update OLED only once when manipulated
   all_notes_off();//If do not turn off all notes, the sound will continue to sound when switching.

   if (select == 0) {
     set_ch++;
     all_notes_off();
     eeprom();//save setting data to eeprom
     if (set_ch >= 7) {
       set_ch = 0;
     }
   }
   else if (select >= 1) {
     select_set = !select_set;
   }
 }

 //-------------------------------ADC and MIDI OUT------------------------------------
 switch (mode) {
   case 1:
     CV12note();
     MIDI12note();
     if (timer + 10 <= millis()) {//CC transmission is every 10msec
       timer = millis();
       CV34CC();
       CV56CC();
       MIDI34CC();
       MIDI56CC();
     }
     break;

   case 2:
     CV12note();
     CV34note();
     MIDI12note();
     MIDI34note();
     if (timer + 10 <= millis()) {//CC transmission is every 10msec
       timer = millis();
       CV56CC();
       MIDI56CC();
     }
     break;

   case 3:
     CV12note();
     CV34note();
     CV56note();
     MIDI12note();
     MIDI34note();
     MIDI56note();
     break;

   case 4:
     if (timer + 10 <= millis()) {//CC transmission is every 10msec
       timer = millis();
       CV12CC();
       CV34CC();
       CV56CC();
       MIDI12CC();
       MIDI34CC();
       MIDI56CC();
     }
     break;

   case 5:
     PRC();
     MIDIPRC();
     break;
 }

 if (disp_lat == 1) {
   disp_lat = 0;
   if (set_ch == 0) {
     display_out();//display reflesh
   }
   else if (set_ch >= 1) {
     display_set();//display reflesh
   }
 }
}

//-------------------------------Read CV------------------------------------
void CV12note() {
 CV1 = analogRead(0);

 //V/oct quantize
 for (int j = 0; j < 61; j++) {//Detects touched keyboard note
   if ( CV1 >= qnt[j] && CV1 < qnt[j + 1]) {
     cmp1 = CV1 - qnt[j];//Detect closest note
     cmp2 = qnt[j + 1] - CV1; //Detect closest note
     k = j - 1;
     break;
   }
 }

 if (cmp1 >= cmp2) {//Detect closest note
   note_no1 = k + 1;
 }
 else if (cmp2 > cmp1) {//Detect closest note
   note_no1 = k;
 }

 //Gate detect
 CV2 = analogRead(1);
 if (old_CV2 <= 64 && CV2 > 64) {
   note_on1 = 1;
 }
 else if (old_CV2 >= 64 && CV2 < 64) {
   note_off1 = 1;
 }
}

void CV34note() {
 CV3 = analogRead(2);

 //V/oct quantize
 for (int j = 0; j < 61; j++) {//Detects touched keyboard note
   if ( CV3 >= qnt[j] && CV3 < qnt[j + 1]) {
     cmp1 = CV3 - qnt[j];//Detect closest note
     cmp2 = qnt[j + 1] - CV3; //Detect closest note
     k = j - 1;
     break;
   }
 }

 if (cmp1 >= cmp2) {//Detect closest note
   note_no2 = k + 1;
 }
 else if (cmp2 > cmp1) {//Detect closest note
   note_no2 = k;
 }

 //Gate detect
 CV4 = analogRead(3);
 if (old_CV4 <= 64 && CV4 > 64) {
   note_on2 = 1;
 }
 else if (old_CV4 >= 64 && CV4 < 64) {
   note_off2 = 1;
 }
}

void CV56note() {
 CV5 = analogRead(6);

 //V/oct quantize
 for (int j = 0; j < 61; j++) {//Detects touched keyboard note
   if ( CV5 >= qnt[j] && CV5 < qnt[j + 1]) {
     cmp1 = CV5 - qnt[j];//Detect closest note
     cmp2 = qnt[j + 1] - CV5; //Detect closest note
     k = j - 1;
     break;
   }
 }

 if (cmp1 >= cmp2) {//Detect closest note
   note_no3 = k + 1;
 }
 else if (cmp2 > cmp1) {//Detect closest note
   note_no3 = k;
 }

 //Gate detect
 CV6 = analogRead(7);
 if (old_CV6 <= 64 && CV6 > 64) {
   note_on3 = 1;
 }
 else if (old_CV6 >= 64 && CV6 < 64) {
   note_off3 = 1;
 }
}

void CV12CC() {
 CV1 = analogRead(0) / 8; //1023->127
 CV2 = analogRead(1) / 8; //1023->127
 CV1 = map(CV1, 0, 127, 0, set_ccmax[0]);
 CV2 = map(CV2, 0, 127, 0, set_ccmax[1]);
}

void CV34CC() {
 CV3 = analogRead(2) / 8; //1023->127
 CV4 = analogRead(3) / 8; //1023->127
 CV3 = map(CV3, 0, 127, 0, set_ccmax[2]);
 CV4 = map(CV4, 0, 127, 0, set_ccmax[3]);
}

void CV56CC() {
 CV5 = analogRead(6) / 8; //1023->127
 CV6 = analogRead(7) / 8; //1023->127
 CV5 = map(CV5, 0, 127, 0, set_ccmax[4]);
 CV6 = map(CV6, 0, 127, 0, set_ccmax[5]);
}

void PRC() {
 CV1 = analogRead(0);
 if (old_CV1 <= 64 && CV1 > 64) {
   note_on1 = 1;
 }
 else if (old_CV1 >= 64 && CV1 < 64) {
   note_off1 = 1;
 }
 CV2 = analogRead(1);
 if (old_CV2 <= 64 && CV2 > 64) {
   note_on2 = 1;
 }
 else if (old_CV2 >= 64 && CV2 < 64) {
   note_off2 = 1;
 }
 CV3 = analogRead(2);
 if (old_CV3 <= 64 && CV3 > 64) {
   note_on3 = 1;
 }
 else if (old_CV3 >= 64 && CV3 < 64) {
   note_off3 = 1;
 }
 CV4 = analogRead(3);
 if (old_CV4 <= 64 && CV4 > 64) {
   note_on4 = 1;
 }
 else if (old_CV4 >= 64 && CV4 < 64) {
   note_off4 = 1;
 }
 CV5 = analogRead(6);
 if (old_CV5 <= 64 && CV5 > 64) {
   note_on5 = 1;
 }
 else if (old_CV5 >= 64 && CV5 < 64) {
   note_off5 = 1;
 }
 CV6 = analogRead(7);
 if (old_CV6 <= 64 && CV6 > 64) {
   note_on6 = 1;
 }
 else if (old_CV6 >= 64 && CV6 < 64) {
   note_off6 = 1;
 }
}

//-------------------------------MIDI OUT------------------------------------
void MIDI12note() {
 if (note_on1 == 1) {
   if (set_ntchd[0] == 0) {
     MIDI.sendNoteOn(note_no1 + set_ntofs[0], 127, set_ntch[0]); // note on(pitch 42, velo 127 on channel 1)
   }
   else if (set_ntchd[0] == 1) {
     MIDI.sendNoteOn(chord_maj[(note_no1 % 12)][0] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_maj[(note_no1 % 12)][1] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_maj[(note_no1 % 12)][2] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_maj[(note_no1 % 12)][3] + set_ntofs[0] + 24, 127, set_ntch[0]);
   }
   else if (set_ntchd[0] == 2) {
     MIDI.sendNoteOn(chord_min[(note_no1 % 12)][0] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_min[(note_no1 % 12)][1] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_min[(note_no1 % 12)][2] + set_ntofs[0] + 24, 127, set_ntch[0]);
     MIDI.sendNoteOn(chord_min[(note_no1 % 12)][3] + set_ntofs[0] + 24, 127, set_ntch[0]);
   }

   note_on1 = 0;
   note_out1 = note_no1;
 }
 if (note_off1 == 1) {
   if (set_ntchd[0] == 0) {
     MIDI.sendNoteOff(note_out1 + set_ntofs[0], 0, set_ntch[0]); // note off
   }
   else if (set_ntchd[0] == 1) {
     MIDI.sendNoteOff(chord_maj[(note_out1 % 12)][0] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_maj[(note_out1 % 12)][1] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_maj[(note_out1 % 12)][2] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_maj[(note_out1 % 12)][3] + set_ntofs[0] + 24, 0, set_ntch[0]);
   }
   else if (set_ntchd[0] == 2) {
     MIDI.sendNoteOff(chord_min[(note_out1 % 12)][0] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_min[(note_out1 % 12)][1] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_min[(note_out1 % 12)][2] + set_ntofs[0] + 24, 0, set_ntch[0]);
     MIDI.sendNoteOff(chord_min[(note_out1 % 12)][3] + set_ntofs[0] + 24, 0, set_ntch[0]);
   }

   note_off1 = 0;
 }
}

void MIDI34note() {
 if (note_on2 == 1) {
   if (set_ntchd[2] == 0) {
     MIDI.sendNoteOn(note_no2 + set_ntofs[2], 127, set_ntch[2]); // note on(pitch 42, velo 127 on channel 1)
   }
   else if (set_ntchd[2] == 1) {
     MIDI.sendNoteOn(chord_maj[(note_no2 % 12)][0] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_maj[(note_no2 % 12)][1] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_maj[(note_no2 % 12)][2] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_maj[(note_no2 % 12)][3] + set_ntofs[2] + 24, 127, set_ntch[2]);
   }
   else if (set_ntchd[2] == 2) {
     MIDI.sendNoteOn(chord_min[(note_no2 % 12)][0] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_min[(note_no2 % 12)][1] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_min[(note_no2 % 12)][2] + set_ntofs[2] + 24, 127, set_ntch[2]);
     MIDI.sendNoteOn(chord_min[(note_no2 % 12)][3] + set_ntofs[2] + 24, 127, set_ntch[2]);
   }
   note_on2 = 0;
   note_out2 = note_no2 ;
 }
 if (note_off2 == 1) {
   if (set_ntchd[2] == 0) {
     MIDI.sendNoteOff(note_out2 + set_ntofs[2], 127, set_ntch[2]); //
   }
   else if (set_ntchd[2] == 1) {
     MIDI.sendNoteOff(chord_maj[(note_out2 % 12)][0] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_maj[(note_out2 % 12)][1] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_maj[(note_out2 % 12)][2] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_maj[(note_out2 % 12)][3] + set_ntofs[2] + 24, 0, set_ntch[2]);
   }
   else if (set_ntchd[2] == 2) {
     MIDI.sendNoteOff(chord_min[(note_out2 % 12)][0] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_min[(note_out2 % 12)][1] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_min[(note_out2 % 12)][2] + set_ntofs[2] + 24, 0, set_ntch[2]);
     MIDI.sendNoteOff(chord_min[(note_out2 % 12)][3] + set_ntofs[2] + 24, 0, set_ntch[2]);
   }
   note_off2 = 0;
 }
}

void MIDI56note() {
 if (note_on3 == 1) {
   if (set_ntchd[4] == 0) {
     MIDI.sendNoteOn(note_no3 + set_ntofs[4], 127, set_ntch[4]); // note on(pitch 42, velo 127 on channel 1)
   }
   else if (set_ntchd[4] == 1) {
     MIDI.sendNoteOn(chord_maj[(note_no3 % 12)][0] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_maj[(note_no3 % 12)][1] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_maj[(note_no3 % 12)][2] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_maj[(note_no3 % 12)][3] + set_ntofs[4] + 24, 127, set_ntch[4]);
   }
   else if (set_ntchd[4] == 2) {
     MIDI.sendNoteOn(chord_min[(note_no3 % 12)][0] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_min[(note_no3 % 12)][1] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_min[(note_no3 % 12)][2] + set_ntofs[4] + 24, 127, set_ntch[4]);
     MIDI.sendNoteOn(chord_min[(note_no3 % 12)][3] + set_ntofs[4] + 24, 127, set_ntch[4]);
   }
   note_on3 = 0;
   note_out3 = note_no3 ;
 }
 if (note_off3 == 1) {
   if (set_ntchd[4] == 0) {
     MIDI.sendNoteOff(note_out3 + set_ntofs[4], 127, set_ntch[4]); //
   }
   else if (set_ntchd[4] == 1) {
     MIDI.sendNoteOff(chord_maj[(note_out3 % 12)][0] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_maj[(note_out3 % 12)][1] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_maj[(note_out3 % 12)][2] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_maj[(note_out3 % 12)][3] + set_ntofs[4] + 24, 0, set_ntch[4]);
   }
   else if (set_ntchd[4] == 2) {
     MIDI.sendNoteOff(chord_min[(note_out3 % 12)][0] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_min[(note_out3 % 12)][1] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_min[(note_out3 % 12)][2] + set_ntofs[4] + 24, 0, set_ntch[4]);
     MIDI.sendNoteOff(chord_min[(note_out3 % 12)][3] + set_ntofs[4] + 24, 0, set_ntch[4]);
   }
   note_off3 = 0;
 }
}

void MIDI12CC() {
 MIDI.sendControlChange(set_cccc[0], CV1, set_ccch[0]); //sendControlChange (DataByte inControlNumber , DataByte inControlValue , Channel inChannel )
 MIDI.sendControlChange(set_cccc[1], CV2, set_ccch[1]);
}
void MIDI34CC() {
 MIDI.sendControlChange(set_cccc[2], CV2, set_ccch[2]); //sendControlChange (DataByte inControlNumber , DataByte inControlValue , Channel inChannel )
 MIDI.sendControlChange(set_cccc[3], CV3, set_ccch[3]);
}
void MIDI56CC() {
 MIDI.sendControlChange(set_cccc[4], CV4, set_ccch[4]); //sendControlChange (DataByte inControlNumber , DataByte inControlValue , Channel inChannel )
 MIDI.sendControlChange(set_cccc[5], CV5, set_ccch[5]);
}

void MIDIPRC() {
 if (note_on1 == 1) {
   MIDI.sendNoteOn(set_prcnt[0], set_prcvl[0], set_prcch[0]); // note on
   note_on1 = 0;
 }
 if (note_off1 == 1) {
   MIDI.sendNoteOff(set_prcnt[0], 0, set_prcch[0]); // note off
   note_off1 = 0;
 }
 if (note_on2 == 1) {
   MIDI.sendNoteOn(set_prcnt[1], set_prcvl[1], set_prcch[1]); // note on
   note_on2 = 0;
 }
 if (note_off2 == 1) {
   MIDI.sendNoteOff(set_prcnt[1], 0, set_prcch[1]); // note off
   note_off2 = 0;
 }
 if (note_on3 == 1) {
   MIDI.sendNoteOn(set_prcnt[2], set_prcvl[2], set_prcch[2]); // note on
   note_on3 = 0;
 }
 if (note_off3 == 1) {
   MIDI.sendNoteOff(set_prcnt[2], 0, set_prcch[2]); // note off
   note_off3 = 0;
 }
 if (note_on4 == 1) {
   MIDI.sendNoteOn(set_prcnt[3], set_prcvl[3], set_prcch[3]); // note on
   note_on4 = 0;
 }
 if (note_off4 == 1) {
   MIDI.sendNoteOff(set_prcnt[3], 0, set_prcch[3]); // note off
   note_off4 = 0;
 }
 if (note_on5 == 1) {
   MIDI.sendNoteOn(set_prcnt[4], set_prcvl[4], set_prcch[4]); // note on
   note_on5 = 0;
 }
 if (note_off5 == 1) {
   MIDI.sendNoteOff(set_prcnt[4], 0, set_prcch[4]); // note off
   note_off5 = 0;
 }
 if (note_on6 == 1) {
   MIDI.sendNoteOn(set_prcnt[5], set_prcvl[5], set_prcch[5]); // note on
   note_on6 = 0;
 }
 if (note_off6 == 1) {
   MIDI.sendNoteOff(set_prcnt[5], 0, set_prcch[5]); // note off
   note_off6 = 0;
 }
}

void all_notes_off() {
 MIDI.sendControlChange(123, 0, set_ccch[0]);
 MIDI.sendControlChange(123, 0, set_ccch[1]);
 MIDI.sendControlChange(123, 0, set_ccch[2]);
 MIDI.sendControlChange(123, 0, set_ccch[3]);
 MIDI.sendControlChange(123, 0, set_ccch[4]);
 MIDI.sendControlChange(123, 0, set_ccch[5]);
}

void display_out() {
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(WHITE);

 display.setCursor(0, 0);
 switch (mode) {
   case 1:
     display.print("N1_C4");
     break;

   case 2:
     display.print("N2_C2");
     break;

   case 3:
     display.print("ALL_N");
     break;

   case 4:
     display.print("ALL_C");
     break;

   case 5:
     display.print("PRC");
     break;

 }


 display.drawLine(0, 12, 31, 12, WHITE);

 display.setCursor(0, 16);
 display.print("1");

 display.setCursor(0, 24);
 if (mode == 1 || mode == 2 || mode == 3) {
   display.print("V/OCT");
 }
 else if (mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.setCursor(0, 32);
 display.print("2");

 display.setCursor(0, 40);
 if (mode == 1 || mode == 2 || mode == 3) {
   display.print(" GATE");
 }
 else if (mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.setCursor(0, 48);
 display.print("3");

 display.setCursor(0, 56);
 if ( mode == 2 || mode == 3) {
   display.print("V/OCT");
 }
 else if (mode == 1 || mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.setCursor(0, 64);
 display.print("4");

 display.setCursor(0, 72);
 if ( mode == 2 || mode == 3) {
   display.print(" GATE");
 }
 else if (mode == 1 || mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.setCursor(0, 80);
 display.print("5");

 display.setCursor(0, 88);
 if ( mode == 3) {
   display.print("V/OCT");
 }
 else if (mode == 1 || mode == 2 || mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.setCursor(0, 96);
 display.print("6");

 display.setCursor(0, 104);
 if (  mode == 3) {
   display.print(" GATE");
 }
 else if (mode == 1 || mode == 2 || mode == 4) {
   display.print("   CV");
 }
 else if (mode == 5) {
   display.print(" GATE");
 }

 display.drawLine(0, 116, 31, 116, WHITE);

 display.display();
}


void display_set() {
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(WHITE);

 display.setCursor(6, 0);
 display.print("CV");
 display.setCursor(18, 0);
 display.print(set_ch);

 display.drawLine(0, 12, 31, 12, WHITE);

 display.setCursor(0, 16);
 display.print("NOTE");


 display.setCursor(6, 24);
 display.print("CH");
 display.setCursor(18, 24);
 if (set_ch == 1 || set_ch == 3 || set_ch == 5) {
   display.print(set_ntch[set_ch - 1]);
 }
 else if (set_ch == 2 || set_ch == 4 || set_ch == 6) {
   display.print(" -");
 }

 display.setCursor(6, 32);
 display.print("OF");
 display.setCursor(18, 32);
 if (set_ch == 1 || set_ch == 3 || set_ch == 5) {
   display.print(set_ntofs[set_ch - 1]);
 }
 else if (set_ch == 2 || set_ch == 4 || set_ch == 6) {
   display.print(" -");
 }


 display.setCursor(6, 40);
 display.print("CH");
 display.setCursor(18, 40);
 if (set_ch == 1 || set_ch == 3 || set_ch == 5) {
   if (set_ntchd[set_ch - 1] == 0) {
     display.print(" N");
   }
   else if (set_ntchd[set_ch - 1] == 1) {
     display.print("MJ");
   }
   else if (set_ntchd[set_ch - 1] == 2) {
     display.print("mn");
   }
 }
 else if (set_ch == 2 || set_ch == 4 || set_ch == 6) {
   display.print(" -");
 }

 display.setCursor(0, 48);
 display.print("CC");

 display.setCursor(6, 56);
 display.print("CH");
 display.setCursor(18, 56);
 display.print(set_ccch[set_ch - 1]);

 display.setCursor(6, 64);
 display.print("C");
 display.setCursor(12, 64);
 display.print(set_cccc[set_ch - 1]);

 display.setCursor(6, 72);
 display.print("L");
 display.setCursor(12, 72);
 display.print(set_ccmax[set_ch - 1]);

 display.setCursor(0, 80);
 display.print("PRC");

 display.setCursor(6, 88);
 display.print("CH");
 display.setCursor(18, 88);
 display.print(set_prcch[set_ch - 1]);

 display.setCursor(6, 96);
 display.print("N");
 display.setCursor(12, 96);
 display.print(set_prcnt[set_ch - 1]);

   display.setCursor(6, 104);
 display.print("V");
 display.setCursor(12, 104);
 display.print(set_prcvl[set_ch - 1]);

 switch (select) {
   case 0:
     display.drawTriangle(0, 0, 0, 6, 5, 3, WHITE);
     break;

   case 1:
     if (select_set == 0) {
       display.drawTriangle(0, 24, 0, 30, 5, 27, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 24, 0, 30, 5, 27, WHITE);
     }
     break;

   case 2:
     if (select_set == 0) {
       display.drawTriangle(0, 32, 0, 38, 5, 35, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 32, 0, 38, 5, 35, WHITE);
     }
     break;

   case 3:
     if (select_set == 0) {
       display.drawTriangle(0, 40, 0, 48, 5, 43, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 40, 0, 48, 5, 43, WHITE);
     }
     break;

   case 4:
     if (select_set == 0) {
       display.drawTriangle(0, 56, 0, 62, 5, 59, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 56, 0, 62, 5, 59, WHITE);
     }
     break;

   case 5:
     if (select_set == 0) {
       display.drawTriangle(0, 64, 0, 70, 5, 67, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 64, 0, 70, 5, 67, WHITE);
     }
     break;

   case 6:
     if (select_set == 0) {
       display.drawTriangle(0, 72, 0, 78, 5, 75, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 72, 0, 78, 5, 75, WHITE);
     }
     break;

   case 7:
     if (select_set == 0) {
       display.drawTriangle(0, 88, 0, 94, 5, 91, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 88, 0, 94, 5, 91, WHITE);
     }
     break;

   case 8:
     if (select_set == 0) {
       display.drawTriangle(0, 96, 0, 102, 5, 99, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 96, 0, 102, 5, 99, WHITE);
     }
     break;

         case 9:
     if (select_set == 0) {
       display.drawTriangle(0, 104, 0, 110, 5, 107, WHITE);
     }
     else if (select_set == 1) {
       display.fillTriangle(0, 104, 0, 110, 5, 107, WHITE);
     }
     break;
 }
 display.display();
}

void eeprom() {
 EEPROM.put(0x00, set_ntch);
 EEPROM.put(0x10, set_ntofs);
 EEPROM.put(0x20, set_ntchd);
 EEPROM.put(0x30, set_ccch);
 EEPROM.put(0x40, set_cccc);
 EEPROM.put(0x50, set_ccmax);
 EEPROM.put(0x60, set_prcch);
 EEPROM.put(0x70, set_prcnt);
 EEPROM.put(0x80, set_prcvl);
}​