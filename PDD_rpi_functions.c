/*
	PDD_rpi_functions.c
	7-3-2022
	Mason Ripper
	Physical Duel Disk RaspberryPi 

*/
#include <stdio.h>
#include <stdint.h>
#include "pn532.h"
#include "pn532_rpi.h"
#include "PDD_rpi.h"

void Card_Detection()
{
	uint32_t tag_id[2];
	uint8_t QTR_State;
	

	for (uint8_t i = 0; i < PN532_READER_AMOUNT; i++)
	{
		if (Disk1.Zone_Timeout[i] && Disk1.Zone_Active[i] == 3)		
		{														//Skip tag polling while zones are full
			Disk1.Zone_Timeout[i]--;							//A changed Zone_Active via game-state opens polling
			continue;
		}

		for (uin8_t j = 0; j < 3; j++)
		{														
			Mux_Select(3 * (i+1) - j);
			QTR_State |= QTR_Read(QTR_DIGITAL) << j;			//Poll QTR Reflectance Sensors to determine populated zones
			if (j == 0) Disk1.Card_Bottom_Face[i] = QTR_Read(QTR_ANALOG);//Determine Face-up or Face-down
			if (j == 4)
			{
				Disk1.Card_Top_Face[i] = QTR_Read(QTR_ANALOG);	//Determine Face-up or Face-down
				Disk1.Card_Top_AtkDef[i] = 0;
			}
			else {
				Disk1.Card_Top_Face[i] = 0;
				Disk1.Card_Top_AtkDef[i] = 1;
			}
			
		}
		if (!QTR_State){Card_Updates(i,0);continue;}				//If zone is empty skip rest of loop


		PN532_timeout = PN532_2TAG_TIMEOUT;
		while (!PN532_timeout)										//Poll PN532 for card data
		{
			PN532_Read();
			PN532_timeout--;
		}
		
		
		


	}
	//Zone Select
	//PN532 Read
	//QTR Read
	//Populate Data
}
//******************
void Card_Updates(uint8_t zone_num, bool empty_zone)
{
	switch
}
//******************
int8_t PN532_Init()
{
	for (uint8_t i = 0; i < PN532_READER_AMOUNT; i++)
	{
		Mux_Select(i);
		PN532_I2C_Init(&pn532);
		if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK)
		{
			printf("Zone: %u\r\n", i);
			printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
		} else {
			return PN532_STATUS_ERROR;
		}
		PN532_SamConfiguration(&pn532);
	}
	return 0;
}
//***********************************************
void PN532_Read(CardData *card)
{
	uint8_t buff[255];
	uint8_t uid[MIFARE_UID_MAX_LENGTH];
	uint8_t key_a[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint32_t pn532_error = PN532_ERROR_NONE;
	int32_t uid_len = 0;

}
//***********************************************
void Mux_Select(uin8_t channel)
{
	if (channel & 1) printf("1");
	if (channel & 2) printf("1");
	if (channel & 4) printf("1");
	if (channel & 8) printf("1");
	printf("MUX\r\n");
}
//***********************************************
uint8_t QTR_Read(bool mode)
{
	printf("QTR\r\n");
	return 1;
}