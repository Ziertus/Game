#include <cmsis_os.h>
#include <lpc17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "GLCD.h"
#include "uart.h"
#include "stdbool.h"
// #include "sodium.h"  https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c/39475626#39475626

int map[16][12] = {1};
int pos_x = 0;
int pos_y = 0;
int blockSize = 20;
int time = 0;
char final_time[6];

// Delay Function: source https://www.exploreembedded.com/wiki/LPC1768:_Led_Blinking
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
	
    for(i = 0; i < ms; i++)
		{
				for(j = 0; j < 20000; j++);
		}
}

// Function for drawing a 20x20 block on the LCD
// Input arguments designate top left pixel of the block
void putPix(unsigned int x, unsigned int y)
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			GLCD_PutPixel(x+i, y+j);
		}
	}
}

// Displays map on LCD
void displayMap(int map[16][12])
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			if (map[i][j] == 0)
			{
				GLCD_SetTextColor(Red);
			}
			else if (map[i][j] == 1)
			{
				GLCD_SetTextColor(White);
			}
			else if (map[i][j] == 2)
			{
				GLCD_SetTextColor(Yellow);
			}
			else if (map[i][j] == 3)
			{
				GLCD_SetTextColor(Green);
			}
			putPix(20*i, 20*j);
		}
	}
}

void monitor (void const *arg)
{
	GLCD_Init();
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
	displayMap(map);
}

void joystick(void const *arg)
{
	int up, right, down, left;
	while (1)
	{
		up = (LPC_GPIO1->FIOPIN >> 23) & 0x01;
		right = (LPC_GPIO1->FIOPIN >> 24) & 0x01;
		down = (LPC_GPIO1->FIOPIN >> 25) & 0x01;
		left = (LPC_GPIO1->FIOPIN >> 26) & 0x01;
		printf("Player Coordinates: (%d, %d) | Current: %d, Above: %d, Right: %d, Below: %d, Left: %d\n", 
			pos_x, pos_y, map[pos_x][pos_y], map[pos_x][pos_y-1], map[pos_x+1][pos_y], map[pos_x][pos_y+1], map[pos_x-1][pos_y]);
		if (!up && (map[pos_x][pos_y-1] == 1 || map[pos_x][pos_y-1] == 3) && pos_y > 0)
		{
			GLCD_SetTextColor(Red);
			pos_y--;
			if (map[pos_x][pos_y] == 1)
			{
				map[pos_x][pos_y] = 0;
				putPix(pos_x*blockSize, pos_y*blockSize);
				GLCD_SetTextColor(White);
				map[pos_x][pos_y+1] = 1;
				putPix(pos_x*blockSize, (pos_y+1)*blockSize);
			}
		}
		else if (!right && (map[pos_x+1][pos_y] == 1 || map[pos_x+1][pos_y] == 3) && pos_x < 16)
		{
			GLCD_SetTextColor(Red);
			pos_x++;
			if (map[pos_x][pos_y] == 1)
			{
				map[pos_x][pos_y] = 0;
				putPix(pos_x*blockSize, pos_y*blockSize);
				GLCD_SetTextColor(White);
				map[pos_x-1][pos_y] = 1;
				putPix((pos_x-1)*blockSize, pos_y*blockSize);
			}
		}
		else if (!down && (map[pos_x][pos_y+1] == 1 || map[pos_x][pos_y+1] == 3) && pos_y < 12)
		{
			GLCD_SetTextColor(Red);
			pos_y++;
			if (map[pos_x][pos_y] == 1)
			{
				map[pos_x][pos_y] = 0;
				putPix(pos_x*blockSize, pos_y*blockSize);
				GLCD_SetTextColor(White);
				map[pos_x][pos_y-1] = 1;
				putPix(pos_x*blockSize, (pos_y-1)*blockSize);
			}
		}
		else if (!left && (map[pos_x-1][pos_y] == 1 || map[pos_x-1][pos_y] == 3) && pos_x > 0)
		{
			GLCD_SetTextColor(Red);
			pos_x--;
			if (map[pos_x][pos_y] == 1)
			{
				map[pos_x][pos_y] = 0;
				putPix(pos_x*blockSize, pos_y*blockSize);
				GLCD_SetTextColor(White);
				map[pos_x+1][pos_y] = 1;
				putPix((pos_x+1)*blockSize, pos_y*blockSize);
			}
		}

		delay_ms(100);
		osThreadYield();
	}
}

void bombs (void const *arg)
{
	int buttonPushed, bomb = 7;
	uint32_t leds[8];

	// Loading array with pin values
	leds[0] = (1 << 28);
	leds[1] = (1 << 29);
	leds[2] = (1 << 31);

	for (int pin = 3; pin < 8; pin++)
	{
		leds[pin] = (1 << (pin-1));
	}
	
	// Set all pins to output
	for (int i = 0; i < 8; i++)
	{
		if (i < 3)
		{
			LPC_GPIO1->FIODIR |= leds[i];
			LPC_GPIO1->FIOSET |= leds[i];
		}
		else
		{
			LPC_GPIO2->FIODIR |= leds[i];
			LPC_GPIO2->FIOSET |= leds[i];
		}
	}
	
	while (true)
	{
		buttonPushed = (LPC_GPIO2->FIOPIN >> 10) & 0x01;
		if (!buttonPushed)
		{
			if (bomb > 2)
			{
				LPC_GPIO2->FIOCLR |= leds[bomb];
			}
			else
			{
				LPC_GPIO1->FIOCLR |= leds[bomb];
			}
			bomb--;
			if (bomb < 0)
			{
				osThreadTerminate(osThreadGetId());
			}
			osDelay(15000);
		}
	}
}

void win (void const *arg)
{
	while (true)
	{
		time++;
		if (map[pos_x][pos_y] == 3)
		{
			GLCD_Clear(White);
			unsigned char string1[6] = "You ";
			unsigned char string2[6] = "Win!";
			GLCD_SetTextColor(Black);
			GLCD_SetBackColor(White);
			GLCD_DisplayString(4, 6, 1, string1);
			GLCD_DisplayString(4, 10, 1, string2);
			sprintf(final_time, "Time: %ds", time);
			GLCD_DisplayString(6, 6, 1, (unsigned char*)final_time);
			while (true);
		}
		osDelay(7100);
	}
}

struct info_t
{
	int rec;
	uint32_t overflow;
	osMailQId qid;
};

typedef struct info_t* INFO;

uint32_t count;
struct info_t *s1, *s2;

int main (void)
{	
	// Generate map
	// Pixels: 240x320
	// Player = 0 | Path = 1 | Walls = 2 | Finish = 3
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			int random = rand() % 19;
			if (random < 11)
			{
				map[i][j] = 2;
			}
			else
			{
				map[i][j] = 1;
			}
		}
	}
	
	pos_y = rand() % 12;
	map[pos_x][pos_y] = 0;
	map[15][rand() % 12] = 3;
	
	monitor(NULL);
	
	// Struct
	s1 = (INFO)malloc(sizeof(struct info_t));
	s1 -> rec = 0;
	s1 -> overflow = 0;
	s2 = (INFO)malloc(sizeof(struct info_t));
	s2 -> rec = 0;
	s2 -> overflow = 0;
	
	// Multithreading
	osKernelInitialize();
	osKernelStart();

	osThreadDef(joystick, osPriorityNormal, 1, 0);
	osThreadDef(bombs, osPriorityNormal, 1, 0);
	osThreadDef(win, osPriorityNormal, 1, 0);

	osThreadCreate(osThread(joystick), NULL);
	osThreadCreate(osThread(bombs), NULL);
	osThreadCreate(osThread(win), NULL);
}
