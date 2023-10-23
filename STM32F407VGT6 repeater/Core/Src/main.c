#include <stdio.h>
#include <string.h>
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_init.h"

//#define TEST_UARTS
/// or:
#define TEST_CAN

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
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
void CAN_Receive(void);

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  MX_USART6_UART_Init();
  MX_CAN2_Init();
  MX_TIM1_Init();
//  MX_WWDG_Init();
  MX_USART2_UART_Init();

  CAN_Config();
//	osDelay(pdMS_TO_TICKS(100));
	CAN_Receive();

  osKernelInitialize();

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  for (int i = 0; i < 6; i++)
	  osThreadNew(StartTaskN, NULL, &defaultTask_attributes);

#ifdef TEST_UARTS
  osThreadNew(StartUart2Task, NULL, &uartTask_attributes);
  osThreadNew(StartUart6Task, NULL, &uartTask_attributes);
#endif
#ifdef TEST_CAN
  osThreadNew(StartCanTask1, NULL, &uartTask_attributes);
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
//	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		osDelay(pdMS_TO_TICKS(1));

		HAL_IWDG_Refresh(&hiwdg);
  }
}

void StartUart2Task(void *argument)
{
	uint16_t buffer_index = 0;
	uint8_t buffer[8] = { 0 };
	uint8_t answer[] = "OK";

	while (1)
	{
		if (HAL_UART_Receive(&huart2, &buffer[buffer_index], 1, 0) == HAL_OK)
		{
			if (buffer[buffer_index] == 0 && buffer_index > 0)
				HAL_UART_Transmit_DMA(&huart2, answer, sizeof(answer));

			if (buffer[buffer_index] == 0)
				buffer_index = 0;

			if (++buffer_index >= sizeof(buffer_index))
				buffer_index = 0;
		}

		portYIELD();
	}
}

void StartUart6Task(void *argument)
{
	uint16_t buffer_index = 0;
	uint8_t buffer[8] = { 0 };
	uint8_t answer[] = "OK";

	while (1)
	{
		if (HAL_UART_Receive(&huart6, &buffer[buffer_index], 1, 0) == HAL_OK)
		{
			if (buffer[buffer_index] == 0 && buffer_index > 0)
				HAL_UART_Transmit_DMA(&huart6, answer, sizeof(answer));

			if (buffer[buffer_index] == 0)
				buffer_index = 0;

			if (++buffer_index >= sizeof(buffer_index))
				buffer_index = 0;
		}

		portYIELD();
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
	while (1)
	{
//		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		osDelay(pdMS_TO_TICKS(100));
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

  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
	/* Filter configuration Error */
	Error_Handler();
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
	/* Start Error */
	Error_Handler();
  }
}

void CAN_Receive(void)
{
  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
	/* Notification Error */
	Error_Handler();
  }
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
//  else
//	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

  /* Display LEDx */
  if ((RxHeader.StdId == 0x11) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
  {
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if (huart->Instance == UART2)
//	{
//
//	}
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
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
