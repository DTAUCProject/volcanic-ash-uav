import spidev
import opc
from time import sleep

# Open a SPI connection on CE0
spi = spidev.SpiDev()
spi.open(0, 0)

# Set the SPI mode and clock speed
spi.mode = 1
spi.max_speed_hz = 500000

alpha = opc.OPCN2(spi)

#~ try:
	#~ alpha = opc.OPCN2(spi)
#~ except Exception as e:
    #~ print ("Startup Error: {}".format(e))

# Turn on the OPC
alpha.on()
sleep(3)

# Read the histogram and print to console
for key, value in alpha.histogram().items():
    print ("Key: {}\tValue: {}".format(key, value))

# Shut down the opc
alpha.off()
