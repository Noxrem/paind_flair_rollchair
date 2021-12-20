import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
import numpy as np

sp = serial.Serial(port = "COM14", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
sp.flush()

def readFSRData():
    fsrList = None
    sp.write(b'fsr value on\r\n')
    while(sp.in_waiting > 0):           # if data is in the waiting buffer (if not return "None")
        bytes = sp.readline()           # read bytes until line feed
        line = bytes.decode("ASCII")    # decode bytes as ascii string

        if "right" in line:
            try:
                line = line[line.find("right"):]    # Remove chars before the word "right"
                line = line.replace("\r\n", "")     # Remove the carriage return and line feed from the string
                splitLineValues = line.split(" ")   # Extract the right and left values
                fsrList = [int(splitLineValues[1]), int(splitLineValues[3])]     # Append one fsr right and left measurement to the list and convert it to an int
            except Exception as e:
                print("Data could not be extracted.\n") 
                #fsrList = [0, 0]           # Return empty values
        else:
            continue

    return fsrList

# Pause the animation
def animationPause(i):
    global ani
    sp.write(b'fsr value off\r\n')      # disable the sending of data from the fsr over uart
    ani.pause()

# Resume the animation
def animationResume(i):
    global ani
    sp.write(b'fsr value on\r\n')       # enable the sending of data from the fsr over uart
    ani.resume()

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)


rightData = []
leftData = []

def animate(i):
    global rightData
    global leftData
    fsrData = readFSRData()
    if(fsrData):                        # If data was received
        if(len(rightData) > 20):        # limit the amount of data points shown
            del rightData[0]            # remove the first datapoint of the list
            del leftData[0]
            
        rightData.append(fsrData[0])    # Append received data to data about to be plotted
        leftData.append(fsrData[1])

    ax1.clear()
    rightAxis, = ax1.plot(rightData, "r-")  # get the list of lines representing the plotted data
    leftAxis, = ax1.plot(leftData, "g-")
    ax1.set_ylim(0, 4096)                   # set the view window to full scale (y axis)
    ax1.set_title("FSR Sensor Live Data")
    ax1.set_ylabel("ADC value")
    ax1.set_xlabel("samples")
    ax1.legend([rightAxis, leftAxis], ["FSR Right", "FSR Left"])

ani = animation.FuncAnimation(fig, animate, interval=100)

ax_pause = plt.axes([0.01, 0.01, 0.08, 0.05])   # Button xpos, ypos, width, height
ax_play = plt.axes([0.1, 0.01, 0.08, 0.05])
btnPause = Button(ax_pause, "Pause")
btnPlay = Button(ax_play, "Play")
btnPause.on_clicked(animationPause)
btnPlay.on_clicked(animationResume)

plt.show()



