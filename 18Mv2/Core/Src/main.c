/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

  /*
   * PIN ASSIGNMENTS:
   *
   * I2C for RFID Scanner:
   * VCC - 5V
   * GND - Breadboard ground
   * SCL - PB6
   * SDA - PB7
   *
   * SPI for LCD Display:
   * VCC      - 3.3V
   * GND      - Breadboard ground
   * CS       - PB15
   * RESET    - PB14
   * DATA/CMD - PB12
   * SDI/MOSI - PA7
   * SCK      - PA5
   * LED      - 3.3V
   * SDO/MISO - NC
   *
   * OUT OF DATE:
   * *****************
   * T_CLK    - PA5
   * T_CS     - PC6 this one is fine
   * T_DIN    - PA7
   * T_DO     - PB4
   * T_IRQ    - PC8 this one is fine
   * *****************
   *
   * Servos:
   * VCC  - 5V
   * GND  - Breaboard Ground
   * PWM1 - PA0
   * PWM2 - PA1
   * PWM3 - PB10
   * PWM4 - PB2
   * PWM5 - PA6
   *
   * IR Sensors:
   * VCC  - 3.3V
   * GND  - Breadboard Ground
   * IQR1 - PA10
   * IQR1 - PA11
   * IQR1 - PA12
   * IQR1 - PA15
   *
   */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "servo.h"
#include "flash.h"
#include "irsensor.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "claude_lcd.h"
#include "claude_touch.h"
#include "rfid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

servo_t pennyServo;
servo_t nickelServo;
servo_t dimeServo;
servo_t quarterServo;
servo_t slotServo;

pn532_t nfc;
pn532_tag_info_t tag;

enum {SPLASH, USE, THANKYOU} state;

uint32_t currentUser;
uint32_t currentMoney;

char buf[32];

TouchPoint tp;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

/*********************************************
 ********* SERVO INITIALIZATION **************
 *********************************************/

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  pennyServo   = servo_new(&(htim2.Instance->CCR1));
  nickelServo  = servo_new(&(htim2.Instance->CCR2));
  dimeServo    = servo_new(&(htim2.Instance->CCR3));
  quarterServo = servo_new(&(htim2.Instance->CCR4));
  slotServo    = servo_new(&(htim3.Instance->CCR1));

/*********************************************
 ********** FLASH INITIALIZATION *************
 *********************************************/

  initialize_accounts();
//  set_money_in_account(JACOB_UID, 0x11111111);
//  add_account(0x0A7593F3);
//  initialize_accounts();

//  ST7796S_SetSPI(&hspi1);
//  ST7796S_Init();
//
//  // Fill screen with blue background
//  ST7796S_FillScreen(0x001F);
//  HAL_Delay(1000);
//
//  // Draw some rectangles
//  ST7796S_FillRect(50, 50, 100, 80, 0xF800);    // Red
//  ST7796S_FillRect(200, 100, 120, 100, 0x07E0);  // Green
//  ST7796S_FillRect(100, 200, 200, 50, 0xFFE0);   // Yellow

/*********************************************
 ********* LCD+TOUCH INITIALIZATION **********
 *********************************************/

  ST7796S_SetSPI(&hspi1);
  XPT2046_SetSPI(&hspi2);
  XPT2046_SetUART(&huart2);

  ST7796S_Init();
  XPT2046_Init();

  //ST7796S_FillScreen(0x0000);  // Black background

  // Optional: Run calibration
  //XPT2046_Calibrate();
  //ST7796S_DrawString(10, 10, "Hello World!", &Font24, WHITE, BLACK);

  // SERVO CODE
//  servo_angle(pennyServo, 180);
//  servo_angle(nickelServo, 180);
//  servo_angle(dimeServo, 180);
//  servo_angle(quarterServo, 180);
//  HAL_Delay(2000);
//
//  servo_angle(pennyServo, 60);
//  servo_angle(nickelServo, 60);
//  servo_angle(dimeServo, 60);
//  servo_angle(quarterServo, 60);
//  HAL_Delay(2000);
//
//  servo_angle(pennyServo, 180);
//  servo_angle(nickelServo, 180);
//  servo_angle(dimeServo, 180);
//  servo_angle(quarterServo, 180);
//  HAL_Delay(2000);

/*********************************************
 *********** RFID INITIALIZATION *************
 *********************************************/

  pn532_SetUART(&huart2);

/*********************************************
 *********** STATE INITIALIZATION ************
 *********************************************/

  state = SPLASH;


  if (pn532_init(&nfc, &hi2c1) != PN532_OK) {
	ST7796S_FillScreen(WHITE);
    ST7796S_DrawString(10, 100, "Error RFID init", &Font24, BLACK, WHITE);
    ST7796S_DrawString(20, 124, "please reflash", &Font24, BLACK, WHITE);
    while(1);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch (state)
    {
      case SPLASH:
    	Splash_Screen();
      break;
      case USE:
        Use_Screen();
      break;
      case THANKYOU:
        //yes
      break;
    }
	//strcpy((char *)buf, "Success\r\n");
	//HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);
	//HAL_Delay(20);


//    servo_angle(pennyServo, 180);
//	servo_angle(nickelServo, 180);
//	servo_angle(dimeServo, 180);
//	servo_angle(quarterServo, 180);
//	HAL_Delay(2000);
//
//	servo_angle(pennyServo, 90);
//	servo_angle(nickelServo, 90);
//	servo_angle(dimeServo, 90);
//	servo_angle(quarterServo, 90);
//	HAL_Delay(2000);
//
//	servo_angle(pennyServo, 180);
//	servo_angle(nickelServo, 180);
//	servo_angle(dimeServo, 180);
//	servo_angle(quarterServo, 180);
//	HAL_Delay(2000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

void Splash_Screen(void)
{
  ST7796S_FillScreen(WHITE);

  ST7796S_DrawString(10, 100, "Please scan your", &Font24, BLACK, WHITE);
  ST7796S_DrawString(10, 122, "andrewID card to", &Font24, BLACK, WHITE);
  ST7796S_DrawString(20, 144, "     begin", &Font24, BLACK, WHITE);
  //HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);
  while (1) {
    if (pn532_read_passive_target(&nfc, &tag, 500) == PN532_OK) {

  	  break;
    }
  }
//  strcpy((char *)buf, "UID: ");
//  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);
//  for (int i = 0; i < tag.uid_len; i++) {
//	sprintf(buf, "%02X ", tag.uid[i]);
//	HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);
//  }

  currentUser = tag.uid[0];
  currentUser = currentUser<<8;
  currentUser = currentUser|tag.uid[1];
  currentUser = currentUser<<8;
  currentUser = currentUser|tag.uid[2];
  currentUser = currentUser<<8;
  currentUser = currentUser|tag.uid[3];

  sprintf(buf, "%x\r\n", get_money_in_account(currentUser));
  HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);

  if(get_money_in_account(currentUser) == 0xFFFFFFFF) {
	strcpy((char *)buf, "New user");
	HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);

	add_account(currentUser);
  }

  HAL_Delay(50);

  currentMoney = get_money_in_account(currentUser);

  const uint16_t LBLUE = RGB565(0xDF, 0xDF, 0xFF);

  ST7796S_FillScreen(LBLUE);
  ST7796S_DrawString(90, 20, "Welcome!", &Font24, BLACK, LBLUE);
  if(currentMoney%100 < 10) sprintf(buf, "$%d.0%d", currentMoney/100, currentMoney%100);
  else sprintf(buf, "$%d.%d", currentMoney/100, currentMoney%100);
  ST7796S_DrawString(110, 48, buf, &Font24, BLUE, LBLUE);
  ST7796S_DrawString(0, 76, "To deposit, slide\ncoins through the\nslot.\nTo withdraw, press\na button below.", &Font24, BLACK, LBLUE);

  ST7796S_FillRect(20, 228, 130, 50, BLUE);
  ST7796S_DrawString(40, 241, "Penny", &Font24, BLACK, BLUE);
  ST7796S_FillRect(170, 228, 130, 50, BLUE);
  ST7796S_DrawString(180, 241, "Nickel", &Font24, BLACK, BLUE);
  ST7796S_FillRect(20, 298, 130, 50, BLUE);
  ST7796S_DrawString(50, 311, "Dime", &Font24, BLACK, BLUE);
  ST7796S_FillRect(170, 298, 130, 50, BLUE);
  ST7796S_DrawString(175, 311, "Quarter", &Font24, BLACK, BLUE);

  ST7796S_FillRect(20, 410, 130, 50, BLUE);
  ST7796S_DrawString(30, 423, "< Exit", &Font24, BLACK, BLUE);

  state = USE;
}

void Use_Screen()
{
  const uint16_t LBLUE = RGB565(0xDF, 0xDF, 0xFF);

  if(ir_read_penny() == GPIO_PIN_RESET)
  {
	currentMoney++;
	if(currentMoney%100 < 10) sprintf(buf, "$%d.0%d", currentMoney/100, currentMoney%100);
	else sprintf(buf, "$%d.%d", currentMoney/100, currentMoney%100);
	ST7796S_DrawString(110, 48, buf, &Font24, BLUE, LBLUE);
  }
  if(ir_read_nickel() == GPIO_PIN_RESET)
  {
	currentMoney+=5;
	if(currentMoney%100 < 10) sprintf(buf, "$%d.0%d", currentMoney/100, currentMoney%100);
	else sprintf(buf, "$%d.%d", currentMoney/100, currentMoney%100);
	ST7796S_DrawString(110, 48, buf, &Font24, BLUE, LBLUE);
  }
  if(ir_read_dime() == GPIO_PIN_RESET)
  {
	currentMoney+=10;
	if(currentMoney%100 < 10) sprintf(buf, "$%d.0%d", currentMoney/100, currentMoney%100);
	else sprintf(buf, "$%d.%d", currentMoney/100, currentMoney%100);
	ST7796S_DrawString(110, 48, buf, &Font24, BLUE, LBLUE);
  }
  if(ir_read_quarter() == GPIO_PIN_RESET)
  {
	currentMoney+=25;
	if(currentMoney%100 < 10) sprintf(buf, "$%d.0%d", currentMoney/100, currentMoney%100);
	else sprintf(buf, "$%d.%d", currentMoney/100, currentMoney%100);
	ST7796S_DrawString(110, 48, buf, &Font24, BLUE, LBLUE);
  }


  tp = XPT2046_GetTouchAvg();

  if(tp.touched) {
	uint16_t screen_x, screen_y;
	XPT2046_GetScreenCoordinates(&tp, &screen_y, &screen_x);

	// Draw a small circle where touched
	ST7796S_FillRect(screen_y - 2, screen_x - 2, 5, 5, BLACK);

	// Display coordinates
	sprintf(buf, "X:%d Y:%d\r\n", screen_x, screen_y);
	HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen((char *)buf), HAL_MAX_DELAY);
  }

  HAL_Delay(50);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 44;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 44;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 19999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
