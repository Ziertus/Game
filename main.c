#include <cmsis_os.h>
#include "stdint.h"
#include <lpc17xx.h>
#include "stdio.h"
#include "stdlib.h"
#include "GLCD.h"
#include "uart.h"
#include "stdbool.h"
//#include "sodium.h"  PAOLO PLZ https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c/39475626#39475626

int map[16][12] = {1};

// Function for drawing a 20x20 block on the LCD
// Input arguments designate top left pixel of the block.
void putPix(unsigned int x, unsigned int y)
{
	for(int i = 0; i < 20; i++)
		for(int j = 0; j <20; j++)
			GLCD_PutPixel(x+i,y+j);
}

//Displays Map on LCD
void displayMap(int map[16][12])
{
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 12; j++){
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
	
	while (true)
	{
		displayMap(map);
	}
}


void bombs (void const *arg) {
	int buttonPushed, bomb=7;
	uint32_t leds[8];
	// Loading array with pin values
	leds[0] = (1 << 28);
	leds[1] = (1 << 29);
	leds[2] = (1 << 31);
	for(int pin = 3; pin < 8; pin++)
		leds[pin] = (1 << (pin-1));
	
	// Set all pins to output
	for(int i = 0; i < 8; i++) {
		if(i < 3){
			LPC_GPIO1->FIODIR |= leds[i];
			LPC_GPIO1->FIOSET |= leds[i];
		}
		else{
			LPC_GPIO2->FIODIR |= leds[i];
			LPC_GPIO2->FIOSET |= leds[i];
		}
	}
	
	while (true){
		buttonPushed = (LPC_GPIO2->FIOPIN >> 10) & 0x01;
		if(!buttonPushed){
			if(bomb > 2){
				LPC_GPIO2->FIOCLR |= leds[bomb];
			}
			else{
				LPC_GPIO1->FIOCLR |= leds[bomb];
			}
			bomb--;
			if(bomb<0)
				osThreadTerminate(osThreadGetId());
			osDelay(15000);
		}
	}
}

int main (void)
{
	//Generate Map
	//Pixel: 240x320
	//Walls = 2  |  Path = 1  |  Player = 0  |  Finish = 3
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 12; j++){
			int random = rand()% 19;
			if(random < 11)
				map[i][j] = 2;
			else
				map[i][j] = 1;
		}
	}
	map[0][rand()%12] = 0;
	map[15][rand()%12] = 3;
	
	// Struct?
	s1 = (INFO)malloc(sizeof(struct info_t));
	s1->rec = 0;
	s1->overflow = 0;
	s2 = (INFO)malloc(sizeof(struct info_t));
	s2->rec = 0;
	s2->overflow = 0;
	
	// Multi-Threading
	osKernelInitialize();
	osKernelStart();
	osThreadDef(monitor, osPriorityNormal, 1, 0);
	osThreadDef(bombs, osPriorityNormal, 1, 0);

	osThreadCreate(osThread(monitor), NULL);
	osThreadCreate(osThread(bombs), NULL);
}
