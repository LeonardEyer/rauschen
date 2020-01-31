#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "AdaptiveFilter.h"

#define PWR_LED 13
#define CTL_FREQ true

AudioOutputI2S i2s1;
AudioSynthWaveform wave;
AudioSynthWaveform wave2;
AdaptiveFilter filter;
AudioConnection patchCord1(wave, 0, filter, 0);
AudioConnection patchCord2(wave2, 0, filter, 1);
AudioConnection patchCord3(filter, 0, i2s1, 0);
AudioConnection patchCord4(filter, 0, i2s1, 1);

AudioControlSGTL5000 sgtl5000_1;
const int myInput = AUDIO_INPUT_LINEIN;

void setup()
{
  // debug output at 9600 baud
  Serial.begin(9600);

  delay(300);
  //Setup power LED
  pinMode(PWR_LED, OUTPUT);
  digitalWrite(PWR_LED, HIGH);

  AudioMemory(20);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  sgtl5000_1.enable();
  sgtl5000_1.volume(.3); // caution: very loud - use oscilloscope only!
  sgtl5000_1.inputSelect(myInput);
  Serial.println("Audio Shield connected");

  wave.begin(WAVEFORM_SQUARE);
  wave.frequency(110.0);
  wave.amplitude(.5);

  wave2.begin(WAVEFORM_SAWTOOTH);
  wave2.frequency(110.0);
  wave2.amplitude(.5);

  filter.begin(10, 0.01);
  delay(1000);
}

float power(float base, int exp) {

  float ans = 1.0;

  for(int i=0; i<exp; i++)
      ans *= base;

  return ans;
}

float round_x(float var, int decimals)
{
  // 37.66666 * 100 =3766.66
  // 3766.66 + .5 =3767.16    for rounding off value
  // then type cast to int so value is 3767
  // then divided by 100 so the value converted into 37.67

  int precision = power(10.0, decimals);

  float value = (int)(var * precision + .5);
  return (float)value / precision;
}

float normalize(float in)
{
  return round_x(1.0 - in / 1023.0, 2);
}

float toFreq(float val)
{
  return 10.0 + val * 500.0;
}

void controlFrequency(float f1, float f2)
{
  f1 = toFreq(f1);
  f2 = toFreq(f2);

  Serial.printf("Freq: %f \t %f\n", f1, f2);

  AudioNoInterrupts();
  wave.frequency(f1);
  //wave2.frequency(f2);
  AudioInterrupts();
}

void controlFilter(float taps, float mu) {

  // Desired range: 1 --> 0.0001
  mu = 0.001 + mu * 0.7;
  uint16_t tapCount = 1 + (uint16_t) (round_x(taps * 50.0, 0));

  Serial.printf("Mu: %f \t Taps: %d\n", mu, tapCount);

  AudioNoInterrupts();
  filter.set_mu(mu);
  filter.set_taps(tapCount);
  AudioInterrupts();
}

void loop()
{
  float knob0 = normalize(analogRead(A6));
  float knob1 = normalize(analogRead(A2));

  if (CTL_FREQ) controlFrequency(knob0, knob1);
  else controlFilter(knob0, knob1);
}
