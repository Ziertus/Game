#include <lpc17xx.h>
#include "stdio.h"
#include "GLCD.h"
#include <cmsis_os.h>

// Delay Function: source https://www.exploreembedded.com/wiki/LPC1768:_Led_Blinking
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
	
    for(i = 0; i < ms; i++)
        for(j = 0; j < 20000; j++);
}

// Represent output in binary with LEDs

int main(void)
{
	int output = 127;
	int num[8];
	// Loading array with pin values
	num[0] = (1 << 28);
	num[1] = (1 << 29);
	num[2] = (1 << 31);
	for(int pin = 3; pin < 8; pin++)
		num[pin] = (1 << (pin - 1));
	
	// Set all pins to output
	for(int i = 0; i < 8; i++) {
		if(i < 3)
			LPC_GPIO1->FIODIR |= num[i];
		else
			LPC_GPIO2->FIODIR |= num[i];
	}
	for (int pin = 7; pin >= 0; pin--) {
		if(output % 2)
		{
			if(pin > 2)
				LPC_GPIO2->FIOSET |= num[pin];
			else
				LPC_GPIO1->FIOSET |= num[pin];
		}
		else
			if(pin > 2)
				LPC_GPIO2->FIOCLR |= num[pin];
			else
				LPC_GPIO1->FIOCLR |= num[pin];
		output /= 2;
	}
	
	while(1);
}


// States the current state of the joystick
/*
int main(void)
{
	int left, right, up, down, press;
	while(1){
		left = (LPC_GPIO1->FIOPIN >> 26) & 0x01;
		right = (LPC_GPIO1->FIOPIN >> 24) & 0x01;
		up = (LPC_GPIO1->FIOPIN >> 23) & 0x01;
		down = (LPC_GPIO1->FIOPIN >> 25) & 0x01;
		press = (LPC_GPIO1->FIOPIN >> 20) & 0x01;
		if(!left)
			printf("Left, ");
		else if(!right)
			printf("Right, ");
		else if(!up)
			printf("Up, ");
		else if(!down)
			printf("Down, ");
		if(!press)
			printf("Pressed\n");
		else
			printf("Not Pressed\n");
		delay_ms(1000);
	}
}
*/

/*
// Read potentiometer
int main(void)
{
	LPC_SC->PCONP |= (1 << 12 );
	LPC_PINCON->PINSEL1 &= ~(0x3 << 18);
	LPC_PINCON->PINSEL1 |= (0x1 << 18);
	LPC_ADC -> ADCR = (1 << 2)| 
										(4 << 8)|
										(1 << 21);
	
	int ADC_Value;
	while(1) {
		LPC_ADC->ADCR |= (1<<24);
		delay_ms(100);
		while ((LPC_ADC->ADGDR) & (1 << 31));
	  ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF;
	  printf("%d\n", ADC_Value);
	}
}
*/

// LCD display
/*
int main(void)
{
	GLCD_Init();
	GLCD_Clear(Blue);
	unsigned char string1[6] = "Hello,";
	unsigned char string2[6] = "world!";
	GLCD_SetTextColor(White);
	GLCD_SetBackColor(Blue);
	GLCD_DisplayString(4, 4, 1, string1);
	GLCD_DisplayString(4, 11, 1, string2);
}
*/

// Putting it all together
/*
void potentiometer(void const *arg) {
	LPC_SC->PCONP |= (1 << 12 );
	LPC_PINCON->PINSEL1 &= ~(0x3 << 18);
	LPC_PINCON->PINSEL1 |= (0x1 << 18);
	LPC_ADC -> ADCR = (1 << 2)| 
										(4 << 8)|
										(1 << 21);
	
	int ADC_Value;
	while(1) {
		LPC_ADC->ADCR |= (1<<24);
		delay_ms(100);
	  ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF;
	  printf("%d\n", ADC_Value);
		osThreadYield();
	}
}

void joystick(void const *arg) {
	int left, right, up, down, press;
	GLCD_Init();
	GLCD_Clear(Blue);
	unsigned char clear[6] = "      ";
	unsigned char left2[6] = "Left  ";
	unsigned char right2[6] = "Right ";
	unsigned char up2[6] = "Up    ";
	unsigned char down2[6] = "Down  ";
	unsigned char press2[6] = "Press ";
	unsigned char notprs2[6] = "NotPrs";
	GLCD_SetTextColor(White);
	GLCD_SetBackColor(Blue);
	while(1){
		left = (LPC_GPIO1->FIOPIN >> 26) & 0x01;
		right = (LPC_GPIO1->FIOPIN >> 24) & 0x01;
		up = (LPC_GPIO1->FIOPIN >> 23) & 0x01;
		down = (LPC_GPIO1->FIOPIN >> 25) & 0x01;
		press = (LPC_GPIO1->FIOPIN >> 20) & 0x01;
		GLCD_DisplayString(4, 4, 1, clear);
		if(!left)
			GLCD_DisplayString(4, 4, 1, left2);
		else if(!right)
			GLCD_DisplayString(4, 4, 1, right2);
		else if(!up)
			GLCD_DisplayString(4, 4, 1, up2);
		else if(!down)
			GLCD_DisplayString(4, 4, 1, down2);
		if(!press)
			GLCD_DisplayString(5, 4, 1, press2);
		else
			GLCD_DisplayString(5, 4, 1, notprs2);

		delay_ms(500);
		osThreadYield();
	}
}

void pushbutton(void const *arg) {
	// Set pin 28 to output
	LPC_GPIO1->FIODIR |= (1 << 28);
	int buttonPushed;
	int count = 1;
	while(1){
		buttonPushed = (LPC_GPIO2->FIOPIN >> 10) & 0x01;
		if(!buttonPushed)
		{
			if(count % 2)
				LPC_GPIO1->FIOSET |= (1 << 28);
			else
				LPC_GPIO1->FIOCLR |= (1 << 28);
			delay_ms(100);
			count++;
		}
		osThreadYield();
	}
}

int main(void)
{
	osKernelInitialize();
	osKernelStart();
	
	osThreadDef(potentiometer, osPriorityNormal, 1, 0);
	osThreadDef(joystick, osPriorityNormal, 1, 0);
	osThreadDef(pushbutton, osPriorityNormal, 1, 0);
	
	osThreadCreate(osThread(potentiometer), NULL);
	osThreadCreate(osThread(joystick), NULL);
	osThreadCreate(osThread(pushbutton), NULL);
}
*/