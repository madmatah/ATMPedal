
#ifndef __ATM_PEDAL_H__
#define __ATM_PEDAL_H__

#include <Arduino.h>
#include <EMA.h>
#include <MIDI.h>
#include <esp_adc_cal.h>

#include "atm_midi.h"

#ifndef EMA_K
#define EMA_K 3
#endif /* EMA_K */

namespace ATMPedal
{

using millivolts = uint16_t;

typedef enum
{
    ADC_UNIT_1 = 1, /*!< SAR ADC 1. */
    ADC_UNIT_2 = 2, /*!< SAR ADC 2. */
} adcUnit;

class IAnalogChannel
{
  public:
    virtual ~IAnalogChannel(){};
    virtual millivolts getMinVoltage() = 0;
    virtual millivolts getMaxVoltage() = 0;
    virtual millivolts read() = 0;
    virtual void setup() = 0;
};

class CalibratedAdcChannel : public IAnalogChannel
{
  public:
    CalibratedAdcChannel(adc_unit_t adcUnit, uint8_t pin,
                         adc_atten_t attenuation = ADC_ATTEN_DB_11,
                         adc_bits_width_t resolution = ADC_WIDTH_BIT_12,
                         uint32_t defaultVref = 1100);
    ~CalibratedAdcChannel();
    millivolts getMinVoltage() override;
    millivolts getMaxVoltage() override;
    millivolts read() override;
    void setup() override;

  private:
    adc_unit_t adcUnit;
    uint8_t pin;
    adc_atten_t attenuation;
    adc_bits_width_t resolution;
    uint32_t defaultVref;
    uint32_t adcRawMax;
    uint16_t *rawToVoltage;
    uint16_t minVoltage;
    uint16_t maxVoltage;
    esp_adc_cal_characteristics_t adcCharacteristics;
};

#define CREATE_ADC1_CHANNEL(Name, Pin)                                         \
    ATMPedal::CalibratedAdcChannel Name(ADC_UNIT_1, Pin, ADC_ATTEN_DB_11,      \
                                        ADC_WIDTH_BIT_12, 1100);

class Pedal
{
  public:
    ~Pedal();
    Pedal(IAnalogChannel &input, IMidiInterface *output, uint8_t controlNumber,
          midiChannel channel);
    void setup();
    bool loop();
    midiValue analogRead();

  private:
    midiValue voltageToMidi(millivolts voltage);
    IAnalogChannel &analogInput;
    IMidiInterface *midiOutput;
    uint8_t ccNumber;
    midiChannel channel;
    EMA<EMA_K> emaFilter;
    midiValue lastMidiValue;
};

class IPedalOutput
{
  public:
    virtual ~IPedalOutput(){};
    virtual void send(midiValue value) = 0;
};

class MidiControlChannel : public IPedalOutput
{
  public:
    MidiControlChannel();
    ~MidiControlChannel();
    void send(midiValue value) override;

  private:
    uint8_t pin;
    adc_bits_width_t resolution;
};

}; // namespace ATMPedal

#endif /* __ATM_PEDAL_H__ */
