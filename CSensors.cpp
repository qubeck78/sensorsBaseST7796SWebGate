
#include "CSensors.h"



#include <WiFi.h>

//IPAddress sqlServerAddr(192,168,10,20);  // IP of the MySQL *server* here

HTTPClient http;

CSensors::CSensors()
{

  owAPIKey = "329afbf1a5ad93e66bd546b779badbd6";
  owLat    = "50.005504";
  owLon    = "18.948541";
}

int CSensors::owGetData()
{
  owReadAttemptMillis = millis();

  if( ow.getForecast( &owForecast, owAPIKey, owLat, owLon, "metric" , "en" ) )
  {
    owReadMillis          = millis();
    owCurTemperature      = owForecast.temp[0];
    owCurPressure         = owForecast.pressure[0];
    owIcon                = owForecast.icon[0];
    owWeatherDescription  = owForecast.description[0]; //main[0] description[0];  //city_name :)
    
    return 0;    
  }
  else
  {
    return 1;
  }  
}

int CSensors::sqlGetData()
{
  int            rv;
  char           request[256];
  int            sensorId;
  int            i;
  int            responseCode;
  float          cpm;

  String         responseStr;
  String         values[6];
  int            tokenIdx;
  int            valueIdx;
 
  rv = 0;

  sqlReadAttemptMillis = millis();


  //read sensors
  for( sensorId = 1; sensorId < 4; sensorId++ )
  {
    sprintf( request, "http://192.168.10.20/ih/sensorsSelect.php?sensor_id=%d", sensorId );

    http.begin( request );
    responseCode = http.GET();
    if( responseCode > 0 )
    {
      responseStr = http.getString();

      for( valueIdx = 0; valueIdx < 6; valueIdx++ )
      {
        if( responseStr.length() > 0 )
        {
          tokenIdx = responseStr.indexOf( ';' );
          
          if( tokenIdx != -1 )
          {
            
            values[valueIdx] = responseStr.substring( 0, tokenIdx );
            responseStr = responseStr.substring( tokenIdx + 1 );
          }
          else
          {
            values[valueIdx] = responseStr;
          }

          //Serial.printf( "|%s|\n", values[valueIdx].c_str() );
        }
      }

      strptime( values[1].c_str(), "%Y-%m-%d %H:%M:%S", &sqlDateTime[sensorId - 1] );
        
      sqlTemperature[sensorId - 1]  = atof( values[2].c_str() );
      sqlHumidity[sensorId - 1]     = atof( values[3].c_str() );
      sqlLux[sensorId - 1]          = atof( values[4].c_str() );
      sqlVoltage[sensorId - 1]      = atof( values[5].c_str() );

      sqlReadMillis = millis();

    }
    else
    {
      rv |= 1;
    }
    
    http.end();

  }

  //read radiation
  strcpy( request, "http://192.168.10.20/ih/geigerSelect.php" );

  http.begin( request );
  responseCode = http.GET();

  if( responseCode > 0 )
  {
    responseStr = http.getString();

    for( valueIdx = 0; valueIdx < 3; valueIdx++ )
    {
      if( responseStr.length() > 0 )
      {
        tokenIdx = responseStr.indexOf( ';' );
          
        if( tokenIdx != -1 )
        {
            
          values[valueIdx] = responseStr.substring( 0, tokenIdx );
          responseStr = responseStr.substring( tokenIdx + 1 );
        }
        else
        {
          values[valueIdx] = responseStr;
        }

        //Serial.printf( "|%s|\n", values[valueIdx].c_str() );
      }
    }

    strptime( values[1].c_str(), "%Y-%m-%d %H:%M:%S", &sqlRadiationDateTime );
        
    cpm           = atof( values[2].c_str() );
    sqlRadiation  = cpm *0.00812;
    sqlReadMillis = millis(); 

  }else
  {
    rv |= 1;
  }
  
  http.end();
  return rv;
}

int CSensors::checkDataTimeout( struct tm *dataTime, time_t secondsToTimeout )
{
  int     rv;
  time_t  dataTimeT;
  time_t  currentTimeT;
  
  dataTimeT     = mktime( dataTime );
  currentTimeT  = DateTime.now();

  if( currentTimeT > dataTimeT + secondsToTimeout )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
