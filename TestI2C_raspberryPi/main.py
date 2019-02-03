# -*- coding: utf-8 -*-
# ===========================================================================
# @brief Fichier principal uC
# @author 
# ===========================================================================

from const import *
import time
import smbus

#--------------------------------------------

dataMag =[]
buffer = 0
bufferSize = 5000
count =0

bus = smbus.SMBus(CHANNEL)
#bus.write_byte_data(DEVICE_ADDRESS, offset, msg)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_ARAUTOINC, ARAUTOINC__ARAUTOINC_MASK)
bus.write_byte_data(DEVICE_ADDRESS, SI72XX_DSPSIGSEL, DSPSIGSEL__MAG_VAL_SEL)
start=time.time()

while(count<bufferSize):
    
    #Write a single register
    bus.write_byte_data(DEVICE_ADDRESS, SI72XX_POWER_CTRL, POWER_CTRL__ONEBURST_MASK)
    bufferL = bus.read_byte_data(DEVICE_ADDRESS, SI72XX_DSPSIGL)
    bufferM = bus.read_byte_data(DEVICE_ADDRESS, SI72XX_DSPSIGM)
    signal = (bufferM & 0x7F) << 8 | bufferL
    #print(bufferM)
    #print(bufferL)
    #print(signal, "  " ,bufferM,  "  ",bufferL)
    dataMag.append(signal)
    count=count+1
        
end=time.time()
print("Temps d'éxécution: ", end-start)
print("Sample rate: ", bufferSize/(end-start))
print("sending data to files...")
with open("dataCourrant.txt", "w") as out_file:
   for c in range(len(dataMag)):
         out_string = ''
         out_string += str(dataMag[c])
         out_string += '\n'
         out_file.write(out_string)
print("file is ready")
    
