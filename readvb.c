#include "stm32f10x_rcc.h"
#include "readvb.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define  __TIMES__    20

/********adc gpio config  ---PA5 6 7 ****/
static void ADC1_GPI0_Config() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static  void ADC1_Mode_Config(void) {
	ADC_InitTypeDef ADC_InitStructure;

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));

}

void ADC1_Init(void) {
	ADC1_GPI0_Config();
	ADC1_Mode_Config();
}

uint16_t readVB(void) {
	uint8_t i = 0;
	uint32_t vref = 0, v = 0;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_55Cycles5);   //PA7 基准电压
	for (i = 0; i < __TIMES__; ++i) {
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		vref +=  ADC_GetConversionValue(ADC1);
	}

	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);   //PA5 待测电压
	for (i = 0; i < __TIMES__; ++i) {
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		v += ADC_GetConversionValue(ADC1);
	}

	v = 11 * v * 2500 / vref;   //基准电压2.5V  分压1/11
//	printf("v is %d \r\n", v);

	return v;
}
