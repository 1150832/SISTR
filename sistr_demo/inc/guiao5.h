/*
  ******************************************************************************
  * Trabalho realizado por:
  *
  * 1150791		Tiago Afonso
  * 1150832		Ronaldo Marques
  *
  ******************************************************************************
*/

#ifndef GUIAO5_H
#define GUIAO5_H

extern volatile uint8_t exercicio;
extern volatile char USART_value;

void RCC_Config_guiao5();

void config_LED_STATE_AF_guiao5();
void config_BOTAO_ENTER_guiao5();

void config_TIM4_as_PWM_guiao5();

void config_USART_guiao5();
void config_USART_e_NVIC_guiao5();
void change_BaudRate_USART(uint32_t BaudRate);
void config_TX_RX();

void send_duty_cycle(uint8_t num);
void recursiva(uint8_t value);
void new_line(uint8_t num, uint8_t percentagem);

void ex_1_guiao5();
void ex_2_guiao5();
void ex_3_guiao5();

#endif
