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

// define new mailbox with 10 messages
osMailQDef(q1, 10, int);
osMailQDef(q2, 10, int);

// mailbox id
osMailQId q1_id;
osMailQId q2_id;

struct info_t {
	int rec;
	uint32_t overflow;
	osMailQId qid;
};

typedef struct info_t* INFO;

uint32_t count;
struct info_t *s1, *s2;
uint32_t time = 0;


void client (void const *arg)
{
  while (true)
	{
		// increment counter
		count++;
		if(count % 2 == 0)
		{
			// allocate messages
			int *mptr = osMailAlloc(q1_id, 0);
			if (mptr == NULL)
				s1->overflow++;
			// send allocated messages
			osMailPut(q1_id, mptr);
		}
		else
		{
			// allocate messages
			int *mptr = osMailAlloc(q2_id, 0);
			if (mptr == NULL)
			{
				s2->overflow++;
			}
			// send allocated message
			osMailPut(q2_id, mptr);
		}
		//uint32_t delay = ((next_event() * 200 * 10) >> 16);
		uint32_t delay = ((next_event() * 200 / 18) >> 16);
    osDelay(delay);
  }
}

void server (void const *arg)
{
	// initialize mailbox
	while (true)
	{
		uint32_t delay = ((next_event() * 200 / 10) >> 16);
		osDelay(delay);
		// receive message
		osEvent evt = osMailGet(((INFO)arg)->qid, osWaitForever);
		if (evt.status == osEventMail)
		{
			((INFO)arg)->rec++;
			// after check evt.status is osEventMail, then we can free up mailbox with
			osMailFree(((INFO)arg)->qid, evt.value.p);
		}
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
		time++;
		// row / column
		GLCD_DisplayString(0, 0, 1, (unsigned char*)"Queue 1");
		sprintf(str, "Sent: %d", count/2 + count%2);
		GLCD_DisplayString(1, 0, 1, (unsigned char*)str);
		sprintf(str, "Received: %d", s1->rec);
		GLCD_DisplayString(2, 0, 1, (unsigned char*)str);
		sprintf(str, "Overflow: %d", s1->overflow);
		GLCD_DisplayString(3, 0, 1, (unsigned char*)str);

		GLCD_DisplayString(5, 0, 1, (unsigned char*)"Queue 2");
		sprintf(str, "Sent: %d", count/2);
		GLCD_DisplayString(6, 0, 1, (unsigned char*)str);
		sprintf(str, "Received: %d", s2->rec);
		GLCD_DisplayString(7, 0, 1, (unsigned char*)str);
		sprintf(str, "Overflow: %d", s2->overflow);
		GLCD_DisplayString(8, 0, 1, (unsigned char*)str);

		sprintf(str, "Time: %d sec", time);
		GLCD_DisplayString(9, 0, 1, (unsigned char*)str);
		osDelay(7100);
	}
}

int main (void)
{
	
	// Multi-Threading
	osKernelInitialize();
	osKernelStart();
	osThreadDef(client, osPriorityNormal, 1, 0);
	osThreadDef(server, osPriorityNormal, 2, 0);
	osThreadDef(monitor, osPriorityNormal, 1, 0);
	q1_id = osMailCreate(osMailQ(q1), NULL);
	q2_id = osMailCreate(osMailQ(q2), NULL);
	s1 = (INFO)malloc(sizeof(struct info_t));
	s1->rec = 0;
	s1->overflow = 0;
	s1->qid = q1_id;
	s2 = (INFO)malloc(sizeof(struct info_t));
	s2->rec = 0;
	s2->overflow = 0;
	s2->qid = q2_id;
	printf("NO");
	osThreadCreate(osThread(monitor), NULL);
	osThreadCreate(osThread(client), NULL);
	osThreadCreate(osThread(server), s1);
	osThreadCreate(osThread(server), s2);
}
