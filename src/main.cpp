#include <Arduino.h>
#include <CapacitiveSensor.h>
#include <Audio.h>

#define DEBUG

#define FREQ_MULTIPLY 0.1
#define AUDIO_LEVEL_MULTIPLY 0.0001
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
raw value in time to load capacitive frequency antenna
@return time in ms
*/
long measureFreqAntenna(void) {
    return freqAntenna.capacitiveSensor(1);
}

/*
raw value in time to load capacitive audio level antenna
@return time in ms
*/
long measureAudioAntenna(void) {
    return audioAntenna.capacitiveSensor(1);
}

/*
raw ADC value
@return int 0 to 1023
*/
int measureWaveForm(void) {
    return analogRead(14);
}
/*
calculates based on a value between 0 and 1024 the selected waveform
*/
short convertToWaveform(int measure) {
    int selection = measure / 93;
    switch (selection) {
        case 0:
            return WAVEFORM_SINE;
        case 1:
            return WAVEFORM_SAWTOOTH;
        case 2:
            return WAVEFORM_BANDLIMIT_SAWTOOTH;
        case 3:
            return WAVEFORM_SAWTOOTH_REVERSE;
        case 4:
            return WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE;
        case 5:
            return WAVEFORM_SQUARE;
        case 6:
            return WAVEFORM_BANDLIMIT_SQUARE;
        case 7:
            return WAVEFORM_TRIANGLE;
        case 8:
            return WAVEFORM_TRIANGLE_VARIABLE;
        case 9:
            return WAVEFORM_PULSE;
        case 10:
            return WAVEFORM_BANDLIMIT_PULSE;
        default:
            return WAVEFORM_SINE;
    }
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
calculates rolling average based on AVERAGE_CYCL
*/
int average(int newVal, int oldVal) {
    return (newVal + (oldVal * (AVERAGE_CYCL -1))) / AVERAGE_CYCL;
}
/*
capacity on startup in ms
*/
long freqAntennaOffset = 0;
/*
capacity on startup in ms
*/
long audioAntennaOffset = 0;
/*
measures and stores base capacity off the antennas
*/
void measureAntennaOffsets(void){
    freqAntennaOffset = measureFreqAntenna();
    audioAntennaOffset = measureAudioAntenna();
    delay(1);
    for (int i = 0; i < AVERAGE_CYCL; i++) {
        freqAntennaOffset = average(measureFreqAntenna(), freqAntennaOffset);
        audioAntennaOffset = average(measureAudioAntenna(), audioAntennaOffset);
        delay(1);
    }
}
/* setup function */
void setup(void){
    Serial.begin(115200);
    #ifdef DEBUG
    Serial.println("Boot");
    #endif
    AudioMemory(24);
    changeWaveForm(WAVEFORM_SINE);
    measureAntennaOffsets();
}

long freqVal = 0;
float audioVal = 0;
/* main loop */
void loop(void) {
    #ifdef DEBUG
    Serial.println("Loop");
    #endif
    freqVal = average(((measureFreqAntenna() - freqAntennaOffset) * FREQ_MULTIPLY), freqVal);
    waveform.frequency(freqVal);
    #ifdef DEBUG
    Serial.println(freqVal);
    #endif
    audioVal = average(((measureAudioAntenna() - audioAntennaOffset) * AUDIO_LEVEL_MULTIPLY), audioVal);
    waveform.amplitude(audioVal);
    #ifdef DEBUG
    Serial.println(audioVal);
    #endif
    int waveRaw = measureWaveForm();
    #ifdef DEBUG
    Serial.println(waveRaw);
    #endif
    short waveForm = convertToWaveform(waveRaw);
    changeWaveForm(waveForm);
    #ifdef DEBUG
    Serial.println(waveForm);
    #endif
    delay(1);
}
