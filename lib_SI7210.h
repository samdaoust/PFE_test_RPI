/* mbed Microcontroller Library
 * Copyright (c) 2017 AT&T, IIoT Foundry, Plano, TX, USA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/** \addtogroup drivers */
 
/** Support for Silicon Labs SI7210: Magnetic field and temperature sensor
 *
 * Example:
 * @code
 *
 * #include "mbed.h"
 * #include "SI7210.h"
 *
 * I2C          i2c(I2C_SDA, I2C_SCL);
 * SI7210<I2C>  si7210(&i2c, SI7210_BASE_ADDR_7BIT);
 *
 * int main() {
 *     si7210_measurements_t  data;
 *     bool                   ok;
 *
 *     ok = si7210.enable() &&
 *          si7210.read(&data) &&
 *          si7210.disable();
 *
 *     if (ok) {
            printf("Mag T: %f\r\n", data.mag_T);
            printf("temp C/F: %f/%f\r\n", data.temp_C, data.temp_C * 9 / 5 + 32);
 *     } else {
 *         printf("si7210 error!\r\n");
 *     }
 * }
 * @endcode
 * @ingroup drivers
 */
 
#pragma once
 
#define ARAUTOINC__ARAUTOINC_MASK       0x01
#define OTP_CTRL__OPT_BUSY_MASK         0x01
#define OTP_CTRL__OPT_READ_EN_MASK      0x02
#define POWER_CTRL__SLEEP_MASK          0x01
#define POWER_CTRL__STOP_MASK           0x02
#define POWER_CTRL__ONEBURST_MASK       0x04
#define POWER_CTRL__USESTORE_MASK       0x08
#define POWER_CTRL__MEAS_MASK           0x80
#define DSPSIGSEL__MAG_VAL_SEL          0
#define DSPSIGSEL__TEMP_VAL_SEL         1
 
/** I2C registers for Si72xx */
#define SI72XX_OTP_TEMP_OFFSET  0x1D
#define SI72XX_OTP_TEMP_GAIN    0x1E
#define SI72XX_HREVID           0xC0
#define SI72XX_DSPSIGM          0xC1
#define SI72XX_DSPSIGL          0xC2
#define SI72XX_DSPSIGSEL        0xC3
#define SI72XX_POWER_CTRL       0xC4
#define SI72XX_ARAUTOINC        0xC5
#define SI72XX_CTRL1            0xC6
#define SI72XX_CTRL2            0xC7
#define SI72XX_SLTIME           0xC8
#define SI72XX_CTRL3            0xC9
#define SI72XX_A0               0xCA
#define SI72XX_A1               0xCB
#define SI72XX_A2               0xCC
#define SI72XX_CTRL4            0xCD
#define SI72XX_A3               0xCE
#define SI72XX_A4               0xCF
#define SI72XX_A5               0xD0
#define SI72XX_OTP_ADDR         0xE1
#define SI72XX_OTP_DATA         0xE2
#define SI72XX_OTP_CTRL         0xE3
#define SI72XX_TM_FG            0xE4
 
#define SI7210_BASE_ADDR_7BIT   0x30
 
typedef struct {
    float       mag_T;
    float       temp_C;
} si7210_measurements_t;
 
template <class T>
class SI7210 {
 public:
    /**
    * Constructor
    *
    * @param i2c I2C class servicing the strip
    */
    SI7210(T * i2c, uint8_t addr_7bit) : _i2c(i2c) {
        _isTempOffsetAndGainValid = false;
        _enabled = false;
        _addr_8bit = ((addr_7bit & 0x3) + SI7210_BASE_ADDR_7BIT) << 1;
    }
 
    /**
    * Activate the sensor (wake if sleeping).
    *
    * @returns true (success) or false (failure)
    */
    bool enable(void) {
        bool ok = _i2c_transfer(_addr_8bit, NULL, 0, 0);
        if (ok)
            _enabled = true;
        return ok;
    }
 
    /**
    * Deactivate the sensor (puts it to sleep)
    *
    * @returns true (success) or false (failure)
    */
    bool disable(void) {
        bool ok = _write_reg(SI72XX_POWER_CTRL, POWER_CTRL__SLEEP_MASK);
        if (ok)
            _enabled = ok;
        return ok;
    }
 
    /**
    * Read temperature and humidity
    *
    * @param data points to struct to store measurements in.  Stucture is
    *      valid only when function returns success indication.
    *
    * @returns true (success) or false (failure)
    */
    bool read(si7210_measurements_t * data) {
        uint16_t    magRaw;
        uint16_t    tempRaw;
 
        bool ok = _write_reg(SI72XX_ARAUTOINC, ARAUTOINC__ARAUTOINC_MASK)
               && _write_reg(SI72XX_DSPSIGSEL, DSPSIGSEL__MAG_VAL_SEL)      //capture mag field measurement
               && _write_reg(SI72XX_POWER_CTRL, POWER_CTRL__ONEBURST_MASK)
               && _read_regs(SI72XX_DSPSIGM, 2, &magRaw)
               && _write_reg(SI72XX_DSPSIGSEL, DSPSIGSEL__TEMP_VAL_SEL)     //capture temp measurement
               && _write_reg(SI72XX_POWER_CTRL, POWER_CTRL__ONEBURST_MASK)
               && _read_regs(SI72XX_DSPSIGM, 2, &tempRaw);
 
        if (ok && !_isTempOffsetAndGainValid) {
            signed char otpTempOffset;
            signed char otpTempGain;
 
            ok = _read_otp(SI72XX_OTP_TEMP_OFFSET, &otpTempOffset)
              && _read_otp(SI72XX_OTP_TEMP_GAIN, &otpTempGain);
            if (ok) {
                _tempOffset = (float)otpTempOffset / 16;
                _tempGain = 1 + (float)otpTempGain / 2048;
                _isTempOffsetAndGainValid = true;
            }
        }
 
        if (ok) {
            magRaw = ((magRaw >> 8) & 0xff) + ((magRaw & 0xff) << 8);
            tempRaw = ((tempRaw >> 8) & 0xff) + ((tempRaw & 0xff) << 8);
            ok = (magRaw & 0x8000) && (tempRaw & 0x8000);
            data->mag_T = (float)(magRaw - 0xC000) * 0.00125F;
            data->temp_C = (float)((tempRaw & ~0x8000) >> 3);
            data->temp_C = _tempGain * (-3.83e-6F * data->temp_C * data->temp_C + 0.16094F * data->temp_C - 279.80F - 0.222F * 3.0F) + _tempOffset;
        }
 
        return ok;
    }
 
 protected:
 
    /**
    * I2C read/write helper function
    *
    * @param address is the register to read/write
    * @param buff holds the data to write and recieves the data to read
    * @param writeSize is the number of bytes to write to register
    * @param readSize is the number of bytes to retrieve from device
    *
    * @returns true (success) or false (failure)
    */
    bool _i2c_transfer(int address, void * buff, size_t writeSize, size_t readSize) {
        bool ok;
        bool expect_response = (readSize != 0);
 
        ok = !_i2c->write(address, (char*)buff, writeSize, expect_response);
        if (ok && expect_response)
            ok = !_i2c->read(address, (char*)buff, readSize);
 
        return ok;
    }
 
    /**
    * Write to an I2C register
    *
    * @param reg sensor register to write
    * @param val value to write
    *
    * @returns true (success) or false (failure)
    */
    bool _write_reg(char reg, char val) {
        char out[2] = {reg, val};
        return 0 == _i2c->write(_addr_8bit, out, 2);
    }
 
    /**
    * Read multiple sensor registers
    *
    * @param start_reg first sensor register to be read
    * @param count number of registers to be read
    * @param buff pointer to buffer where to store the register values
    *
    * @returns true (success) or false (failure)
    */
    bool _read_regs(char start_reg, char count, void * buff) {
        bool ok;
        ok = (0 == _i2c->write(_addr_8bit, &start_reg, 1, true))
             && (0 == _i2c->read(_addr_8bit, (char *)buff, count));
        return ok;
    }
 
    /**
    * Read sensor OTP
    *
    * @param otpAddr OTP address to be read
    * @param *data where to store the OTP data
    *
    * @returns true (success) or false (failure)
    */
    bool _read_otp(uint8_t otpAddr, void *data) {
        uint8_t optCtrl;
 
        bool ok = _read_regs(SI72XX_OTP_CTRL, 1, &optCtrl)
               && !(optCtrl & OTP_CTRL__OPT_BUSY_MASK)
               && _write_reg(SI72XX_OTP_ADDR, otpAddr)
               && _write_reg(SI72XX_OTP_CTRL, OTP_CTRL__OPT_READ_EN_MASK)
               && _read_regs(SI72XX_OTP_DATA, 1, data);
 
        return ok;
    }
 
    bool    _isTempOffsetAndGainValid;
    float   _tempOffset;
    float   _tempGain;
    bool    _enabled;
    int     _addr_8bit;
    T      *_i2c;
};
 