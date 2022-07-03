/*
	PDD_rpi.h
	7-3-2022
	Mason Ripper
	Physical Duel Disk RaspberryPi   

*/


#ifndef PDD_RPI_H
#define PDD_RPI_H

#include PDD_rpi_functions.c

#define PN532_READER_AMOUNT		3
#define PN532_2TAG_TIMEOUT		120
#define QTR_READER_AMOUNT		3
#define QTR_ANALOG				1
#define QTR_DIGITAL				0


void Card_Detection();
void Card_Updates(uint8_t zone_num, bool empty_zone);
void Mux_Select(uint8_t channel);
int8_t PN532_Init();

typedef struct _DuelDisk 
{
	bool Card_Bottom_Face[PN532_READER_AMOUNT];		//0-face down, 1-face up
	bool Card_Top_Face[PN532_READER_AMOUNT];		//0-face down, 1-face up
	bool Card_Top_AtkDef[PN532_READER_AMOUNT];		//0-defense position, 1-attack position
	uint8_t Zone_Active[PN532_READER_AMOUNT];		//0-empty, 1-top zone, 2-bottom zone, 3-both zones 
	uint8_t Zone_Timeout[PN532_READER_AMOUNT];		//Time remaing before zone is rechecked
	uint32_t Card_Serial_Bottom[PN532_READER_AMOUNT];	//Card Serial # in bottom zone
	uint32_t Card_Serial_Top[PN532_READER_AMOUNT];		//Card Serial # in top zone
} DuelDisk;

typedef struct _CardData
{
	bool Spell_Trap;
	bool Monster;
	bool Stackable;
	uint32_t Serial_Num;
} CardData;

#endif	//PDD_RPI_H