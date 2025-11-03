//Header file for roaster controls functions
//void IRAM_ATTR onRoastTimer();
void onRoastTimerT(int *val);
void SetupRoastTimerT();
void ClearRoastTimerT();
void UpdateRoastState(void *);
void readConfigData();
void SetupRoastingLog(bool onlyPreheat=false);
void CloseRoastingLog(bool manualStop=false);
//void SetupRoastTimer();
//void ClearRoastTimer();
#ifndef NEW_WIFI
void ProcessButtonRoastStart();
void ProcessMinButton_0();
void ProcessSecButton_0();
void ProcessPreMinButton_0();
void ProcessPreSecButton_0();
void ProcessPreTempButton();
void ProcessFinalTempButton();
void ProcessBeanQuantityButton();
void ProcessCoffeeType();
void ProcessRoastSettingsButton();
void UpdateSlider();
void ProcessButtonMixPwr();
void ProcessButtonMixDir();
void ProcessButtonHeaterPwr();
void ProcessButtonTimerStart();
void ProcessButtonTimerAdd();
void ProcessButtonTimerSub();
void ProcessPreheatTimerStart();

String SetRoastFilename();
void DownloadConfigData();
void DownloadLogData();
#else
void ProcessButtonRoastStart(AsyncWebServerRequest *request);
void ProcessMinButton_0(AsyncWebServerRequest *request);
void ProcessSecButton_0(AsyncWebServerRequest *request);
void ProcessPreMinButton_0(AsyncWebServerRequest *request);
void ProcessPreSecButton_0(AsyncWebServerRequest *request);
void ProcessPreTempButton(AsyncWebServerRequest *request);
void ProcessFinalTempButton(AsyncWebServerRequest *request);
void ProcessBeanQuantityButton(AsyncWebServerRequest *request);
void ProcessCoffeeType(AsyncWebServerRequest *request);
void ProcessRoastSettingsButton(AsyncWebServerRequest *request);
void UpdateSlider(AsyncWebServerRequest *request);
void ProcessButtonMixPwr(AsyncWebServerRequest *request=NULL);
void ProcessButtonMixDir(AsyncWebServerRequest *request);
void ProcessButtonHeaterPwr(AsyncWebServerRequest *request=NULL);
void ProcessButtonTimerStart(AsyncWebServerRequest *request=NULL);
void ProcessButtonTimerAdd(AsyncWebServerRequest *request);
void ProcessButtonTimerSub(AsyncWebServerRequest *request);
void ProcessPreheatTimerStart(AsyncWebServerRequest *request=NULL);
void ProcessStartPreHeat(AsyncWebServerRequest *request);
#endif
//void IRAM_ATTR onPreHeatTimer();
