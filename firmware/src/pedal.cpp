
#include <atm_pedal.h>

namespace ATMPedal
{

Pedal::Pedal(IAnalogChannel &input, IMidiInterface *output,
             uint8_t controlNumber, midiChannel channel)
    : analogInput(input), midiOutput(output), ccNumber(controlNumber),
      channel(channel), lastMidiValue(0)
{
}

Pedal::~Pedal()
{
}

void Pedal::setup()
{
    analogInput.setup();
}

bool Pedal::loop()
{
    midiValue readMidiValue = analogRead();
    if ((readMidiValue != lastMidiValue) &&
        (readMidiValue == 0 || readMidiValue == 127 ||
         abs(readMidiValue - lastMidiValue) >= 2))
    {
        midiOutput->sendControlChange(ccNumber, readMidiValue, channel);
        lastMidiValue = readMidiValue;
        return true;
    }
    return false;
}

midiValue Pedal::voltageToMidi(millivolts voltage)
{
    millivolts min = analogInput.getMinVoltage();
    millivolts max = analogInput.getMaxVoltage();

    midiValue result;

    if (voltage <= min)
        result = 0;
    if (voltage >= max)
        result = 127;
    else
    {
        millivolts width = max - min;
        voltage = voltage - min;

        result = round(voltage * 127 / width);
    }
    return 127 - result;
}

midiValue Pedal::analogRead()
{
    millivolts voltage = analogInput.read();
    millivolts filteredVoltage = emaFilter(voltage);
    return voltageToMidi(filteredVoltage);
}

} // namespace ATMPedal