/*
 * display.c
 *
 *  Created on:
 *      Author: Severyn
 */

#include "display.h"

void Display_Init()
{
	ssd1306_Init();
}

void Display_TurnOn(const uint8_t on)
{
	ssd1306_SetDisplayOn(on);
}

void Display_Update()
{
	ssd1306_UpdateScreen();
}
/*---------------- Pages ----------------*/
void Display_Paint_DefaultPage()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(25, 23);
	ssd1306_WriteString("BANK:", Font_11x18, White);
}
void Display_Paint_MenuPage()
{
	ssd1306_Fill(Black);
	Display_MenuPage_Set_Option(0);
	Display_Paint_Battery(97, 19);
}
void Display_Paint_ReceiverPage()
{
	Display_Paint_Protocol();
	Display_Paint_RecvOptions();
	Display_Paint_BtnOption();
}
void Display_Paint_SavesPage()
{
	Display_Paint_Protocol();
	Display_Paint_BtnOption();
}
/*---------------- Fragments ----------------*/
void Display_Paint_Protocol()
{
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("Prot", Font_7x10, White);

	ssd1306_SetCursor(0, 10);
	ssd1306_WriteString("Addr", Font_7x10, White);

	ssd1306_SetCursor(0, 20);
	ssd1306_WriteString("Comm", Font_7x10, White);

	ssd1306_SetCursor(0, 30);
	ssd1306_WriteString("Flag", Font_7x10, White);

	ssd1306_Line(31, 0, 31, 40, White);

	ssd1306_Line(31+1, 5, 35, 5, White);
	ssd1306_Line(31+1, 15, 35, 15, White);
	ssd1306_Line(31+1, 25, 35, 25, White);
	ssd1306_Line(31+1, 35, 35, 35, White);
}

void Display_Paint_RecvOptions()
{
	ssd1306_SetCursor(8, 54);
	ssd1306_WriteString("38", Font_7x10, White);
	ssd1306_DrawRectangle(25, 54, 34, 63, White);

	ssd1306_SetCursor(50, 54);
	ssd1306_WriteString("36", Font_7x10, White);
	ssd1306_DrawRectangle(67, 54, 76, 63, White);

	ssd1306_SetCursor(93, 54);
	ssd1306_WriteString("40", Font_7x10, White);
	ssd1306_DrawRectangle(110, 54, 119, 63, White);
}

void Display_Paint_BtnOption()
{
	ssd1306_SetCursor(92, 0);
	ssd1306_WriteString("BTN", Font_6x8, White);
	ssd1306_DrawRectangle(87, 8, 112, 34, White);
	ssd1306_SetCursor(87-6, 34+4);
	ssd1306_WriteString("BANK:", Font_6x8, White);
}

void Display_Paint_Battery(const uint8_t x, const uint8_t y)
{
	ssd1306_Line(x, y+2, x+5, y+2, White);
	ssd1306_Line(x+5, y, x+5, y+1, White);
	ssd1306_Line(x+6, y, x+9, y, White);
	ssd1306_Line(x+10, y, x+10, y+1, White);
	ssd1306_Line(x+10, y+2, x+15, y+2, White);
	ssd1306_Line(x, y+3, x, y+24, White);
	ssd1306_Line(x+15, y+3, x+15, y+24, White);
	ssd1306_Line(x, y+25, x+15, y+25, White);
}

/*---------------- Data fields fillers ----------------*/
void Display_DefaultPage_Set_Bank(const uint8_t bank_id)
{
	ssd1306_FillRectangle(80, 23, 101, 40, Black);
	char buf[3];
	snprintf(buf, 3, "%u", bank_id);
	ssd1306_SetCursor(80, 23);
	ssd1306_WriteString(buf, Font_11x18, White);
}

void Display_MenuPage_Set_Option(const uint8_t option_id)
{
	ssd1306_FillRectangle(0, 17, 59, 46, Black);

	ssd1306_SetCursor(2, 19+1);
	ssd1306_WriteString("RECEIVER", Font_7x10, White);
	ssd1306_SetCursor(2, 35+1);
	ssd1306_WriteString("SAVES", Font_7x10, White);

	ssd1306_DrawRectangle(0, 17, 59, 30, White);
	ssd1306_DrawRectangle(0, 33, 59, 46, White);

	switch (option_id) {
		case 0:
			ssd1306_InvertRectangle(0+1, 17+1, 59-1, 30-1);
			break;
		case 1:
			ssd1306_InvertRectangle(0+1, 33+1, 59-1, 46-1);
			break;
		default:
			break;
	}
}

void Display_Set_BatteryLevel(float perc)
{
	const uint8_t x = 97;
	const uint8_t y = 19;
	ssd1306_FillRectangle(x+2, y+4, x+13, y+23, Black);
	uint8_t y_offset = (perc-0.0)/(1.0-0.0) * (4-23) + 23;
	ssd1306_FillRectangle(x+2, y+y_offset, x+13, y+23, White);
}

void Display_ReceiverPage_Set_IrmpData(IRMP_DATA * irmp_data)
{
	  ssd1306_FillRectangle(42, 0, 70, 40, Black);

	  if (irmp_data == 0)
		  return;

	  char buf[8];

	  sprintf(buf, "%u", irmp_data->protocol);
	  ssd1306_SetCursor(42, 0);
	  ssd1306_WriteString(buf, Font_7x10, White);

	  sprintf(buf, "%04X", irmp_data->address);
	  ssd1306_SetCursor(42, 10);
	  ssd1306_WriteString(buf, Font_7x10, White);

	  sprintf(buf, "%04X", irmp_data->command);
	  ssd1306_SetCursor(42, 20);
	  ssd1306_WriteString(buf, Font_7x10, White);

	  sprintf(buf, "%02X", irmp_data->flags);
	  ssd1306_SetCursor(42, 30);
	  ssd1306_WriteString(buf, Font_7x10, White);
}

void Display_ReceiverPage_Set_RecvOption(const uint8_t selection)
{
	  ssd1306_FillRectangle(27, 56, 32, 61, Black);
	  ssd1306_FillRectangle(69, 56, 74, 61, Black);
	  ssd1306_FillRectangle(112, 56, 117, 61, Black);

	  switch (selection) {
		case 1:
			ssd1306_FillRectangle(27, 56, 32, 61, White);
			break;
		case 2:
			ssd1306_FillRectangle(69, 56, 74, 61, White);
			break;
		case 3:
			ssd1306_FillRectangle(112, 56, 117, 61, White);
			break;
		default:
			break;
	}
}

void Display_ReceiverPage_Set_BtnOption(const uint8_t btn_id)
{
	ssd1306_FillRectangle(87+1, 8+1, 112-1, 34-1, Black);
	if (btn_id == 0)
		return;
	char buf[2];
	snprintf(buf, 2, "%u", btn_id);
	ssd1306_SetCursor(95, 13);
	ssd1306_WriteString(buf, Font_11x18, White);
}

void Display_ReceiverPage_Set_BankOption(const uint8_t bank_id)
{
	ssd1306_FillRectangle(87-4+5*6, 34+4, 87-4+5*6+2*6, 34+4+8, Black);
	char buf[3];
	snprintf(buf, 3, "%u", bank_id);
	ssd1306_SetCursor(87-4+5*6, 34+4);
	ssd1306_WriteString(buf, Font_6x8, White);
}

void Display_ReceiverPage_Set_BtnSet(const uint8_t set)
{
	const uint8_t x = 116;
	const uint8_t y = 15;
	if (set)
	{
		ssd1306_FillRectangle(x, y, x+6, y+12, Black);
		ssd1306_Line(x, y+12/2, x+6, y, White);
		ssd1306_Line(x, y+12/2, x+6, y+12, White);
	}
	else
		ssd1306_FillRectangle(x, y, x+6, y+12, Black);
}

void Display_ReceiverPage_Set_ProtocolName(const char* name)
{
	ssd1306_FillRectangle(0, 4*10+4, 6*13, 4*10+8+4, Black);
	ssd1306_SetCursor(0, 4*10+4);
	ssd1306_WriteString(name, Font_6x8, White);
}

void Display_SavesPage_Set_ProtocolName(const char* name)
{
	ssd1306_FillRectangle(0, 64-8, 128, 64, Black);
	ssd1306_SetCursor(0, 64-8);
	ssd1306_WriteString(name, Font_6x8, White);
}

