
////// Configuration ////////

// Default MIDI channel for pedal CC events
#define DEFAULT_MIDI_CHANNEL 1

// Sustain
#define PEDAL1_PIN 33
#define PEDAL1_MIDI_CC 64
#define PEDAL1_MIDI_CHANNEL DEFAULT_MIDI_CHANNEL

// Sostenuto
#define PEDAL2_PIN 32
#define PEDAL2_MIDI_CC 66
#define PEDAL2_MIDI_CHANNEL DEFAULT_MIDI_CHANNEL

// Una corda
#define PEDAL3_PIN 34
#define PEDAL3_MIDI_CC 67
#define PEDAL3_MIDI_CHANNEL DEFAULT_MIDI_CHANNEL

////// Advanced configuration ////////

// EMA noise filtering configuration :
// The Alpha parameter should be defined with a K value, in such a way that
// Alpha = 1 / 2^K
#define EMA_K 3

/////// End of configuration ///////

#include <Arduino.h>
#include <atm_pedal.h>

using ATMPedal::CalibratedAdcChannel;
using ATMPedal::IMidiInterface;
using ATMPedal::Pedal;
using ATMPedal::SerialMidiInterface;

SerialMidiInterface<> serialMidiInterface(Serial2);
IMidiInterface *midiInterface =
    static_cast<IMidiInterface *>(&serialMidiInterface);

CREATE_ADC1_CHANNEL(pedal1Input, PEDAL1_PIN);
CREATE_ADC1_CHANNEL(pedal2Input, PEDAL2_PIN);
CREATE_ADC1_CHANNEL(pedal3Input, PEDAL3_PIN);

Pedal pedals[] = {
    Pedal(pedal1Input, midiInterface, PEDAL1_MIDI_CC, PEDAL1_MIDI_CHANNEL),
    Pedal(pedal2Input, midiInterface, PEDAL2_MIDI_CC, PEDAL2_MIDI_CHANNEL),
    Pedal(pedal3Input, midiInterface, PEDAL3_MIDI_CC, PEDAL3_MIDI_CHANNEL)};

void setup()
{
    // Init serial 0 for debugging purpose
    Serial.begin(115200);

    serialMidiInterface.setup();

    for (auto &pedal : pedals)
        pedal.setup();
}

uint64_t adcReadcount = 0;
unsigned long monitoredSecond = 0;
unsigned int midiChange = 0;

unsigned long seconds()
{
    return millis() / 1000UL;
}

void loop()
{
    for (auto &pedal : pedals)
        pedal.loop();
}
