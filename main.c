#include <cmsis_os.h>
#include "stdint.h"
#include <lpc17xx.h>
#include "stdio.h"
#include "stdlib.h"
#include "GLCD.h"
#include "uart.h"
#include "stdbool.h"

int map[16][12];

void putPix(unsigned int x, unsigned int y)
{
	for(int i = 0; i < 20; i++)
		for(int j = 0; j <20; j++)
			GLCD_PutPixel(x+i,y+j);
}

void displayMap(int map[16][12])
{
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j <12; j++)
		{
			if(map[i][j] == 0)
				GLCD_SetTextColor(Red);
			else if(map[i][j] == 1)
				GLCD_SetTextColor(White);
			else if(map[i][j] == 2)
				GLCD_SetTextColor(Yellow);
			else if(map[i][j] == 3)
				GLCD_SetTextColor(Green);
			putPix(20*i, 20*j);
		}
	}
}

struct info_t {
	int rec;
	uint32_t overflow;
	osMailQId qid;
};

typedef struct info_t* INFO;

uint32_t count;
struct info_t *s1, *s2;

void monitor (void const *arg) {
	GLCD_Init();
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
	char str[25];
	while (true)
	{
		// row / column
//		GLCD_DisplayString(0, 0, 1, (unsigned char*)"12345678901234567890");
//		sprintf(str, "Sent: %d", count/2 + count%2);
//		GLCD_DisplayString(1, 0, 1, (unsigned char*)str);
//		sprintf(str, "Received: %d", s1->rec);
//		GLCD_DisplayString(2, 0, 1, (unsigned char*)str);
//		sprintf(str, "Overflow: %d", s1->overflow);
//		GLCD_DisplayString(3, 0, 1, (unsigned char*)str);

//		GLCD_DisplayString(5, 0, 1, (unsigned char*)"Queue 2");
//		sprintf(str, "Sent: %d", count/2);
//		GLCD_DisplayString(6, 0, 1, (unsigned char*)str);
//		sprintf(str, "Received: %d", s2->rec);
//		GLCD_DisplayString(7, 0, 1, (unsigned char*)str);
//		sprintf(str, "Overflow: %d", s2->overflow);
//		GLCD_DisplayString(8, 0, 1, (unsigned char*)str);
		displayMap(map);
	}
}

int main (void)
{
	//Generate Map
	//Pixel: 240x320
	//Walls = 2  |  Path = 1  |  Player = 0  |  Finish = 3
	for(int i = 0; i < 16; i++)
		for(int j = 0; j < 12; j++)
		{
			int random = rand()% 19;
			if(random < 11)
				map[i][j] = 2;
			else
				map[i][j] = 1;
		}

	map[0][rand()%12] = 0;
	map[15][rand()%12] = 3;
	
	// Multi-Threading
	osKernelInitialize();
	osKernelStart();
	osThreadDef(monitor, osPriorityNormal, 1, 0);
	s1 = (INFO)malloc(sizeof(struct info_t));
	s1->rec = 0;
	s1->overflow = 0;
	s2 = (INFO)malloc(sizeof(struct info_t));
	s2->rec = 0;
	s2->overflow = 0;
	osThreadCreate(osThread(monitor), NULL);
}
