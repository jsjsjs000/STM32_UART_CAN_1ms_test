#ifndef INC_STM32F4XX_INIT_H_
#define INC_STM32F4XX_INIT_H_

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern WWDG_HandleTypeDef hwwdg;

extern void SystemClock_Config(void);
extern void MX_CAN1_Init(void);
extern void MX_CAN2_Init(void);
extern void MX_I2C1_Init(void);
extern void MX_IWDG_Init(void);
extern void MX_TIM1_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void MX_WWDG_Init(void);
extern void MX_DMA_Init(void);
extern void MX_GPIO_Init(void);

#endif /* INC_STM32F4XX_INIT_H_ */
