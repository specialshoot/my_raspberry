#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>

#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define CPU_TEMP "vcgencmd measure_temp"
#define CPU_USE "top -n1 | awk '/Cpu\\(s\\):/ {print $2}'"
#define MAX_SIZE 32

int LCDAddr = 0x27;
int BLEN = 1;
int fd;

FILE *fp;

void write_word(int data){
	int temp = data;
	if ( BLEN == 1 )
		temp |= 0x08;
	else
		temp &= 0xF7;
	wiringPiI2CWrite(fd, temp);
}

void send_command(int comm){
	int buf;
	// Send bit7-4 firstly
	buf = comm & 0xF0;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (comm & 0x0F) << 4;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void send_data(int data){
	int buf;
	// Send bit7-4 firstly
	buf = data & 0xF0;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (data & 0x0F) << 4;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void init(){
	send_command(0x33);	// Must initialize to 8-line mode at first
	delay(5);
	send_command(0x32);	// Then initialize to 4-line mode
	delay(5);
	send_command(0x28);	// 2 Lines & 5*7 dots
	delay(5);
	send_command(0x0C);	// Enable display without cursor
	delay(5);
	send_command(0x01);	// Clear Screen
	wiringPiI2CWrite(fd, 0x08);
}

void clear(){
	send_command(0x01);	//clear Screen
}

void write_1602(int x, int y, char data[]){
	int addr, i;
	int tmp;
	if (x < 0)  x = 0;
	if (x > 15) x = 15;
	if (y < 0)  y = 0;
	if (y > 1)  y = 1;

	// Move cursor
	addr = 0x80 + 0x40 * y + x;
	send_command(addr);
	
	tmp = strlen(data);
	for (i = 0; i < tmp; i++){
		send_data(data[i]);
	}
}

double getCpuTemp(){
	int fd_cpu;
	double temp = 0;
	char buf[MAX_SIZE];
	
	fd_cpu = open(TEMP_PATH, O_RDONLY);
	if(fd_cpu < 0){
		fprintf(stderr, "failed to open thermal_zone0/temp\n");
		return -1;
	}
	
	if(read(fd_cpu, buf, MAX_SIZE) < 0){
		fprintf(stderr, "failed to read temp\n");
		return -1;
	}
	
	temp = atoi(buf)/1000.0;
	printf("temp: %.2f\n", temp);
	
	close(fd_cpu);
	return temp;
}

void stop(int signo){
	pclose(fp);
	fp = NULL;
	clear();
	write_1602(0, 0, "Bye");
	delay(3000);
	clear();
	_exit(0);
}

void main(){
	fd = wiringPiI2CSetup(LCDAddr);
	signal(SIGINT, stop);
	init();
	char info[16];
	while(1){
		//sprintf(cpu_temp, "temp: %.2f", getCpuTemp());
		memset(info,0,sizeof(info)/sizeof(char));
		fp = NULL;
		fp = popen(CPU_TEMP, "r");
		fread(info, sizeof(char),sizeof(info), fp);
		write_1602(0, 0, info);
		memset(info,0,sizeof(info)/sizeof(char));
		fp = NULL;
		fp = popen(CPU_USE, "r");
		fread(info, sizeof(char),sizeof(info), fp);
		write_1602(0, 1, info);
		delay(1000);
	}
	stop(0);
	clear();
}
