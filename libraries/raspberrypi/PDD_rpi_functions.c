/*
	PDD_rpi_functions.c
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

void Mux_Select(uint8_t channel)
{
	if (channel & 1) printf("1");
	if (channel & 2) printf("1");
	if (channel & 4) printf("1");
	if (channel & 8) printf("1");
	printf("MUX\r\n");
}
//*********************************************
int8_t PN532_Init()
{
	uint8_t buff[255];
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
// PN532_Read tag_amount is the expected amount of tags to be read
void PN532_Read(uint8_t tag_amount)
{
	bool cardtwo = false;
	uint8_t buff[255];
	uint8_t uid[MIFARE_UID_MAX_LENGTH];
	uint8_t key_a[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint8_t block_number = 1;									//BLOCK ONE is used to store card serial number and monster/spell status
	uint8_t PN532_Timeout = PN532_2TAG_TIMEOUT;
	uint32_t pn532_error = PN532_ERROR_NONE;
	int32_t uid_len = 0;										//Check for uint vs int declaration

	Card1.Serial_Num = 0; Card2.Serial_Num = 0; Disk1.Card_Serial_Top = 0; Disk1.Card_Serial_Bottom = 0; //Value purge

	while (PN532_Timeout)
	{
		uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);

		if (uid_len == PN532_STATUS_ERROR) {
			printf(".");
		}
		else {
			printf("Found card with UID: ");				//TO BE REMOVED IN PRODUCTION BUILD
			for (uint8_t i = 0; i < uid_len; i++) {
				printf("%02x ", uid[i]);
			}
			printf("\r\n");
			break;
		}

		pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len, block_number, MIFARE_CMD_AUTH_A, key_a);
		if (pn532_error != PN532_ERROR_NONE) continue;

		pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
		if (pn532_error != PN532_ERROR_NONE) continue;


		switch (tag_amount)				//If two tags are required fill card2 first then compare for card1
		{
		case 1:
			Card1.Stackable = buff[1];
			Card1.Serial_Num = buff[10] << 16 + buff[9] << 14 + buff[8] << 12 + buff[7] << 10 + buff[6] << 8 + buff[5] << 6 + buff[4] << 4 + buff[3] << 2 + buff[2];
			if (buff[0] == 0x01)
			{
				Card1.Monster = true;
				Card1.Spell_Trap = false;
				Disk1.Card_Serial_Top = Card1.Serial_Num;
				Disk1.Card_Serial_Bottom = 0;
			}
			if (buff[0] == 0x02)
			{
				Card1.Monster = false;
				Card1.Spell_Trap = true;
				Disk1.Card_Serial_Top = 0;
				Disk1.Card_Serial_Bottom = Card1.Serial_Num;
			}
			break;

		case 2:
			Card2.Stackable = buff[1];
			Card2.Serial_Num = buff[10] << 16 + buff[9] << 14 + buff[8] << 12 + buff[7] << 10 + buff[6] << 8 + buff[5] << 6 + buff[4] << 4 + buff[3] << 2 + buff[2];
			if (buff[0] == 0x01)
			{
				Card2.Monster = true;
				Card2.Spell_Trap = false;
				Disk1.Card_Serial_Top = Card2.Serial_Num;
				Disk1.Card_Serial_Bottom = 0;
			}
			if (buff[0] == 0x02)
			{
				Card2.Monster = false;
				Card2.Spell_Trap = true;
				Disk1.Card_Serial_Top = 0;
				Disk1.Card_Serial_Bottom = Card2.Serial_Num;
			}
			if (Card2.Serial_Num) cardtwo = true;
			break;

		case 0:
			printf("How did we get here?");
			break;
		}

		for (uint8_t i = 0; i < 16; i++)
		{
			printf("%02x ", buff[i]);
		}
		printf("\r\n");

		if (cardtwo)			//If one tag as already been read proceed to populating card1 with received data untill card serial # are different or timeout
		{
			tag_amount = 1;
			if (Card1.Serial_Num != 0 || Card1.Serial_Num != Card2.Serial_Num) return;
		}
		if (tag_amount == 1 && Card1.Serial_Num != 0) return;	//If one tag was read and one tag is required exit
		PN532_Timeout--;
	}

}
//***********************************************
// Card_Updates empty_zone is set to 1 when no cards are present
void Card_Updates(uint8_t zone_num, bool empty_zone)
{
	printf("Card Updates/r/n");
}
//***********************************************
uint8_t QTR_Read(bool mode)
{
	printf("QTR\r\n");
	return 1;
}
//**********************************************
void Card_Detection()
{
	uint32_t tag_id[2];
	uint8_t QTR_State;
	uint8_t PN532_Timeout;

	for (uint8_t i = 0; i < PN532_READER_AMOUNT; i++)			//Polls zones
	{
		if (Disk1.Zone_Timeout[i] && Disk1.Zone_Active[i] == 3)		
		{														//Skip tag polling while zones are full
			Disk1.Zone_Timeout[i]--;							//A changed Zone_Active via game-state opens polling
			continue;
		}

		for (uint8_t j = 0; j < 3; j++)							//Polls QTRs
		{														
			Mux_Select(3 * (i+1) - j);
			QTR_State |= QTR_Read(QTR_DIGITAL) << j;			//Poll QTR Reflectance Sensors to determine populated zones
			if (QTR_State & 1) Disk1.Card_Bottom_Face[i] = QTR_Read(QTR_ANALOG);	//Determine Face-up or Face-down
			if (QTR_State & 4)	// QTR_STATE 0XXX 0,DEF,ATK,Bottom
			{
				Disk1.Card_Top_Face[i] = QTR_Read(QTR_ANALOG);	//Determine Face-up or Face-down
				Disk1.Card_Top_AtkDef[i] = 0;					//0 -> Face-up
			} 
			else if(QTR_State & 2)
			{
				Disk1.Card_Top_Face[i] = 0;						//0 -> Face-up
				Disk1.Card_Top_AtkDef[i] = 1;					//1 -> Atk position
			}
			
		}
		if (!QTR_State){Card_Updates(i,1);continue;}			//If zone is empty skip rest of loop
		if (QTR_State == 1) PN532_Read(1,0);
		PN532_Read();											//Read nfc tag data
		
		
		
		


	}
	//Zone Select
	//PN532 Read
	//QTR Read
	//Populate Data
}
//******************
