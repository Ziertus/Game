#include "stdint.h"
#include "MPU9250.h"
#include "led.h"
#include <lpc17xx.h>
#include "stdio.h"
#include <cmsis_os.h>
#include "sensor_fusion.h"

// define mutexes
osMutexDef(mutex1);
osMutexDef(mutex2);
osMutexDef(mutex3);
osMutexId(mutex_id_1);
osMutexId(mutex_id_2);
osMutexId(mutex_id_3);

// read the data
void pull_data(void const *arg) {
	mutex_id_1 = osMutexCreate(osMutex(mutex1));
	mutex_id_2 = osMutexCreate(osMutex(mutex2));
	MPU9250_init(1,1);
	LED_setup();
	LED_display(MPU9250_whoami());  // Should be 0x71 on LEDS like the lights not the display.
	while(1) {
		osMutexWait(mutex_id_1, osWaitForever);
		MPU9250_read_gyro();
		MPU9250_read_acc();
		MPU9250_read_mag();
		osMutexRelease(mutex_id_2);
	}
}

// calculate x, y, z values from raw data
void commit_data(void const *arg) {
	mutex_id_2 = osMutexCreate(osMutex(mutex2));
	mutex_id_3 = osMutexCreate(osMutex(mutex3));
	int frequency = 5000;
	sensor_fusion_init();
	sensor_fusion_begin(frequency);
	while(1) {
		osMutexWait(mutex_id_2, osWaitForever);
		sensor_fusion_update(MPU9250_gyro_data[0],MPU9250_gyro_data[1],MPU9250_gyro_data[2],
			MPU9250_accel_data[0],MPU9250_accel_data[1],MPU9250_accel_data[2],
			MPU9250_mag_data[0],MPU9250_mag_data[1],MPU9250_mag_data[2]);
		osMutexRelease(mutex_id_3);
	}
}

// print data to serial
void push_data(void const *arg) {
	mutex_id_3 = osMutexCreate(osMutex(mutex2));
	mutex_id_1 = osMutexCreate(osMutex(mutex3));
	osMutexWait(mutex_id_3, osWaitForever);
	while(1) {
		osMutexWait(mutex_id_3, osWaitForever);
		float x, y, z;
		x = sensor_fusion_getPitch();
		y = sensor_fusion_getYaw();
		z = sensor_fusion_getRoll();
		printf("%f,%f,%f\n",  z, -x ,y); //-z -x y
		osMutexRelease(mutex_id_1);
	}
}

int main(void)
{
	// Multi-Threading
	osKernelInitialize();
	osKernelStart();
	
	osThreadDef(pull_data, osPriorityNormal, 1, 0);
	osThreadDef(commit_data, osPriorityNormal, 1, 0);
	osThreadDef(push_data, osPriorityNormal, 1, 0);
	
	osThreadCreate(osThread(pull_data), NULL);
	osThreadCreate(osThread(commit_data), NULL);
	osThreadCreate(osThread(push_data), NULL);
}
