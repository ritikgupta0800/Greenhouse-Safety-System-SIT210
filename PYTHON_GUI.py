# Importing Packages
import tkinter as tk
from tkinter import ttk
import serial
import matplotlib.pyplot as plt
from tkinter import *
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg


ser = serial.Serial(port='/dev/ttyACM0',baudrate=9600,parity=serial.PARITY_ODD,stopbits=serial.STOPBITS_TWO,bytesize=serial.SEVENBITS)
# Creating functions

def readData():
    global waterpumpFlag,fanFlag,lightFLag
    global loop,moistureList,temperatureList,lightList,ax1,humidList
    if loop:
        try:
            data = ser.readline().decode('utf-8').strip()
            print(data)
            rawData =data.split(",")
            print(f"Moisture Value is :- {rawData[0]}")
            moistureList.append(float(rawData[0].strip()))
            print(f"Humidity Value is :- {rawData[1]}")
            humidList.append(float(rawData[1].strip()))
            print(f"Temperature Value is :- {rawData[2]}")
            temperatureList.append(float(rawData[2].strip()))
            print(f"Light Value is :- {rawData[3]}")
            lightList.append(float(rawData[3].strip()))
            print("My values: ",int(rawData[4]),int(rawData[5]),int(rawData[6]))
            if(int(rawData[4]) == 1):
                waterpumpFlag="ON"
            else:
                waterpumpFlag="OFF"

            if (int(rawData[5]) == 1):
                fanFlag = "ON"
            else:
                fanFlag = "OFF"

            if (int(rawData[6]) == 1):
                lightFLag = "ON"
            else:
                lightFLag = "OFF"
            update_labels()

        except Exception as e:
            print(e)
            print("Serial Not Available")
    plot()
    root.after(1000,readData)

def plot():
    ax1.clear()
    ax1.plot(temperatureList)
    ax1.set_title("Temperature Ploting")

    ax2.clear()
    ax2.plot(lightList)
    ax2.set_title("Light Ploting")

    ax3.clear()
    ax3.plot(moistureList)
    ax3.set_title("Moisture Ploting")

    ax4.clear()
    ax4.plot(humidList)
    ax4.set_title("Humidity Plotting")

    print(f"Moisture list is : {moistureList}")
    print(f"Temperature list is : {temperatureList}")
    print(f"Light list is : {lightList}")
    print((f"Humidity list is : {humidList}"))
    canvas.draw()
def stop():
    global loop
    loop = False
    exit()

def start():
    global loop
    loop = True

def update_labels():
    global waterpumpFlag,fanFlag,lightFLag
    global Sensor1,Sensor2,Sensor3
    Sensor1.config(text=f"Water Pump Status : {waterpumpFlag}")
    Sensor2.config(text=f"Fan Status : {fanFlag}")
    Sensor3.config(text=f"Light Status : {lightFLag}")


if __name__ == '__main__':
    waterpumpFlag = "N/A"
    fanFlag = "N/A"
    lightFLag = "N/A"

    # Variables
    moistureList = []
    temperatureList = []
    lightList=[]
    humidList=[]
    loop = False

    #GUI from tkinter
    root = tk.Tk()
    root.geometry("300x700")  # width,height
    root.title('Value Plotting')


    # Create figure and axes for graphs
    fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, figsize=(8, 5))
    canvas = FigureCanvasTkAgg(fig, master=root)

    plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1, hspace=1.3)
    canvas.get_tk_widget().pack()


    Sensor1 = tk.Label(root,text="Water Pump Status : N/A")
    Sensor2 = tk.Label(root,text="Fan Status : N/A")
    Sensor3 = tk.Label(root,text="Light Status : N/A")

    root.after(1000, update_labels)


    # Buttons of GUI

    exitbtn = tk.Button(text="Exit", padx=10, pady=10, command=stop,width=8)
    
    root.after(1000, readData)


    startbtn = tk.Button(text ="Start",padx=10,pady=10,command=start,width=8)
   
    startbtn.pack()
    exitbtn.pack()
    Sensor1.pack()
    Sensor2.pack()
    Sensor3.pack()
    

    root.mainloop()
