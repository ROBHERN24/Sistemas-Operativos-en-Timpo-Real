/*Enciende el Blootooth mediante un boton y el esp32 se conecta a bluetooth*/
#include <BluetoothSerial.h>
#define LED1_GPIO 5
#define boton 15
BluetoothSerial SerialBT;
void ConfigByBluetooth(void);

void setup() {
  // put your setup code here, to run once:
pinMode(LED1_GPIO,OUTPUT);
pinMode(boton,INPUT_PULLUP);
Serial.begin(115200);
Serial.println("Inicio de Código");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (digitalRead == 0){
    delay(500);
    if(digitalRead(boton) == 0){
      while(digitalRead(boton) == 0);
      Serial.println("Boton Presionado");
      ConfigByBluetooth();
    }
  }
  else{
    Serial.println("Boton no Presionado");
  }
  delay(1000);
}

void ConfigByBluetooth(void){
  String message = "";
  String message2 = "";
  char incomingChar;
  char incomingChar2;
  bool BTHControl = true;
  SerialBT.begin("DISP_R");
  while(BTHControl==true){
    Serial.println("Bluetooth Disponible");
    if(SerialBT.available()){
      char incomingChar = SerialBT.read();
      if (incomingChar !='\n'){
        message += String(incomingChar);
      }
      else{
        message = "";
      }
      Serial.write(incomingChar);
    }
    if (message =="Reset"){
      BTHControl =false;
    }
    if(SerialBT.available()){
      char incomingChar2 = SerialBT.read();
      if (incomingChar2 !='\n'){
        message2 += String(incomingChar2);
      }
      else{
        message2 = "";
      }
      Serial.write(incomingChar2);
    }
  }
  SerialBT.end();
}
