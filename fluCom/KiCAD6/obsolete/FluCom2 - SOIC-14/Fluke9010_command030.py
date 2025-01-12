import RPi.GPIO as GPIO
import time

# Setup GPIOs
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# Data bus pin numbers (bit 7, ... 0)
dbPin = [5, 6, 13, 19, 26, 21, 20, 16]
# GPIO names
POD0 = 16
POD1 = 20
POD2 = 21
POD3 = 26
POD4 = 19
POD5 = 13
POD6 = 6
POD7 = 5
MAINSTAT = 12
PODSTAT = 7
RESET = 8
# MAINSTAT = write, RESET = write, PODSTAT = read (since I am Mainframe)
GPIO.setup(MAINSTAT, GPIO.OUT) #_MAINSTAT
GPIO.setup(PODSTAT, GPIO.IN, pull_up_down=GPIO.PUD_UP) #_PODSTAT
GPIO.setup(RESET, GPIO.OUT) #_RESET
#Initialize all data bus lines to 'in'..    
GPIO.setup(POD7, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD7
GPIO.setup(POD6, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD6
GPIO.setup(POD5, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD5
GPIO.setup(POD4, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD4
GPIO.setup(POD3, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD3
GPIO.setup(POD2, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD2
GPIO.setup(POD1, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD1
GPIO.setup(POD0, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD0

#initialize status lines
GPIO.output(MAINSTAT, 1)
GPIO.output(RESET, 1)

# ----------------------------Functions-----------------------

#RESET--------------------------------------------------------
def reset():
    GPIO.output(RESET, 0)
    time.sleep(.0003)
    GPIO.output(RESET, 1)

# Write bin_command to GPIOs----------------------------------
def write_command(hex_command):
    #     convert hex_command to bin
    bin_command = bin(int(hex_command, 16))[2:].zfill(8)
    GPIO.setup(MAINSTAT, GPIO.OUT)
    GPIO.setup(PODSTAT, GPIO.IN)
    GPIO.setup(POD7, GPIO.OUT) #POD7
    GPIO.setup(POD6, GPIO.OUT) #POD6
    GPIO.setup(POD5, GPIO.OUT) #POD5
    GPIO.setup(POD4, GPIO.OUT) #POD4
    GPIO.setup(POD3, GPIO.OUT) #POD3
    GPIO.setup(POD2, GPIO.OUT) #POD2
    GPIO.setup(POD1, GPIO.OUT) #POD1
    GPIO.setup(POD0, GPIO.OUT) #POD0

    GPIO.output(MAINSTAT, 1)
    GPIO.output(POD0, int(bin_command[7]))
    GPIO.output(POD1, int(bin_command[6]))
    GPIO.output(POD2, int(bin_command[5]))
    GPIO.output(POD3, int(bin_command[4]))
    GPIO.output(POD4, int(bin_command[3]))
    GPIO.output(POD5, int(bin_command[2]))
    GPIO.output(POD6, int(bin_command[1]))
    GPIO.output(POD7, int(bin_command[0]))

    # Handshaking with the pod:
    print ("waiting for _PODSTAT...")
    while GPIO.input(PODSTAT) == GPIO.HIGH:
        GPIO.output(MAINSTAT, 0)
    print ("_PODSTAT detected")
    GPIO.output(MAINSTAT, 1)
        
    # Re-Initialize all data bus lines to 'in'..    
    GPIO.setup(POD7, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD7
    GPIO.setup(POD6, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD6
    GPIO.setup(POD5, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD5
    GPIO.setup(POD4, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD4
    GPIO.setup(POD3, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD3
    GPIO.setup(POD2, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD2
    GPIO.setup(POD1, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD1
    GPIO.setup(POD0, GPIO.IN, pull_up_down=GPIO.PUD_UP) #POD0




# read data from GPIOs-----------------------------------
def read_data():
    # initialize pin list
    bin_list = [1, 1, 1, 1, 1, 1, 1, 1]
    # initialize bin_data string
    bin_data = ""
    
    GPIO.setup(MAINSTAT, GPIO.OUT)
    GPIO.setup(PODSTAT, GPIO.IN)
    GPIO.output(MAINSTAT, 0) #RDY 4 DATA
    time.sleep(.00000001) #withjout that it only works on every second read
    while GPIO.input(PODSTAT) == GPIO.LOW:
        bin_list[0] = str(GPIO.input(POD7))
        bin_list[1] = str(GPIO.input(POD6))
        bin_list[2] = str(GPIO.input(POD5))
        bin_list[3] = str(GPIO.input(POD4))
        bin_list[4] = str(GPIO.input(POD3))
        bin_list[5] = str(GPIO.input(POD2))
        bin_list[6] = str(GPIO.input(POD1))
        bin_list[7] = str(GPIO.input(POD0))
        GPIO.output(MAINSTAT, 1)
    # convert list to string
        for y in bin_list:
            bin_data += y
    # ..and now in hex:      
        hex_data = hex(int(bin_data, 2))    
    # print ("hex_data: " + str(hex_data))
        return hex_data

#     GPIO.cleanup()

# -------------------------------------------------------
# Hex Variables
reset()
hex_command = input("Enter hex command: ")

# Convert hex values to int
int_command = int(hex_command, base=16)

# Convert integer to binary,
# remove "0b"- prefix (slice from the second character onwards,
# fill empty bits with 0
bin_command = bin(int_command) [2:] .zfill(8)
# all-in-one solution would be: bin(int(hex_command, 16))[2:].zfill(8)

print ("Command value conversions:")
print ("hex: 0x" + hex_command.zfill(2))
print ("dec: " + str(int_command).zfill(2))
print ("bin: " + bin_command)
print ("bin reversed: " + bin_command [::-1])
print ("2nd bit of bin: " + bin_command[5])
print ("\n")

#------------------------------------------
#reset()
#hex_command = "35"
#write_command(hex_command)
#hex_command = "01"
#write_command(hex_command)
#hex_command = "23"
#write_command(hex_command)
#exit()

# -----------------------------------------
# call write
print ("writing hex_command to databus:")
write_command(hex_command)

# call read
print ("\nreading databus")
# time.sleep(.001)
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))

print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
print ("Data value: " + str(read_data()))
GPIO.cleanup()