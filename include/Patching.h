#include <Audio.h>
#include "AdaptiveFilter.h"

// I/O
AudioInputI2S i2s0;
AudioOutputI2S i2s1;

// // Internal oscillators
AudioSynthWaveform wave;
AudioSynthWaveform wave2;

// The filter
AdaptiveFilter filter;

// Amps for attenuation
AudioAmplifier amp1;
AudioAmplifier amp2;

// Analysis stuff
AudioAnalyzePeak peak;
AudioAnalyzeRMS rms;
AudioAnalyzeNoteFrequency notreFreq;

// Reading Audio INput
AudioConnection patchCord1(i2s0, 0, amp1, 0);
AudioConnection patchCord2(i2s0, 1, amp2, 0);

// Patching into Filter
AudioConnection patchCord3(amp1, 0, filter, 0);
AudioConnection patchCord4(amp2, 0 , filter, 1);

// Patching out of filter
AudioConnection patchCord5(filter, 0, i2s1, 0);
AudioConnection patchCord6(filter, 0, i2s1, 1);

// Analysis patching
// AudioConnection patchCordX(i2s0, 0, peak, 0);
// AudioConnection patchCordY(i2s0, 0, rms, 0);
AudioConnection patchCordZ(i2s0, 0, notreFreq, 0);

AudioControlSGTL5000 sgtl5000_1;
const int myInput = AUDIO_INPUT_LINEIN;