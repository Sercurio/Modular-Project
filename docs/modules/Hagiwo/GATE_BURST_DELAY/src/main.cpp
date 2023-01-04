#include <MsTimer2.h>

//specification
//ディレイ中にゲート入力があった場合、ゲート出力は0のままディレイはリセットされる。
//ワイズタイム中にゲート入力があった場合、ゲート出力は0になりディレイからリスタートする。

//ゲート入出力
unsigned long ms_count = 0;//タイマーのカウント。ゲートINで0になり、カウントを始める
byte gate_in = 0 ;//外部ゲート入力状況。0=LOW,1=HIGH。
byte old_gate_in = 0;//
byte gate_detect_latch = 0;//ゲート出力が完了するまで1のまま。
byte gate_out = 0;//ゲート出力可否。0=LOW,1=HIGH
byte sw_in = 0;//メカswの入力
byte old_sw_in = 0;
byte onoff_in = 1;//onoff入力。内部プルアップのため、1=ON又はジャック未接続,0=OFF,

//モード選択
byte mode = 0;//1=delayモード,0=glitchモード

//delayモード
byte delay_on = 0;//ゲート出力。0=LOW,1=HIGH。ディレイ時間カウント後に1になる。
int delay_knob = 0;//knob1から読み取った値
int delay_CV = 0;//CVから読み取った値
int delay_time = 0;//knobとCVの合算値（計算式は別に記載)
int width_knob = 0;//knob2から読み取った値
int width = 1;//ゲート出力時間

//glitchモード
byte times = 1;//バーストの回数,knobとCVの合算値（計算式は別に記載)
int times_knob = 0;//
int times_CV = 0;//
int freq = 0;//
int time_onoff = 0;//
byte time_count = 1;//出力したら1増える。timesに達したら0に戻り、gate_latchを0にする


void setup() {

 pinMode(2, INPUT);//外部クロックIN
 pinMode(3, INPUT_PULLUP);//mode選択
 pinMode(4, INPUT_PULLUP);//PUSH SW
 pinMode(6, OUTPUT);//GATE OUT
 pinMode(9, INPUT_PULLUP);//ON_OFF GATE IN


 //開発用シリアル通信
 //Serial.begin(9600);

 //Timer用設定
 MsTimer2::set(1, timer_count); // 1ms毎にタイマーカウント
 MsTimer2::start();//外部入力Highになったら、次のHighまでカウント
}

void loop() {
 old_gate_in = gate_in ;//gate0→1を検出する用
 old_sw_in = sw_in;

 //-------------モード選択-----------------
 mode = digitalRead(3);//1=delayモード,0=glitchモード

 //-------------on/off_in判定-----------------
 onoff_in = digitalRead(9);

 if (onoff_in == 1) {

   //--------------------delayモード------------------------
   if ( mode == 1) {
     //delay設定
     delay_knob = analogRead(1);
     delay_CV = analogRead(6);
     delay_time = delay_knob + delay_CV;

     if (delay_time > 1023) {
       delay_time = 1023;
     }

     //width設定
     width_knob = analogRead(2);
     width = width_knob * 4 + 1 ;//1ms～4097msの値をとる

     //外部ゲート入力検出
     gate_in = digitalRead(2);
     sw_in = digitalRead(4);//internal pullupのため0=ON,1=OFF

     if (gate_in == 1 && old_gate_in == 0) {
       gate_detect_latch = 1;
       ms_count = 0;
     }

     else if (sw_in == 0 && old_sw_in == 1) {
       gate_detect_latch = 1;
       ms_count = 0;
     }

     //ゲート出力判定

     if ( gate_detect_latch == 1 && ms_count >= delay_time) {
       gate_out = 1;
     }

     else {
       gate_out = 0;
     }

     if (gate_detect_latch == 1 && ms_count >= delay_time + width) {
       gate_detect_latch = 0;
     }

     //ゲート出力
     if (gate_out == 1 ) {
       digitalWrite(6, HIGH);
     }
     else {
       digitalWrite(6, LOW);
     }
   }
   //--------------------glitchモード------------------------

   if ( mode == 0) {

     //delay設定
     freq = analogRead(1) / 2 + 5; //5～512msの値
     time_onoff = 5 + freq;//ON時間が5ms、OFF時間がFREQ

     //times設定
     times_CV =  analogRead(6);
     times_knob = analogRead(2);
     times = (times_CV + times_knob) / 32 + 1; //1～33の値

     if (times > 33) {
       times = 32;
     }

     //外部ゲート入力検出
     gate_in = digitalRead(2);
     sw_in = digitalRead(4);//internal pullupのため0=ON,1=OFF

     if (gate_in == 1 && old_gate_in == 0) {
       gate_detect_latch = 1;
       time_count = 0;
     }

     else if (sw_in == 0 && old_sw_in == 1) {
       gate_detect_latch = 1;
       time_count = 0;
     }

     //ゲート出力
     if (gate_detect_latch == 1 && time_count <= times) {

       if (ms_count <= time_onoff / 2 ) {
         gate_out = 1;
       }

       else if (ms_count < time_onoff && ms_count > time_onoff / 2) {
         gate_out = 0;
       }

       else if (ms_count >= time_onoff ) {
         ms_count = 0;
         time_count++;
       }
     }
     if (gate_out == 1 ) {
       digitalWrite(6, HIGH);
     }
     else {
       digitalWrite(6, LOW);
     }
   }
 }

 if (onoff_in == 0) {
   digitalWrite(6, LOW);
 }
 // 開発用　serial通信すると、ゲート出力動作が不安定になる。
 //  Serial.print(sw_in);
 //  Serial.print(",");
 //  Serial.print(gate_in * 10);
 //  Serial.println("");
}

//タイマーカウント
void timer_count() {
 ms_count ++;
}