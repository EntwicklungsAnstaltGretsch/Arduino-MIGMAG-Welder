#include <Arduino.h>
/*
Program for control of transformer MIG/MAG welder 
*/

//Pins
int ItGasOn         = A0;
int ItGasOff    	  = A1;
int InSpeedWire     = A2;
int ItDottingDurr   = A3;
int ItDottingPause  = A4;
int IbDotting       = 2;
int IbButtonPress   = 12;
int ObMainContactor = 11;
int obGasValve      = 10;
int OpwmWireFeed    = 9;

//Variables Input
int t_I_GasOn;            //preflow time gas
int t_I_GasOff;           //postflow time gas
int n_I_SpeedWire;        //wirefeed Speed
int t_I_DottingDur;      //dotting durration time
int t_I_DottingPause;     //dotting pause time
bool b_I_Dotting;         //dotting switch
bool b_I_ButtonPress;     //welding gun button

//variables output
bool b_O_MainContactor;   //main contactor
bool b_O_GasValve;        //gas valve
bool b_O_WireFeed;        //PWM wirefeed

//factors for analog inputs
int nFactTON = 2;
int nFactTOF = 2;
int nFactDotDur = 4;
int nFactDotPau = 4;

//program variables
bool bButtonOperated;     //button operated
bool bTurningOff;         //postflow active
bool bPwmActiv;           //PWM active
bool bDottingActiv;       //Dotting active
long nPWMHIGH;            //time for pwm pulse high
long nPWMLOW;             //time for pwm pulse low

//saving system time
long nTStart;             //saving system time for preflow
long nTStop;              //saving system time for postflow
long nTDottingDur;        //saving system time for dotting duration
long nTDottingPause;      //saving system time for dotting pause
long nTPWM;               //saving system time for pwm control
long curmillis;           //saving curent system time
long curmicros;           //saving curent system time

void setup() {
  //assigning pin modes
  pinMode(IbButtonPress, INPUT_PULLUP);
  pinMode(IbDotting, INPUT);
  pinMode(obGasValve, OUTPUT);
  pinMode(ObMainContactor, OUTPUT);
  pinMode(OpwmWireFeed, OUTPUT);
}

void ReadInput(){
  //reading inputs and writing to local variables
  b_I_Dotting = !digitalRead(IbDotting);
  b_I_ButtonPress = !digitalRead(IbButtonPress);
  curmillis = millis();
  curmicros = micros();
}

void ReadAnalog(){
  //Analog inputs, values are saved during welding operation
  t_I_GasOn = analogRead(ItGasOn);
  t_I_GasOff = analogRead(ItGasOff);
  t_I_DottingDur = analogRead(ItDottingDurr);
  t_I_DottingPause = analogRead(ItDottingPause);
  n_I_SpeedWire = analogRead(InSpeedWire);
}

void WriteOutput(){
  //Write outputs
  digitalWrite(ObMainContactor, b_O_MainContactor);
  digitalWrite(obGasValve, b_O_GasValve);
  digitalWrite(OpwmWireFeed, b_O_WireFeed);
}

void TurningOn(){
  //function for preflow
  if ((!bButtonOperated) && b_I_ButtonPress){
    nTStart = curmillis;
    bButtonOperated = true;
    b_O_GasValve = true;
  }

  else if (bButtonOperated){
    if (nTStart + (t_I_GasOn * nFactTON) <= curmillis){
      b_O_MainContactor = true;
    }
  }

  else{
    return;
  }
}

void Pulsing(){
  //function for dotting
  if (b_O_MainContactor && (!bDottingActiv)){
    nTDottingDur = curmillis;
    bDottingActiv = true;
  }
  else if (bDottingActiv && b_O_MainContactor){    //Dotting duration
    if (nTDottingDur + (long(t_I_DottingDur) * nFactDotDur) <= curmillis){
      b_O_MainContactor = false;
      nTDottingPause = curmillis;
    }
  }
  else if (bDottingActiv && (!b_O_MainContactor)){ //Dotting pause
    if (nTDottingPause + (long(t_I_DottingPause) * nFactDotPau) <= curmillis){
      b_O_MainContactor = true;
      bDottingActiv = false;
    }
  }
  else{
    return;
  }
}

void WireFeed(){
  //function for pwm control of wirefeed
  if (!bPwmActiv){
    nTPWM = curmicros;
    bPwmActiv = true;
    b_O_WireFeed = true;
  }
  if (bPwmActiv){
    nPWMHIGH = (1023 - n_I_SpeedWire);
    nPWMLOW = n_I_SpeedWire;
    if (nTPWM + nPWMHIGH <= curmicros){
      b_O_WireFeed = false;
    }
    if (nTPWM + nPWMHIGH + nPWMLOW <= curmicros){
      bPwmActiv = false;
    }
    else{
      return;
    }
  }
  else{
    return;
  }
}

void TurningOff(){
  //function for postflow
  if (bButtonOperated && (!b_I_ButtonPress)){
    b_O_MainContactor = false;
    bButtonOperated = false;
    bTurningOff = true;
    nTStop = curmillis;
  }

  else if (bTurningOff){
    if (nTStop + (t_I_GasOff * nFactTOF) <= curmillis){
      b_O_GasValve = false;
      bTurningOff = false;
    }
  }

  else{
    return;
  }

}

void loop() {
  ReadInput();          

  if (!bButtonOperated){                                  //if the welding gun button wasn't operated analog inputs are read
    ReadAnalog();
  }
  TurningOn();

  if (b_I_Dotting && bButtonOperated && b_I_ButtonPress){   //if switch for dotting is toggled
    Pulsing();          
  }

  if (b_O_MainContactor){                                 //as long as the main contactor is on, wire is fed
    WireFeed();
  }
  TurningOff();
  WriteOutput();        
}
