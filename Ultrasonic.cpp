#include "NewPing.h"
#include "Types.h"
#include "Ultrasonic.h"

uint32 pingTimer1;
uint32 pingTimer2;
uint32 pingTimer3;

uint16 trig_1 = 4;  
uint16 echo_1 = 10;    //left

uint16 trig_2 = 11;
uint16 echo_2 = 12;  //right

uint16 trig_3 = 7;
uint16 echo_3 = 8;  //forward

uint16 Max_Dist = 100;

uint32 pingSpeed = 500;

uint16 delay_ping = 35;




