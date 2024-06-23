#include "bsp.h"

void copyArray(unsigned char source[], unsigned char destination[], unsigned char length) {
	memcpy(destination, source, length * sizeof(int));
}

void AppTaskLED(void *argument){	
	osStatus_t os_StatusQueue; 
	uint16_t usFrequency = 50;
	uint32_t tick;
	tick = osKernelGetTickCount();
	while(1){
		tick += usFrequency;                          
		osDelayUntil(tick);		
	}
}

