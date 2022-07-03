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
#include "pn532_rpi.h"
#include "PDD_rpi.h"


int main(int argc, char** argv) {
	DuelDisk Disk1;
	CardData Card1;
	CardData Card2;
	PN532 pn532;

	if (!(PN532_Init())) printf("Initialization Eorror\r\n");

	


	//while(1)
}
