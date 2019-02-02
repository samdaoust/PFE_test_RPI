# -*- coding: utf-8 -*-
# ===========================================================================
# @brief Constantes pour SI7210
# @author 
# ===========================================================================


DEVICE_ADDRESS = 0x30
CHANNEL = 1


ARAUTOINC__ARAUTOINC_MASK       =	0x01
OTP_CTRL__OPT_BUSY_MASK         =	0x01
OTP_CTRL__OPT_READ_EN_MASK      =	0x02
POWER_CTRL__SLEEP_MASK          =	0x01
POWER_CTRL__STOP_MASK           =	0x02
POWER_CTRL__ONEBURST_MASK       =	0x04
POWER_CTRL__USESTORE_MASK       =	0x08
POWER_CTRL__MEAS_MASK           =	0x80
DSPSIGSEL__MAG_VAL_SEL          =	0
DSPSIGSEL__TEMP_VAL_SEL         =	1

#I2C registers for Si72xx 
SI72XX_OTP_TEMP_OFFSET  =	0x1D
SI72XX_OTP_TEMP_GAIN   =	0x1E
SI72XX_HREVID           =	0xC0
SI72XX_DSPSIGM          =	0xC1
SI72XX_DSPSIGL          =	0xC2
SI72XX_DSPSIGSEL        =	0xC3
SI72XX_POWER_CTRL       =	0xC4
SI72XX_ARAUTOINC        =	0xC5
SI72XX_CTRL1            =	0xC6
SI72XX_CTRL2            =	0xC7
SI72XX_SLTIME           =	0xC8
SI72XX_CTRL3            =	0xC9
SI72XX_A0               =	0xCA
SI72XX_A1               =	0xCB
SI72XX_A2               =	0xCC
SI72XX_CTRL4            =	0xCD
SI72XX_A3               =	0xCE
SI72XX_A4               =	0xCF
SI72XX_A5               =	0xD0
SI72XX_OTP_ADDR         =	0xE1
SI72XX_OTP_DATA         =	0xE2
SI72XX_OTP_CTRL         =	0xE3
SI72XX_TM_FG            =	0xE4

