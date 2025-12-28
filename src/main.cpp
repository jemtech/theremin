#include <Arduino.h>
#include <CapacitiveSensor.h>
#include <Audio.h>

#define DEBUG

#define FREQ_MUMLTIPLY 0.1
#define AUDIO_LEVEL_MUMLTIPLY 0.0001
#define AVERAGE_CYCL 10

/*
loading pin 2 (22MOhm)
reading pin 3
*/
CapacitiveSensor audioAntenna(2, 3);
/*
loading pin 4 (22MOhm)
reading pin 5
*/
CapacitiveSensor freqAntenna(4, 5);
/*
LRCLK: Pin 20/A6
BCLK: Pin 21/A7
DIN: Pin 7
Ground: Ground
VIN: 3.3v or VIN
*/
AudioOutputI2S i2s;
AudioSynthWaveform waveform;
AudioConnection patchCord2(waveform, 0, i2s, 0);

/*
raw value in time to load capacative freqency antenna
@return time in ms
*/
long mesureFreqAntenna(void) {
    return freqAntenna.capacitiveSensor(1);
}

/*
raw value in time to load capacative audio level antenna
@return time in ms
*/
long mesureAudioAntenna(void) {
    return audioAntenna.capacitiveSensor(1);
}


/*
WAVEFORM_SINE
WAVEFORM_SAWTOOTH
WAVEFORM_BANDLIMIT_SAWTOOTH
WAVEFORM_SAWTOOTH_REVERSE
WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE
WAVEFORM_SQUARE
WAVEFORM_BANDLIMIT_SQUARE
WAVEFORM_TRIANGLE
WAVEFORM_TRIANGLE_VARIABLE
WAVEFORM_ARBITRARY
WAVEFORM_PULSE
WAVEFORM_BANDLIMIT_PULSE
WAVEFORM_SAMPLE_HOLD
*/
void changeWaveForm(short waveForm) {
    waveform.begin(waveForm);
}
/*
capacity on startup in ms
*/
long freqAntennaOffset = 0;
/*
capacity on startup in ms
*/
long audioAntennaOffset = 0;
/* setup function */
void setup(void){
    Serial.begin(115200);
    #ifdef DEBUG
    Serial.println("Boot");
    #endif
    AudioMemory(24);
    changeWaveForm(WAVEFORM_SINE);
    freqAntennaOffset = freqAntenna.capacitiveSensor(20);
    audioAntennaOffset = audioAntenna.capacitiveSensor(20);
}

long freqVal = 0;
float audioVal = 0;
/* main loop */
void loop(void) {
    #ifdef DEBUG
    Serial.println("Loop");
    #endif
    freqVal = (((mesureFreqAntenna() - freqAntennaOffset) * FREQ_MUMLTIPLY) + (freqVal * (AVERAGE_CYCL -1 ))) / AVERAGE_CYCL;
    waveform.frequency(freqVal);
    #ifdef DEBUG
    Serial.println(freqVal);
    #endif
    audioVal = (((mesureAudioAntenna() - audioAntennaOffset) * AUDIO_LEVEL_MUMLTIPLY) + (audioVal * (AVERAGE_CYCL - 1))) / AVERAGE_CYCL;
    waveform.amplitude(audioVal);
    #ifdef DEBUG
    Serial.println(audioVal);
    #endif
    delay(1);
}
