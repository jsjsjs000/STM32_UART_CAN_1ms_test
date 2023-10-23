#include "main.h"
#include "stm32f4xx_it.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern I2C_HandleTypeDef hi2c1;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim2;

void NMI_Handler(void)
{
  HAL_RCC_NMI_IRQHandler();
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
}

void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

void CAN1_TX_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX1_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_SCE_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}

void CAN2_TX_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}

void CAN2_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}

void CAN2_RX1_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}

void CAN2_SCE_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan2);
}

void DMA2_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart6_tx);
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);
}
