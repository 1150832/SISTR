/*
  ******************************************************************************
  * Trabalho realizado por:
  *
  * 1150791		Tiago Afonso
  * 1150832		Ronaldo Marques
  *
  ******************************************************************************
*/

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "lcd.h"
#include "guiao5.h"

volatile char USART_value = ' '; 	//variável global para valores recebidos da USART
volatile uint8_t exercicio = 0;		//variável global para indicação de exercício


/*-------------------------------------------------------------------- Funções de Configurações --------------------------------------------------------------------*/

void RCC_Config_guiao5() //configuração relógio para 72MHz
{
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET );
	FLASH_SetLatency(FLASH_Latency_2);
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
	RCC_PLLCmd(ENABLE);
	while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
	rcc_lcd_info();
}

void config_LED_STATE_AF_guiao5() // Configuração do pino do LED STATE como alternative function uma vez que é para ser controlado pelo TIMER
{
	/* Enable do GPIOB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Variavel para fazer a configuração do GPIO */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Ativar o Pino 6 do GPIOB como saída que é onde está o LED STATE no modo Alternative Function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Modo Alternative Function
	GPIO_Init(GPIOB, &GPIO_InitStructure); // Inicialização
}

void config_BOTAO_ENTER_guiao5() // Configuração do Botão ENTER
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void config_TIM4_as_PWM_guiao5() // Configuração do TIM4 em Modo PWM
{
	/* Enable do TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Variavel para fazer a configuração do TIM4 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* Configuração do Timer */
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Prescaler = 719;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	/* Inicizalização do TIM4 */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Definição que a fonte de relógio do temporização deverá ser o valor proveniente do RCC (72MHz) */
	TIM_InternalClockConfig(TIM4);

	/* Variável para configuração do modo pretendido */
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* Configuração do Modo */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0x7FF; /*0 a 65535*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/* Inicialização no Canal 1 do TIM4 no modo PWM */
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	/* Enable do TIMER4 */
	TIM_Cmd(TIM4, ENABLE);
}

void config_USART_guiao5() //Configuração da Usart2
{

	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600; // 9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 bits de dados
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1 stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //sem bit de paridade
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // habilitar trasmissão e recepção
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS; //controlo de fluxo pelos pinos RTS e CTS

	USART_Init(USART2, &USART_InitStructure); // Inicialização com as configs anteriores
	USART_Cmd(USART2, ENABLE); // ativação da usart
}

void config_USART_e_NVIC_guiao5() //Configuração da Usart2 com ativação da sua interrupção e respectiva configuração do NVIC
{
	// configuração USART
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);

	// configuração do NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //prioridade mais alta
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //Ativação da interrupção quando a recepção de informação pela flag USART_IT_RXNE
}

void change_BaudRate_USART_guiao5(uint32_t BaudRate) //função para fazer mudar o baudrate da comunicação
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
}

void config_TX_RX_guiao5() //configuração dos GPIOS referentes à comunicação série, nomeadamente RX e TX
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // USART2 TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // USART2 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void ex_1_guiao5()
{
	RCC_Config_guiao5(); //config RCC 72Mhz

	config_TX_RX_guiao5(); //config GPIOS
	config_USART_guiao5(); //config USART

	while(1)
	{
		while( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET ); 	// aguardar recebimento comunicação
		uint8_t RxData; 													// variável de armazenamento
		RxData = USART_ReceiveData(USART2);									// armazenamento da informação recebida
		USART_ClearFlag(USART2,USART_FLAG_RXNE);							// limpar flag
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		// aguardar registo TDR está vazio
		USART_SendData(USART2, RxData);										// envio da informação armazenada
		USART_ClearFlag(USART2,USART_FLAG_TXE);								// limpar flag
	}
}

void ex_2_guiao5()
{
	exercicio = 2; // atribuição do valor 2 à variável global

	RCC_Config_guiao5();
	config_TX_RX_guiao5();
	config_USART_e_NVIC_guiao5(); //configuração da USART com interrupção

	while(1);
}

void ex_3_guiao5()
{
	exercicio = 3;
	RCC_Config_guiao5();

	config_LED_STATE_AF_guiao5();
	config_BOTAO_ENTER_guiao5();

	config_TIM4_as_PWM_guiao5();

	config_TX_RX_guiao5();
	config_USART_e_NVIC_guiao5();

	uint8_t ENTER_Status = 2;			//variável representativa do estado do botão ENTER
	uint16_t aux = ' ';					//variável auxiliar para envio de caracteres pela USART
	uint8_t duty_cycle = 100;			//variável representativa do duty cycle
	TIM_SetCompare1(TIM4, duty_cycle);	//atribuição de duty_cycle ao TIM4 em modo PWM

	while(1)
	{
		switch(USART_value)
		{
			case 'S':
			case 's':
				ENTER_Status = 1-GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
				//uma vez que o botão ENTER está definido como Input Pull-up o nível lógico 1 é quando não se está a carregar e 0 quando se está a carregar
				//devido a isso fez-se a subtração desse valor à unidade para se ter a lógica "normal", isto é, nível lógico 1 quando se carrega e nível
				//lógico 0 quando não se carrega

				aux = ENTER_Status + '0'; //passagem do valor para ascii
				if (ENTER_Status == 0) //caso não se esteja a carregar
				{
					while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 	//aguardar registo TDR está vazio
					USART_SendData(USART2, aux);									//enviar valor
					USART_ClearFlag(USART2,USART_FLAG_TXE);							//limpar flag
					new_line(1,0);													//envio de escrita para USART de nova linha
				}
				else if (ENTER_Status == 1) //caso se esteja a carregar
				{
					while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//aguardar registo TDR está vazio
					USART_SendData(USART2, aux);									//enviar valor
					USART_ClearFlag(USART2,USART_FLAG_TXE);							//limpar fla
					new_line(1,0);													//envio de escrita para USART de nova linha
				}
				USART_value=' '; //limpar o valor recebido pela USART para não voltar a entrar neste caso do switch-casa
				break;

			case 'M':
			case 'm':
				lcd_draw_fillrect(0, 0, 127, 63, BLACK);				//fundo negro
				lcd_draw_string(0, 0, "BaudRate Options", WHITE, 1);	//escrita do que é pretendido
				lcd_draw_string(0, 20, "1 - 9600", WHITE, 1);
				lcd_draw_string(0, 30, "2 - 19200", WHITE, 1);
				lcd_draw_string(0, 40, "3 - 115200", WHITE, 1);
				display();												//envio para o display
				USART_value=' ';
				break;

			case '1':
				change_BaudRate_USART_guiao5(9600); //mudança do baudrate para 9600
				USART_value=' ';
				break;

			case '2':
				change_BaudRate_USART_guiao5(19200); //mudança do baudrate para 19200
				USART_value=' ';
				break;

			case '3':
				change_BaudRate_USART_guiao5(115200); //mudança do baudrate para 115200
				USART_value=' ';
				break;

			case 'C':
			case 'c':
				lcd_draw_fillrect(0, 0, 127, 63, BLACK); 	//fundo negro
				display();
				USART_value=' ';
				break;

			case '+':
				if (duty_cycle < 100) //caso o duty_cyle seja inferior a 100 entra para incrementar valor
				{
					duty_cycle = duty_cycle + 10; //incrementa 10%
					TIM_SetCompare1(TIM4, duty_cycle);
				}
				// caso o duty cycle sejá já de 100 não continua a incrementar o valor
				USART_value=' ';
				break;

			case '-':
				if (duty_cycle > 0) //caso o duty_cyle seja superior a 0 entra para decrementar valor
				{
					duty_cycle = duty_cycle - 10; //decrementa 10%
					TIM_SetCompare1(TIM4, duty_cycle);
				}
				// caso o duty cycle sejá já de 0 não continua a decrementar o valor
				USART_value=' ';
				break;

			case 'P':
			case 'p':
				send_duty_cycle(duty_cycle); //função que faz o print pela USART do valor atual do duty_cycle
				USART_value=' ';
				break;

			default:
				break;
		}
	}
}


void send_duty_cycle(uint8_t value) //função que faz o print pela USART de um valor numérico até 8bits
{
	uint8_t send = 0; //variável auxiliar para envio de informação
	if (value == 0) //caso o duty cycle seja zero
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		send = value + '0';
		USART_SendData(USART2, send);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
		//return;
	}
	if (value > 0) //caso seja maior que zero
	{
		recursiva(value); //função recursiva
	}
	new_line(1,1);
}

void recursiva(uint8_t value) //função recursiva para realizar o split de um número em digitos para os imprimir da esquerda para a direita
{
	uint8_t aux = 0;
	uint8_t send = 0;
	if (value > 0) //se value for maior que 0
	{
		aux=value%10; //resto da divisão
		value=value/10; //parte inteira da divisão
		recursiva(value); //função recursiva
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		send = aux + '0';
		USART_SendData(USART2, send); //escrita do caracter devido
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}

void new_line(uint8_t num, uint8_t percentagem) //função para enviar o caracter de percentagem e/ou caracter para nova linha
{
	if (percentagem)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, 37); //37 representa em ascii o simbolo %
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
	for(uint8_t i=0; i<num;i++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, 10); //10 representa em ascii o simbolo NL (New Line)
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	}
}
