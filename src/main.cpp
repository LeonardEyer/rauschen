#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <utils.h>
#include <AnalogSmooth.h>
#include <Patching.h>

#define PWR_LED 13
#define SWITCH1 2
#define SWITCH2 3
#define CTL_FREQ true

// Smoothing analog reads (window size 10)
AnalogSmooth as1 = AnalogSmooth(10);
AnalogSmooth as2 = AnalogSmooth(10);
AnalogSmooth as3 = AnalogSmooth(10);
AnalogSmooth as4 = AnalogSmooth(10);
AnalogSmooth as5 = AnalogSmooth(10);

bool lastSwitchState = false;
unsigned long SerialMillisecondCounter;


/**
 * @brief Debug printing of Peak and RMS info
 * 
 */
void print_peak_rms()
{
  if (peak.available() && rms.available())
  {
    Serial.printf("Peak: %f \t RMS: %f\n", peak.read(), rms.read());
    //amp1.gain(as1.smooth(rms.read()));
  }
}

/**
 * @brief Control frequency of internal oscillators
 * 
 * @param f1 frequency one (between 0 and 1)
 * @param f2 frequency two (between 0 and 1)
 */
void controlFrequency(float f1, float f2)
{
  f1 = toFreq(f1);
  f2 = toFreq(f2);

  // Serial.printf("FREQ: %f \t %f\n", f1, f2);

  AudioNoInterrupts();
  wave.frequency(f1);
  wave2.frequency(f2);
  AudioInterrupts();
}

/**
 * @brief Control amplitude of internal oscillators
 * 
 * @param a1 amplitude 1
 * @param a2 amplitude 2
 */
void controlAmp(float a1, float a2)
{

  // Serial.printf("AMP: %f \t %f\n", a1, a2);

  AudioNoInterrupts();
  wave.amplitude(a1);
  wave2.amplitude(a2);
  AudioInterrupts();
}

/**
 * @brief Control adaptive filter parameters
 * 
 * @param taps change number of taps
 * @param mu change learning rate
 */
void controlFilter(float taps, float mu)
{

  // Desired range: 1 --> 0.0001
  mu = 0.001 + mu * 0.1;
  uint16_t tapCount = 1 + (uint16_t)(round(taps * 50.0));

  Serial.printf("Mu: %f \t Taps: %d\n", mu, tapCount);

  AudioNoInterrupts();
  filter.set_mu(mu);
  // filter.set_taps(tapCount);
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

  AudioMemory(30);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.3); // caution: very loud - use oscilloscope only!
  sgtl5000_1.inputSelect(myInput);

  sgtl5000_1.lineInLevel(0);   // 3.12 volts p-p
  sgtl5000_1.lineOutLevel(31); // 3.16 volts p-p

  Serial.println("Audio Shield connected");

  // Setup internal waveforms
  wave.begin(WAVEFORM_SQUARE);
  wave.frequency(110.0);
  wave.amplitude(.5);

  wave2.begin(WAVEFORM_SAWTOOTH);
  wave2.frequency(110.0);
  wave2.amplitude(.5);

  filter.begin(10, 0.01);

  delay(2000);
  notreFreq.begin(0.15);

  SerialMillisecondCounter = millis();
}

/**
 * @brief Detect incoming signal not frequency
 * 
 * @return float frequency
 */
float frequencyDetection() {
    float freq = notreFreq.read();
    float prob = notreFreq.probability();
    Serial.printf("Note freq:%f\t prob: %f\n", freq, prob);
    return freq;
}


void loop()
{
  // Smooth reading of knob values
  float knob4 = normalize(as4.analogReadSmooth(A0));
  float knob3 = normalize(as3.analogReadSmooth(A2));
  float knob2 = normalize(as2.analogReadSmooth(A3), 545);
  float knob1 = normalize(as1.analogReadSmooth(A8), 545);

  if (millis() - SerialMillisecondCounter >= 5000)
  {
    //print_stats();
    SerialMillisecondCounter = millis();
    // Serial.printf("Knobs: %f \t %f \t %f \t %f \n", knob1, knob2, knob3, knob4);
  }

  // if(notreFreq.available()) {

  //   float freq = round(frequencyDetection() * knob3);
  //   wave2.frequency(freq);
  // }

  controlFrequency(knob4, knob3);
  // // controlAmp(knob3, knob4);

  // Knob 1 and 2 control the amplitude of signals
  amp1.gain(knob1);
  amp2.gain(knob2);
  // controlFilter(0, knob4);
  // bool switchState = digitalRead(SWITCH2);
  // if (switchState != lastSwitchState)
  // {
  //   lastSwitchState = switchState;
  // }
}