import time
import bluetooth
import re
import RPi.GPIO as GPIO
from MindwaveDataPoints import RawDataPoint
from MindwaveDataPointReader import MindwaveDataPointReader

GPIO.setmode(GPIO.BOARD)

GPIO.setup(15, GPIO.OUT) #Poor Signal
GPIO.setup(16, GPIO.OUT) #Blink

GPIO.setup(29, GPIO.OUT) #Attention 1
GPIO.setup(31, GPIO.OUT) #Attention 2
GPIO.setup(33, GPIO.OUT) #Attention 3
GPIO.setup(35, GPIO.OUT) #Attention 4
GPIO.setup(37, GPIO.OUT) #Attention 5

GPIO.setup(18, GPIO.OUT) #Stop
GPIO.setup(40, GPIO.OUT) #Forward
GPIO.setup(38, GPIO.OUT) #Reverse
GPIO.setup(36, GPIO.OUT) #Right
GPIO.setup(32, GPIO.OUT) #Left

ST=18
FW=40
RV=38
RT=36
LT=32

EEG_AVG=70

THR_EYE=125
THR_ATT=50
THR_MED=50

PoorSignal=0
Attention=0
Meditation=0
RawData=0
BlinkC=0

FWD=0
BFlag=0
Eye_EN=0
On_Flag=0
Off_Flag=0

Temp=0
AvgRaw=0
TempAvg=0
j=0
n=0

def drive():
    global BlinkC,BFlag,FWD
    if (Attention>THR_ATT):
        GPIO.output(ST, 0)
        GPIO.output(FW, 1)
        GPIO.output(RV, 0)
        print "Forward"
        if (BlinkC==2 and BFlag==0):
            GPIO.output(RT, 1)
            GPIO.output(LT, 0)
            print "Right"
            time.sleep(3)
            GPIO.output(RT, 0)
            GPIO.output(LT, 0)
            BlinkC=0
        elif (BlinkC==3 and BFlag==0):
            GPIO.output(RT, 0)
            GPIO.output(LT, 1)
            print "Left"
            time.sleep(3)
            GPIO.output(RT, 0)
            GPIO.output(LT, 0)
            BlinkC=0
        elif (BFlag==0):
            BlinkC=0
        elif (BlinkC==0):
            FWD=1
    elif (BlinkC==2 and BFlag==0 and FWD==1):
        GPIO.output(ST, 0)
        GPIO.output(FW, 1)
        GPIO.output(RV, 0)
        GPIO.output(RT, 1)
        GPIO.output(LT, 0)
        time.sleep(3)
        GPIO.output(RT, 0)
        GPIO.output(LT, 0)
        BlinkC=0
        FWD=0
    elif (BlinkC==3 and BFlag==0 and FWD==1):
        GPIO.output(ST, 0)
        GPIO.output(FW, 1)
        GPIO.output(RV, 0)
        GPIO.output(RT, 0)
        GPIO.output(LT, 1)
        time.sleep(3)
        GPIO.output(RT, 0)
        GPIO.output(LT, 0)
        BlinkC=0
        FWD=0
    elif (Meditation>THR_MED):
        BlinkC=0
        BFlag=0
        GPIO.output(ST, 0)
        GPIO.output(FW, 0)
        GPIO.output(RV, 1)
        GPIO.output(RT, 0)
        GPIO.output(LT, 0)
        print "Reverse"
    else:
        BlinkC=0
        BFlag=0
        GPIO.output(ST, 1)
        GPIO.output(FW, 0)
        GPIO.output(RV, 0)
        GPIO.output(RT, 0)
        GPIO.output(LT, 0)

def Eye_Blink():
    global BFlag, BlinkC,On_Flag,Off_Flag
    if (Eye_EN == 1):
        if (On_Flag==1 and Off_Flag==0):
            if(AvgRaw>THR_EYE and AvgRaw<350):
                BFlag=1
                GPIO.output(16, 1)
                print "------BLINK-----"
                BlinkC = BlinkC+1
                drive()
            else:
                if(AvgRaw>350):
                    On_Flag=0
                    Off_Flag=1
                BFlag=0
                drive()
                GPIO.output(16, 0)
        else:
            BFlag=0
            drive()
            GPIO.output(16, 0)
    else:
        BFlag=0
        drive()
        GPIO.output(16, 0)
            
    
def OneSecRawFunc():
    global j,n,Temp,AvgRaw,TempAvg,On_Flag,Off_Flag
    if(j<512):
        Temp = Temp+RawData
        j = j+1
    else:
        AvgRaw=Temp/512
        if (On_Flag==0 and Off_Flag==1):
            if (n<3):
                TempAvg = TampAvg+AvgRaw
                n = n+1
            else:
                TempAvg = TempAvg/3
                if (TempAvg<EEG_AVG):
                    On_Flag=1
                    Off_Flag=0
                n=0
                TempAvg=0
        Eye_Blink()
        j=0
        Temp=0

if __name__ == '__main__':
    mac = '20:68:9D:A8:50:04'
##    print "do you want to use the default MAC address?(Y/n):"
##    mac_yn = raw_input().lower()
##    while not (mac_yn=='' or mac_yn.startswith('y')):
##        print "What mac address would you like to use?:"
##        mac = raw_input().lower()
##        if re.match(r'[a-f0-9][a-f0-9]:[a-f0-9][a-f0-9]:[a-f0-9][a-f0-9]:[a-f0-9][a-f0-9]:[a-f0-9][a-f0-9]:[a-f0-9][a-f0-9]',mac):
##            mac_yn = 'y'
##        else:
##            print "A valid mac address is something like 9C:B7:0D:72:CD:02." 
##            print "Try again?"
    mindwaveDataPointReader = MindwaveDataPointReader()
    mindwaveDataPointReader.start(mac)
    print "Connecting to MindWave Mobile with Mac Address 20:68:9D:A8:50:04 ..."
    
    while(True):
        dataPoint = mindwaveDataPointReader.readNextDataPoint()
        if (not dataPoint.__class__ is RawDataPoint):
            if (str(dataPoint).find("Poor") >= 0):
                PoorSignal=int(re.search(r'\d+', str(dataPoint)).group())
                if( PoorSignal == 0):
                    GPIO.output(15, 1)
                    Eye_EN=1
                else:
                    GPIO.output(15, 0)
                    GPIO.output(29, 0)
                    GPIO.output(31, 0)
                    GPIO.output(33, 0)
                    GPIO.output(35, 0)
                    GPIO.output(37, 0)
                    Eye_EN=0
            if (str(dataPoint).find("Attention") >= 0):
                Attention=int(re.search(r'\d+', str(dataPoint)).group())
                if(Attention>0 and Attention<20):
                    GPIO.output(29, 1)
                    GPIO.output(31, 0)
                    GPIO.output(33, 0)
                    GPIO.output(35, 0)
                    GPIO.output(37, 0)
                elif (Attention>=20 and Attention<40):
                    GPIO.output(29, 1)
                    GPIO.output(31, 1)
                    GPIO.output(33, 0)
                    GPIO.output(35, 0)
                    GPIO.output(37, 0)
                elif (Attention>=40 and Attention<60):
                    GPIO.output(29, 1)
                    GPIO.output(31, 1)
                    GPIO.output(33, 1)
                    GPIO.output(35, 0)
                    GPIO.output(37, 0)
                elif (Attention>=60 and Attention<80):
                    GPIO.output(29, 1)
                    GPIO.output(31, 1)
                    GPIO.output(33, 1)
                    GPIO.output(35, 1)
                    GPIO.output(37, 0)
                elif (Attention>=80):
                    GPIO.output(29, 1)
                    GPIO.output(31, 1)
                    GPIO.output(33, 1)
                    GPIO.output(35, 1)
                    GPIO.output(37, 1)
            if (str(dataPoint).find("Meditation") >= 0):
                Meditation=int(re.search(r'\d+', str(dataPoint)).group())
            print dataPoint
            drive()
        else:
            if (str(dataPoint).find("Raw") >= 0):
                RawData=int(re.search(r'\d+', str(dataPoint)).group())
            OneSecRawFunc()
        
            

            
                
