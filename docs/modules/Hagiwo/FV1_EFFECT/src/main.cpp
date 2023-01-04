
#include <Encoder.h>
#include <avr/io.h>//for fast PWM

//encoder library setting
#define  ENCODER_OPTIMIZE_INTERRUPTS //contermeasure of encoder noise
#include <Encoder.h>

//OLED display setting
#include <Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>

#define OLED_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//OLED display data table
//PRESET 1st line
const char str01[8][12] PROGMEM = {//str1 is effect name of 1st line
 "Chorus", "Flange", "Tremolo", "Pitch", "Pitch", "No effect", "Reverb", "Reverb"
};
const char* const name01[] PROGMEM = {
 str01[0], str01[1], str01[2], str01[3], str01[4], str01[5], str01[6], str01[7],
};

//PRESET 2nd line
const char str02[8][12] PROGMEM = {//str1 is effect name of 1st line
 "reverb", "reverb", "reverb", "shift", "echo", " ", "1", "2"
};
const char* const name02[] PROGMEM = {
 str02[0], str02[1], str02[2], str02[3], str02[4], str02[5], str02[6], str02[7],
};

//PRESET param1
const char str03[8][12] PROGMEM = {//str1 is effect name of 1st line
 "rev mix", "rev mix", "rev mix", "pitch", "pitch", "-", "rev time", "rev time"
};
const char* const name03[] PROGMEM = {
 str03[0], str03[1], str03[2], str03[3], str03[4], str03[5], str03[6], str03[7],
};

//PRESET param2
const char str04[8][12] PROGMEM = {//str1 is effect name of 1st line
 "cho rate", "flng rate", "trem rate", "-", "echo delay", "-", "HF filter", "HF filter"
};
const char* const name04[] PROGMEM = {
 str04[0], str04[1], str04[2], str04[3], str04[4], str04[5], str04[6], str04[7],
};

//PRESET param3
const char str05[8][12] PROGMEM = {//str1 is effect name of 1st line
 "cho mix", "flng mix", "trem mix", "-", "echo mix", "-", "LF filter", "LF filter"
};
const char* const name05[] PROGMEM = {
 str05[0], str05[1], str05[2], str05[3], str05[4], str05[5], str05[6], str05[7],
};


//ROM1 1st line
const char str11[8][12] PROGMEM = {//str1 is effect name of 1st line
"Hall","echo","shimmer","Dual tape","Shingle","Echo","Star","Triple"
};
const char* const name11[] PROGMEM = {
 str11[0], str11[1], str11[2], str11[3], str11[4], str11[5], str11[6], str11[7],
};

//ROM1 2nd line
const char str12[8][12] PROGMEM = {//str1 is effect name of 1st line
"reverb","reverb","reverb","reverb","tape echo"," ","field","echo"
};
const char* const name12[] PROGMEM = {
 str12[0], str12[1], str12[2], str12[3], str12[4], str12[5], str12[6], str12[7],
};

//ROM1 param1
const char str13[8][12] PROGMEM = {//str1 is effect name of 1st line
"pre-delay","delay","shimmer","delay","time","rev level","delay","time1"

};
const char* const name13[] PROGMEM = {
 str13[0], str13[1], str13[2], str13[3], str13[4], str13[5], str13[6], str13[7],
};

//ROM1 param2
const char str14[8][12] PROGMEM = {//str1 is effect name of 1st line
"rev time","repeat","rev level","feed back","feed back","delay","tremolo","time2"

};
const char* const name14[] PROGMEM = {
 str14[0], str14[1], str14[2], str14[3], str14[4], str14[5], str14[6], str14[7],
};

//ROM1 param3
const char str15[8][12] PROGMEM = {//str1 is effect name of 1st line
"damping","reverb","rev time","damping","damping","echo level","mix","time3"

};
const char* const name15[] PROGMEM = {
 str15[0], str15[1], str15[2], str15[3], str15[4], str15[5], str15[6], str15[7],
};

//ROM2 1st line
const char str21[8][12] PROGMEM = {//str1 is effect name of 1st line
"Dual","Chorus","Flanger","Wah","Distortion","octave","Digital","Sine"
};
const char* const name21[] PROGMEM = {
 str21[0], str21[1], str21[2], str21[3], str21[4], str21[5], str21[6], str21[7],
};

//ROM2 2nd line
const char str22[8][12] PROGMEM = {//str1 is effect name of 1st line
"chorus","ring mod","","","","","fuzz","osc"
};
const char* const name22[] PROGMEM = {
 str22[0], str22[1], str22[2], str22[3], str22[4], str22[5], str22[6], str22[7],
};

//ROM2 param1
const char str23[8][12] PROGMEM = {//str1 is effect name of 1st line
"cho level","blend","delay","reverb","gain","mix","rate","freq"
};
const char* const name23[] PROGMEM = {
 str23[0], str23[1], str23[2], str23[3], str23[4], str23[5], str23[6], str23[7],
};

//ROM2 param2
const char str24[8][12] PROGMEM = {//str1 is effect name of 1st line
"rate1","offset","rate","sensitivity","tone","oct up","distortion","fine"
};
const char* const name24[] PROGMEM = {
 str24[0], str24[1], str24[2], str24[3], str24[4], str24[5], str24[6], str24[7],
};

//ROM2 param3
const char str25[8][12] PROGMEM = {//str1 is effect name of 1st line
"rate2","chorus","width","Q/level","mix","oct down","volume","amp"
};
const char* const name25[] PROGMEM = {
 str25[0], str25[1], str25[2], str25[3], str25[4], str25[5], str25[6], str25[7],
};

//rotary encoder setting
Encoder myEnc(2, 4);
int oldPosition  = -999;
int newPosition = -999;
int i = 1;

int POT0 = 150;
int POT1 = 150;
int POT2 = 150;

bool old_sw = 0;
bool sw = 0;
byte bank = 0; //0=fv1 rom FX , 1 = EEPROM1 , 2 = EEPROM2

bool disp_reflesh = 0;
long disp_ref_count = 0;

void setup() {
 //OLED library setting
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 display.clearDisplay(); 

 //pin mode setting
 pinMode(3, OUTPUT) ;//POT2 PWM
 pinMode(5, OUTPUT) ;//S2
 pinMode(6, OUTPUT) ;//S1
 pinMode(7, OUTPUT) ;//S0
 pinMode(8, OUTPUT) ;//EEPROM SW1
 pinMode(9, OUTPUT) ;//EEPROM SW2
 pinMode(10, OUTPUT);//POT2 PWM
 pinMode(11, OUTPUT) ;//POT0 PWM
 pinMode(12, INPUT_PULLUP); //external triger detect
 pinMode(13, OUTPUT) ;//T0

 //fast pwm setting
 TCCR2B &= B11111000;
 TCCR2B |= B00000001;
 //fast pwm setting
 TCCR1B &= B11111000;
 TCCR1B |= B00000001;

 disp_ref_count = millis();
}

void loop() {
 //change bank(ROM) by push button
 old_sw = sw;
 sw = digitalRead(12);
 if (sw == 1 && old_sw == 0) { //bank change
   bank ++;
   disp_reflesh = 1;
   if (bank >= 3) {
     bank = 0;
   }

   if (bank == 2) {
     digitalWrite(9, LOW);
     delay(5);
     digitalWrite(8, HIGH);
     delay(5);
     digitalWrite(13, HIGH);
     delay(5);
     digitalWrite(13, LOW);
     delay(5);
     digitalWrite(13, HIGH);
   }
   else  if (bank == 1) {
     digitalWrite(8, LOW);
     delay(5);
     digitalWrite(9, HIGH);
     delay(5);
     digitalWrite(13, HIGH);
     delay(5);
     digitalWrite(13, LOW);
     delay(5);
     digitalWrite(13, HIGH);
   }
   else if (bank == 0) {
     digitalWrite(13, LOW);
     digitalWrite(8, LOW);
     digitalWrite(9, LOW);
   }
 }


 //rotary encoder
 newPosition = myEnc.read();
   if ( (newPosition - 3) / 4  > oldPosition / 4) {
   oldPosition = newPosition;
   i = i - 1;
   disp_reflesh = 1;
   if ( i <= -1) {
     i = 7;
   }
 }

 else if ( (newPosition + 3) / 4  < oldPosition / 4 ) { 
   oldPosition = newPosition;
   i = i + 1;
   disp_reflesh = 1;
   if ( i >= 8) {
     i = 0;
   }
 }
 i = constrain(i, 0, 7);

 //PWM value calc
 POT0 = ( analogRead(0) + analogRead(3) );
 POT1 = ( analogRead(1) + analogRead(6) );
 POT2 = ( analogRead(2) + analogRead(7) );
 POT0 = map(POT0, 0, 1023, 0, 150);//150 : reduce voltage 5V to 3V (fv-1 voltage rate)
 POT1 = map(POT1, 0, 1023, 0, 150);//150 : reduce voltage 5V to 3V (fv-1 voltage rate)
 POT2 = map(POT2, 0, 1023, 0, 150);//150 : reduce voltage 5V to 3V (fv-1 voltage rate)

 //select fv1 effect number
 digitalWrite(7, bitRead(i, 0)); //program LSB
 digitalWrite(6, bitRead(i, 1)); //program
 digitalWrite(5, bitRead(i, 2)); //program MSB

 //PWM output
 analogWrite(3, POT2);
 analogWrite(10, POT1);
 analogWrite(11, POT0);

 //dispray reflesh frequency
 if ((disp_reflesh == 1) || (millis() >= disp_ref_count + 100)) { //reflesh rate is 100ms/once
   disp_reflesh = 0;
   disp_ref_count = millis();
   display_out();
 }
}

void display_out() {
 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(WHITE);

 display.setCursor(0, 0);//effect name , 1st line
 char buf1[30];
 if (bank == 0) {
   strcpy_P(buf1, pgm_read_word(&(name01[i])));
 }
 else if (bank == 1) {
   strcpy_P(buf1, pgm_read_word(&(name11[i])));
 }
 else if (bank == 2) {
   strcpy_P(buf1, pgm_read_word(&(name21[i])));
 }
 display.print(buf1);

 display.setCursor(0, 16);//effect name , 2nd line
 char buf2[30];
 if (bank == 0) {
   strcpy_P(buf2, pgm_read_word(&(name02[i])));
 }
 else if (bank == 1) {
   strcpy_P(buf2, pgm_read_word(&(name12[i])));
 }
 else if (bank == 2) {
   strcpy_P(buf2, pgm_read_word(&(name22[i])));
 }
 display.print(buf2);

 display.setTextSize(0);
 display.setCursor(120, 0);
 display.print(i);

 display.setCursor(0, 34);//effect param1
 char buf3[30];
 if (bank == 0) {
   strcpy_P(buf3, pgm_read_word(&(name03[i])));
 }
 else if (bank == 1) {
   strcpy_P(buf3, pgm_read_word(&(name13[i])));
 }
 else if (bank == 2) {
   strcpy_P(buf3, pgm_read_word(&(name23[i])));
 }
 display.print(buf3);

 display.setCursor(0, 44);//effect param2
 char buf4[30];
 if (bank == 0) {
   strcpy_P(buf4, pgm_read_word(&(name04[i])));
 }
 else if (bank == 1) {
   strcpy_P(buf4, pgm_read_word(&(name14[i])));
 }
 else if (bank == 2) {
   strcpy_P(buf4, pgm_read_word(&(name24[i])));
 }
 display.print(buf4);

 display.setCursor(0, 54);//effect param3
 char buf5[30];
 if (bank == 0) {
   strcpy_P(buf5, pgm_read_word(&(name05[i])));
 }
 else if (bank == 1) {
   strcpy_P(buf5, pgm_read_word(&(name15[i])));
 }
 else if (bank == 2) {
   strcpy_P(buf5, pgm_read_word(&(name25[i])));
 }
 display.print(buf5);

 //POT value round square
 display.fillRoundRect(74, 37, POT0 / 3, 5, 2, WHITE);
 display.fillRoundRect(74, 47, POT1 / 3, 5, 2, WHITE);
 display.fillRoundRect(74, 57, POT2 / 3, 5, 2, WHITE);

 display.display();
}