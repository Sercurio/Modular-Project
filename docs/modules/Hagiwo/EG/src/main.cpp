#include <avr/io.h>

//PWMsetting
unsigned int frq = 50000 ; // PWM frequency
float duty = 0.5;//PWM duty

//envelope curve wavetable
const static word LOG[200] PROGMEM = {
 0,  64, 93, 122,  150,  177,  203,  228,  253,  277,  299,  322,  343,  364,  384,  404,  423,  441,  459,  476,  493,  509,  524,  539,  554,  568,  582,  595,  608,  620,  632,  644,  655,  666,  677,  687,  697,  707,  716,  725,  734,  742,  751,  759,  766,  774,  781,  788,  795,  801,  808,  814,  820,  825,  831,  836,  842,  847,  852,  856,  861,  865,  870,  874,  878,  882,  886,  889,  893,  896,  900,  903,  906,  909,  912,  915,  917,  920,  923,  925,  928,  930,  932,  934,  937,  939,  941,  942,  944,  946,  948,  950,  951,  953,  954,  956,  957,  959,  960,  961,  963,  964,  965,  966,  967,  968,  969,  970,  971,  972,  973,  974,  975,  976,  977,  978,  978,  979,  980,  980,  981,  982,  982,  983,  984,  984,  985,  985,  986,  986,  987,  987,  988,  988,  988,  989,  989,  990,  990,  990,  991,  991,  991,  992,  992,  992,  993,  993,  993,  993,  994,  994,  994,  994,  995,  995,  995,  995,  995,  996,  996,  996,  996,  996,  997,  997,  997,  997,  997,  997,  997,  998,  998,  998,  998,  998,  998,  998,  998,  998,  999,  999,  999,  999,  999,  999,  999,  999,  999,  999,  999,  999,  999,  1000, 1000, 1000, 1000, 1000, 1000, 1000
};
const static word RAMP[200] PROGMEM = {
 0,  10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100,  105,  110,  115,  120,  125,  130,  135,  140,  145,  150,  155,  160,  165,  170,  175,  180,  185,  190,  195,  200,  205,  210,  215,  220,  225,  230,  235,  240,  245,  250,  255,  260,  265,  270,  275,  280,  285,  290,  295,  300,  305,  310,  315,  320,  325,  330,  335,  340,  345,  350,  355,  360,  365,  370,  375,  380,  385,  390,  395,  400,  405,  410,  415,  420,  425,  430,  435,  440,  445,  450,  455,  460,  465,  470,  475,  480,  485,  490,  495,  500,  505,  510,  515,  520,  525,  530,  535,  540,  545,  550,  555,  560,  565,  570,  575,  580,  585,  590,  595,  600,  605,  610,  615,  620,  625,  630,  635,  640,  645,  650,  655,  660,  665,  670,  675,  680,  685,  690,  695,  700,  705,  710,  715,  720,  725,  730,  735,  740,  745,  750,  755,  760,  765,  770,  775,  780,  785,  790,  795,  800,  805,  810,  815,  820,  825,  830,  835,  840,  845,  850,  855,  860,  865,  870,  875,  880,  885,  890,  895,  900,  905,  910,  915,  920,  925,  930,  935,  940,  945,  950,  955,  960,  965,  970,  975,  980,  985,  990,  995,  1000
};
const static word EXP[200] PROGMEM = {
 0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 40, 41, 43, 44, 46, 47, 49, 50, 52, 54, 56, 58, 59, 61, 63, 66, 68, 70, 72, 75, 77, 80, 83, 85, 88, 91, 94, 97, 100,  104,  107,  111,  114,  118,  122,  126,  130,  135,  139,  144,  148,  153,  158,  164,  169,  175,  180,  186,  192,  199,  205,  212,  219,  226,  234,  241,  249,  258,  266,  275,  284,  293,  303,  313,  323,  334,  345,  356,  368,  380,  392,  405,  418,  432,  446,  461,  476,  491,  507,  524,  541,  559,  577,  596,  616,  636,  657,  678,  701,  723,  747,  772,  797,  823,  850,  878,  907,  936,  967,  1000
};

int i = 0;//time count
bool trig = 1;//external input detect
bool old_trig = 0;

long Atime = 100;//Attack time
long Dtime = 100;//Decay(rerease) time
int level = 1023;//output amp
byte mode = 0;//0=AR,1=ASR,2=LONG AR,3=INV,4=LFO
byte old_mode = 0;
byte curve = 0;//waveform curve ,0=LOG,1=Ramp,2=EXP
int ASR_top = 500;//for ASRmode , remind EG height when trig on to off.

bool attack_end = 0;// end = 1 , not end =0

void setup() {
 pinMode(3, INPUT);//gate in
 pinMode(5, INPUT_PULLUP); //curve select
 pinMode(6, INPUT_PULLUP); //curve select
 pinMode(10, OUTPUT);//EG out

 //PWM register setting
 TCCR1A = 0b00100001;
 TCCR1B = 0b00010001;//分周比1

 mode_select();
}

void loop() {
 old_trig = trig;
 old_mode = mode;
 mode_select();//mode knob

 if (mode != 4) {
   trig = digitalRead(3);
 }
 if ( old_mode != 4 && mode == 4) {
   trig = 1;
   old_trig = 0;
   i = 0;
   attack_end = 0;
 }

 if (old_trig == 0 && trig == 1) {//when trigger in detect
   Atime = analogRead(0) * 10;//Attack konb
   Dtime = analogRead(1) * 10;//Decay knob
   level = map(analogRead(5), 0, 1023, 1, 100);//Outpu level
   mode_select();//mode knob

   //setting curve form SW
   if (digitalRead(5) == 1 && digitalRead(6) == 1) {
     curve = 1;//Ramp
   }
   else if (digitalRead(5) == 0 && digitalRead(6) == 1) {
     curve = 0;//LOG
   }
   else if (digitalRead(5) == 1 && digitalRead(6) == 0) {
     curve = 2;//exp
   }

   duty = 0;
   attack_end = 0;
   i = 0;
 }

 switch (mode) {
   case 0://AR------------------------------------------------------------------------
     if (attack_end == 0) {
       i++;
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[i])));
           break;
       }

       if ( i >= 199 ) {
         i = 0;
         attack_end = 1;
       }
     }
     else if (attack_end == 1) {
       i++;
       if ( i >= 199) {
         i = 199;
       }
       switch (curve) {
         case 0:
           duty =  1000 - (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty = 1000 - (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  1000 - (pgm_read_word(&(EXP[i])));
           break;
       }
     }
     break;

   case 1://ASR------------------------------------------------------------------------
     if (attack_end == 0) {
       i++;
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[i])));
           break;
       }
       if ( i >= 199 ) {
         attack_end = 1;
       }
     }
     if ( old_trig == 1 && trig == 0) {
       attack_end = 1;
       if ( i >= 199) {
         i = 199;
       }
       ASR_top = i;
       i = 0;
     }
     if (attack_end == 1 && trig == 1) {
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[i])));
           break;
       }
     }

     if (attack_end == 1 && trig == 0) {
       i++;
       if ( i >= ASR_top) {
         i = ASR_top;
       }
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[ASR_top]))) - (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty = (pgm_read_word(&(RAMP[ASR_top]))) - (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[ASR_top]))) - (pgm_read_word(&(EXP[i])));
           break;
       }
     }
     break;

   case 2://LONG AR------------------------------------------------------------------------
     if (attack_end == 0) {
       i++;
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i / 20])));//By reducing the count of i to 1/20, the advance of time is multiplied by 20.
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i / 20])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[i / 20])));
           break;
       }
       if (Atime == 0) {//When Atime is 0, the decay is applied immediately without counting 1/20.
         switch (curve) {
           case 0:
             duty =  (pgm_read_word(&(LOG[199])));
             break;

           case 1:
             duty =  (pgm_read_word(&(RAMP[199])));
             break;

           case 2:
             duty =  (pgm_read_word(&(EXP[199])));
             break;
         }
       }

       if ( i >= 199 * 20 ) {
         i = 0;
         attack_end = 1;
       }
     }
     else if (attack_end == 1) {
       i++;
       if ( i >= 199 * 20) {
         i = 199 * 20;
       }
       switch (curve) {
         case 0:
           duty =  1000 - (pgm_read_word(&(LOG[i / 20])));
           break;

         case 1:
           duty = 1000 - (pgm_read_word(&(RAMP[i / 20])));
           break;

         case 2:
           duty =  1000 - (pgm_read_word(&(EXP[i / 20])));
           break;
       }
     }
     break;

   case 3://INV------------------------------------------------------------------------
     if (attack_end == 0) {
       i++;
       switch (curve) {
         case 0:
           duty =  1000 - (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  1000 - (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  1000 - (pgm_read_word(&(EXP[i])));
           break;
       }

       if ( i >= 199 ) {
         i = 0;
         attack_end = 1;
       }
     }
     else if (attack_end == 1) {
       i++;
       if ( i >= 199) {
         i = 199;
       }
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =   (pgm_read_word(&(EXP[i])));
           break;
       }
     }
     break;

   case 4://LFO------------------------------------------------------------------------
     if (attack_end == 0) {
       i++;
       trig = 0;
       switch (curve) {
         case 0:
           duty =  (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty =  (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  (pgm_read_word(&(EXP[i])));
           break;
       }

       if ( i >= 199 ) {
         i = 0;
         attack_end = 1;
       }
     }
     else if (attack_end == 1) {
       i++;
       if ( i >= 199) {
         i = 199;
       }
       switch (curve) {
         case 0:
           duty =  1000 - (pgm_read_word(&(LOG[i])));
           break;

         case 1:
           duty = 1000 - (pgm_read_word(&(RAMP[i])));
           break;

         case 2:
           duty =  1000 - (pgm_read_word(&(EXP[i])));
           break;
       }
       if ( i >= 199) {
         trig = 1;
         old_trig = 0;
         Atime = analogRead(0) * 10;//Attack konb
         Dtime = analogRead(1) * 10;//Decay knob
         mode_select();


         level = map(analogRead(5), 0, 1023, 1, 100);

         if (digitalRead(5) == 1 && digitalRead(6) == 1) {
           curve = 1;//Ramp
         }
         else if (digitalRead(5) == 1 && digitalRead(6) == 0) {
           curve = 0;//LOG
         }
         else if (digitalRead(5) == 0 && digitalRead(6) == 1) {
           curve = 2;//exp
         }
         attack_end = 0;
         i = 0;
       }
     }
     break;
 }

 //time count------------------------------------------------------------------------
 if (attack_end == 0) {//Attack time count
   delayMicroseconds(Atime);
 }
 else {//Decay(rerease)time count
   delayMicroseconds(Dtime);
 }
 PWM_OUT();//PWM duty setting
}

void PWM_OUT() {//PWM duty setting
 duty = duty * level / 100;//apply level knob setting
 // set TOP parameter
 OCR1A = (unsigned int)(8000000 / frq );
 // set DUTY parameter
 OCR1B = (unsigned int)(8000000  / frq * duty / 1000);
}

void mode_select() {
 if ( analogRead(3) <= 127) {
   mode = 0;//AR
 }
 else if ( analogRead(3) > 127 && analogRead(3) <= 383) {
   mode = 1;//ASR
 }
 else if ( analogRead(3) > 383 && analogRead(3) <= 639) {
   mode = 2;//LONG AR
 }
 else if ( analogRead(3) > 639 && analogRead(3) <= 895) {
   mode = 3;//INV
 }
 else if ( analogRead(3) > 897 && analogRead(3) <= 1023) {
   mode = 4;//LFO
   
 }
}