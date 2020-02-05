#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "AdaptiveFilter.h"
#include <utils.h>
#include <AnalogSmooth.h>

#define PWR_LED 13
#define SWITCH1 2
#define SWITCH2 3
#define CTL_FREQ true

// I/O
AudioInputI2S i2s0;
AudioOutputI2S i2s1;

// // Internal oscillators
// AudioSynthWaveform wave;
AudioSynthWaveform wave2;

// The filter
AdaptiveFilter filter;

// Amps for attenuation
AudioAmplifier amp1;
AudioAmplifier amp2;

// Analysis stuff
AudioAnalyzePeak peak;
AudioAnalyzeRMS rms;

// Reading Audio INput
AudioConnection patchCord1(i2s0, 0, amp1, 0);
AudioConnection patchCord2(i2s0, 1, amp2, 0);

// Patching into Filter
AudioConnection patchCord3(i2s0, 0, filter, 0);
AudioConnection patchCord4(wave2, 0 , filter, 1);

// Patching out of filter
AudioConnection patchCord5(filter, 0, i2s1, 0);
AudioConnection patchCord6(filter, 0, i2s1, 1);

// Analysis patching
AudioConnection patchCordX(i2s0, 0, peak, 0);
AudioConnection patchCordY(i2s0, 0, rms, 0);

AudioControlSGTL5000 sgtl5000_1;
const int myInput = AUDIO_INPUT_LINEIN;

// Smoothing analog reads (window size 10)
AnalogSmooth as1 = AnalogSmooth(10);
AnalogSmooth as2 = AnalogSmooth(10);
AnalogSmooth as3 = AnalogSmooth(10);
AnalogSmooth as4 = AnalogSmooth(10);

bool lastSwitchState = false;
unsigned long SerialMillisecondCounter;

// void controlFrequency(float f1, float f2)
// {
//   f1 = toFreq(f1);
//   f2 = toFreq(f2);

//   //Serial.printf("FREQ: %f \t %f\n", f1, f2);

//   AudioNoInterrupts();
//   wave.frequency(f1);
//   wave2.frequency(f2);
//   AudioInterrupts();
// }

void controlAmp(float a1, float a2)
{

  // Serial.printf("AMP: %f \t %f\n", a1, a2);

  AudioNoInterrupts();
  // wave.amplitude(a1);
  wave2.amplitude(a2);
  AudioInterrupts();
}

void controlFilter(float taps, float mu)
{

  // Desired range: 1 --> 0.0001
  mu = 0.001 + mu * 0.7;
  uint16_t tapCount = 1 + (uint16_t)(round(taps * 50.0));

  //Serial.printf("Mu: %f \t Taps: %d\n", mu, tapCount);

  AudioNoInterrupts();
  filter.set_mu(mu);
  filter.set_taps(tapCount);
  AudioInterrupts();
}

void setup()
{
  // debug output at 9600 baud
  Serial.begin(9600);

  delay(2000);

  //Setup power LED
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH);

  // Setup switches
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);

  AudioMemory(16);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.3); // caution: very loud - use oscilloscope only!
  sgtl5000_1.inputSelect(myInput);
  
  sgtl5000_1.lineInLevel( 0 );  // 3.12 volts p-p
  sgtl5000_1.lineOutLevel( 13 );  // 3.16 volts p-p

  Serial.println("Audio Shield connected");

  wave2.begin(WAVEFORM_SAWTOOTH);
  wave2.frequency(110.0);
  wave2.amplitude(0.5);

  filter.begin(10, 0.01);

  SerialMillisecondCounter = millis();
}

void loop()
{
  float knob4 = normalize(as4.analogReadSmooth(A0));
  float knob3 = normalize(as3.analogReadSmooth(A2));
  float knob2 = normalize(as2.analogReadSmooth(A3));
  float knob1 = normalize(as1.analogReadSmooth(A8));

  if (millis() - SerialMillisecondCounter >= 5000) {
    print_stats();
    SerialMillisecondCounter = millis();
    // Serial.printf("Knobs: %f \t %f \t %f \t %f \n", knob1, knob2, knob3, knob4);
  } 

  // controlFrequency(knob4, knob3);
  // controlAmp(knob3, knob4);

  // amp1.gain(knob1);
  // amp2.gain(knob2);

  // bool switchState = digitalRead(SWITCH2);

  // if (switchState != lastSwitchState)
  // { 
  //   lastSwitchState = switchState;
  // }
  // if (peak.available() && rms.available())
  // {
  //   Serial.printf("Peak: %f \t Gain: %f \t RMS: %f\n", peak.read(), knob2, rms.read());
  //   //amp1.gain(as1.smooth(rms.read()));
  // }
}