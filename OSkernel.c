#include <osKernel.h>

//khai bao so thread toi da co the co trong os
#define NUM_OF_THREADS	4
//khai bao vung stack cho moi thread
#define STACK_SIZE 			200

//khoi tao tan so cho systick	
#define BUS_FREQ 			72000000
//khai bao cac macro can thiet
#define CTRL_ENABLE 		(1<<0)
#define CTRL_TICKINT 		(1<<1)
#define CTRL_CLCKSRC 		(1<<2)
#define CTRL_COUNTFLAG  (1<<16)
#define SYSTICK_RST 		0
//tao bien cho systick den tinh quanta time
uint32_t MILLIS_PRESCALER;

//tao macro de bat ngat cho systick
#define INTCTRL (*((volatile uint32_t *)0XE000ED04))
#define PENDSTSET   (1<<26)

//TCB Struct
struct tcb{
	uint32_t *stackPt;
	struct tcb* nextPt;
};
//tao kieu du lieu tcbType
typedef struct tcb tcbType;
//tao mang kieu tcb cho cac thread
tcbType tcbs[NUM_OF_THREADS];
//tao con tro hien tai de biet task nao dang chay
tcbType *currentPt;

//Khoi tao vung nho Stack bang mang 2 chieu
uint32_t TCB_STACK[NUM_OF_THREADS][STACK_SIZE];

//khoi tao so thread co trong os ban dau bang 0
uint32_t current_num_of_thread = 0;
//tao con tro xu ly ngat ngoai
uint32_t *edge_semaphore;

//chuong trinh con khoi tao nhan cua os
void osKernelInit(void){
	//1ms = 7200 xung
	MILLIS_PRESCALER = (BUS_FREQ/1000);
}

void osKernelLaunch(uint32_t quanta){
	//reset systick
	SysTick->CTRL = SYSTICK_RST;
	//reset bo dem cua systick ve 0
	SysTick->VAL = 0;
	//load quanta time 
	SysTick->LOAD = (quanta*MILLIS_PRESCALER-1);
	//set muc uu tien ngat cua systick la ca nhat trong he thong
	NVIC_SetPriority(SysTick_IRQn, 0);
	//bat systick su sung giao dong noi
	SysTick->CTRL |= CTRL_CLCKSRC|CTRL_ENABLE;
	//bat ngat cho systick
	SysTick->CTRL |= CTRL_TICKINT;
	//den ham os launch scheduler
	osSchedulerLaunch();
}

//khoi tao vung stack cho moi thread
void osKernelStackInit(int i){
	//16 o nho dau tien danh cho 16 thanh ghi da dung cua he thong
	tcbs[i].stackPt = &TCB_STACK[i][STACK_SIZE-16];
	/*Set bit21 (T-Bit) in PSR to 1 to operate it*/
	/*Thumb Mode*/
	TCB_STACK[i][STACK_SIZE-1] = 1<<24;	/*PSR*/
	/*Skip the PC*/
	//
	/*Dumming Stack Content*/
	TCB_STACK[i][STACK_SIZE-3] = 0xAAAAAAAA;/*R14 (LR)*/
	TCB_STACK[i][STACK_SIZE-4] = 0xAAAAAAAA;/*R12*/
	TCB_STACK[i][STACK_SIZE-5] = 0xAAAAAAAA;/*R3*/
	TCB_STACK[i][STACK_SIZE-6] = 0xAAAAAAAA;/*R2*/
	TCB_STACK[i][STACK_SIZE-7] = 0xAAAAAAAA;/*R1*/
	TCB_STACK[i][STACK_SIZE-8] = 0xAAAAAAAA;/*R0*/
	
	TCB_STACK[i][STACK_SIZE-9] = 0xAAAAAAAA; /*R11*/
	TCB_STACK[i][STACK_SIZE-10] = 0xAAAAAAAA;/*R10*/
	TCB_STACK[i][STACK_SIZE-11] = 0xAAAAAAAA;/*R9*/
	TCB_STACK[i][STACK_SIZE-12] = 0xAAAAAAAA;/*R8*/
	TCB_STACK[i][STACK_SIZE-13] = 0xAAAAAAAA;/*R7*/
	TCB_STACK[i][STACK_SIZE-14] = 0xAAAAAAAA;/*R6*/
	TCB_STACK[i][STACK_SIZE-15] = 0xAAAAAAAA;/*R5*/
	TCB_STACK[i][STACK_SIZE-16] = 0xAAAAAAAA;/*R4*/
}

uint8_t osKernelAdd1Thread(void (*task)(void)){
	//ko cho phep ngat toan cuc
	__disable_irq();
	//chay chuong trinh con osKernelStackInit voi so thread hien tai 
	osKernelStackInit(current_num_of_thread);
	//gan gia tri cua vung stack_size - 2 (vung nho cua thanh ghi pc) bang dia chi cua task duoc them vao
	TCB_STACK[current_num_of_thread][STACK_SIZE-2] = (uint32_t)(task);
	//gan nextPt cua thread truoc = stackPt cua thread tiep theo
	if(!current_num_of_thread){
		tcbs[current_num_of_thread].nextPt = &tcbs[current_num_of_thread];
	} 
	else {
		tcbs[current_num_of_thread-1].nextPt = &tcbs[current_num_of_thread];
		tcbs[current_num_of_thread].nextPt = &tcbs[0];
	}
	//gian gia tri con tro = tsb cua task dau tien
	currentPt = &tcbs[0];
	//tang gia tri thread hien tai len 1
	current_num_of_thread++;
	//cho phep ngat toan cuc
	__enable_irq();
	
}
//chuong trinh con chuyen task dang chay sang task tiep theo
void osSchedulerRoundRobin(void){
	//tro con tro hien tai toi stackPt cua thread tiep theo
	currentPt = currentPt->nextPt;
}
//chuong trinh con chu dong bat ngat systick de chuyen task
void osThreadYield(void){
	//xoa bo dem cua systick
	SysTick->VAL = 0;
	//bat co ngat
	INTCTRL |= PENDSTSET ;
}
//chuong trinh con xu ly ngat systick
void SysTick_Handler(void) {
	//bat ngat cho pendsv de thuc hien chuyen task
	SCB->ICSR |= SCB_ICSR_PENDSVSET;
}
//chuong trinh con quan ly ngat ngoai
void osEdgeTriggerInit(uint32_t *semaphore){
	//gan con tro edge_semaphore = semaphore duoc truyen vao o file main
	edge_semaphore = semaphore;
	//cau hinh chan pa0 chuc nang ngat ngoai
	EdgeTrigger_Init();
}

//chuong trinh con cau hih ngat ngoai cho chan pa0
void EdgeTrigger_Init(void){
	//khong cho phep ngat
	__disable_irq();
	//cap xung clock cho GPIOA va AFIO
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN| RCC_APB2ENR_AFIOEN; //Kich hoat xung clock cap cho GPIOA
	//cau hinh input mode				
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0; 	//MODE = 00: Intput mode.
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1; 
	GPIOA->CRL |= GPIO_CRL_CNF0_1;	 //CNF = 10: Input with pull-up / pull-down
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;	       
	GPIOA->ODR |= GPIO_ODR_ODR0; //Set ODR0, PA0 now working as input pull up.
	//cau hinh kenh ngat
	AFIO->EXTICR[0] &=~(0x0F<<0);
	EXTI->IMR |= (1<<0);
	EXTI->RTSR &=~ (1<<0);
	EXTI->FTSR |= (1<<0);
	//cau hinh muc uu tien ngat la 1 (sau systick)
	NVIC_SetPriority(EXTI0_IRQn, 1);
	//cho phep ngat
	NVIC_EnableIRQ(EXTI0_IRQn);
}
//chuong trinh con khoi tao semaphore thuong
void osSemaphore_Init(uint32_t *semaphore, uint32_t val){
	*semaphore = val;
}
//chuong trinh con give samephore 
void osSemaphore_Give(uint32_t *semaphore){
	//khong cho phep ngat toan cuc
	__disable_irq();
	//cong gia tri cua semaphore len 1
	*semaphore +=1;
	//bat lai ngat toan cuc
	__enable_irq();
}
//chuong trinh con doi cap semaphore
void osCooperative_Wait(uint32_t *semaphore){
	//ko cho phep ngat
	__disable_irq();
	//cho neu nhu ngoai vi can dung dang duoc chuong trinh khac su dung
	while(*semaphore <= 0){
		//cho phep ngat
		__enable_irq();
		//chu dong goi ham osThreadYield() de chuyen sang task khac tranh lang phi thoi gian
		osThreadYield();
		//ko cho phep ngat
		__disable_irq();
	}
	//neu duoc phep su dung ngoai vi thi tru semaphore di 1
	*semaphore -=1;
	//cho phep ngat toan cuc
	__enable_irq();
}

