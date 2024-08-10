#include "Globals.h"
#include "UtilityFunctions.h"

// Send Xml back to web client
void SendXML() {
  int timerValue=0;
  //loopCounter++;
  //Serial.println("SendXML->sending xml");
  //Serial.print("1 sizeof XML ");Serial.println(sizeof(XML));
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");
 //Serial.print("1 XML length");Serial.println(strlen(XML));
  // send temp in F and C
  //if ( loopCounter >= 10)
  //{
    //String tempc = get_temp_string();
  //  loopCounter = 0;
  //}
  // Send Version info
  sprintf(buf, "<VERSION>%d.%02d.%02d</VERSION>\n", Release,Version,Revision);
  strcat(XML, buf);

  // Send Temp values in C and F
  sprintf(buf, "<TEMPC>%.2f</TEMPC>\n", tempC);
  strcat(XML, buf);
  sprintf(buf, "<TEMPF>%.2f</TEMPF>\n", tempF);
  strcat(XML, buf);
  // if temp goes whacky send values
  if (tempC < 0 || tempC > 330)
  {
    sprintf(buf, "<adcValue>%d</adcValue>\n", adcValue);
    strcat(XML, buf);
    sprintf(buf, "<voltage>%.2f</voltage>\n", voltageTemp);
    strcat(XML, buf);
    sprintf(buf, "<Rt>%.2f</Rt>\n", Rt);
    strcat(XML, buf);
  }
  //Serial.print("2 XML length");Serial.println(strlen(XML));
  // Temp readout preference
  sprintf(buf, "<TEMP_PREF>%d</TEMP_PREF>\n", TempPref);
  strcat(XML, buf);

  // Temp probe type
  sprintf(buf, "<TEMP_PROBE>%d</TEMP_PROBE>\n", TempSensorKOhms);
  strcat(XML, buf);

  // Bean Quantity 
  sprintf(buf, "<BEAN_QTY>%d</BEAN_QTY>\n", BeanQuantity);
  strcat(XML, buf);
  
  // Send Timer 0 Minutes
  //if (TimerMin0 != TimerMin0New)
 // {
    TimerMin0 = TimerMin0New;
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    timerValue = TimerStartValue;
    sprintf(buf, "<TMIN0>%d</TMIN0>\n", TimerMin0);
    strcat(XML, buf);
 // }
  // Send Timer 0 Seconds
  //if (TimerSec0 != TimerSec0New)
  //{
    TimerSec0 = TimerSec0New;
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    //TimerValue = TimerStartValue;
    sprintf(buf, "<TSEC0>%d</TSEC0>\n", TimerSec0);
    strcat(XML, buf);
 // }

//Serial.print("3 XML length");Serial.println(strlen(XML));
  // Send Timer Remaining Time
  sprintf(buf, "<TIMEREM0>%s</TIMEREM0>\n", get_timer_string(TimerValue));
  strcat(XML, buf);
  // Timer start stop
  if (rState->timerStart) {
    strcat(XML, "<TSTART>1</TSTART>\n");
 
  }
  else {
    strcat(XML, "<TSTART>0</TSTART>\n");
  }
//Serial.print("4 XML length");Serial.println(strlen(XML));
   // Send Config Timer Remaining Time
  sprintf(buf, "<CTIMEREM>%s</CTIMEREM>\n", get_timer_string(ConfigTimerValue));
  strcat(XML, buf);
  // Send Config Step
  sprintf(buf, "<C_STEP>%d</C_STEP>\n", ConfigStep);
  strcat(XML, buf);

    // Send Config Number of Steps
  sprintf(buf, "<C_MAXSTEPS>%d</C_MAXSTEPS>\n", ConfigMaxSteps);
  strcat(XML, buf);

    // Send heatgun selection low=0 high=1
  sprintf(buf, "<HEATGUN>%d</HEATGUN>\n", HEATGUNHIGH);
  strcat(XML, buf);

  
    // Send Config Step Time
  sprintf(buf, "<C_STEP_TIME>%d</C_STEP_TIME>\n", ConfigTimerStartValue);
  strcat(XML, buf);
  
  // Send UtilTimer Timer 0 Minutes
  if (PreTimerMin != PreTimerMinNew)
  {
    PreTimerMin = PreTimerMinNew;
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    PreheatTimerValue = PreheatTimerStartValue;
    //CooldownTimerValue = PreheatTimerStartValue;
  }
    sprintf(buf, "<PRE_TMIN0>%d</PRE_TMIN0>\n", PreTimerMin);
    strcat(XML, buf);
  
 //Serial.print("5 XML length");Serial.println(strlen(XML));
  // Send UtilTimer Timer 0 Seconds
 
  if (PreTimerSec != PreTimerSecNew)
  {
    PreTimerSec = PreTimerSecNew;
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    PreheatTimerValue = PreheatTimerStartValue;
    //CooldownTimerValue = PreheatTimerStartValue;
    //Serial.print("SendXML PreheatTimer timer value ");Serial.println(PreheatTimerValue);
  }
    sprintf(buf, "<PRE_TSEC0>%d</PRE_TSEC0>\n", PreTimerSec);
    strcat(XML, buf);
 

   //  Send temp servo position
    sprintf(buf, "<SERVO_POS>%d</SERVO_POS>\n", servoPos);
    strcat(XML, buf);
   //Serial.print("6 XML length");Serial.println(strlen(XML));  
   
   //  Send temp servo new position
    sprintf(buf, "<SERVO_POS_NEW>%d</SERVO_POS_NEW>\n", servoPosNew);
    strcat(XML, buf);
    //Serial.print("7 XML length");Serial.println(strlen(XML));
  
  // Send UtilTimer Timer Remaining Time
  sprintf(buf, "<PRE_TIMEREM0>%s</PRE_TIMEREM0>\n", get_timer_string(PreheatTimerValue));
  strcat(XML, buf);
  
  // Send Finish Temp
  sprintf(buf, "<FINISH_TEMP>%d</FINISH_TEMP>\n", FinishTemp);
  strcat(XML, buf);
  
  // Send Preheat Temp
  sprintf(buf, "<PREHEAT_TEMP>%d</PREHEAT_TEMP>\n", PreheatTemp);
  strcat(XML, buf);
  
  // Send Coffee Selected
  sprintf(buf, "<Coffee_Type>%d</Coffee_Type>\n", CoffeeOpt);
  strcat(XML, buf);
  
  // Send Set Temp
  sprintf(buf, "<Set_Temp>%.2f</Set_Temp>\n", setTemp);
  strcat(XML, buf);
 
    // Timer start stop
  if (rState->preheatTimerStart) {
    strcat(XML, "<PTSTART>1</PTSTART>\n");
  }
  else {
    strcat(XML, "<PTSTART>0</PTSTART>\n");
  }
//Serial.print("8 XML length");Serial.println(strlen(XML));
  // show mixer power enable led0 status
  if (rState->mixerpwr) {
    strcat(XML, "<MIX>1</MIX>\n");
  }
  else {
    strcat(XML, "<MIX>0</MIX>\n");
  }

  if (MIXDIR) {
    strcat(XML, "<MIXDIR>1</MIXDIR>\n");
  }
  else {
    strcat(XML, "<MIXDIR>0</MIXDIR>\n");
  }
  // send heater power enable status
  // False is ON for SSR
  if (!rState->heaterpwr) {
    strcat(XML, "<HEATER>1</HEATER>\n");
  }
  else {
    strcat(XML, "<HEATER>0</HEATER>\n");
  }
  //Serial.print("9 XML length");Serial.println(strlen(XML));
  // send roasting status
  if (rState->doRoast) {
    strcat(XML, "<ROASTING>1</ROASTING>\n");
  }
  else {
    strcat(XML, "<ROASTING>0</ROASTING>\n");
  }
  
  //Serial.print("10 XML length");Serial.println(strlen(XML));
  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  //Serial.println("size of xml data");
  //Serial.println(strlen(XML));

  // you may have to play with this value, big pages need more processing time, and hence
  // a longer timeout that 200 ms
  Server.send(200, "text/xml", XML);
  //Serial.print("After send XML length");Serial.println(strlen(XML));
  XML[0]='\0';
}
