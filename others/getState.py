import os
def getCPUTemperature():
	res=os.popen('vcgencmd measure_temp').readline()
	return (res.replace("temp=","").replace("'C\n",""))
	
def getRAMinfo():
	p=os.popen('free')
	i=0;
	while 1:
		i=i+1
		line=p.readline()
		if i==2:
			return(line.split()[1:4])

def getCPUuse():
	return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip()))

def getDiskSpace():
	p=os.popen("df -h /")
	i=0
	while 1:
		i=i+1
		line=p.readline()
		if i==2:
			return(line.split()[1:5])
			
CPU_temp=getCPUTemperature()
CPU_usage=getCPUuse()

RAM_status=getRAMinfo()
RAM_total=round(int(RAM_status[0])/1000,1)
RAM_used=round(int(RAM_status[1])/1000,1)
RAM_free=round(int(RAM_status[2])/1000,1)

DISK_status=getDiskSpace()
DISK_total=DISK_status[0]
DISK_used=DISK_status[1]
DISK_free=DISK_status[2]

if __name__=='__main__':
	print('')
	print('CPU Temperature = '+CPU_temp)
	print('CPU Usage = '+CPU_usage)
	print('')
	print('RAM total = '+str(RAM_total)+' MB')
	print('RAM used = '+str(RAM_used)+' MB')
	print('RAM free = '+str(RAM_free)+' MB')
	print('')
	print('DISK total = '+DISK_total)
	print('DISK used = '+DISK_used)
	print('DISK free = '+DISK_free)
