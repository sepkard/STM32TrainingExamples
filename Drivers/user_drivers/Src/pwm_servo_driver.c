

#include "pwm_servo_driver.h"


static void PWM_Error_Handler(void);

#define  PERIOD_VALUE       (uint32_t)(20000)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) /* Capture Compare 4 Value  */

/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;



void pwm_init(void)
{
	 GPIO_InitTypeDef   GPIO_InitStruct;
  /* -1- Clock 'lari aciyoruz */
  
  __HAL_RCC_TIM1_CLK_ENABLE();

  
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* PA.08 (TIM1_Channel1)
     PA.09 (TIM1_Channel2)
	   PA.10 (TIM1_Channel3),
     PA.11 (TIM1_Channel4) 
	   output, push-pull, alternate function mode
  */
  
	/* Timer kanali olarak kullanilacak GPIO Ayarlari */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	/*##-1- Configure the TIM peripheral */
  /* -----------------------------------------------------------------------
  TIM1 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM1 input clock (TIM1CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
      TIM1CLK = PCLK1
      PCLK1 = HCLK
      => TIM1CLK = HCLK = SystemCoreClock (48 Mhz)

    TIM1 Counter Clock ==> 24 Mhz 
       Prescaler = (TIM1CLK / TIM1 counter clock) - 1
       Prescaler = ((SystemCoreClock) /24 MHz) - 1
			 Prescaler = 1

       
		TIM1 output clock ==> 24 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM1 counter clock / TIM1 output clock) - 1
           = (24 Mhz / 24 Khz ) -1 
					 = 999

    TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR + 1)* 100 = 50%
    TIM1 Channel2 duty cycle = (TIM1_CCR2/ TIM1_ARR + 1)* 100 = 37.5%
    TIM1 Channel3 duty cycle = (TIM1_CCR3/ TIM1_ARR + 1)* 100 = 25%
    TIM1 Channel4 duty cycle = (TIM1_CCR4/ TIM1_ARR + 1)* 100 = 12.5%

   
  ----------------------------------------------------------------------- */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = (48 Mhz / 1 Mhz) - 1 = 47 
       + Period = (20000) = 20 ms
       + ClockDivision = 0
       + Counter direction = Up
  */
	
  TimHandle.Instance = TIM1;

  TimHandle.Init.Prescaler         = 47;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    PWM_Error_Handler();
  }

  /*##-2- PWM Kanal Ayarlari */
  /* T�m kanallar icin ortak ayarlar */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Kanal-1 Duty Cycle Degeri */
  sConfig.Pulse = PULSE1_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }

  /* Kanal-2 Duty Cycle Degeri */
  sConfig.Pulse = PULSE2_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }

  /* Kanal-3 Duty Cycle Degeri */
  sConfig.Pulse = PULSE3_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Configuration Error */
    PWM_Error_Handler();
  }

  /* Kanal-4 Duty Cycle Degeri */
  sConfig.Pulse = PULSE4_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }

  /* -3- PWM sinyali uretmeye basliyoruz */
  
	/* Kanal 1'i baslat */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }
  
	/* Kanal 2'yi baslat */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }
  
	/* Kanal 3'u baslat */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  }
  
	/* Kanal 4'u baslat */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Error */
    PWM_Error_Handler();
  } 	
}


void pwm_enable(uint32_t channel)
{
	switch(channel)
	{
		case 1:
				HAL_TIM_PWM_Start(&TimHandle,TIM_CHANNEL_1);
		break;
		
		case 2:
				HAL_TIM_PWM_Start(&TimHandle,TIM_CHANNEL_2);
			break;
				
		case 3:
				HAL_TIM_PWM_Start(&TimHandle,TIM_CHANNEL_3);
			break;
					
		case 4:
				HAL_TIM_PWM_Start(&TimHandle,TIM_CHANNEL_4);
			break;
	}
	
}

void pwm_disable(uint32_t channel)
{
		switch(channel)
	{
		case 1:
				HAL_TIM_PWM_Stop(&TimHandle,TIM_CHANNEL_1);
		break;
		
		case 2:
				HAL_TIM_PWM_Stop(&TimHandle,TIM_CHANNEL_2);
			break;
				
		case 3:
				HAL_TIM_PWM_Stop(&TimHandle,TIM_CHANNEL_3);
			break;
					
		case 4:
				HAL_TIM_PWM_Stop(&TimHandle,TIM_CHANNEL_4);
			break;
	}
	
}

/**
  * @brief  Yuzde cinsinden verilen(0-100) duty cycle degerini kanallara atar
  * @param  duty (yuzde degeri)
  * @retval channel (duty cycle degeri ayarlanacak kanal numarasi (1-4)
  */
void pwm_set_duty_cycle(uint32_t duty, uint32_t channel)
{
	uint32_t arr_val = 0;
	arr_val = (PERIOD_VALUE * duty) / 100;
	
	switch(channel)
	{
		case 1:
				__HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, arr_val);
		break;
		
		case 2:
				__HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_2, arr_val);
			break;
				
		case 3:
				__HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_3, arr_val);
			break;
					
		case 4:
				__HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_4, arr_val);
			break;
	}
}


void pwm_test(void)
{
	uint32_t i = 0;
	
	for(i = 0; i<=2000;i=i+100)
	{
	  __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, i);
		HAL_Delay(100);
	}
	
		
	for(i = 2000; i>=10;i=i-100)
	{
	  __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, i);
		HAL_Delay(100);
	}
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void PWM_Error_Handler(void)
{

  while (1)
  {
  }
}
