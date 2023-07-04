#include "comandos.h"
#include <string.h>
#include <stdio.h>
#include "timer_config.h"
#include "usart_config.h"

bool verificar_comandos(char * buffer, Comando_SCPI * cmd){
    if(strcmp(buffer,"RST")==0){
        cmd->comando=RST;
        cmd->angle=0;
        return true;
    //se fija si luego de la cadena "set_angle" se introdujo UN numero entero, y luego se almacena en la variable
    //cmd->angle   
    }else if(sscanf(buffer,"ANG %d",&(cmd->angle))==1){    
        cmd->comando=SET_ANGLE;
        if(cmd->angle <-90 ||cmd->angle >90){
            cmd->comandoValido=false;
        }else{
            cmd->comandoValido=true;
        }
        return true;
    }else if(strcmp(buffer,"ANG?")==0){
        cmd->comando=GET_ANGLE;
        //cmd->comandoValido=true;
        return true;
    }else{
        cmd->comando=DESCONOCIDO;
        return true;
    }
}