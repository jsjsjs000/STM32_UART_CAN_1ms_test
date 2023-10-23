#include <stdio.h>
#include <string.h>
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_init.h"
#include "stm32f4xx_hal_iwdg.h"

//#define TEST_UARTS
/// or:
#define TEST_CAN

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;

void StartDefaultTask(void *argument);
void StartUart2Task(void *argument);
void StartUart6Task(void *argument);
void StartTaskN(void *argument);
void StartCanTask1(void *argument);
void CAN_Config(void);
void CAN_Transmit(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();	/// 1 / 32000 * (4-1) * 4096 = 0,384 s
  MX_USART6_UART_Init();
  MX_CAN2_Init();
  MX_TIM1_Init();
//  MX_WWDG_Init();$$
  MX_USART2_UART_Init();

  CAN_Config();

//  HAL_Delay(100);$$

  osKernelInitialize();

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  for (int i = 0; i < 20; i++)
	  osThreadNew(StartTaskN, NULL, &defaultTask_attributes);

#ifdef TEST_UARTS
  osThreadNew(StartUart2Task, NULL, &defaultTask_attributes);
  osThreadNew(StartUart6Task, NULL, &defaultTask_attributes);
#endif
#ifdef TEST_CAN
  osThreadNew(StartCanTask1, NULL, &defaultTask_attributes);
#endif

  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1)
  {
  }
}

volatile int a = 0;

void StartDefaultTask(void *argument)
{
	for(;;)
	{
//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		osDelay(pdMS_TO_TICKS(1));

		HAL_IWDG_Refresh(&hiwdg);
	}
}

void StartUart2Task(void *argument)
{
	char buffer[10] = { 0 };
	for (;;)
	{
		int a_;
		taskENTER_CRITICAL();
		a_ = a;
		taskEXIT_CRITICAL();

		sprintf(buffer, "%04d", a_);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t*)buffer, strlen(buffer) + 1);
		osDelay(pdMS_TO_TICKS(1));
	}
}

void StartUart6Task(void *argument)
{
	char buffer[10] = { 0 };
	for (;;)
	{
		int a_;
		taskENTER_CRITICAL();
		a_ = a;
		taskEXIT_CRITICAL();

		sprintf(buffer, "%04d", a_);
		HAL_UART_Transmit_DMA(&huart6, (uint8_t*)buffer, strlen(buffer) + 1);
		osDelay(pdMS_TO_TICKS(1));
	}
}

void StartTaskN(void *argument)
{
	for (;;)
	{
		taskENTER_CRITICAL();
		a = (a + 1) % 10000;
		taskEXIT_CRITICAL();

//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		osDelay(pdMS_TO_TICKS(1));
	}
}

void StartCanTask1(void *argument)
{
	osDelay(pdMS_TO_TICKS(100));

	while (1)
	{
//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		CAN_Transmit();

		osDelay(pdMS_TO_TICKS(1));
	}
}

void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-1- Configure the CAN peripheral #######################################*/

//  hcan1->Init.AutoRetransmission = ENABLE;

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
	/* Filter configuration Error */
	Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
	/* Start Error */
	Error_Handler();
  }
}

void CAN_Transmit(void)
{
  /*##-4- Start the Transmission process #####################################*/
  TxHeader.StdId = 0x11;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxData[0] = 0xCA;
  TxData[1] = 0xFE;

  /* Request transmission */
  if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
  {
	/* Transmission request Error */
//	Error_Handler();
  }
  else
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

  /* Wait transmission complete */
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) { }
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

  /*##-5- Start the Reception process ########################################*/
//  if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 1)
//  {
//	/* Reception Missing */
//	Error_Handler();
//  }
//
//  if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
//  {
//	/* Reception Error */
//	Error_Handler();
//  }
//
//  if((RxHeader.StdId != 0x11)                     ||
//	 (RxHeader.RTR != CAN_RTR_DATA)               ||
//	 (RxHeader.IDE != CAN_ID_STD)                 ||
//	 (RxHeader.DLC != 2)                          ||
//	 ((RxData[0]<<8 | RxData[1]) != 0xCAFE))
//  {
//	/* Rx message Error */
//	//return HAL_ERROR;
//  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */

/*
	zmiany $$:
		LED Pull up
		UART1 -> UART2
*/
