import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
import numpy as np
import keyboard

# Variables
funcEnabled = False;        # If keyboard output (arrow keys) are enabled
inputNotOk = True           # If COM port input was not valid
fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

rightData = []
leftData = []

# Take the list [right, left] values of the fsr sensor and execute a key event
def fsrDataToKeyEvent(fsrData):
    thresh1 = 200
    thresh2 = 500           # Threshold 2
    thresh3 = 2000
    
    rightFsr = fsrData[0]
    leftFsr = fsrData[1]
    if(thresh1 < rightFsr and leftFsr < thresh1):
        keyboard.send("right")
    elif(rightFsr < thresh1 and thresh1 < leftFsr):
        keyboard.send("left")
    elif(thresh2 < rightFsr < thresh3 and thresh2 < leftFsr < thresh3):
        keyboard.send("down")
    elif(rightFsr > thresh3 and leftFsr > thresh3):
        keyboard.send("up")
    
# Read fsr data from serial port
def readFSRData():
    fsrList = None
    sp.write(b'fsr value on\r\n')
    while(sp.in_waiting > 0):           # if data is in the waiting buffer (if not return "None")
        bytes = sp.readline()           # read bytes until line feed
        line = bytes.decode("ASCII")    # decode bytes as ascii string

        if "right" in line:
            line = line.replace("\r\n", "")     # Remove the carriage return and line feed from the string
            splitLineValues = line.split(" ")   # Extract the right and left values
            try:
                fsrList = [int(splitLineValues[2]), int(splitLineValues[4])]     # Append one fsr right and left measurement to the list and convert it to an int
            except Exception as e:
                print("Data could not be extracted.")
        else:
            continue

    return fsrList

# Pause the animation
def animationPause():
    global ani
    sp.write(b'fsr value off\r\n')      # disable the sending of data from the fsr over uart
    ani.pause()

# Resume the animation
def animationResume():
    global ani
    sp.write(b'fsr value on\r\n')       # enable the sending of data from the fsr over uart
    ani.resume()

# Toggle the global bool variable, change Function button color and pause animation
def toggleFuncEnabled(event):
    global funcEnabled
    global btnFunc
    if funcEnabled:
        funcEnabled = False
        btnFunc.color = "red"
        #animationResume()      # Resume the plot animation
    else:
         funcEnabled = True
         btnFunc.color = "green"
         #animationPause()        # Pause the plot animation
    btnFunc.hovercolor = btnFunc.color
    btnFunc.ax.figure.canvas.draw()
    
def animate(i):
    global rightData
    global leftData
    fsrData = readFSRData()
    if funcEnabled:                 # Either control keyboard or plot data
        fsrDataToKeyEvent(fsrData)
    else:
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

# Take input in console
while(inputNotOk):  # Iterate until valid COM port is entered
    try:
        comPort = input("Enter the COM port:")
        sp = serial.Serial(port = comPort, baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
    except:
        print("Could not connect to COM port. Try again...\n")
    else:
        inputNotOk = False
sp.flush()          # Flush COM port

ani = animation.FuncAnimation(fig, animate, interval=100)   # Start animation

ax_pause = plt.axes([0.01, 0.01, 0.08, 0.05])   # Button xpos, ypos, width, height
ax_play = plt.axes([0.1, 0.01, 0.08, 0.05])
ax_func = plt.axes([0.19, 0.01, 0.1, 0.05])
btnPause = Button(ax_pause, "Pause")
btnPlay = Button(ax_play, "Play")
btnFunc = Button(ax_func, "Function", color="red")
btnPause.on_clicked(animationPause)
btnPlay.on_clicked(animationResume)
btnFunc.on_clicked(toggleFuncEnabled)

plt.show()



