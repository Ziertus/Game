#include <cmsis_os.h>
#include "stdint.h"
#include <lpc17xx.h>
#include "stdio.h"
#include "stdlib.h"
#include "random.h"
#include "lfsr113.h"
#include "GLCD.h"
#include "uart.h"
#include "stdbool.h"

void putPix(unsigned int x, unsigned int y)
{
	for(int i = 0; i < 16; i++)
		for(int j = 0; j <24; j++)
			GLCD_PutPixel(x-3+i,y-4+j);
}
struct info_t {
	int rec;
	uint32_t overflow;
	osMailQId qid;
};

typedef struct info_t* INFO;

uint32_t count;
struct info_t *s1, *s2;


void client (void const *arg)
{
  while (true)
	{
	}
}

void server (void const *arg)
{
	while (true)
	{
		
	}
}

void monitor (void const *arg) {
	GLCD_Init();
	GLCD_Clear(Blue);
	GLCD_SetTextColor(White);
	GLCD_SetBackColor(Blue);
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
		putPix(5,5);
	}
}

int main (void)
{
	//Generate Map 10x20 0-9, 0-19
	//  Pixel: 24*10 x 16*20
	int map[10][20];
	// Multi-Threading
	osKernelInitialize();
	osKernelStart();
	osThreadDef(client, osPriorityNormal, 1, 0);
	osThreadDef(server, osPriorityNormal, 2, 0);
	osThreadDef(monitor, osPriorityNormal, 1, 0);
	s1 = (INFO)malloc(sizeof(struct info_t));
	s1->rec = 0;
	s1->overflow = 0;
	s2 = (INFO)malloc(sizeof(struct info_t));
	s2->rec = 0;
	s2->overflow = 0;
	printf("NO");
	osThreadCreate(osThread(monitor), NULL);
	osThreadCreate(osThread(client), NULL);
	osThreadCreate(osThread(server), s1);
	osThreadCreate(osThread(server), s2);
}
