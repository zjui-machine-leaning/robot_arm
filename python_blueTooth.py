# This project requires PyBluez

from tkinter import *

import bluetooth

import socket

from time import sleep

import time

import serial

import pdb

import numpy as np



#some variables that need to be preset



'''

class ConnectToArduino

connect to the arduino with bluetooth and send data to it



usage example:

    a = ConnectToArduino("HC-06")

    a.SendData(dataArr, StartByte, InterStart, InterEnd, EndByte, WaitTime)



'''



class ConnectToArduino:

    deviceName = ""

    deivceAvailable = False

    Connected = False

    port = 1

    MacAddr = ""

    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    StartByte = 121

    InterStart = 122

    InterEnd = 123

    EndByte = 124

    positive = 125

    negative = 126

    WaitTime = 0.01

    Timeout = 0.1

    expansion = 2

    

    def __init__(self, name):

        self.SetName(name)

        self.FindMacAddr()

        self.ConnectDevice()

    def SetName(self, newName):

        self.deviceName = newName

        

    def FindMacAddr(self):

        #Look for all Bluetooth devices

        #the computer knows about.

        #Create an array with all the MAC

        #addresses of the detected devices

        nearby_devices = bluetooth.discover_devices()

        #Run through all the devices found and list their name

        for i in nearby_devices:

            if(self.deviceName==bluetooth.lookup_name(i)):

                print("Find", self.deviceName)

                self.MacAddr = i

                self.deviceAvailable = True

                return

        print("Cannot find", self.deviceName)

    

    def ConnectDevice(self):

        #Allow the user to select their Arduino

        #bluetooth module. They must have paired

        #it before hand.

        if (self.deviceAvailable == False):

            print("Device Unavailable")

            return

        if (self.Connected == True):

            print("Already Connected")

            return

        print ("Starting a communication with", bluetooth.lookup_name(self.MacAddr))

        

        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

        #s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # sock has the type bluetooth.msbt.BluetoothSocket

        try:

            sock.connect((self.MacAddr, self.port))

            self.sock = sock

            self.Connected = True

            print("Connected Successfully")

        except:

            print("Unable to Connected")

    

    def CloseConnection(self):

        self.sock.close()

    '''

    function SendAndRead

    input: 

        Byte: to be sent

        ExpectedResponse

    

    '''

    def SendAndRead(self, Byte, ExpectedResponse):

        self.sock.send(chr(Byte))

        print("Sending", Byte)

        self.sock.settimeout(self.Timeout)

        try:

            Response = self.sock.recv(1)

        except socket.error:

            print("Timed out!")

            return 0

        print("Receive", Response)

        print(ExpectedResponse)

        if (ord(Response) == ExpectedResponse):

            print("Handshake success")

            return 1 # success

        else:

            print("Handshake failed")

            return 0 # failed

    

    '''

    function SendData:

    '''

    

    def SendData(self, dataArr):

        print("SendData")

        if (self.Connected == False):

            print("Not Connected")

            return

        while (1!=self.SendAndRead(self.StartByte, self.StartByte)):

            print("StartByte, StatByte")

            #sleep(WaitTime)     

        while (1!=self.SendAndRead(len(dataArr), len(dataArr))):

            print("len(dataArr), len(dataArr)")

            #sleep(WaitTime)  

        for idx in range(len(dataArr)):

            

            while (1!=self.SendAndRead(self.InterStart, self.InterStart)):

                print("InterStart, InterStart")

                #sleep(WaitTime)        

                

            if (dataArr[idx] < 0):

                while (1!=self.SendAndRead(self.negative, self.negative)):

                    print("Negative", self.InterStart)

                    #sleep(WaitTime)

            else:

                while (1!=self.SendAndRead(self.positive, self.positive)):

                    print("Positive", self.positive)

                    #sleep(WaitTime)                

            while (1!=self.SendAndRead(abs(dataArr[idx]), abs(dataArr[idx]))):

                print("dataArr", idx, dataArr[idx])

                #sleep(WaitTime)

                

            while (1!=self.SendAndRead(self.InterEnd, self.InterEnd)):

                print("InterEnd, InterEnd")

                #sleep(WaitTime)

                

        while (1!=self.SendAndRead(self.EndByte, self.EndByte)):

                print("InterEnd", self.EndByte)

        

    def DataConverter(self, dataArr):

        res = [0]*len(dataArr)*self.expansion

        for i in range(len(dataArr)):

            sign = 1

            current_val = dataArr[i]

            if current_val < 0:

                sign = -1

                current_val = -current_val

            for j in range(self.expansion):

                res[i*self.expansion+self.expansion-j-1] = int(sign*(current_val%100))

                current_val = int((current_val - (current_val%100))/100)

        return res    

            



dataArr = [-1800, 2200, 1500, 8000, 3600, -1800, 2200, 1500, 8000, 3600]

a = ConnectToArduino("MyBlueTooth")

arr = a.DataConverter(dataArr)

start_time = time.time()

a.SendData(a.DataConverter(dataArr))

print("--- %s seconds ---" % (time.time() - start_time))

a.CloseConnection()