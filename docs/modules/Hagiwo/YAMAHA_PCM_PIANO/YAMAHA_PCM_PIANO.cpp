#include <FlashAsEEPROM.h>//flash memory use as eeprom
#include <usbh_midi.h>
#include <usbhub.h>

#if (USB_VID==0x2341 && defined(ARDUINO_SAMD_ZERO)) || (USB_VID==0x2a03 && defined(ARDUINO_SAM_ZERO))
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

float AD_CH1 = 0;
float AD_CH2 = 0;
float AD_CH1_calb = 1.04;//reduce resistance error
float AD_CH2_calb = 1.04;//reduce resistance error
int cmp1 = 0;
int cmp2 = 0;
byte search_qnt = 0;
int cmp12 = 0;
int cmp22 = 0;
byte search_qnt2 = 0;

int old_POT1, old_POT2, old_POT3, old_POT4, POT1, POT2, POT3, POT4, toggle1, toggle2, old_toggle1, old_toggle2;
byte cutoff1 = 127; //cutoff freq
byte sus1 = 10; // sis1 * 10msec , sustain time
byte rel1 = 100; //Release time
byte rev1 = 64; //Reverb wet
byte oct1 = 2; //octave
byte cho1 = 70;//chorus
byte vol1 = 127;//volume
byte pan1 = 63;//LR pan
byte calb1 = 0; //AD error calibration
byte FX_wet1 = 10; //FX wet
byte FX_select1 = 0; //FX
byte art_cnt1 = 0; //count up when trig in
byte art_freq1 = 0;//
byte art_vel1 = 0; //velocity table select
byte art_sus1 = 0; //velocity table select
byte art_sustime1 = 0;
byte cutoff2 = 127; //cutoff freq
byte sus2 = 10; // sis1 * 10msec , sustain time
byte rel2 = 100; //Release time
byte rev2 = 64; //Reverb wet
byte oct2 = 2; //octave
byte cho2 = 70;//chorus
byte vol2 = 127;//volume
byte pan2 = 63;//LR pan
byte calb2 = 0; //AD error calibration
byte FX_wet2 = 100; //FX wet
byte FX_select2 = 0; //FX
byte art_cnt2 = 0; //count up when trig in
byte art_freq2 = 0;//
byte art_vel2 = 0; //velocity table select

byte FX_MSB[12] = {
  1, 18, 17, 68, 66, 65, 5, 7, 20, 75, 74, 74
};
byte FX_LSB[12] = {
  7, 0, 0, 16, 2, 7, 16, 0, 0, 19, 8, 0
};

//Reverb  Hall L  1 7
//Reverb  CANYON reverb 18  0
//Reverb  Tunnel  17  0
//Chorus  SYMPHONIC 68  16
//Chorus  CELESTE2  66  2
//Chorus  FB Chorus 65  7
//Delay DELAY LCR 5 16
//Delay ECHO  7 0
//Delay Karaoke 20  0
//Distortion  ST Dist soft  75  19
//Distortion  ST Over drive 74  8
//Distortion  Over drive  74  0


byte mode = 0; //change POT assing
byte old_mode = 0;
byte select_ch = 1; //1=CH1 setting ,2=CH2 setting
byte old_select_ch = 1;
byte initial = 1; //1 = before initial setting , 0 = initital setting done

byte select_bank1 = 0;
byte select_bank2 = 0;
byte bank1[12] = {//Instrument bank
  0,4,8,12,17,26,60,78,81,98,108,114
};

//00  Acoustic Pino
//04  Electric Piano
//08  Celesta
//12  Marimba
//17  Percussive Organ
//26  Electric Guitar (jazz)
//60  French horn
//78  Whistle
//81  Lead 2 (sawtooth)
//98  FX 3 (crystal)
//108 Kalimba
//114 Steel Drums

int note1 = 0; //CH1 quantized note
int note2 = 0; //CH2 quantized note
int gate_delay1 = 0;
int gate_delay_timer1 = 0;
int gate_delay2 = 0;
int gate_delay_timer2 = 0;

long timer = 0;

int cv_qnt[62] = {
  0, 9,  26, 43, 60, 77, 94, 111,  128,  145,  162,  179,  196,  213,  230,  247,  264,  281,  298,  315,  332,  349,  366,  383,  400,  417,  434,  451,  468,  485,  502,  519,  536,  553,  570,  587,  604,  621,  638,  655,  672,  689,  706,  723,  740,  757,  774,  791,  808,  825,  842,  859,  876,  893,  910,  927,  944,  961,  978,  995,  1012, 1024
};//input quantize

byte art_vel3[8][3] = {//Velocity type
  {0, 0, 0},
  {0, 40, 40},
  {40, 40, 0},
  {0, 20, 40},
  {20, 0, 40},
  {0, 40, 0},
  {40, 0, 0},
  {0, 0, 40}
};

byte art_sus3[8][3] = {//Sustin type
  {0, 0, 0},
  {0, 40, 40},
  {40, 40, 0},
  {0, 20, 40},
  {20, 0, 40},
  {0, 40, 0},
  {40, 0, 0},
  {0, 0, 40}
};

byte art_vel4[8][4] = {//Velocity type
  {0, 0, 0, 0},
  {40, 40, 40, 0},
  {0, 40, 40, 40},
  {0, 40, 20, 40},
  {40, 0, 20, 0},
  {20, 20, 0, 40},
  {20, 0, 20, 0},
  {20, 40, 40, 0}
};

byte art_sus4[8][4] = {//Sustin type
  {0, 0, 0, 0},
  {40, 40, 40, 0},
  {0, 40, 40, 40},
  {0, 40, 20, 40},
  {40, 0, 20, 0},
  {20, 20, 0, 40},
  {20, 0, 20, 0},
  {20, 40, 40, 0}
};

byte art_vel6[8][6] = {//Velocity type
  {0, 0, 0, 0, 0, 0},
  {40, 0, 20, 0, 20, 20},
  {40, 40, 40, 0, 20, 20},
  {0, 40, 20, 0, 20, 40},
  {0, 40, 0, 20, 0, 40},
  {20, 40, 40, 20, 0, 40},
  {20, 0, 20, 40, 40, 20},
  {20, 20, 0, 0, 20, 40}
};

byte art_sus6[8][6] = {//Sustin type
  {0, 0, 0, 0, 0, 0},
  {40, 0, 20, 0, 20, 20},
  {40, 40, 40, 0, 20, 20},
  {0, 40, 20, 0, 20, 40},
  {0, 40, 0, 20, 0, 40},
  {20, 40, 40, 20, 0, 40},
  {20, 0, 20, 40, 40, 20},
  {20, 20, 0, 0, 20, 40}
};

USBHost UsbH;
USBH_MIDI  Midi(&UsbH);

uint16_t pid, vid;

bool note_on1 = 0;
int note_on_timer1 = 100;
bool old_note_on1 = 0;
bool note_off1 = 0; //0=note_off , 1 = note_on

bool note_on2 = 0;
int note_on_timer2 = 100;
bool old_note_on2 = 0;
bool note_off2 = 0; //0=note_off , 1 = note_on

void setup()
{
  analogReadResolution(12);
  pinMode(0, INPUT); //toggle2 , mode select , push sw
  pinMode(1, INPUT); //toggle1 , ch select
  pinMode(2, INPUT); //pot1
  pinMode(3, INPUT); //CV1
  pinMode(4, INPUT); //pot2
  pinMode(5, INPUT_PULLDOWN); //gate1
  pinMode(7, INPUT_PULLDOWN); //gate2
  pinMode(8, INPUT); //pot3
  pinMode(9, INPUT); //CV2
  pinMode(10, INPUT); //pot4

  pinMode(12, OUTPUT); //debug LED
  pinMode(13, OUTPUT); //debug LED

  timer = millis();

  vid = pid = 0;
  SerialDebug.begin(115200);

  if (UsbH.Init()) {
    SerialDebug.println("USB host did not start");
    while (1); //halt
  }
  delay( 4000 );//wait NSX-39 setup
}

void loop()
{
  UsbH.Task();

  if ( Midi ) {//Initial setting midi data send
    if (initial == 1) {
      initial_setting();
      initial = 0;
    }
    
    old_mode = mode;
    old_select_ch = select_ch;
    old_toggle2 = toggle2;
    
    //--------------------analog read POT---------------------------
    if (timer + 50 <= millis()) {//Read at 50 msec intervals to reduce the processing load.
      POT1 = analogRead(2) / 32;//0-127
      POT2 = analogRead(4) / 32;//0-127
      POT3 = analogRead(8) / 32;//0-127
      POT4 = analogRead(10) / 32;//0-127
      if (select_ch == 1) {//CH1 setting
        if (mode == 0) {
          if (abs(old_POT1 - POT1) > 10) {//old_POT for Disables POT reading at the moment of switching.
            cutoff1 = POT1;
          }
          if (abs(old_POT2 - POT2) > 10) {
            vol1 = POT2;
          }
          if (abs(old_POT3 - POT3) > 10) {
            sus1 = POT3;
          }
          if (abs(old_POT4 - POT4) > 10) {
            rel1 = POT4;
          }
        }

        else if (mode == 1) {
          if (abs(old_POT1 - POT1) > 10) {
            if (POT1 / 32 == 0) {
              art_freq1 = 0; //0times
            }
            else if (POT1 / 32 == 1) {
              art_freq1 = 3; //0times
            }
            else if (POT1 / 32 == 2) {
              art_freq1 = 4; //0times
            }
            else if (POT1 / 32 == 3) {
              art_freq1 = 6; //0times
            }
          }
          if (abs(old_POT2 - POT2) > 10) {
            art_vel1 = POT2 / 16;
          }
          if (abs(old_POT3 - POT3) > 10) {
            art_sus1 = POT2 / 16;
          }
          if (abs(old_POT4 - POT4) > 10) {
            
            calb1 = POT4;
          }
        }
        else if (mode == 2) {
          if (abs(old_POT1 - POT1) > 10) {
            oct1 = POT1 / 32;
          }
          if (abs(old_POT2 - POT2) > 10) {
            rev1 = POT2;
          }
          if (abs(old_POT3 - POT3) > 10) {
            cho1 = POT3;
          }
          if (abs(old_POT4 - POT4) > 10) {
            FX_wet1 = POT4;
          }
        }
      }
      else if (select_ch == 2) {//CH2 setting
        if (mode == 0) {
          if (abs(old_POT1 - POT1) > 10) {
            cutoff2 = POT1;
          }
          if (abs(old_POT2 - POT2) > 10) {
            vol2 = POT2;
          }
          if (abs(old_POT3 - POT3) > 10) {
            sus2 = POT3;
          }
          if (abs(old_POT4 - POT4) > 10) {
            rel2 = POT4;
          }
        }

        else if (mode == 1) {
          if (abs(old_POT1 - POT1) > 10) {
            if (POT1 / 32 == 0) {
              art_freq2 = 0; //0times
            }
            else if (POT1 / 32 == 1) {
              art_freq2 = 3; //0times
            }
            else if (POT1 / 32 == 2) {
              art_freq2 = 4; //0times
            }
            else if (POT1 / 32 == 3) {
              art_freq2 = 6; //0times
            }
          }
          if (abs(old_POT2 - POT2) > 10) {
            art_vel2 = POT2 / 16;
          }
          if (abs(old_POT4 - POT4) > 10) {
            
            calb2 = POT4;
          }
        }
        else if (mode == 2) {
          if (abs(old_POT1 - POT1) > 10) {
            oct2 = POT1 / 32;
          }
          if (abs(old_POT2 - POT2) > 10) {
            rev2 = POT2;
          }
          if (abs(old_POT3 - POT3) > 10) {
            cho2 = POT3;
          }
          if (abs(old_POT4 - POT4) > 10) {
            FX_wet2 = POT4;
          }
        }
      }
      timer = millis();
      param_set();
    };

    //--------------------analog read SW---------------------------
    toggle2 = analogRead(1);//select CH or push SW
    toggle1 = analogRead(0);

    if (old_toggle2 > 1023 && toggle2 <= 1023) {//push sw on
      if (mode == 0) {
        if (select_ch == 1) {
          select_bank1++;
          if (select_bank1 >= 12) {
            select_bank1 = 0;
          }
          uint8_t buf[2];
          buf[0] = 0xC1;//program change
          buf[1] = bank1[select_bank1]; // program number
          Midi.SendData(buf);

          delay(1);
        }
        else if (select_ch == 2) {
          select_bank2++;
          if (select_bank2 >= 12) {
            select_bank2 = 0;
          }
          uint8_t buf[2];
          buf[0] = 0xC2;//program change
          buf[1] = bank1[select_bank2]; // program number
          Midi.SendData(buf);

          delay(1);
        }
      }
      else if (mode == 2) {
        FX_select1++;//change effect fx
        if (FX_select1 >= 12) {
          FX_select1 = 0;
        }

        uint8_t buf1[10];//send fx midi data
        buf1[0] = 0xF0;
        buf1[1] = 0x43;
        buf1[2] = 0x10;
        buf1[3] = 0x4C;
        buf1[4] = 0x02;
        buf1[5] = 0x01;
        buf1[6] = 0x40;
        buf1[7] = FX_MSB[FX_select1];
        buf1[8] = FX_LSB[FX_select1];
        buf1[9] = 0xF7;
        Midi.SendData(buf1);
        delay(1);
      }
      else if (mode == 1) {
        save();//save flash mem
        delay(1000);
      }
    }
    else if (toggle2 > 4095 / 3 && toggle2 <= 4095 * 2 / 3) {
      select_ch = 1;
    }
    else if (toggle2 > 4095 * 2 / 3) {
      select_ch = 2;
    }

    if (toggle1 <= 4095 / 3) {
      mode = 1;
    }
    else if (toggle1 > 4095 / 3 && toggle1 <= 4095 * 2 / 3) {
      mode = 0;
    }
    else if (toggle1 > 4095 * 2 / 3) {
      mode = 2;
    }
    if (old_mode != mode || old_select_ch != select_ch) {
      old_POT1 = POT1;//old_POT for Disables POT reading at the moment of switching.
      old_POT2 = POT2;
      old_POT3 = POT3;
      old_POT4 = POT4;
    }

    //---------------------analog read quantize---------------------
    if (gate_delay1 == 1 && gate_delay_timer1 + 5 <= millis()) {//delay 5msec for adc latency
      AD_CH1 = analogRead(3) / 4 * (AD_CH1_calb + 0.0016 * calb1); //12bit to 10bit
      gate_delay1 = 2;//note on flug
      for ( search_qnt = 0; search_qnt <= 61 ; search_qnt++ ) {// quantize
        if ( AD_CH1 >= cv_qnt[search_qnt] && AD_CH1 < cv_qnt[search_qnt + 1]) {
          cmp1 = AD_CH1 - cv_qnt[search_qnt];
          cmp2 = cv_qnt[search_qnt + 1] - AD_CH1;
          break;
        }
      }
      if (cmp1 >= cmp2) {//quantize
        note1 = search_qnt + 1;
      }
      else if (cmp2 > cmp1) {//quantize
        note1 = search_qnt;
      }
    }

    if (gate_delay2 == 1 && gate_delay_timer2 + 5 <= millis()) {//delay 5msec for adc latency
      AD_CH2 = analogRead(9) / 4 * (AD_CH2_calb + 0.0016 * calb2); //12bit to 10bit
      gate_delay2 = 2;//note on flug
      for ( search_qnt2 = 0; search_qnt2 <= 61 ; search_qnt2++ ) {// quantize
        if ( AD_CH2 >= cv_qnt[search_qnt2] && AD_CH2 < cv_qnt[search_qnt2 + 1]) {
          cmp12 = AD_CH2 - cv_qnt[search_qnt2];
          cmp22 = cv_qnt[search_qnt2 + 1] - AD_CH2;
          break;
        }
      }
      if (cmp12 >= cmp22) {//quantize
        note2 = search_qnt2 + 1;
      }
      else if (cmp22 > cmp12) {//quantize
        note2 = search_qnt2;
      }
    }
    //---------------------trigger---------------------
    old_note_on1 = note_on1;
    old_note_on2 = note_on2;
    note_on1 = digitalRead(5);
    note_on2 = digitalRead(7);

    if (note_on1 == 1 && old_note_on1 == 0) {
      digitalWrite(12, LOW); //seeeduino xiao led on
      gate_delay1 = 1;//Timer count until CV stabilizes.
      gate_delay_timer1 = millis();//Timer count until CV stabilizes.
      art_cnt1++;//Articulation count up
      if (art_cnt1 > art_freq1) { //when count max
        art_cnt1 = 1;//Articulation reset
      }
      if (art_freq1 == 0) {
        art_sustime1 = 0;
      }
      else if (art_freq1 == 3) {
        art_sustime1 = art_sus3[art_sus1][art_cnt1];
      }
      else if (art_freq1 == 4) {
        art_sustime1 = art_sus4[art_sus1][art_cnt1];
      }
      else if (art_freq1 == 6) {
        art_sustime1 = art_sus6[art_sus1][art_cnt1];
      }
    }

    if (note_on2 == 1 && old_note_on2 == 0) {
      digitalWrite(11, LOW); //seeeduino xiao led on
      gate_delay2 = 1;
      gate_delay_timer2 = millis();
      art_cnt2++;
      if (art_cnt2 > art_freq2) { //when count max
        art_cnt2 = 1;
      }
    }


    if (note_on_timer1 + 100 + sus1 * 7 + art_sustime1 * sus1 / 4 <= millis() && note_off1 == 1) {
      digitalWrite(12, HIGH); //seeeduino xiao led off
      noteOff(note1 + 40);
      note_off1 = 0;
    }

    if (note_on_timer2 + 100 + sus2 * 7 <= millis() && note_off2 == 1) {
      digitalWrite(11, HIGH); //seeeduino xiao led off
      noteOff2(note2 + 40);
      note_off2 = 0;
    }

    if (gate_delay1 == 2) { //finish pitch adc
      noteOn(note1 + 24);
      note_on_timer1 = millis();
      note_off1 = 1;
      gate_delay1 = 0;
    }
    if (gate_delay2 == 2) { //finish pitch adc
      noteOn2(note2 + 24);
      note_on_timer2 = millis();
      note_off2 = 1;
      gate_delay2 = 0;
    }

  }
}

void noteOn(uint8_t note)//note on
{
  uint8_t buf[3];
  buf[0] = 0x91;// note on
  buf[1] = note + oct1 * 12; // note
  if (art_freq1 == 0) {
    buf[2] = 0x7f; // Velocity
  }
  else if (art_freq1 == 3) {
    buf[2] = 127 - art_vel3[art_vel1][art_cnt1]; // Velocity
  }
  else if (art_freq1 == 4) {
    buf[2] = 127 - art_vel4[art_vel1][art_cnt1]; // Velocity
  }
  else if (art_freq1 == 6) {
    buf[2] = 127 - art_vel6[art_vel1][art_cnt1]; // Velocity
  }

  Midi.SendData(buf);
}

void noteOff(uint8_t note)//note off
{
  uint8_t buf[3];
  buf[0] = 0xB1;//control change
  buf[1] = 123;//all note off
  buf[2] = 0x00;
  Midi.SendData(buf);
}

void noteOn2(uint8_t note)//note on
{
  uint8_t buf[3];
  buf[0] = 0x92;// note on
  buf[1] = note + oct2 * 12; // note
  if (art_freq2 == 0) {
    buf[2] = 0x7f; // Velocity
  }
  else if (art_freq2 == 3) {
    buf[2] = 127 - art_vel3[art_vel2][art_cnt2]; // Velocity
  }
  else if (art_freq2 == 4) {
    buf[2] = 127 - art_vel4[art_vel2][art_cnt2]; // Velocity
  }
  else if (art_freq2 == 6) {
    buf[2] = 127 - art_vel6[art_vel2][art_cnt2]; // Velocity
  }
  Midi.SendData(buf);
}

void noteOff2(uint8_t note)//note off
{
  uint8_t buf[3];
  buf[0] = 0xB2;//control change
  buf[1] = 123;//all note off
  buf[2] = 0x00;
  Midi.SendData(buf);
}

void param_set() {
  uint8_t buf[3];
  if (select_ch == 1) {
    if (mode == 0) {
      buf[0] = 0xB1;//control change
      buf[1] = 74; // Cf
      buf[2] = cutoff1;
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB1;//control change
      buf[1] = 07; // volume
      buf[2] = vol1; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB1;//control change
      buf[1] = 72; // release
      buf[2] = rel1; //
      Midi.SendData(buf);
      delay(1);
    }
    else if (mode == 2) {
      buf[0] = 0xB1;//control change
      buf[1] = 91; // rev
      buf[2] = rev1; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB1;//control change
      buf[1] = 93; // chorus wet
      buf[2] = cho1; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB1;//control change
      buf[1] = 0x5E; // FX wet
      buf[2] = FX_wet1; //
      Midi.SendData(buf);
      delay(1);
    }
  }
  else if (select_ch == 2) {
    if (mode == 0) {
      buf[0] = 0xB2;//control change
      buf[1] = 74; // Cf
      buf[2] = cutoff2;
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB2;//control change
      buf[1] = 07; // volume
      buf[2] = vol2; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB2;//control change
      buf[1] = 72; // release
      buf[2] = rel2; //
      Midi.SendData(buf);
      delay(1);
    }
    else if (mode == 2) {
      buf[0] = 0xB2;//control change
      buf[1] = 91; // rev
      buf[2] = rev2; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB2;//control change
      buf[1] = 93; // chorus wet
      buf[2] = cho2; //
      Midi.SendData(buf);
      delay(1);

      buf[0] = 0xB2;//control change
      buf[1] = 0x5E; // FX wet
      buf[2] = FX_wet2; //
      Midi.SendData(buf);
      delay(1);
    }
  }
}

void initial_setting() {
  if (EEPROM.isValid() == 1) { //already writed eeprom
    cutoff1 = EEPROM.read(1);
    sus1 = EEPROM.read(2);
    rel1 = EEPROM.read(3);
    rev1 = EEPROM.read(4);
    oct1 = EEPROM.read(5);
    cho1 = EEPROM.read(6);
    vol1 = EEPROM.read(7);
    calb1 = EEPROM.read(8);
    FX_wet1 = EEPROM.read(9);
    FX_select1 = EEPROM.read(10);
    select_bank1 = EEPROM.read(11);
    cutoff2 = EEPROM.read(12);
    sus2 = EEPROM.read(13);
    rel2 = EEPROM.read(14);
    rev2 = EEPROM.read(15);
    oct2 = EEPROM.read(16);
    cho2 = EEPROM.read(17);
    vol2 = EEPROM.read(18);
    calb2 = EEPROM.read(19);
    FX_wet2 = EEPROM.read(20);
    FX_select2 = EEPROM.read(21);
    select_bank2 = EEPROM.read(22);
  }
  else if ( EEPROM.isValid() == 0) { //no eeprom data , setting any number to eeprom
    cutoff1 = 20;
    sus1 = 20;
    rel1 = 80;
    rev1 = 100;
    oct1 = 3;
    cho1 = 63;
    vol1 = 127;
    calb1 = 50;
    FX_wet1 = 30;
    FX_select1 = 0;
    select_bank1 = 3;

    cutoff2 = 20;
    sus2 = 20;
    rel2 = 80;
    rev2 = 100;
    oct2 = 3;
    cho2 = 63;
    vol2 = 127;
    calb2 = 80;
    FX_wet2 = 30;
    FX_select2 = 0;
    select_bank2 = 0;
  }

  delay(10);
  uint8_t buf1[9];//system effect_FX setting
  buf1[0] = 0xF0;
  buf1[1] = 0x43;
  buf1[2] = 0x10;
  buf1[3] = 0x4C;
  buf1[4] = 0x02;
  buf1[5] = 0x01;
  buf1[6] = 0x5A;
  buf1[7] = 0x01;
  buf1[8] = 0xF7;
  Midi.SendData(buf1);

  delay(2);

  buf1[0] = 0xF0;//FX return
  buf1[1] = 0x43;
  buf1[2] = 0x10;
  buf1[3] = 0x4C;
  buf1[4] = 0x02;
  buf1[5] = 0x01;
  buf1[6] = 0x56;
  buf1[7] = 0x7F;
  buf1[8] = 0xF7;
  Midi.SendData(buf1);

  delay(2);

  uint8_t buf[3];

  buf[0] = 0xB1;//control change
  buf[1] = 74; // Cf
  buf[2] = cutoff1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB1;//control change
  buf[1] = 07; // volume
  buf[2] = vol1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB1;//control change
  buf[1] = 72; // release
  buf[2] = rel1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB1;//control change
  buf[1] = 91; // rev
  buf[2] = rev1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB1;//control change
  buf[1] = 93; // chorus wet
  buf[2] = cho1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB1;//control change
  buf[1] = 0x5E; // FX wet
  buf[2] = FX_wet1; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 74; // Cf
  buf[2] = cutoff2; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 07; // volume
  buf[2] = vol2; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 72; // release
  buf[2] = rel2; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 91; // rev
  buf[2] = rev2; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 93; // chorus wet
  buf[2] = cho2; //
  Midi.SendData(buf);
  delay(1);

  buf[0] = 0xB2;//control change
  buf[1] = 0x5E; // FX wet
  buf[2] = FX_wet2; //
  Midi.SendData(buf);
  delay(1);

  uint8_t buf2[10];
  buf2[0] = 0xF0;
  buf2[1] = 0x43;
  buf2[2] = 0x10;
  buf2[3] = 0x4C;
  buf2[4] = 0x02;
  buf2[5] = 0x01;
  buf2[6] = 0x40;
  buf2[7] = FX_MSB[FX_select1];
  buf2[8] = FX_LSB[FX_select1];
  buf2[9] = 0xF7;
  Midi.SendData(buf2);
  delay(1);

  uint8_t buf3[2];
  buf3[0] = 0xC1;//program change
  buf3[1] = bank1[select_bank1]; // program number
  Midi.SendData(buf3);

  buf3[0] = 0xC2;//program change
  buf3[1] = bank1[select_bank2]; // program number
  Midi.SendData(buf3);
}

void save() {
  EEPROM.write(1, cutoff1);
  EEPROM.write(2, sus1);
  EEPROM.write(3, rel1);
  EEPROM.write(4, rev1);
  EEPROM.write(5, oct1);
  EEPROM.write(6, cho1);
  EEPROM.write(7, vol1);
  EEPROM.write(8, calb1);
  EEPROM.write(9, FX_wet1);
  EEPROM.write(10, FX_select1);
  EEPROM.write(11, select_bank1);
  EEPROM.commit();
  delay(100);
}