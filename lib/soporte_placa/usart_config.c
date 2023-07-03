#include <soporte_placa.h>
#include "usart_config.h"

static void habilitar_reloj_usart (void){
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}

static void habilitar_reloj_GPIOA (void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

static void usart_reset(void){
    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
}
static void configuracion_usart(uint32_t baudrate){
    usart_reset();
    USART1->BRR = SystemCoreClock/baudrate;
    USART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE) ; //hab usart1, tx y rx;
    /*No haria falta lo de abajo al estar CR1 en reset, pero por las dudas*/    
    USART1->CR1 &= ~USART_CR1_M;  //1 bit de entrada 8 de datos y n de paridad 
    USART1->CR2 &=~(USART_CR2_STOP); // 1 BIT DE PARADA
}

void USART1_Init(uint32_t baudrate) {
    // Habilitar el reloj para USART1 y el gpioA
    SystemCoreClockUpdate();
    habilitar_reloj_usart();
    habilitar_reloj_GPIOA();
    // Configurar pines para USART1 (PA9 como TX y PA10 como RX)
    SP_Pin_setModo(SP_PA10, SP_PIN_ENTRADA);

        //Configurar PA9 como salida funcion alternativa PUSH/PULL de velocidad media
    GPIOA->CRH = (GPIOA->CRH & ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9)) | (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1); 

    // Configurar los parámetros del USART1
    configuracion_usart(baudrate);
}

void USART1_write(uint8_t c) {
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}

bool USART1_read(char *caracter) {
    bool bandera=false;
    //usart_sr_rxne=1 cuando el dato se puede leer
    if(USART1->SR & USART_SR_RXNE){
        *caracter= (char) USART1->DR;
        bandera=true;
    }
    return bandera;
}

/*Esta función lee caracteres a través de USART1 y los almacena en el buffer buffer.
 Verifica si se ha recibido el carácter de retorno de carro ('\r') o salto de línea ('\n') 
 para determinar el final de la cadena. La función devuelve true si se ha completado 
 la recepción de la cadena y false si el tamaño del buffer se ha alcanzado antes de 
 recibir el final de la cadena.*/
bool USART1_Receive_String(char* buffer, uint32_t bufferSize){
    uint32_t i=0;
    while(i<bufferSize-1){
        if(USART1_read(&buffer[i])){
            if(buffer[i]=='\r' || buffer[i]=='\n'){
                buffer[i]='\0'; //termina la cadena
                return true;
            }
            i++;
        }
    }
    buffer[i]='\0';//termina la cadena
    return false;
}
/*Esta función recorre la cadena de caracteres str y envía 
cada carácter a través de USART1 utilizando la función USART1_write().*/
void USART1_Write_String(const char* string){
    while(*string != '\0'){
        USART1_write(*string);
        string++;
    }
}