#include "timer_config.h"
#define ANCHO_PULSO_MIN 180
#define ANCHO_PULSO_MAX 900

static void habilitar_reloj_TIM2(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}
static void habilitar_reloj_GPIOA(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}
#if 0
static void tim2_reset(void){
    RCC->APB1RSTR = RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR =0;
}
#endif

static void pin_PA0_reset (void){
    GPIOA->BRR = GPIO_BRR_BR0;
}

static uint32_t determinar_duty_cycle(uint32_t angulo){
    uint32_t valor= ANCHO_PULSO_MIN + ((ANCHO_PULSO_MAX-ANCHO_PULSO_MIN)*angulo)/180;
    return valor;
}

void configurar_TIM2(int angulo){
    uint32_t anchoPulso;
    anchoPulso=determinar_duty_cycle(angulo+90);
    TIM2->CCR1=anchoPulso;
}

void TIM2_init(void) {
    SystemCoreClockUpdate();
    habilitar_reloj_TIM2();
    habilitar_reloj_GPIOA();

    //resetear el pin PA0
    pin_PA0_reset();
    //habilitar pin PA0 como salida alternativa push/pull de velocidad media (2MHz)
    GPIOA->CRL = (GPIOA->CRH & ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0)) | (GPIO_CRL_MODE0_1 | GPIO_CRL_CNF0_1);
    
    //configuro ARR register (frecuencia) y preescaler
    TIM2->ARR = 7200; //1paso cada 0.5° -> 1ms(entre -90° y 90°) cada 360 pasos -> 20ms en 7200 pasos
    TIM2->PSC = SystemCoreClock/360000; //para que la frecuencia del reloj sea 360KHz

    //upcounting mode y auto-reload preload enable
    TIM2->CR1 = (TIM2->CR1 & ~TIM_CR1_DIR) | TIM_CR1_ARPE;
    
    //habilito contador del timer
    TIM2->CR1 |= TIM_CR1_CEN;

    //Configuro modo PWM en modo 1 
    TIM2->CCMR1 = (TIM2->CCMR1 & ~TIM_CCMR1_OC1M) | (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2) | TIM_CCMR1_OC1PE ;
    //configuro canal 1 como salida
    TIM2->CCER |= TIM_CCER_CC1E;

    //configuro ciclo de trabajo
    configurar_TIM2(0);
}

int32_t obtener_angulo(void){
    uint32_t duty_cycle=TIM2->CCR1;
    int32_t angulo= ((duty_cycle-ANCHO_PULSO_MIN)*180)/(ANCHO_PULSO_MAX-ANCHO_PULSO_MIN);
    return angulo-90;    
}