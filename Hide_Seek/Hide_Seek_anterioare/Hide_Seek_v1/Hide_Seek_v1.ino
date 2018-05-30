
#include <MovingAverageFilter.h>

#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#include "Types.h"
#include "Motors.h"
#include "Ultrasonic.h"

#define RED 41
#define GREEN 39 
#define BLUE 43
#define K 10
#define DIST_WALL 8

MovingAverageFilter Fwd(100);
MovingAverageFilter L(100);
MovingAverageFilter R(100);

uint32 forward, left, right;


uint8 medie;
uint8 right_dist,left_dist,front_dist;

uint8 counter;
uint8 state;
uint8 find_robot=0;
uint8 alg_used=0;//0 0->right, 1->left
uint32 timer=millis();
uint32 speed_val_right;
uint32 speed_val_left;

IRrecv irrecv(2);
IRsend irsend;
decode_results results;

NewPing sonar1(trig_1, echo_1, Max_Dist); 
NewPing sonar2(trig_2, echo_2, Max_Dist); 
NewPing sonar3(trig_3, echo_3, Max_Dist);

uint16 value;



void setup() {
    pinMode(43, OUTPUT);
  // put your setup code here, to run once:
   // irrecv.enableIRIn(); // Start the receiver
    
    pingTimer1 = millis() + pingSpeed; // Sensor 1 fires after 1 second (pingSpeed)
    pingTimer2 = pingTimer1 + delay_ping; // Sensor 2 fires 35ms later
    pingTimer3 = pingTimer2 + delay_ping;
    
  Serial.begin(9600);
}

/*
uint8 filter(NewPing sonar, uint8 samples)
{
  uint32 data;

  for(int i = 0; i < samples; i++)
    data += sonar.ping_cm();

  return data/samples;
}

*/
sint8 opreste()
{
  //Serial.println(forward);
  Serial.println(forward);
  //if(filter(sonar3, 20) < 8)
  if(forward < 12)
    return 1;
   else return 0;
}
void go(uint8 volt1, uint8 volt2, sint8 directie)
{
  if(directie == 1)
  {
  analogWrite(MOTOR_Left_1, volt1);
  analogWrite(MOTOR_Right_1, 0);

  analogWrite(MOTOR_Left_2, volt2);
  analogWrite(MOTOR_Right_2, 0);
  }
  else
  {
      analogWrite(MOTOR_Left_1, 0);
      analogWrite(MOTOR_Right_1, volt1);

      analogWrite(MOTOR_Left_2, 0);
      analogWrite(MOTOR_Right_2, volt2);
  } 
}



void seek()
{
  
  if(opreste())
    go(0, 0, 1);
    else
    go(speed_val_left, speed_val_right, 1);
    

    
   
}

bool cond_fwd()
{
  return  ((forward>12 && right<10 && left <10) || (forward>12 && left>10 && right<10)); 
}

bool cond_right()
{
  return ( (forward<12 && left<8 && right >10) || (forward<10 && left>10 & right>10) || (forward>12 && right>10));
}

bool cond_left()
{
  return ((forward<12 && right <8 && left >10)) ;
}

void rotate_right()
{
  if(millis()- timer < 500)
    {
      //if(right > 8) state = 1;
      go(150, 50, 1);
    }
    else state = 1;

  
}
void rotate_left()
{
  if(millis()- timer < 500)
    {
      //if(right > 8) state = 1;
      go(50, 150, 1);
    }
    else state = 1;

  
}

void loop() {


right_dist = sonar2.ping_cm();
left_dist=sonar1.ping_cm();
front_dist=sonar3.ping_cm();

forward = Fwd.process(sonar3.ping_cm());
left=L.process(sonar1.ping_cm());
right=R.process(sonar2.ping_cm());

speed_val_right = 150 + K * (DIST_WALL - right);
speed_val_left = 150 + K * (DIST_WALL - left);


//deplasare
if(find_robot==0){
  switch(state){
   
    //start
    case 0:{
      //if(millis()-timer>5000)
      //state=1;
      if(cond_fwd) state = 1;
      //if(cond_right) state = 2;
     //if(cond_left) state = 3;
      break;
    }
    
    //go fwd
    case 1:{
      seek();
      
      if(right>20 && right>left) 
      {
        timer = millis();
        state = 2;
      }
      else
      if(left>16 && left>right && forward<25){
        timer=millis();
        state=3; 
      }
      
      break;
    }
    case 2:{
      rotate_right();
      break;
    }
    case 3:{
      rotate_left();
      break;
    }
    
  }
}


//delay(2000);

 /*if ((irrecv.decode(&results)))
   {
     irrecv.resume();  
     value= results.value;
    Serial.println(value);
   }*/
  // irsend.sendSony(0xa90, 12); //send 0xa90 on infrared
  
// put your main code here, to run repeatedly:


}

/*
   digitalWrite(43, HIGH);
  delay(1000);
  digitalWrite(43,LOW);
  delay(1000);
 */
