#include <atm_pedal.h>

namespace ATMPedal
{

CalibratedAdcChannel::CalibratedAdcChannel(adc_unit_t adcUnit, uint8_t pin,
                                           adc_atten_t attentuation,
                                           adc_bits_width_t resolution,
                                           uint32_t defaultVref)
    : adcUnit(adcUnit), pin(pin), attenuation(attentuation),
      resolution(resolution), defaultVref(defaultVref)
{
    adcRawMax = pow((double)2, 9 + resolution);
    this->rawToVoltage = new uint16_t[this->adcRawMax];
}

millivolts CalibratedAdcChannel::getMinVoltage()
{
    return this->minVoltage;
}

millivolts CalibratedAdcChannel::getMaxVoltage()
{
    return this->maxVoltage;
}

millivolts CalibratedAdcChannel::read()
{
    uint16_t rawAdcRead = analogRead(this->pin);
    return this->rawToVoltage[rawAdcRead];
}

void CalibratedAdcChannel::setup()
{
    esp_adc_cal_characterize(adcUnit, attenuation, resolution, defaultVref,
                             &this->adcCharacteristics);

    for (int rawValue = 0; rawValue < this->adcRawMax; rawValue++)
    {
        uint16_t voltage = (uint16_t)esp_adc_cal_raw_to_voltage(
            rawValue, &this->adcCharacteristics);
        if (rawValue == 0 || voltage < this->minVoltage)
        {
            this->minVoltage = voltage;
        }
        if (rawValue == 0 || voltage > this->maxVoltage)
        {
            this->maxVoltage = voltage;
        }
        this->rawToVoltage[rawValue] = voltage;
    }
}

CalibratedAdcChannel::~CalibratedAdcChannel()
{
    delete[] this->rawToVoltage;
}

} // namespace ATMPedal