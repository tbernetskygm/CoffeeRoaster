#include "Globals.h"
#include "UtilityFunctions.h"

// Send Xml back to web client
#ifndef NEW_WIFI
void SendXML() {
#else
void SendXML(AsyncWebServerRequest *request) {
  //char * xml_p = (char*)&XML[0];
  String XMLs;
  String tmp;
#endif
  char buf[64];
  char * buf_p = &buf[0];
  int timerValue=0;
  //loopCounter++;
  
  //Serial.print("1 sizeof XML ");Serial.println(sizeof(XML));
  sprintf(buf, "<?xml version =\"1.0\" encoding=\"UTF-8\"?><Data>");
  tmp=String(buf);
  XMLs += tmp;
 //Serial.print("1 XML length");Serial.println(strlen(XML));
  // send temp in F and C
  //if ( loopCounter >= 10)
  //{
    //String tempc = get_temp_string();
  //  loopCounter = 0;
  //}
  // Send Version info
  sprintf(buf, "<VERSION>%d.%02d.%02d</VERSION>", Release,Version,Revision);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Send Temp values in C and F
  sprintf(buf, "<TEMPC>%.2f</TEMPC>", tempC);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  sprintf(buf, "<TEMPF>%.2f</TEMPF>", tempF);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // if temp goes whacky send values
  if (tempC < 0 || tempC > 330)
  {
    sprintf(buf, "<adcValue>%d</adcValue>", adcValue);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
    sprintf(buf, "<voltage>%.2f</voltage>", voltageTemp);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
    sprintf(buf, "<Rt>%.2f</Rt>", Rt);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  //Serial.print("2 XML length");Serial.println(strlen(XML));
  // Temp readout preference
  sprintf(buf, "<TEMP_PREF>%d</TEMP_PREF>", TempPref);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // Temp sensor select
  sprintf(buf, "<TEMP_SENSOR>%d</TEMP_SENSOR>", tempSensorSelect);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // Debug setting
  sprintf(buf, "<DEBUG>%d</DEBUG>", DebugNum);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Temp probe type
  sprintf(buf, "<TEMP_PROBE>%d</TEMP_PROBE>", TempSensorKOhms);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Bean Quantity 
  sprintf(buf, "<BEAN_QTY>%d</BEAN_QTY>", BeanQuantity);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  
  // Send Timer 0 Minutes
  //if (TimerMin0 != TimerMin0New)
 // {
    TimerMin0 = TimerMin0New;
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    timerValue = TimerStartValue;
    sprintf(buf, "<TMIN0>%d</TMIN0>", TimerMin0);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
 // }
  // Send Timer 0 Seconds
  //if (TimerSec0 != TimerSec0New)
  //{
    TimerSec0 = TimerSec0New;
    TimerStartValue = TimerMin0 * 60 + TimerSec0;
    //TimerValue = TimerStartValue;
    sprintf(buf, "<TSEC0>%d</TSEC0>", TimerSec0);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
 // }

//Serial.print("3 XML length");Serial.println(strlen(XML));
  // Send Timer Remaining Time
  sprintf(buf, "<TIMEREM0>%s</TIMEREM0>", get_timer_string(TimerValue));
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // Timer start stop
  if (rState->timerStart) {
    sprintf(buf, "<TSTART>1</TSTART>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<TSTART>0</TSTART>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
//Serial.print("4 XML length");Serial.println(strlen(XML));
   // Send Config Timer Remaining Time
  sprintf(buf, "<CTIMEREM>%s</CTIMEREM>", get_timer_string(ConfigTimerValue));
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // Send Config Step
  sprintf(buf, "<C_STEP>%d</C_STEP>", ConfigStep);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

    // Send Config Number of Steps
  sprintf(buf, "<C_MAXSTEPS>%d</C_MAXSTEPS>", ConfigMaxSteps);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

    // Send heatgun selection low=0 high=1
  sprintf(buf, "<HEATGUN>%d</HEATGUN>", HEATGUNHIGH);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  
    // Send Config Step Time
  sprintf(buf, "<C_STEP_TIME>%d</C_STEP_TIME>", ConfigTimerStartValue);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // Send UtilTimer Timer 0 Minutes
  if (PreTimerMin != PreTimerMinNew)
  {
    PreTimerMin = PreTimerMinNew;
    PreheatTimerStartValue = PreTimerMin * 60 + PreTimerSec;
    PreheatTimerValue = PreheatTimerStartValue;
    //CooldownTimerValue = PreheatTimerStartValue;
  }
    sprintf(buf, "<PRE_TMIN0>%d</PRE_TMIN0>", PreTimerMin);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
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
    sprintf(buf, "<PRE_TSEC0>%d</PRE_TSEC0>", PreTimerSec);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;

   //  Send temp servo position
    sprintf(buf, "<SERVO_POS>%d</SERVO_POS>", servoPos);
    //strcat(XML, buf);
    tmp=String(buf);
     XMLs += tmp;
   //Serial.print("6 XML length");Serial.println(strlen(XML));  
   
   //  Send temp servo new position
    sprintf(buf, "<SERVO_POS_NEW>%d</SERVO_POS_NEW>", servoPosNew);
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
    //Serial.print("7 XML length");Serial.println(strlen(XML));
  
  // Send UtilTimer Timer Remaining Time
  sprintf(buf, "<PRE_TIMEREM0>%s</PRE_TIMEREM0>", get_timer_string(PreheatTimerValue));
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Send Finish Temp
  sprintf(buf, "<FINISH_TEMP>%d</FINISH_TEMP>", FinishTemp);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Send Preheat Temp
  sprintf(buf, "<PREHEAT_TEMP>%d</PREHEAT_TEMP>", PreheatTemp);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Send Coffee Selected
  sprintf(buf, "<Coffee_Type>%d</Coffee_Type>", CoffeeOpt);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;

  // Send Set Temp
  sprintf(buf, "<Set_Temp>%.2f</Set_Temp>", setTemp);
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
    // Timer start stop
  if (rState->preheatTimerStart) {
    sprintf(buf, "<PTSTART>1</PTSTART>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<PTSTART>0</PTSTART>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
//Serial.print("8 XML length");Serial.println(strlen(XML));
  // show mixer power enable led0 status
  if (rState->mixerpwr) {
    sprintf(buf, "<MIX>1</MIX>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<MIX>0</MIX>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }

  if (MIXDIR) {
    sprintf(buf, "<MIXDIR>1</MIXDIR>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<MIXDIR>0</MIXDIR>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  // send heater power enable status
  // False is ON for SSR
  if (!rState->heaterpwr) {
    sprintf(buf, "<HEATER>1</HEATER>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<HEATER>0</HEATER>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  //Serial.print("9 XML length");Serial.println(strlen(XML));
  // send roasting status
  if (rState->doRoast) {
    sprintf(buf, "<ROASTING>1</ROASTING>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  else {
    sprintf(buf, "<ROASTING>0</ROASTING>");
    //strcat(XML, buf);
    tmp=String(buf);
    XMLs += tmp;
  }
  
  //Serial.print("10 XML length");Serial.println(strlen(XML));
  sprintf(buf, "</Data>");
  //strcat(XML, buf);
  tmp=String(buf);
  XMLs += tmp;
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  //Serial.println("size of xml data");
  //Serial.println(strlen(XML));

  // you may have to play with this value, big pages need more processing time, and hence
  // a longer timeout that 200 ms
#ifndef NEW_WIFI
  Server.send(200, "text/xml", XML);
#else
  //Serial.printf("SendXML->sending xml length %d\n",XMLs.length());
  AsyncWebServerResponse *resp = request->beginResponse(200, "application/xml", XMLs);
  //resp->setContentType("application/xml");
  //resp->setContent(XMLs);
  resp->addHeader("Content-Length", String(XMLs.length()));
  request->send(resp);
#endif
  //Serial.print("After send XML length");Serial.println(strlen(XML));
  //XML[0]='\0';
}
