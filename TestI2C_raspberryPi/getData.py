# -*- coding: utf-8 -*-
# ===========================================================================
# @brief Fichier principal uC
# @author Samuel Daoust
# ===========================================================================

from const import *
import time
from smbus2 import SMBus
import numpy as np

# --------------------------------------------

dataMag = []
buffer = 0
bufferSize = 5000
count = 0

bus = SMBus(CHANNEL)
# bus.write_byte_data(DEVICE_ADDRESS, offset, msg)

bus.write_byte_data(DEVICE_ADDRESS, SI72XX_ARAUTOINC, ARAUTOINC__ARAUTOINC_MASK)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_DSPSIGSEL, DSPSIGSEL__MAG_VAL_SEL)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_CTRL4, 0x04)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_SLTIME, 0x00)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_CTRL3, 0x02)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_POWER_CTRL, 0x00)

start = time.time()

while (count < bufferSize):
    # Write a single register
    bufferData = bus.read_i2c_block_data(DEVICE_ADDRESS,SI72XX_DSPSIGM,2)
    signal = (bufferData[0] & 0x7F) << 8 | bufferData[1]
    #print(bufferData[0],bufferData[1])
    #print(signal)
    dataMag.append(signal)
    count = count + 1

end = time.time()
array = np.array(dataMag)
stdDeviation = np.std(array)

rmsValue = np.sqrt(np.mean(array**2))
print("Temps d'execution: ", end - start)
print("Sample rate: ", bufferSize / (end - start))
print("ecart type: ", stdDeviation)
print("Valeur RMS: ", rmsValue)
print("sending data to files...")

with open("dataCourant.txt", "w") as out_file:
    for c in range(len(dataMag)):
        out_string = ''
        out_string += str(dataMag[c])
        out_string += '\n'
        out_file.write(out_string)
print("file is ready")
del dataMag[:]
