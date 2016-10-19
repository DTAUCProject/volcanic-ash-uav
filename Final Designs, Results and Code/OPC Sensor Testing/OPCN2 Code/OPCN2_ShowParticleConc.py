########################################################################
# Program to print various particle concentration readings from the OPCN2
# sensor via Raspberry Pi. Currently 5 intermediate particle 
# concentrations are printed, as well as the total concentration. Note 
# that this is done assuming a certain particle bulk density as defined 
# in the constants.
#
# For the Python OPCN2 library, see: 
# https://github.com/dhhagan/py-opc
# http://py-opc.readthedocs.io/en/latest/
#
# Project: DTA FYP - Distal Volcanic Ash Sensing
# Author: Jamie Van de Laar
# Last modified: 20.10.2016
########################################################################

import spidev
import opc
import math
import serial
import time


########################################################################
# Constants

# Particle assumed bulk density in g/mL
DENSITY = 1.31 # This was the density found for the Mt Bromo ash
# Average particle diameter in each bin in microns
BIN_AVG_DIAM = [0.46, 0.66, 0.915, 1.195, 1.465, 1.83, 2.535, 3.5, 4.5, 5.75, 7.25, 9, 11, 13, 15, 16.75]
# Sample volume weightings for each bin as given by Alphasense
SAMPLE_VOL_WEIGHTINGS = [4.5, 3, 2, 0.5, 0.3, 0.25, 0.25, 0.25, 0.35, 0.45, 0.5, 0.8, 1, 1, 1, 1]
# The loop interval also sets the approximate OPCN2 sample time
LOOP_INTERVAL_SEC = 2


########################################################################
# OPC-N2 initialization and functions

# Return spi object defining port, mode and speed
def initSPI():
	
	# Open a SPI connection on CE0 
	spi = spidev.SpiDev()
	spi.open(0, 0)

	# Set the SPI mode and clock speed
	spi.mode = 1
	spi.max_speed_hz = 500000
	
	return spi
	
	
# Create OPCN2 object and turn the sensor on
def initOPC(spi):
	
	try:
		OPC = opc.OPCN2(spi)
	except Exception as e:
		print ("Startup Error: {}".format(e))
		return 0
	
	# Turn the opc ON
	if (not OPC.on()):
		print("OPCN2 won't turn on")
		return 1
		
	time.sleep(2)
	return OPC
	
	
# Returns the total mass of particles in the specified bin in the order 
# of 10^-18 (atto)grams
def returnBinMass(histogram, binNum):
	binName = "Bin 0"
	binName = binName.replace("0", str(binNum))
	
	binMass = histogram[binName] * DENSITY * (4/3) * math.pi * \
	pow((BIN_AVG_DIAM[binNum]/2),3) * SAMPLE_VOL_WEIGHTINGS[binNum]
	
	return binMass
	
	
# Returns the particle concentration in the specified bin range in either
# ug/m^3 or mg/m^3 (with /1000 divisor) 	
def returnConcInRange(histogram, lowBin, highBin):
	totalMass = 0
	
	for i in range(lowBin, highBin):
		totalMass += returnBinMass(histogram, i)
	
	totalConc = totalMass/(histogram["SFR"] * histogram["Sampling Period"])
	
	return totalConc#/1000  # Comment for concentration in ug/m^3
	
	

########################################################################
# Main function to set up ports then print concentration data

def main():
	
	spi = initSPI()
	alpha = initOPC(spi)
	
	if (alpha == 0 or alpha == 1):
		print("Start up error")
		quit()
	
	# Now start the program running until the Keyboard Interrupt is 
	# received (Ctrl+C), at which time the sensor is turned off
	try:
		while (1):		
			
			# Store histogram data
			hist = alpha.histogram()
			
			# Calculate concentration for each bin range
			myPM1 = returnConcInRange(hist, 0, 2)
			myPM3 = returnConcInRange(hist, 3, 6)
			myPM10 = returnConcInRange(hist, 7, 11)
			myPM14 = returnConcInRange(hist, 12, 13)
			myPM17_5 = returnConcInRange(hist, 14, 15)
			# Calculate total concentration
			totalConc = returnConcInRange(hist, 0, 15)
			
			# Print out all readings
			print("0.38 to 1.05 micron conc =   {0:.2f} ug/m^3".format(myPM1))
			print("1.05 to 3 micron conc =      {0:.2f} ug/m^3".format(myPM3))
			print("3 to 10 micron conc =        {0:.2f} ug/m^3".format(myPM10))
			print("10 to 14 micron conc =       {0:.2f} ug/m^3".format(myPM14))
			print("14 to 17.5 micron conc =     {0:.2f} ug/m^3".format(myPM17_5))
			print("Total conc =                 {0:.2f} ug/m^3".format(totalConc))
			
			print("")
			
			time.sleep(LOOP_INTERVAL_SEC)
			
	except KeyboardInterrupt:
		# Turn the opc OFF
		alpha.off()

main()
