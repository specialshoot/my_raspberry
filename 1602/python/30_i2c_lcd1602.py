#!/usr/bin/env python
import LCD1602
import time
import os

def getCPUTemperature():
	return os.popen('vcgencmd measure_temp').readline()
	
def getCPUuse():
	return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip()))
	
def getRAMinfo():
	p=os.popen('free')
	i=0;
	while 1:
		i=i+1
		line=p.readline()
		if i==2:
			RAM_status=line.split()[1:4]
			RAM_total=int(RAM_status[0])
			RAM_used=int(RAM_status[1])
			return RAM_used*100/RAM_total

def setup():
	LCD1602.init(0x27, 1)  # init(slave address, background light)
	while True:
		LCD1602.write(0, 0, getCPUTemperature())
		LCD1602.write(0, 1, getCPUuse()+" : "+str(getRAMinfo())+'%')
		time.sleep(2)


def loop():
    space = '                '
    greetings = 'Thank you for buying ZHINENG Sensor Kit for Raspberry! ^_^'
    greetings = space + greetings
    while True:
        tmp = greetings
        for i in range(0, len(greetings)):
            LCD1602.write(0, 0, tmp)
            tmp = tmp[1:]
            time.sleep(0.8)
            LCD1602.clear()


def destroy():
    pass


if __name__ == "__main__":
    try:
        setup()
        while True:
            pass
    except KeyboardInterrupt:
        destroy()
