#ifndef _CSENSORS_H
#define _CSENSORS_H

#include <Arduino.h>
#include <time.h>

#include <JSON_Decoder.h>
#include <OpenWeather.h>
#include <HTTPClient.h>


#include "ESPDateTime.h"


class CSensors
{
  private:

  OW_Weather             ow;
  OW_forecast            owForecast;

  public:

  String                 owAPIKey;
  String                 owLat;
  String                 owLon;

  CSensors();

  unsigned long   owReadAttemptMillis;
  unsigned long   owReadMillis;
  volatile float  owCurTemperature;
  volatile float  owCurPressure;
  String          owIcon;
  String          owWeatherDescription;
  
  unsigned long   sqlReadAttemptMillis;
  unsigned long   sqlReadMillis;
  
  float           sqlTemperature[4];
  float           sqlHumidity[4];
  float           sqlLux[4];
  float           sqlVoltage[4];
  struct tm       sqlDateTime[4];

  float           sqlRadiation;
  struct tm       sqlRadiationDateTime;
  
  int owGetData();
  int sqlGetData();

  int checkDataTimeout( struct tm *dataTime, time_t secondsToTimeout );
  
};

#endif
