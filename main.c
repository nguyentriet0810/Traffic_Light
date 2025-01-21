#include <main.h>

//bien dem thoi gian dung timer 2
uint32_t t = 0;

//bien quan sat task nao dang chay
uint32_t task = 1;

//bien quan ly ngat ngoai bang semaphore
uint32_t edgsema;

//chuong trinh task 1 voi xanh: 26s; vang: 4s; do 30s
void task1(void){
	while(1) {
		//neu task 1 dang dc phep chay la task 1 thi thuc thi
		if (task == 1) {
			//reset chu ki dem
			if (t == 60) { 
				t = 0;
			}
			//den xanh chay
			else if (t < 26) {
				do1_xanh2();
			}
			//den vang chay
			else if ((t >= 26) && (t < 30)) {
				do1_vang2();
			}
			//dao lai
			else if ((t >= 30) && (t < 56)) {
				xanh1_do2();
			}
			else {
				vang1_do2();
			}
		}
		//neu task 1 khong duoc phep chay thi chu dong chuyen task
		else {
			osThreadYield();
		}
	}
}

//chuong trinh task 2 nhap nhay den vang chu ki 2s
void task2(void){
	while(1){
		//neu task 2 dang dc phep chay la task 1 thi thuc thi
		if (task == 2) {
			//resset chu ki dem
			if ( t == 2) {
				t =0;
			}
			else if (t < 1) {
				GPIO_SetBits(GPIOA, GPIO_Pin_2);
				GPIO_SetBits(GPIOA, GPIO_Pin_5);
			}
			else {
				reset_GPIO();
			}
		}
		//neu task 2 khong duoc phep chay thi chu dong chuyen task
		else {
			osThreadYield();
		}
	}
}

//task dat biet chay ngam duoi su quan ly cua scheduler
void sporadictask(void){
	while(1){
		//cho ngat xay ra
		osCooperative_Wait(&edgsema);
		//cho phep task khac chay
		task++; 
		//gio han 2 task
		if (task > 2) {
			task = 1;
		}
		//reset cac chan gpio
		reset_GPIO();
		//reset bo dem cua timer2
		TIM_SetCounter(TIM2, 0);
		//reset thoi gian ve 0
		t = 0;
		//chu dong chuyen qua task khac
		osThreadYield();
	}
}

int main(void) {
	//cau hinh xung clock 72M
	RCC_Init();
	//cau hinh cac chan gpio su dung
	GPIO_Config();
	//cau hinh timer2 chu ki 1s va co su dung ngat
	TIMER2_Init();
	//cau hinh cho nhan os
	osKernelInit();
	//cau hinh ngat ngoai o chan pa0 va semaphore cua no
	osEdgeTriggerInit(&edgsema);
	//add task 1 vao os cho scheduler quan ly
	osKernelAdd1Thread(task1);
	//add task 2 vao os cho scheduler quan ly
	osKernelAdd1Thread(task2);
	//add task dat biet xu ly ngat ngoai vao os cho scheduler quan ly
	osKernelAdd1Thread(sporadictask);
	//cau hinh thoi gian chay cua moi task
	osKernelLaunch(QUANTA);
	//vong lap
	while(1) {
		//ko dung vong lap trong chuong trinh chinh
	}
}
//chuong trinh con khoi tao xung clock = 72M
void RCC_Init(void) {
		//bat HSE
		RCC_HSEConfig(RCC_HSE_ON);
		//cho cho bo hse duoc khoi dong
		while ((RCC_WaitForHSEStartUp) == ERROR){};
		//bat PLL
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		//cho cho bo PLL duoc khoi dong
		while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET){};
		//cau hinh clock cho bo AHB = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);   
    //cau hinh clock cho APB1 = AHB
		RCC_PCLK1Config(RCC_HCLK_Div2);   
		//cau hinh clock cho APB2 = AHB/2
    RCC_PCLK2Config(RCC_HCLK_Div1);   
		//chon nguon cho SYSCLK la nguon PLLCLK 
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//cho cho nguon clock on dinh
    while (RCC_GetSYSCLKSource() != 0x08) {};
		//cau hinh cho bo nho flash
		//Doi 2 chu ki cho bo nho flash on dinh
    FLASH_SetLatency(FLASH_Latency_2);
		//bat Prefetch Buffer
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
}
//chuong trinh con cau hinh gpio theo thu vien std
void GPIO_Config(void) {
		//cap xung clock cho gpio port A
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
    GPIO_InitTypeDef GPIO_InitStruct;
		//cau hinh cac chan tu 1 -> 6 chuc nang output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;             
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;         
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;         
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}
//chuong trinh con khoi tao timer2 chu ki dem 1s va co su dung ngat
void TIMER2_Init(void) {
		//cap xung clock cho timer2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		//cau hinh timer2
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		//cau hinh bo chia truoc
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; 
		//cau hinh bo dem
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;   
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		//cau hinh chuc nang dem len
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		
		//kich hoat ngat cho timer2
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
		//kich hoat timer2
    TIM_Cmd(TIM2, ENABLE);
		
		//cau hinh ngat cho timer2
    NVIC_InitTypeDef NVIC_InitStructure;
		//cau hinh kenh ngat
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		//cau hinh muc uu tien ngat = 5
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
		//cau hinh muc uu tien ngat phu (neu can) va ko can
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		//cho phep ngat
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void do1_xanh2 (void) {
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}

void do1_vang2 (void) {
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void xanh1_do2 (void) {
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void vang1_do2 (void) {
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void reset_GPIO(void) { 
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}

//xu ly ngat cho timer2
void TIM2_IRQHandler(void) {
		//neu co ngat duoc bat
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
				//cong bien dem thoi gian len 1
				t++;
        //xoa co ngat
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

//chuong trinh con xu ly ngat ngoai tai chan pa0
void EXTI0_IRQHandler(void){
		//give semaphore
		osSemaphore_Give(&edgsema);
		//xoa co ngat
		EXTI->PR |= 1<<0;
}
