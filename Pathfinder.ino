#include <Wire.h>
#include "RGB_Ring_V3.h"
#include <LSM303DLH.h>


///////////////////////////////////
//Notes
//Serial Communication
//Start a serial command with ! and end with *
//This will pipe confirmation messages back
//We encapsulate within, commands such as DIR,100,150,200
//Commands are comma seperated
//This sets a command and the arguments parsed which goes into a list
//The DIR command sets the direction heading and the number of variables parsed
//is a list of directions to be processed and shown
///////////////////////////////////

LSM303DLH compass;

boolean debug = true;

int val1 = 0;     // variable to store the read value
int val2 = 0;

int S1 = 3; 
int S2 = 4;

//Input serial
char string[25];
int i=0;
int string_len;
int inByte;
int last_inByte;

//Serial processing
int argcnt;
int dircnt;
int cmdlen;
char* cmd;
boolean newData = false;

//Lets store the direction heading values
int dir[5];
int cmps[5];
int hue[5] = {360,240,60,180,120};


void setup() {
  Serial.begin(4800);
  Serial.println("Serial Ready");
  
  Wire.begin();

  pinMode(S1, INPUT);     
  pinMode(S2, INPUT);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  
  InitIO();

  compass.enableDefault();
  
  // Calibration headingues. Use the Calibrate example program to get the headingues for
  // your compass.
  compass.m_min.x = -858; compass.m_min.y = -632; compass.m_min.z = -591;
  compass.m_max.x = +499; compass.m_max.y = +664; compass.m_max.z = 628;
  
}

void loop() {
  
  if (Serial.available() > 0) {
    inByte = Serial.read();
    string_len=0;
    if (inByte == '!') read_serial();
  }  // end of incoming data
  
    //Read compass information
      compass.read();
      int heading = compass.heading((LSM303DLH::vector){0,-1,0}) - 90;
//      if(debug)Serial.println(heading);


  
  val1 = digitalRead(S1);
  val2 = digitalRead(S2);
  
  set_all_hsv(0,0,5);
  
  //Go through the direction list and provide an up to date bearing
  //Todo, create a timer and flush out old, unupdated headings for timeouts
  //Also set a serial command to flush the list
  for(int j = 0; j < dircnt; j++){
      cmps[j] = updateHeading(dir[j],heading);
//      setDirection(cmps[j], rgb[j][0], rgb[j][1], rgb[j][2]);
      setDirection(cmps[j],hue[j],255,255);
      if(newData && debug){
        Serial.print("Got new dirs :");
        Serial.print(j);
        Serial.print(" :");
        Serial.println(dir[j]);
        Serial.print("Updating Compass position :");
        Serial.println(j);
        Serial.print("New compass direction :");
        Serial.println(cmps[j]);
      }
  }
  delay(10);
  newData = false;
}

void setDirection(int heading, int H, int S, int V){
  if((heading >= 0 && heading <= 20) || (heading > 340 && heading <=360)){
    set_led_hsv (0, H,S,V);
  } if(heading > 10 && heading <= 50){
    set_led_hsv (1, H,S,V);
  } if(heading > 40 && heading <= 80){
    set_led_hsv (2, H,S,V);
  } if(heading > 70 && heading <= 110){
    set_led_hsv (3, H,S,V);
  } if(heading > 100 && heading <= 140){
    set_led_hsv (4, H,S,V);
  } if(heading > 130 && heading <= 170){
    set_led_hsv (5, H,S,V);
  } if(heading > 160 && heading <= 200){
    set_led_hsv (6, H,S,V);
  } if(heading > 190 && heading <= 230){
    set_led_hsv (7, H,S,V);
  } if(heading > 220 && heading <= 260){
    set_led_hsv (8, H,S,V);
  } if(heading > 250 && heading <= 290){
    set_led_hsv (9, H,S,V);
  } if(heading > 280 && heading <= 320){
    set_led_hsv (10, H,S,V);
  }  if(heading > 310 && heading <= 350){
    set_led_hsv (11, H,S,V);
  }
}

int updateHeading(int arg, int bearing){ //Translate the direction to our current direction
  int val = (arg - bearing); 
  val = fmod(val,360);
  if(val < 0) val += 360;
  return val;
}

void read_serial()  // FUNCTION FOR READING THE SERIAL MESSAGE
{
   Serial.println ("SOL FOUND and reading");  // THE SOL (Start of Line) found notice.
   Serial.print("READ : !\n");  //Saying that it has read "!" on the serial port
   while (inByte!= '*') // As long as EOL not found, keep reading
   if (Serial.available() > 0) // if new data is available
        { inByte = Serial.read(); // Read new byte
           if(debug)Serial.print("READ : "); // Display the new byte
           string[string_len] = inByte; // Save the data in a character array
           if(debug)Serial.println(string[string_len]); // Print the characters that was recieved
           string_len++;}
    else if (Serial.available() == 0)
          {Serial.println("EOL not available, data string invalid"); // If EOL not in the string
          Serial.println("$N"); //FAIL SIGNATURE
           break;
           }

 if (inByte == '*')
  {
       Serial.println ("eol FOUND, and full string was");  // Echoes Success Message
       Serial.println ("$Y"); //SUCCESS SIGNATURE
       newData = true;
       processCommand(string);
       for (i=0;i<(string_len-1);i++) Serial.print(string[i]);
       Serial.print ("\n");
   }
 }
 
 void processCommand(char* cmdbuf){
   int j = 0;
   while( cmdbuf[++j] != ',' ) ; // find first comma
   cmdbuf[j] = 0;          // null terminate command
   cmd = cmdbuf;     //
   cmdlen = j;         // length of cmd
  
   int args[5], a;         // five args max, 'a' is arg counter
   char* s; char* argbuf = cmdbuf+cmdlen+1;
   while( (s = strtok(argbuf, ",")) != NULL && a < 5 ) {
     argbuf = NULL;
     args[a++] = (int)strtol(s,NULL,0); // parse hex or decimal arg //Was (byte) however was clipping vals
   }
   argcnt = a;         // number of args read  
   
   if(debug){
     Serial.print("Cmd Recieved :" );
     Serial.println(cmd);
     Serial.print("Number of args recieved :");
     Serial.println(argcnt);
     for(j=0;j<argcnt;j++){
       Serial.print("Arg :"); 
       Serial.print(j);
       Serial.print(" = "); 
       Serial.println(args[j]);
     }
   }
   
   if(strcmp(cmd,"DIR")==0){
     Serial.println("Got DIR");
     dircnt = argcnt;
     for(j=0;j<argcnt;j++){
       dir[j] = args[j];
       if(debug){
         Serial.print("Got argument :");
         Serial.print(j);
         Serial.print(" with value :");
         Serial.println(args[j]);
         Serial.print("New dir :");
         Serial.println(dir[j]);
       }
     }  
   }
    
   
 }
