import spidev
import opc
import math
import serial
import time

# Constants
DENSITY = 1.31  # Particle bulk density in kg/m^3
BIN_AVG_DIAM = [0.46, 0.66, 0.915, 1.195, 1.465, 1.83, 2.535, 3.5, 4.5, 5.75, 7.25, 9, 11, 13, 15, 16.75]
SAMPLE_VOL_WEIGHTINGS = [4.5, 3, 2, 0.5, 0.3, 0.25, 0.25, 0.25, 0.35, 0.45, 0.5, 0.8, 1, 1, 1, 1]
LOOP_INTERVAL_SEC = 2

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
# ug/m^3 (unchanged) or mg/m^3 (with /1000 divisor) 	
def returnConcInRange(histogram, lowBin, highBin):
	totalMass = 0
	
	for i in range(lowBin, highBin):
		totalMass += returnBinMass(histogram, i)
	
	totalConc = totalMass/(histogram["SFR"] * histogram["Sampling Period"])
	
	return totalConc/1000  # Comment for concentration in ug/m^3
	
	
def main():
	
	spi = initSPI()
	alpha = initOPC(spi)
	
	#~ port = serial.Serial("/dev/ttyAMA0", baudrate=115200, timeout=3.0)
	#~ port.write("LABEL,Time,Conc\r\n")
	
	if (alpha == 0 or alpha == 1):
		print("Start up error")
		quit()
		
	# Record the  start time so the time since experiment/program start 
	# can be recorded
	start = time.time()
	
	
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
			
			# Record time since program start
			currTime = time.time() - start

			# Now log data in format required for serial logging program
			
			# For Gobetwino:
			print("#S|OPCLOGTEST|[{:03.1f},{:03.2f},{:03.2f},{:03.2f},{:03.2f},{:03.2f},{:03.2f},{:03.2f}]#"\
			.format(currTime, myPM1, myPM3, myPM10, myPM14, myPM17_5, totalConc, hist["SFR"]))
			#print("#S|OPCLOGTEST|[{:03.1f},{:03.2f}]#".format(currTime, hist["SFR"]))
			
			#~ port.write("#S|OPCLOGTEST|[,{0:.2f}]#\n".format(totalConc)) 
			#~ port.flush()
			
			# For PLX-DAQ:
			#~ port.write("DATA,TIME,")
			#~ port.write("{0:.2f}\r\n".format(totalConc))
			
			
			time.sleep(LOOP_INTERVAL_SEC)
			
	except KeyboardInterrupt:
		# Turn the OPC off
		alpha.off()


main()
