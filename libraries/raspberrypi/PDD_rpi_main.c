/*
	PDD_rpi.h
	7-3-2022
	Mason Ripper
	Physical Duel Disk RaspberryPi 

*/

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "pn532.h"
#include "PDD_rpi.h"
#include "pn532_rpi.h"
#include "PDD_rpi_functions.c"



int main(int argc, char** argv) {
	

	if (!(PN532_Init())) printf("Initialization Error\r\n");

	


	//while(1)
}
