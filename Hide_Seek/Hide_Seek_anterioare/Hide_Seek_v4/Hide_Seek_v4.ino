#include <MovingAverageFilter.h>

#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#include "Types.h"
#include "Motors.h"
#include "Ultrasonic.h"

#define MAX_SPEED 115
#define RED 41
#define GREEN 39 
#define BLUE 43
#define K 10
#define DIST_WALL 6.0
#define COFF 0.15

MovingAverageFilter Fwd(100);
MovingAverageFilter L(100);
MovingAverageFilter R(100);

//we made this from uint32 in double for accuracy
double forward = 15, left= 6, right= 6;
double rightCOR = 0, leftCOR = 0;//COR -> CORECTION

uint8 right_dist,left_dist,front_dist;

uint8 state = 0;//0->wait 5 seconds; 1->forward; 2->right; 3->left
uint8 find_robot=0;
uint8 alg_used=0;//0 0->right, 1->left
uint32 timer=millis();
uint32 prevTime = 0;
uint32 speed_val_right;
uint32 speed_val_left;
uint8 danceState = 0; //used for dancing

int counter = 0, previousState = 0;

IRrecv irrecv(2);
IRsend irsend;
decode_results results;

NewPing sonar1(trig_1, echo_1, Max_Dist); 
NewPing sonar2(trig_2, echo_2, Max_Dist); 
NewPing sonar3(trig_3, echo_3, Max_Dist);

uint16 value;



void setup() {
    pinMode(43, OUTPUT);
    irrecv.enableIRIn(); // Start the receiver
    
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


void STOP()
{
  go(0,0,1);
}


//dancing function
void dance()//dancing: left wheel goes forward for 1 second, then right wheel goes backwards for 1 second
{
  uint32 currTime = millis();

  
  while((millis() - currTime < 500) && ((danceState % 2) == 0))
  {
    go(0, 150, 1);
  }
  
  //currTime = millis();
 
  while((millis() - currTime < 500) && ((danceState % 2) != 0))
  {
    //go(0, 150, 1);
    analogWrite(MOTOR_Left_1, 0);
    analogWrite(MOTOR_Right_1, 150);

    analogWrite(MOTOR_Left_2, 0);
    analogWrite(MOTOR_Right_2, 0);
  }

  danceState++;//we now use the other wheel to move
}

//adjust->returns the error between the distance we want the robot to be from the right wall
double adjust()
{
  double rez = 0;
  double err = DIST_WALL - right;
  
  rez = COFF * err;
  
  return rez;
}


//go back and rotate function -> CHANGE TO JUST ROTATE!!!!!!!
void back()
{
  uint32 currTime = millis();

  while(millis() - currTime < 1000)//we go back
  {
    go(MAX_SPEED - leftCOR*100, MAX_SPEED - rightCOR*100, 0);//NOTICE THE THIRD ARGUMENT IS 0!! SO WE GO BACKWARDS NOW!
  }

  currTime = millis();

  while(millis() - currTime < 700)//we rotate now
  {
     go(0 , MAX_SPEED - 100, 1); 
  }

  state = 1;//we go forward again
  
}

//opreste -> returns 1 if we are in a hiding spot or return 0 if we can advance
sint8 opreste()
{
  sint8 res = 0;
  
  if(((uint8)forward <15)  && ((uint8)left < 12) && ((uint8)right < 12))
    {
      res = 1;
    }
  
    return res;
}

//go function that sets the tensions to the pin of the motors of the wheels
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

//in this function we go forward
void seek()
{
  Serial.print("opreste: ");
  Serial.println(opreste());
  go(((uint8)(MAX_SPEED - leftCOR*100)),((uint8)( MAX_SPEED - rightCOR*100)), 1); // we guide ourselves only using the right wall
}

/*
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
*/

void rotate_right() //state = 2 -> for turning right
{
  go(MAX_SPEED, MAX_SPEED/4 + 25, 1);

  if(right < 10)
    {
      go(0, 0, 1);
      state = 1; //go forward again
    }
  
}
void rotate_left()
{
  if(millis()- timer < 650)
    {
      go(0 , 200, 1);
    }
    else state = 1; //go forward again
}

//function that decides which way to go
void robot_move()
{
   
  if(results.value == 4294967295 )
    find_robot = 1;
  
  if(right < 15 && state == 1)//we guide the robot ONLY when we GO FORWARD! And using right wall 
  {
    if (adjust() > 0)// if positive then we are to close to the right wall and we need to correct by going to the left
    {
      leftCOR = adjust();
      rightCOR = 0;
    }
    else  // if negative then we are to close to the left wall and we need to correct by going to the right
    {
      rightCOR = -adjust();
      leftCOR = 0;
    }
  }
  else// if we are not in the 'go forward' state, then we do NOT adjust and we keep the same speed
  {
    leftCOR = rightCOR = 0;
  }
  
    
  if(find_robot==0)
  {
    if(counter == 25)
    {
      if(previousState == 0)
      {
        digitalWrite(BLUE, 30);
        previousState = 1;
      }
      else
      {
        digitalWrite(BLUE,LOW);
        previousState = 0;
      }
      counter = 0;
      }
      counter++;
      switch(state)
      {
        case 0: //before start, wait 5 seconds
        { 
        //if(millis()-timer>5000) //wait 5 seconds
          state=1;

          break;
        }
    
        case 1: // go forward
        {  
          if(right>18) 
          {
            timer = millis();
            state = 2; //turn right
          }
          else
            if(forward > 18)
            {
              seek();
            }
            else
              if(left > 18)
              {
                timer=millis();
                state=3; //turn left
              }
             /* else
              {
                go(0,0,1);
              }
*/
          break;
        }
    
        case 2: // go right
        {
          rotate_right();
          break;
        }

        case 3:
        {
          rotate_left();// go left
          break;
        }

        case 4:
        {
          back();// go back
          break;
        }
    
      }
    }
   else //place the code for when robot is found
   {
     //go(0, 0, 1);
     digitalWrite(BLUE, 40);
     dance();
   }
  
}



void loop() 
{

  right_dist = sonar2.ping_cm();
  left_dist = sonar1.ping_cm();
  front_dist = sonar3.ping_cm();

  forward = Fwd.process(sonar3.ping_cm());
  left=L.process(sonar1.ping_cm());
  right=R.process(sonar2.ping_cm());

  
  Serial.print("fata: ");
  Serial.println(forward);
  Serial.print("dreapta: ");
  Serial.println(right);
  Serial.print("stanga: ");
  Serial.println(left);
   
  /*
  Serial.print("Diferenta: ");
  Serial.println(DIST_WALL - right);
  
  Serial.print("Viteza dreapta: ");
  Serial.println((uint8)(MAX_SPEED - rightCOR*100));
  */
  /*
  speed_val_right = 100 + K * (DIST_WALL - right);  
  speed_val_left = 100 + K * (DIST_WALL - left);
  */


  if(opreste())
  {
    STOP();//we call go(0,0,1) here -> we will implement the rotation function here!!!
  }
  else
  {
    robot_move();
  }
 
 //dance();
  if ((irrecv.decode(&results)))
   {
     irrecv.resume();  
     value = results.value;
     Serial.println(results.value);
   }

  //irsend.sendSony(0x76656f6e656572, 12); //send 0xa90 on infrared
}

/*
   digitalWrite(43, HIGH);
  delay(1000);
  digitalWrite(43,LOW);
  delay(1000);
 */
