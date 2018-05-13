#include <NewPing.h>

#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_
#include "Types.h"
/* Defines */
extern uint16 trig_1;  
extern uint16 echo_1 ;    //left

extern uint16 trig_2 ;
extern uint16 echo_2 ;  //right

extern uint16 trig_3 ;
extern uint16 echo_3 ;  //forward

extern uint16 Max_Dist;

extern uint32 pingTimer1;
extern uint32 pingTimer2;
extern uint32 pingTimer3;

extern uint32 pingSpeed;

extern uint16 delay_ping;



#endif
