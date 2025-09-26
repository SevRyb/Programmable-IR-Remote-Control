/*
 * display.h
 *
 *  Created on:
 *      Author: Severyn
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "irmp.h"
#include "stdio.h"

void Display_Init();
void Display_Update();
void Display_TurnOn(const uint8_t on);
// Pages
void Display_Paint_DefaultPage();
void Display_Paint_MenuPage();
void Display_Paint_ReceiverPage();
void Display_Paint_SavesPage();
// Fragments
void Display_Paint_Protocol();
void Display_Paint_RecvOptions();
void Display_Paint_BtnOption();
void Display_Paint_Battery(const uint8_t x, const uint8_t y);
// Data fields fillers
void Display_DefaultPage_Set_Bank(const uint8_t bank_id);
void Display_MenuPage_Set_Option(const uint8_t option_id);
void Display_Set_BatteryLevel(float perc);
void Display_ReceiverPage_Set_IrmpData(IRMP_DATA* irmp_data);
void Display_ReceiverPage_Set_RecvOption(const uint8_t selection);
void Display_ReceiverPage_Set_BtnOption(const uint8_t btn_id);
void Display_ReceiverPage_Set_BankOption(const uint8_t bank_id);
void Display_ReceiverPage_Set_BtnSet(const uint8_t set);
void Display_ReceiverPage_Set_ProtocolName(const char* name);
void Display_SavesPage_Set_ProtocolName(const char* name);

#endif /* INC_DISPLAY_H_ */
