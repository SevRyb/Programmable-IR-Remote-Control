/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "irmp.h"
#include "irsnd.h"
#include "display.h"
#include "button.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	DEVICE_MODE_OFF = 0,
    DEVICE_MODE_DEFAULT,
	DEVICE_MODE_MENU,
	DEVICE_MODE_RECEIVER,
	DEVICE_MODE_SAVES
} DeviceMode;

typedef struct {
	IRMP_DATA irmp_data;
	uint8_t btn_id;
} GPBtnBinding_t;

#define MAX_BANKS 10
#define MAX_GPBUTTONS 9

typedef struct {
    GPBtnBinding_t btn_bindings[MAX_BANKS][MAX_GPBUTTONS];
    uint32_t checksum;
} DeviceSettings_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FLASH_PAGE_ADDR ((uint32_t)0x0801FC00)  // Last 1KB of 128KB Flash

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DeviceMode g_DeviceMode = DEVICE_MODE_OFF;

uint8_t g_currentReceiver = 1;
uint8_t g_receiversEnabled = 0;
uint16_t g_currentReceiverPin = IR_RECV_38_Pin;

uint8_t g_currentBank = 0;
uint8_t g_currentGPButton = 0;  // From 1 to 9
uint8_t g_isGPButtonPressed = 0;
IRMP_DATA g_currentIrmpData;

uint8_t g_menuPageOption = 0;

DeviceSettings_t g_DeviceSettings;

struct {
	uint16_t adc_value;
	uint16_t update_time;
} g_batLevel;

struct {
	uint16_t time;
	uint8_t show;
} g_displayHint;

const char *PROTOCOL_NAMES[IRMP_N_PROTOCOLS+1] = {
		"uknown", "SIRCS", "NEC", "SAMSUNG", "MATSUSHITA", "KASEIKYO",
		"RECS80", "RC5", "DENON", "RC6", "SAMSUNG32", "APPLE",
		"RECS80EXT", "NUBERT", "BANG_OLUFSEN", "GRUNDIG", "NOKIA", "SIEMENS",
		"FDC", "RCCAR", "JVC", "RC6A", "NIKON", "RUWIDO",
		"IR60", "KATHREIN", "NETBOX", "NEC16", "NEC42", "LEGO",
		"THOMSON", "BOSE", "A1TVBOX", "ORTEK", "TELEFUNKEN", "ROOMBA",
		"RCMM32", "RCMM24", "RCMM12", "SPEAKER", "LGAIR", "SAMSUNG48",
		"MERLIN", "PENTAX", "FAN", "S100", "ACP24", "TECHNICS",
		"PANASONIC", "MITSU_HEAVY", "VINCENT", "SAMSUNGAH", "IRMP16", "GREE",
		"RCII", "METZ", "ONKYO", "RF_GEN24", "RF_X10", "RF_MEDION",
		"IRMP_MELINERA"
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/*--------------------------------------------------------*/
/*-------------------- Device Settings -------------------*/
/*--------------------------------------------------------*/
uint32_t CalcSimpleChecksum(uint8_t* data, size_t len);
uint8_t LoadSettingsFromFlash();
void SaveSettingsToFlash();
/*--------------------------------------------------------*/
/*----------------- Device State Controls ----------------*/
/*--------------------------------------------------------*/
void Device_ChangeMode(DeviceMode new_mode);
void Device_NextBank();
void Device_PrevBank();
void Device_ChangeMenuPageOption();
void Device_EnterMenuPageOption(uint8_t option);
void Device_SelectGPButton(uint8_t btn_id);
void Device_SetGPButton();
void Device_SwitchIRSensor();
void Device_RetrieveBindingFromGPBtn(uint8_t bank_id, uint8_t btn_id);
/*--------------------------------------------------------*/
/*------------------- Button Callbacks -------------------*/
/*--------------------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void onBtnMenu(ButtonEvent event, uint8_t id);
void onBtnSys(ButtonEvent event, uint8_t id);
void onBtnGeneralPurpose(ButtonEvent event, uint8_t id);
void onSysTickUpdate();
/*--------------------------------------------------------*/
/*-------------- IRMP Data Received Callback -------------*/
/*--------------------------------------------------------*/
void onIrmpDataReceived(IRMP_DATA* irmp_data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*--------------------------------------------------------*/
/*-------------------- Device Settings -------------------*/
/*--------------------------------------------------------*/
uint32_t CalcSimpleChecksum(uint8_t* data, size_t len)
{
    uint32_t sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}

uint8_t LoadSettingsFromFlash()
{
    // Read directly from flash
    DeviceSettings_t *stored = (DeviceSettings_t*)FLASH_PAGE_ADDR;
    // Calculate checksum of stored bindings
    uint32_t calc = CalcSimpleChecksum((uint8_t*)stored->btn_bindings, sizeof(stored->btn_bindings));
    if (calc == stored->checksum)
    {
        memcpy(&g_DeviceSettings, stored, sizeof(DeviceSettings_t));
        return 1;  // valid checksum
    }
    return 0;  // invalid checksum
}

void SaveSettingsToFlash()
{
	HAL_FLASH_Unlock();
	// Erase the flash page
	FLASH_EraseInitTypeDef eraseInit;
	uint32_t pageError;

	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInit.PageAddress = FLASH_PAGE_ADDR;
	eraseInit.NbPages = 1;

	if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK)
	{
	   // Handle erase error
	   HAL_FLASH_Lock();
	   return;
	}

	g_DeviceSettings.checksum = CalcSimpleChecksum((uint8_t*)g_DeviceSettings.btn_bindings, sizeof(g_DeviceSettings.btn_bindings));

    uint32_t* data = (uint32_t*)&g_DeviceSettings;
    uint32_t word_count = sizeof(DeviceSettings_t) / 4;

    for (uint32_t i = 0; i < word_count; ++i)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PAGE_ADDR + i * 4, data[i]);
    }
	HAL_FLASH_Lock();

	Display_TurnOn(0);
	Display_TurnOn(1);
}

/*--------------------------------------------------------*/
/*----------------- Device State Controls ----------------*/
/*--------------------------------------------------------*/
void Device_ChangeMode(DeviceMode new_mode)
{
	if (g_DeviceMode == new_mode)
		return;
	DeviceMode prev_mode = g_DeviceMode;
	g_DeviceMode = new_mode;
	if (prev_mode == DEVICE_MODE_RECEIVER)
	{
		g_currentReceiver = 1;
		g_receiversEnabled = 0;
		HAL_GPIO_WritePin(IR_RECV_EN_GPIO_Port, IR_RECV_EN_Pin, GPIO_PIN_RESET);  // Enable IR receivers
	}
	else if (prev_mode == DEVICE_MODE_DEFAULT)
	{
		Display_TurnOn(1);
	}
	switch (g_DeviceMode)
	{
		case DEVICE_MODE_DEFAULT:
			g_displayHint.show = 1;
			Display_TurnOn(1);
			Display_Paint_DefaultPage();
			Display_DefaultPage_Set_Bank(g_currentBank);
			Display_Update();
			break;
		case DEVICE_MODE_MENU:
			Display_Paint_MenuPage();
			Display_MenuPage_Set_Option(g_menuPageOption);
			Display_Update();
			HAL_ADC_Start_IT(&hadc1);
			break;
		case DEVICE_MODE_RECEIVER:
			g_currentReceiver = 1;
			g_receiversEnabled = 1;
			g_currentGPButton = 0;
			Display_Paint_ReceiverPage();
			Display_ReceiverPage_Set_BankOption(g_currentBank);
			Display_ReceiverPage_Set_RecvOption(g_currentReceiver);
			Display_Update();
			HAL_GPIO_WritePin(IR_RECV_EN_GPIO_Port, IR_RECV_EN_Pin, GPIO_PIN_SET);  // Enable IR receivers
			break;
		case DEVICE_MODE_SAVES:
			g_currentGPButton = 1;
			Display_Paint_SavesPage();
			Display_ReceiverPage_Set_BtnOption(g_currentGPButton);
			Display_ReceiverPage_Set_BankOption(g_currentBank);
			Device_RetrieveBindingFromGPBtn(g_currentBank, g_currentGPButton);
			Display_Update();
			break;
		default:
			break;
	}
}

void Device_NextBank()
{
	g_currentBank = g_currentBank < (MAX_BANKS-1) ? g_currentBank + 1 : g_currentBank;
	if (g_DeviceMode == DEVICE_MODE_DEFAULT)
	{
		g_displayHint.show = 1;
		Display_TurnOn(1);
		Display_DefaultPage_Set_Bank(g_currentBank);
		Display_Update();
	}
	else if (g_DeviceMode == DEVICE_MODE_RECEIVER)
	{
		Display_ReceiverPage_Set_BankOption(g_currentBank);
		Display_Update();
	}
	else if (g_DeviceMode == DEVICE_MODE_SAVES)
	{
		Device_RetrieveBindingFromGPBtn(g_currentBank, g_currentGPButton);
		Display_ReceiverPage_Set_BankOption(g_currentBank);
		Display_Update();
	}
}

void Device_PrevBank()
{
	g_currentBank = g_currentBank > 0 ? g_currentBank - 1 : g_currentBank;
	if (g_DeviceMode == DEVICE_MODE_DEFAULT)
	{
		g_displayHint.show = 1;
		Display_TurnOn(1);
		Display_DefaultPage_Set_Bank(g_currentBank);
		Display_Update();
	}
	else if (g_DeviceMode == DEVICE_MODE_RECEIVER)
	{
		Display_ReceiverPage_Set_BankOption(g_currentBank);
		Display_Update();
	}
	else if (g_DeviceMode == DEVICE_MODE_SAVES)
	{
		Device_RetrieveBindingFromGPBtn(g_currentBank, g_currentGPButton);
		Display_ReceiverPage_Set_BankOption(g_currentBank);
		Display_Update();
	}
}

void Device_ChangeMenuPageOption()
{
	g_menuPageOption = !g_menuPageOption;
	Display_MenuPage_Set_Option(g_menuPageOption);
	Display_Update();
}

void Device_EnterMenuPageOption(uint8_t option)
{
	switch (option) {
		case 0:
			Device_ChangeMode(DEVICE_MODE_RECEIVER);
			break;
		case 1:
			Device_ChangeMode(DEVICE_MODE_SAVES);
			break;
		default:
			break;
	}
}

void Device_SelectGPButton(uint8_t btn_id)
{
	g_currentGPButton = btn_id;
	if (g_DeviceMode == DEVICE_MODE_SAVES)
	{
		Device_RetrieveBindingFromGPBtn(g_currentBank, g_currentGPButton);
	}
	Display_ReceiverPage_Set_BtnOption(g_currentGPButton);
	Display_ReceiverPage_Set_BtnSet(0);
	Display_Update();
}

void Device_SetGPButton()
{
	// General buttons have ids in range from 1 to 9
	if (g_currentGPButton > 0 && g_currentIrmpData.protocol != 0)
	{
		g_DeviceSettings.btn_bindings[g_currentBank][g_currentGPButton-1].irmp_data = g_currentIrmpData;
		g_DeviceSettings.btn_bindings[g_currentBank][g_currentGPButton-1].btn_id = g_currentGPButton;
		Display_ReceiverPage_Set_BtnSet(1);
		Display_Update();
	}
	else
	{
		Display_ReceiverPage_Set_BtnSet(0);
		Display_Update();
	}
}

void Device_SwitchIRSensor()
{
	g_receiversEnabled = !g_receiversEnabled;
	if (g_receiversEnabled)
	{
		HAL_GPIO_WritePin(IR_RECV_EN_GPIO_Port, IR_RECV_EN_Pin, GPIO_PIN_SET);  // Enable IR receivers
		if (g_currentReceiver < 3)
			++g_currentReceiver;
		else
			g_currentReceiver = 1;
		Display_ReceiverPage_Set_RecvOption(g_currentReceiver);
		switch (g_currentReceiver) {
			case 1:
				g_currentReceiverPin = IR_RECV_38_Pin;
				break;
			case 2:
				g_currentReceiverPin = IR_RECV_36_Pin;
				break;
			case 3:
				g_currentReceiverPin = IR_RECV_40_Pin;
				break;
			default:
				break;
		}
	}
	else
	{
		HAL_GPIO_WritePin(IR_RECV_EN_GPIO_Port, IR_RECV_EN_Pin, GPIO_PIN_RESET);  // Disable IR receivers
		Display_ReceiverPage_Set_RecvOption(0);
	}
	Display_Update();
}

void Device_RetrieveBindingFromGPBtn(uint8_t bank_id, uint8_t btn_id)
{
	IRMP_DATA* irmp_data = &g_DeviceSettings.btn_bindings[bank_id][btn_id-1].irmp_data;
	Display_ReceiverPage_Set_IrmpData(irmp_data);
	Display_SavesPage_Set_ProtocolName(PROTOCOL_NAMES[irmp_data->protocol]);
}

/*--------------------------------------------------------*/
/*------------------- Button Callbacks -------------------*/
/*--------------------------------------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	Button_EXTI_Handler(GPIO_Pin);
}

void onBtnMenu(ButtonEvent event, uint8_t id)
{
	switch (event)
	{
		case BUTTON_EVENT_RELEASE:
			if (g_DeviceMode == DEVICE_MODE_DEFAULT)
			{
				Device_NextBank();
			}
			else if (g_DeviceMode == DEVICE_MODE_MENU)
			{
				Device_EnterMenuPageOption(g_menuPageOption);
			}
			else if (g_DeviceMode == DEVICE_MODE_SAVES)
			{
				Device_NextBank();
			}
			else if (g_DeviceMode == DEVICE_MODE_RECEIVER)
			{
				if (g_currentGPButton > 0 && g_currentGPButton <= 9 && g_isGPButtonPressed)
				{
					Device_NextBank();
				}
			}
			break;
		case BUTTON_EVENT_PRESS:
			if (g_DeviceMode == DEVICE_MODE_RECEIVER)
			{
				if (!g_isGPButtonPressed)
				{
					Device_SetGPButton();
				}
			}
			break;
		case BUTTON_EVENT_HOLD:
			if (g_DeviceMode == DEVICE_MODE_DEFAULT)
			{
				Device_ChangeMode(DEVICE_MODE_MENU);
			}
			else if (g_DeviceMode == DEVICE_MODE_RECEIVER)
			{
				SaveSettingsToFlash();
			}
			break;
		default:
			break;
	}
}

void onBtnSys(ButtonEvent event, uint8_t id)
{
	switch (event)
	{
		case BUTTON_EVENT_RELEASE:
			if (g_DeviceMode == DEVICE_MODE_DEFAULT)
			{
				Device_PrevBank();
			}
			else if (g_DeviceMode == DEVICE_MODE_MENU)
			{
				Device_ChangeMenuPageOption();
			}
			else if (g_DeviceMode == DEVICE_MODE_RECEIVER)
			{
				if (!g_isGPButtonPressed)
				{
					Device_SwitchIRSensor();
				}
				if (g_currentGPButton > 0 && g_currentGPButton <= 9 && g_isGPButtonPressed)
				{
					Device_PrevBank();
				}
			}
			else if (g_DeviceMode == DEVICE_MODE_SAVES)
			{
				Device_PrevBank();
			}
			break;
		case BUTTON_EVENT_PRESS:
			break;
		case BUTTON_EVENT_HOLD:
			if (g_DeviceMode == DEVICE_MODE_MENU)
			{
				Device_ChangeMode(DEVICE_MODE_DEFAULT);
			}
			if (g_DeviceMode == DEVICE_MODE_RECEIVER || g_DeviceMode == DEVICE_MODE_SAVES)
			{
				Device_ChangeMode(DEVICE_MODE_MENU);
			}
			break;
		default:
			break;
	}
}

void onBtnGeneralPurpose(ButtonEvent event, uint8_t id)
{
	switch (event)
	{
		case BUTTON_EVENT_PRESS:
			g_isGPButtonPressed = 1;
			if (g_DeviceMode == DEVICE_MODE_DEFAULT)
			{
				IRMP_DATA* irmp_data = &g_DeviceSettings.btn_bindings[g_currentBank][id-1].irmp_data;
				if (irmp_data->protocol == IRMP_JVC_PROTOCOL)
				{
					irmp_data->flags = 1;
				}
				irsnd_send_data(irmp_data, FALSE);
			}
			if (g_DeviceMode == DEVICE_MODE_RECEIVER || g_DeviceMode == DEVICE_MODE_SAVES)
			{
				Device_SelectGPButton(id);
			}
			break;
		case BUTTON_EVENT_RELEASE:
		case BUTTON_EVENT_LONG_RELEASE:
			g_isGPButtonPressed = 0;
			break;
		default:
			break;
	}
}

void onSysTickUpdate()
{
	Button_TimerTick();
	if (g_DeviceMode == DEVICE_MODE_DEFAULT)
	{
		if (g_displayHint.show)
		{
			if (g_displayHint.time < 1000)
				++g_displayHint.time;
			else
			{
				Display_TurnOn(0);
				g_displayHint.time = 0;
				g_displayHint.show = 0;
			}
		}
	}
	else if (g_DeviceMode == DEVICE_MODE_MENU)
	{
		if (g_batLevel.update_time < 2000)
			++g_batLevel.update_time;
		else
		{
			g_batLevel.update_time = 0;
			HAL_ADC_Start_IT(&hadc1);
		}
	}
}

/*--------------------------------------------------------*/
/*-------------- IRMP Data Received Callback -------------*/
/*--------------------------------------------------------*/
void onIrmpDataReceived(IRMP_DATA* irmp_data)
{
	g_currentIrmpData = *irmp_data;
	Display_ReceiverPage_Set_IrmpData(irmp_data);
	Display_ReceiverPage_Set_BtnSet(0);
	Display_ReceiverPage_Set_ProtocolName(PROTOCOL_NAMES[irmp_data->protocol]);
	Display_Update();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  if (!LoadSettingsFromFlash())
  {
      // Data corrupted or uninitialized — set defaults
      memset(&g_DeviceSettings, 0, sizeof(DeviceSettings_t));
      for (uint8_t bank = 0; bank < MAX_BANKS; ++bank)
      {
          for (uint8_t btn = 0; btn < MAX_GPBUTTONS; ++btn)
          {
        	  g_DeviceSettings.btn_bindings[bank][btn].btn_id = btn + 1;
          }
      }
      SaveSettingsToFlash();
  }
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);

  irmp_init();
  Display_Init();
  Button_Init();
  irsnd_init();

  HAL_ADCEx_Calibration_Start(&hadc1);

  Device_ChangeMode(DEVICE_MODE_DEFAULT);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
    	  irsnd_ISR();
    	  irmp_ISR();
    	  static IRMP_DATA irmp_data;
    	  if (irmp_get_data(&irmp_data))
    	  {
    		  onIrmpDataReceived(&irmp_data);
    	  }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (g_DeviceMode == DEVICE_MODE_MENU)
	{
		g_batLevel.adc_value = HAL_ADC_GetValue(&hadc1);
		float voltage =  ((3.3 * g_batLevel.adc_value) / 4096) * 2;
		float perc = (voltage - 2.9) / (4.2 - 2.9);
		perc = perc < 0.0 ? 0.0 : perc;
		perc = perc > 1.0 ? 1.0 : perc;
		Display_Set_BatteryLevel(perc);
		Display_Update();
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
