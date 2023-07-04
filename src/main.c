#include "stm32f1xx.h"
#include "usart_config.h"
#include "timer_config.h"
#include "comandos.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

int main(void) {
    // Inicializar el USART1 con un baudrate de 9600
    USART1_Init(9600);
    TIM2_init();
    char buffer[20];
    char angulo[20];

    for (;;) {
        if (USART1_Receive_String(buffer, sizeof(buffer))) { 
            USART1_Write_String(buffer); 
            USART1_write('\r');
            USART1_write('\n');

            Comando_SCPI cmd;
            if(verificar_comandos(buffer,&cmd)){
                switch (cmd.comando)
                {
                case RST:
                    USART1_Write_String("se eligio reset\r\n");
                    configurar_TIM2(cmd.angle);
                    break;
                case GET_ANGLE:
                    snprintf(angulo,sizeof(angulo),"%li",obtener_angulo());
                    USART1_Write_String(angulo);
                    USART1_Write_String("\r\n");
                    break;
                case SET_ANGLE:
                    if(cmd.comandoValido){
                        USART1_Write_String("angulo valido\r\n");
                        configurar_TIM2(cmd.angle);
                    }else{
                        USART1_Write_String("angulo invalido, debe estar entre -90° y 90°\r\n");
                    }
                    break;
                case DESCONOCIDO:
                    USART1_Write_String("El comando ingresado es desconocido\r\n");
                    break;    
                default:
                    break;
                }
            }
            else{
               USART1_Write_String("comando invalido\r\n");
            }
            
        }
    }

    return 0;
}