#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_midi.h>
#include "capsensor.h"
#include "sounds.h"
#include "scales.h"
#include "notes.h"


int freq = -100;
int count = 0;
int dcount = 0;
int hcount = 0;
int decay = 127;
float f1, f2, f3, f4, f5, f6, f7, f8;
int sensorValue = 0;
int gsr_average = 0;
int currScale = 0;



void setup() {
  // select base frequencies using mtof
  // C F♯ B♭ E A D the "Promethean chord"
  f1 = mtof(48.f);
  f2 = mtof(54.f);
  f3 = mtof(58.f);
  f4 = mtof(64.f);
  f5 = mtof(69.f);
  f6 = mtof(74.f);
  f7 = mtof(79.f);
  f8 = mtof(84.f);


  // set Oscils with chosen frequencies
  aCos1.setFreq(f1);
  aCos2.setFreq(f2);
  aCos3.setFreq(f3);
  aCos4.setFreq(f4);
  aCos5.setFreq(f5);
  aCos6.setFreq(f6);
  aCos7.setFreq(f7);
  aCos8.setFreq(f8);

  v1 = v2 = v3 = v4 = v5 = v6 = v7 = v8 = decay;

  startMozzi();
  Serial.begin(115200);

}

void setPanFreq() {

  aCos1.setFreq(freq);
  aCos2.setFreq(freq + 14);
  aCos3.setFreq(freq + 4);
  aCos4.setFreq(freq + 17);
  aCos5.setFreq(freq + 7);
  aCos6.setFreq(freq + 21);
  aCos7.setFreq(freq);
  aCos8.setFreq(freq + 24);

  v1 = v2 = v3 = v4 = v5 = v6 = v7 = v8 = decay;
}

void touchCheck()
{
  // get cap sensor
  long pad0 =  cs_23_22.capacitiveSensor(30);
  long pad1 =  cs_25_24.capacitiveSensor(30);
  long pad2 =  cs_27_26.capacitiveSensor(30);
  long pad3 =  cs_29_28.capacitiveSensor(30);
  long pad4 =  cs_31_30.capacitiveSensor(30);
  long pad5 =  cs_33_32.capacitiveSensor(30);
  long pad6 =  cs_35_34.capacitiveSensor(30);
  long pad7 =  cs_37_36.capacitiveSensor(30);

  if (pad0 > 80) {
    freq = note_frequency[scale[currScale][0]];
    setPanFreq();
  }

  if (pad1 > 80) {
    freq = note_frequency[scale[currScale][1]];
    setPanFreq();
  }

  if (pad2 > 80) {
    freq = note_frequency[scale[currScale][2]];
    setPanFreq();
  }

  if (pad3 > 80) {
    freq = note_frequency[scale[currScale][3]];
    setPanFreq();
  }

  if (pad4 > 80) {
    freq = note_frequency[scale[currScale][4]];
    setPanFreq();
  }

  if (pad5 > 80) {
    freq = note_frequency[scale[currScale][5]];
    setPanFreq();
  }

  if (pad6 > 80) {
    freq = note_frequency[scale[currScale][6]];
    setPanFreq();
  }

  if (pad7 > 80) {
    freq = note_frequency[scale[currScale][7]];
    setPanFreq();
  }

  //  Serial.print(pad0);
  //  Serial.print("\t");
  //  Serial.print(pad1);
  //  Serial.print("\t");
  //  Serial.print(pad2);
  //  Serial.print("\t");
  //  Serial.print(pad3);
  //  Serial.print("\t");
  //  Serial.print(pad4);
  //  Serial.print("\t");
  //  Serial.print(pad5);
  //  Serial.print("\t");
  //  Serial.print(pad6);
  //  Serial.print("\t");
  //  Serial.println(pad7);

}

void loop() {
  audioHook();

}


void updateControl() {
  // gsr
  if (dcount == 0) {
    long sum = 0;
    for (int i = 0; i < 10; i++)    //Average the 10 measurements to remove the glitch
    {
      sensorValue = mozziAnalogRead(0);
      sum += sensorValue;
      delay(5);
    }
    gsr_average = sum / 10;
    int dc = map(gsr_average, 90, 200, 0, 250);
    decay = dc;

    //    Serial.println(gsr_average);
    Serial.print("Decay: ");
    Serial.println(decay);
  }

  // touch
  if (count == 0) {

    touchCheck();

  }

  // ppg
  if (hcount == 0) {
    long hr = 0;
    int value = 0;
    int sval = 0;
    int hrate = 0;
    for (int i = 0; i < 10; i++)    //Average the 10 measurements to remove the glitch
    {
      value = mozziAnalogRead(A1);
      sval += value;
      delay(5);
    }
    hrate = sval / 10;

    currScale = hrate % 5;

    Serial.print("Scale: ");
    Serial.println(currScale);
  }


  count ++;
  dcount ++;
  hcount ++;


  if (v1 != 0) {
    v1 --;
    v2 --;
    v3 --;
    v4 --;
    v5 --;
    v6 --;
    v7 --;
    v8 --;
  }


  if (count == 5) {
    count = 0;
  }
  if (dcount == 80) {
    dcount = 0;
  }
  if (hcount == 30) {
    hcount = 0;
  }
}

AudioOutput_t updateAudio() {
  long asig = (long)
              aCos1.next() * v1 +
              aCos2.next() * v2 +
              aCos3.next() * v3 +
              aCos4.next() * v4 +
              aCos5.next() * v5 +
              aCos6.next() * v6 +
              aCos7.next() * v7 +
              aCos8.next() * v8;
  return MonoOutput::fromAlmostNBit(18, asig);
}
