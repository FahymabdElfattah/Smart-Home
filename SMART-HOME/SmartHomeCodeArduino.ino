//------------------------------- Les bibliothéques nécessaire pour notre projet ----------------------------
#include<SoftwareSerial.h>
#include<DHT.h>
#include <Wire.h>
//#include<LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
//SoftwareSerial mySerial (1,0);

//------------------------------------Les variabeles---------------------------
int const LmpPin1=4,LmpPin2=12,LmpPin3=7,LmpPin4=8 ,LmpPin5=2,LmpPin6;
int const MotPin1=13,  Mot1Pin1=5,Mot1Pin2=6, Mot2Pin1=9,  Mot2Pin2=10 ;
int const BuzPin=14 ,GazPin= A2;
bool HautM1= true , BasM1= false ,HautM2= true , BasM2= false ;
DHT dht(3,DHT11);
//LiquidCrystal lcd(15,16,17,18,19,20);
LiquidCrystal_I2C lcd(0x27, 20, 4);

int Temperature,TempDht12,HumidDht12;
char c ;
String  ContMOT3;
bool ContMOT2 ,ContMOT1,ContLMP;
bool Alume = false ;
bool EtatNormal =true  ,ContLigne =true ;
bool EtatNormal1 =true  ,ContLigne1 =true ;
//-------------------------------------setup----------------------------------
void setup() {
   lcd.init();
  Serial.begin(9600);
  dht.begin();
  lcd.begin(20,4);
  int i;
  for(i=5;i<=14;i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,LOW);
  }
 
 lcd.backlight();
 lcd.setCursor(5, 0);
 lcd.print("SESNUM");
 lcd.setCursor(3,1);
 lcd.print("SMART HOME");

 delay(1000);
 lcd.clear();
}
//-------------------------------------loop---------------------------------
void loop() {
   
  RegTemperature();
  AutoEclerage();
  DetectionFumee();
  ContRideaux();
  ContrGarage();
 if(Serial.available()){
  char c = Serial.read();
  switch(c){
  //-------------------------------------Controle des lampes par bluetoothe------------------------------
    case 'A':
       ContLMP = true;
       Serial.print("A");
       break ;
    case 'a':
       ContLMP = false ;
       Serial.print("a");
       break;
    case 'B':
       digitalWrite(LmpPin2 , HIGH);
       Serial.print("B");
       break ;
    case 'b':
       digitalWrite(LmpPin2 , LOW);
       Serial.print("b");
       break ;
    case 'C':
       digitalWrite(LmpPin3 , HIGH);
       Serial.print("C");
       break ;
    case 'c':
       digitalWrite(LmpPin3 , LOW);
       Serial.print("c");
       break ;
    case 'D':
       digitalWrite(LmpPin1 , HIGH);
       Serial.print("D");
       break ;
    case 'd':
       digitalWrite(LmpPin1 , LOW);
       Serial.print("d");
       break ;
    case 'E':
       digitalWrite(LmpPin5 , HIGH);
       Serial.print("E");
       break ;
    case 'e':
       digitalWrite(LmpPin5 , LOW);
       Serial.print("e");
       break ;
   case 'F':
       digitalWrite(LmpPin6 , HIGH);
       Serial.print("F");
       break ;
   case 'f':
       digitalWrite(LmpPin6 , LOW);
       Serial.print("f");
       break ;
 //-----------------------------------Controle de moteur de refroidissement par bluetooth --------------------
  case 'G':
       ContMOT1 = true ;
       Serial.print("G");
       break ;
  case 'g':
       ContMOT1 = false ;
       Serial.print("g");
       break ;
 //-----------------------------------------Controle du Rideaux par bluetooth------------------
   case 'H':
       ContMOT2 = true ;
       break ;
  case 'h':
       ContMOT2 = false ;
       break ;
  //--------------------------Control du Garage par bluetooth ---------------------------
    case 'I':
       ContMOT3 = "MONT" ;
       break ;
  case 'i':
       ContMOT3 = "DESC" ;
       break ;
  //------------------------RESET par bluetooth--------------------------
  case 'J':
       int j ;
       for(j=0 ;j<=15;j++){
        digitalWrite(j,LOW);
        Serial.print("J");
       }
       Serial.print("J");
       delay(30000);
       break ;
    }   
   }
   delay(100);
   lcd.clear();
}
//-----------------------Réglage de la Température--------------------------
void RegTemperature(){
  /*float  Valeur = analogRead(A0);
  float  Vout = Valeur *5.0/1023*1000;
  int Temp = Vout/10;*/
  int Temp = dht.readTemperature();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("T=");
  lcd.print(Temp);
  lcd.write(223);
  lcd.print("C");
  if((Temp<30 && ContMOT1 == true) ||(Temp>=30 && ContMOT1 == false) ||(Temp>=30 && ContMOT1 == true))
    digitalWrite(MotPin1,HIGH);
  if(Temp<30 && ContMOT1 == false)
    digitalWrite(MotPin1,LOW); 
}
//----------Eclerage par LDR-------------------------------------------------------
void AutoEclerage(){
  float Eclr = analogRead(A1);
  //Serial.println(Eclr);
  if ((Eclr < 600 && ContLMP == true)||(Eclr >= 600 && ContLMP == false)||(Eclr >= 600 && ContLMP == true) ){
    digitalWrite(LmpPin4,HIGH);
    
  } 
 if( Eclr < 600 && ContLMP == false){
     digitalWrite(LmpPin4,LOW); 
    }
}

//-----------------------Détection de fumée --------------------------------------------
void DetectionFumee(){
  lcd.setCursor(0,1);
  int Reff = 600 ;
  int analogSensor = analogRead(A3);
  lcd.setCursor(2, 1);
  lcd.print("Gaz:");

  
  //lcd.print("analogSensor");
  //lcd.println(analogSensor);
  if(analogSensor > Reff ){
    tone(BuzPin,100,200);
    EtatNormal = false ;
    lcd.print(" Danger");
   
  }    
  else{
    noTone(BuzPin);
    EtatNormal = true ;
   lcd.print(" Normal");
  } 
  if(EtatNormal == false && ContLigne == false ){
    Serial.print("k");
    ContLigne = true ;
  }
  if(EtatNormal == true && ContLigne == true){
    Serial.print("j");
    ContLigne = false ;
  }
  delay(250);
}
//---------------------------Controle du Rideaux avec DHT12 --------------------------------
void ContRideaux(){
  //HautM1= true , BasM1= false  ContMOT2 = "MONT"   ContMOT2 = "DESC"
  TempDht12 = dht.readTemperature();
  HumidDht12 = dht.readHumidity();
  lcd.setCursor(8, 0);
  lcd.print("Hum=");
  lcd.print(HumidDht12);
  lcd.print("%");
 //Serial.println("Temperature = " + String(dht.readTemperature())+" °C");
  //Serial.println("Humidite = " + String(dht.readHumidity())+" %");
  if(((ContMOT2 == true && HumidDht12 < 80 )|| (ContMOT2 == true && HumidDht12 >=80 ))and BasM1== true and HautM1== false){
    // monterCommande  MotPin2Start=9, MotPin2Rotat=10
    //Mot1Pin1=5,Mot1Pin2=6, Mot2Pin1=9,Mot2Pin2=10
    digitalWrite(Mot1Pin1,HIGH);
    digitalWrite(Mot1Pin2,LOW);
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,HIGH);
    Serial.print("H");
    delay(3000);
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,LOW);
    digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,LOW);
    Serial.print("h");
    HautM1= true;
    BasM1= false ;
  }
  if(((ContMOT2 == false && HumidDht12 <80 )||(ContMOT2 == false && HumidDht12 >=80 ) )and HautM1 == true and BasM1 == false){
    // DescandeCommande
    digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,HIGH);
    digitalWrite(Mot2Pin1,HIGH);
    digitalWrite(Mot2Pin2,LOW);
    Serial.print("H");
    delay(3000);
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,LOW);
    digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,LOW);
    Serial.print("h");
    HautM1 = false;
    BasM1 = true ;
  } 
  if(HumidDht12 >=90){
    EtatNormal1 = false ;
  }
  else{
    EtatNormal1 = true ;
  }
 if(EtatNormal1 == false and ContLigne1 == false ){//
    Serial.print("l");
    ContLigne1 = true ;
  }
  if(EtatNormal1 == true and ContLigne1 == true){//
    Serial.print("m");
    ContLigne1 = false ;
  }
  delay(250);
}
//---------------------------------------------------Controle du Garage------------------------------------
void ContrGarage(){
   if(ContMOT3 == "MONT" and BasM2== true and HautM2== false){
    // monterCommande  
    digitalWrite(Mot2Pin1,HIGH);
    digitalWrite(Mot2Pin2,LOW);
     digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,HIGH);
    Serial.print("I");
    delay(3000);
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,LOW);
    digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,LOW);
    Serial.print("i");
    HautM2= true;
    BasM2= false ;
  }
  if(ContMOT3 == "DESC" and HautM2 == true and BasM2 == false){
    // DescandeCommande
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,HIGH);
     digitalWrite(Mot1Pin1,HIGH);
    digitalWrite(Mot1Pin2,LOW);
    Serial.print("I");
    delay(3000);
    digitalWrite(Mot2Pin1,LOW);
    digitalWrite(Mot2Pin2,LOW);
    digitalWrite(Mot1Pin1,LOW);
    digitalWrite(Mot1Pin2,LOW);
    Serial.print("i");
    HautM2 = false;
    BasM2 = true ;
  } 
}
