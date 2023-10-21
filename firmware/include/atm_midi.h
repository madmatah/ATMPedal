#ifndef __ATM_MIDI_H__
#define __ATM_MIDI_H__

#include "midi_Platform.h"
#include <MIDI.h>

using MIDI_NAMESPACE::Channel;
using MIDI_NAMESPACE::DataByte;
using MIDI_NAMESPACE::DefaultPlatform;
using MIDI_NAMESPACE::DefaultSettings;

namespace ATMPedal
{

using midiValue = DataByte;
using midiChannel = DataByte;

// Since the MIDI arduino library use templates to avoid dynamic memory
// allocation at runtime, we cannot get a pointer to the MIDI object that we can
// pass to other objects.
//
// To solve this, we define a IMidiInterface interface that expose the MIDI
// commands that we need. And we alse define a template class implementing the
// interface, that will contain the midi object. With that, we'll be able to
// pass a IMidiInterface * pointer to other objects.
//
// Credits:
// https://github.com/FortySevenEffects/arduino_midi_library/issues/162

class IMidiInterface
{
  public:
    virtual ~IMidiInterface(){};
    virtual void sendControlChange(uint8_t controlNumber,
                                   midiValue controlValue,
                                   midiChannel channel) = 0;
};

template <class _Settings = DefaultSettings, class _Platform = DefaultPlatform>
class SerialMidiInterface : public IMidiInterface
{
  public:
    explicit SerialMidiInterface(HardwareSerial &serialPort)
        : serialMidi(serialPort), midi(serialMidi){};

    void setup()
    {
        midi.begin(1);
    }

    void sendControlChange(uint8_t controlNumber, midiValue controlValue,
                           midiChannel channel)
    {
        Serial.printf("CC#%i -> %i\n", controlNumber, controlValue);
        midi.sendControlChange(controlNumber, controlValue, channel);
    }

  private:
    MIDI_NAMESPACE::SerialMIDI<HardwareSerial> serialMidi;
    MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>,
                                  _Settings, _Platform>
        midi;
};

}; // namespace ATMPedal

#endif /* __ATM_MIDI_H__ */