#define   BAUDRATE           57600
#define   LED                13
#define   Theshold_Eyeblink  125
#define   EEG_AVG            70

#define GREENLED1  8
#define GREENLED2  9
#define GREENLED3  10
#define YELLOWLED1 11
#define YELLOWLED2 12
#define FWD 7
#define RV 6
#define LT 5
#define RT 4
#define ST 3
#define PS 2

byte fwd=0;
byte attention = 0;
byte meditation = 0;
byte bflag=0;
byte blinkc=0;

long payloadDataS[5] = {0};

long payloadDataB[32] = {0};

byte checksum=0,generatedchecksum=0;

unsigned int Raw_data,Poorquality,Plength,Eye_Enable=0,On_Flag=0,Off_Flag=1 ;

unsigned int j,n=0;

long Temp,Avg_Raw,Temp_Avg;

 boolean bigPacket = false;

 void setup()

 {
   pinMode(GREENLED1, OUTPUT);
  pinMode(GREENLED2, OUTPUT);
  pinMode(GREENLED3, OUTPUT);
  pinMode(YELLOWLED1, OUTPUT);
  pinMode(YELLOWLED2, OUTPUT);
  pinMode(FWD, OUTPUT);
  pinMode(RT, OUTPUT);
  pinMode(LT, OUTPUT);
  pinMode(RV, OUTPUT);
  pinMode(ST, OUTPUT);
  pinMode(PS,OUTPUT);
   
   Serial.begin(BAUDRATE);           // USB

   pinMode(LED, OUTPUT);
   digitalWrite(ST,HIGH);

 }

  byte ReadOneByte()           // One Byte Read Function
 {
   int ByteRead;
   while(!Serial.available());
   ByteRead = Serial.read();
   return ByteRead;
 }

void loop()                     // Main Function
 {
   if(ReadOneByte() == 170)        // AA 1 st Sync data
   {
     if(ReadOneByte() == 170)      // AA 2 st Sync data
     {
       Plength = ReadOneByte();
       if(Plength == 4)   // Small Packet
       {
         Small_Packet ();
       }
       else    // Big Packet
       {
         Big_Packet ();
       }
     }
   }        
 }

 void Small_Packet ()
 {
   generatedchecksum = 0;
   for(int i = 0; i < Plength; i++)
   { 
     payloadDataS[i]     = ReadOneByte();      //Read payload into memory
     generatedchecksum  += payloadDataS[i] ;
  }
   generatedchecksum = 255 - generatedchecksum;
   checksum  = ReadOneByte();
   if(checksum == generatedchecksum)        // Varify Checksum
   { 
     if (j<512)
     {
       Raw_data  = ((payloadDataS[2] <<8)| payloadDataS[3]);
       if(Raw_data&0xF000)
       {
         Raw_data = (((~Raw_data)&0xFFF)+1);
       }
       else
       {
         Raw_data = (Raw_data&0xFFF);
       }
       Temp += Raw_data;
       j++;
     }
     else
     {
       Onesec_Rawval_Fun ();
     }
   }
 }

 void Big_Packet()
 {
   generatedchecksum = 0;
   for(int i = 0; i < Plength; i++)
   { 
     payloadDataB[i]     = ReadOneByte();      //Read payload into memory
     generatedchecksum  += payloadDataB[i] ;
   }
   generatedchecksum = 255 - generatedchecksum;
   checksum  = ReadOneByte();
   if(checksum == generatedchecksum)        // Varify Checksum
   {
     Poorquality = 200;
        attention = 0;
        meditation = 0;
        for(int i = 0; i < Plength; i++) {    // Parse the payload
          switch (payloadDataB[i]) {
          case 2:
            i++;            
            Poorquality = payloadDataB[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadDataB[i];                        
            break;
          case 5:
            i++;
            meditation = payloadDataB[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop
     
     if(bigPacket) {
          if(Poorquality == 0)
            digitalWrite(PS, HIGH);
          else
            digitalWrite(PS, LOW);

          switch(attention / 10) {
          case 0:
          case 1:
            digitalWrite(GREENLED1, HIGH);
            digitalWrite(GREENLED2, LOW);
            digitalWrite(GREENLED3, LOW);
            digitalWrite(YELLOWLED1, LOW);
            digitalWrite(YELLOWLED2, LOW);
            break;
          case 2:
          case 3:
            digitalWrite(GREENLED1, HIGH);
            digitalWrite(GREENLED2, HIGH);
            digitalWrite(GREENLED3, LOW);
            digitalWrite(YELLOWLED1, LOW);
            digitalWrite(YELLOWLED2, LOW);
            break;
          case 4:
          case 5:
            digitalWrite(GREENLED1, HIGH);
            digitalWrite(GREENLED2, HIGH);
            digitalWrite(GREENLED3, HIGH);
            digitalWrite(YELLOWLED1, LOW);
            digitalWrite(YELLOWLED2, LOW);
            break;
          case 6:
          case 7:              
            digitalWrite(GREENLED1, HIGH);
            digitalWrite(GREENLED2, HIGH);
            digitalWrite(GREENLED3, HIGH);              
            digitalWrite(YELLOWLED1, HIGH);
            digitalWrite(YELLOWLED2, LOW);
            break;
          case 8:
          case 9:
          case 10:
            digitalWrite(GREENLED1, HIGH);
            digitalWrite(GREENLED2, HIGH);
            digitalWrite(GREENLED3, HIGH);              
            digitalWrite(YELLOWLED1, HIGH);
            digitalWrite(YELLOWLED2, HIGH);           
            break;
          }                     
        }
     drive();
     if (Poorquality==0 )
     {
       Eye_Enable = 1;
     }
     else
     {
       Eye_Enable = 0;
     }
   }
 }

 void Onesec_Rawval_Fun ()
 {
   Avg_Raw = Temp/512;
   if (On_Flag==0 && Off_Flag==1)
   {
     if (n<3)
     {
       Temp_Avg += Avg_Raw;
       n++;
     }
     else
     {
       Temp_Avg = Temp_Avg/3;
       if (Temp_Avg<EEG_AVG)
       {
         On_Flag=1;Off_Flag=0;
       }
       n=0;Temp_Avg=0;
     } 
   }             
   Eye_Blink ();
   j=0;
   Temp=0;
 }

 void Eye_Blink ()
 {
   if (Eye_Enable)         
   {
     if (On_Flag==1 && Off_Flag==0)
     {
       if ((Avg_Raw>Theshold_Eyeblink) && (Avg_Raw<350))
       {
         bflag=1;
         digitalWrite(LED,HIGH);
         blinkc++;
         drive();
       }
       else
       {
         if (Avg_Raw>350)
         {
           On_Flag==0;Off_Flag==1;
         }
         bflag=0;
         drive();
         digitalWrite(LED,LOW);
       }
     }
     else
     {
       //blinkc=0;
       bflag=0;
       drive();
       digitalWrite(LED,LOW);
     }
   }       
   else
   {
     //blinkc=0;
     bflag=0;
     drive();
     digitalWrite(LED,LOW);
   }
 }

 void drive()
 {
  if(attention>30)
  {
    digitalWrite(ST, LOW);
    digitalWrite(FWD, HIGH);
    digitalWrite(RV,LOW);
    //fwd=1;
    if(blinkc==2 && bflag==0)
    {
      digitalWrite(RT, HIGH);
      digitalWrite(LT, LOW);
      delay(3000);
      digitalWrite(RT, LOW);
      digitalWrite(LT, LOW);
      blinkc=0;
      //bflag=0;
    }
    else if(blinkc==3 && bflag==0)
    {
      digitalWrite(RT, LOW);
      digitalWrite(LT, HIGH);
      delay(3000);
      digitalWrite(RT, LOW);
      digitalWrite(LT, LOW);
      blinkc=0;
      //bflag=0; 
    }
    else if(bflag==0)
    {
      blinkc=0;
    }
    else if(blinkc==0)
    {
      fwd=1;
    }
  }
  else if(blinkc==2 && bflag==0 && fwd==1)
    {
      digitalWrite(ST, LOW);
    digitalWrite(FWD, HIGH);
    digitalWrite(RV,LOW);
      digitalWrite(RT, HIGH);
      digitalWrite(LT, LOW);
      delay(3000);
      digitalWrite(RT, LOW);
      digitalWrite(LT, LOW);
      blinkc=0;
      fwd=0;
      //bflag=0; 
    }
    else if(blinkc==3 && bflag==0 && fwd==1)
    {
      digitalWrite(ST, LOW);
    digitalWrite(FWD, HIGH);
    digitalWrite(RV,LOW);
      digitalWrite(RT, LOW);
      digitalWrite(LT, HIGH);
      delay(3000);
      digitalWrite(RT, LOW);
      digitalWrite(LT, LOW);
      blinkc=0;
      fwd=0;
      //bflag=0; 
    }
  else if(meditation>60)
  {
    blinkc=0;
    bflag=0;
    digitalWrite(FWD, LOW);
    digitalWrite(RV,HIGH);
    digitalWrite(RT, LOW);
    digitalWrite(LT, LOW);
    digitalWrite(ST,LOW);
  }
  else
  {
    blinkc=0;
    bflag=0;
    digitalWrite(FWD, LOW);
    digitalWrite(RV,LOW);
    digitalWrite(RT, LOW);
    digitalWrite(LT, LOW);
    digitalWrite(ST, HIGH);
  }
 }
