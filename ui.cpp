#include "ui.h"

int currentUiBackgroundIdx;

extern CSensors               sensors;

extern tgfBitmap              screen;
extern tgfTextOverlay         con;
extern tgfFont                font8;
extern tgfFont                font14seg16;
extern tgfFont                font7seg24;
extern tgfBitmap              bg1;
extern tgfBitmap              bg2;
extern tgfBitmap              weatherIcons;

String uiBackgrounds[16];

int switchComa( char *buf );
int uiBatteryIcon( tgfBitmap *bmp, short x, short y, float voltage );


int switchComa( char *buf )
{
  int i;
  
  for( i = 0; i < strlen( buf ); i++ )
  {
    if( buf[i] == '.' )
    {
      buf[i] = ',';
    }
  }
  
  return 0;
}

int uiBatteryIcon( tgfBitmap *bmp, short x, short y, float voltage )
{
  int     i;
  short   barY;
  ushort  barColor;
  
  //min voltage 3.5 - 1 bar
  //max voltage 4.3 - 5 bars 

  if( voltage > 4.3 )
  {
    voltage = 4.3;
  }

  voltage -= 3.5;
  
  if( voltage < 0 )
  {
    voltage = 0;
  }

  //max difference - 0.8V


  barColor = gfColor( 255, 255, 255 );

  for( i = 0; i < 6; i+= 1 )
  {
    
    barY = y + 20 - i * 5;
    
    if( i * ( 0.8 / 5 ) <= voltage )
    {
      gfFillRect( bmp, x + 1, barY + 1, x + 11, barY + 2, gfColor( 32, 32, 32 ) );
      gfFillRect( bmp, x, barY, x + 10, barY + 1, barColor );
    }
    else
    {
      break;
    }
  }

  return 0;
}


int uiInit()
{
  uiBackgrounds[0] = "/clearday.gbm";
  uiBackgrounds[1] = "/cloudyday.gbm";
  uiBackgrounds[2] = "/rainyday.gbm";

  uiBackgrounds[8] = "/clearnight.gbm";
  uiBackgrounds[9] = "/cloudynight.gbm";
  uiBackgrounds[10] = "/rainynight.gbm";

  currentUiBackgroundIdx = 0;  //clearday loaded by default
  return 0;
}

int uiMainScreen()
{
  char       buf[256];
  time_t     currentTime;
  struct tm  timeinfo;
  short      x;
  short      y;
  int        i;
  short      shadowColor;
  int        newUiBackgroundIdx;

  gfBlitBitmap( &screen, &bg1, 0, 0 );

  shadowColor = gfColor( 32, 32, 32 );
  

  currentTime = DateTime.now();
  localtime_r( &currentTime, &timeinfo);

  strftime( buf, sizeof( buf ) - 1, "%H:%M:%S", &timeinfo );


  x = 300;
  y = 15;

  font7seg24.charColor = shadowColor;
  gfPutString( &screen, &font7seg24, x + 2, y + 2, buf );
  gfPutString( &screen, &font7seg24, x - 2, y - 2, buf );

  font7seg24.charColor = gfColor( 255, 255, 255 );
  gfPutString( &screen, &font7seg24, x, y, buf );


  strftime( buf, sizeof( buf ) - 1, "%d:%m:%Y", &timeinfo );

  x = 322;
  y = 65;

  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  font14seg16.charColor = gfColor( 255, 255, 255 );
  gfPutString( &screen, &font14seg16, x, y, buf );


  //weather forecast

  //forecaset timeout - 60 minutes
  if( !sensors.checkDataTimeout( &sensors.sqlDateTime[0], 60 * 60 ) )
  {
    
  
    //base icon - sun, day
    x = 0;
    y = 200;

    newUiBackgroundIdx = 0;

    if( sensors.owIcon == "02d" )
    {
      newUiBackgroundIdx = 0;

      //few clouds
      x = 200;
      y = 100;
    }
    if( sensors.owIcon == "03d" )
    {
      newUiBackgroundIdx = 0;

      //scattered clouds
      x = 100;
      y = 100;
    }
    if( sensors.owIcon == "04d" )
    {
      newUiBackgroundIdx = 1;

      //broken clouds
      x = 0;
      y = 100;
    }
    if( sensors.owIcon == "09d" )
    {
      newUiBackgroundIdx = 2;
      
      //shower rain
      x = 100;
      y = 400;
    }
    if( sensors.owIcon == "10d" )
    {
      newUiBackgroundIdx = 2;

      //rain
      x = 100;
      y = 400;
    }
    if( sensors.owIcon == "11d" )
    {
      newUiBackgroundIdx = 2;

      //thunderstorm
      x = 100;
      y = 200;
    }
    if( sensors.owIcon == "13d" )
    {
      newUiBackgroundIdx = 2;

      //snow
      x = 300;
      y = 100;
    }
    if( sensors.owIcon == "50d" )
    {
      newUiBackgroundIdx = 2;

      //fog
      x = 300;
      y = 200;
    }

    if( sensors.owIcon == "01n" )
    {
      newUiBackgroundIdx = 8;

      x = 0;
      y = 300;
    }
    if( sensors.owIcon == "02n" )
    {
      newUiBackgroundIdx = 8;

      x = 200;
      y = 200;
    }
    if( sensors.owIcon == "03n" )
    {
      newUiBackgroundIdx = 9;

      x = 300;
      y = 400;
    }
    if( sensors.owIcon == "04n" )
    {
      newUiBackgroundIdx = 9;

      x = 300;
      y = 400;
    }
    if( sensors.owIcon == "09n" )
    {
      newUiBackgroundIdx = 10;

      x = 200;
      y = 400;
    }
    if( sensors.owIcon == "10n" )
    {
      newUiBackgroundIdx = 10;

      x = 0;
      y = 400;
    }
    if( sensors.owIcon == "11n" )
    {
      newUiBackgroundIdx = 10;

      x = 100;
      y = 0;
    }
    if( sensors.owIcon == "13n" )
    {
      newUiBackgroundIdx = 10;

      x = 0;
      y = 0;
    }

    if( currentUiBackgroundIdx != newUiBackgroundIdx )
    {
      if( bg1.buffer != NULL )
      {
        osFree( (void*)bg1.buffer );
        bg1.buffer = NULL;
      }

      currentUiBackgroundIdx = newUiBackgroundIdx;

      gfLoadBitmapFS( &bg1, (char*)uiBackgrounds[currentUiBackgroundIdx].c_str() );

    }

    gfBlitBitmapRect( &screen, &weatherIcons, 20, 20, x, y, 100, 100 );


    sprintf( buf, "%.0f C", sensors.owCurTemperature );

    x = ( 20 + 50 ) - ( ( strlen( buf ) * 24 ) / 2 );
    y = 125;


    font7seg24.charColor = shadowColor;
    gfPutString( &screen, &font7seg24, x + 2, y + 2, buf );
    gfPutString( &screen, &font7seg24, x - 2, y - 2, buf );

    font7seg24.charColor = gfColor( 255, 255, 255 );
    gfPutString( &screen, &font7seg24, x, y, buf );
   
    sprintf( buf, "%.0f hPa", sensors.owCurPressure );

    x = ( 20 + 50 ) - ( ( strlen( buf ) * 16 ) / 2 );
    y = 170;

    font14seg16.charColor = shadowColor;
    gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
    gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

    font14seg16.charColor = gfColor( 255, 255, 255 );
    gfPutString( &screen, &font14seg16, x, y, buf );

  }
  


  x = 140;
  y = 110;

  uiBatteryIcon( &screen, x, y, sensors.sqlVoltage[0] );

  x += 20;


  if( sensors.checkDataTimeout( &sensors.sqlDateTime[0], 60 * 60 ) )
  {
    sprintf( buf, "IN:   NO DATA" );
  }
  else
  {
    sprintf( buf, "IN:   %.1f C  %.0f %%  %.0f L", sensors.sqlTemperature[0], sensors.sqlHumidity[0], sensors.sqlLux[0] );
    switchComa( buf );
  }
  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  //sensor timeout - 10 minutes
  if( sensors.checkDataTimeout( &sensors.sqlDateTime[0], 60 * 10 ) )
  {
    font14seg16.charColor = gfColor( 255, 255, 0 );
  }
  else
  {  
    font14seg16.charColor = gfColor( 255, 255, 255 );
  }

  gfPutString( &screen, &font14seg16, x, y, buf );

/*  x = 140;
  y = 150;

  uiBatteryIcon( &screen, x, y, sensors.sqlVoltage[2] );

  x += 20;
  
  if( sensors.checkDataTimeout( &sensors.sqlDateTime[2], 60 * 60 ) )
  {
    sprintf( buf, "IN2:  NO DATA" );
  }
  else
  {
    sprintf( buf, "IN2:  %.1f C  %.0f %%", sensors.sqlTemperature[2], sensors.sqlHumidity[2] );
    switchComa( buf );
  }
  
  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  //sensor timeout - 10 minutes
  if( sensors.checkDataTimeout( &sensors.sqlDateTime[2], 60 * 10 ) )
  {
    font14seg16.charColor = gfColor( 255, 255, 0 );
  }
  else
  {  
    font14seg16.charColor = gfColor( 255, 255, 255 );
  }
  
  gfPutString( &screen, &font14seg16, x, y, buf );
*/
  x = 140;
  y = 150;
//  y = 190;

  uiBatteryIcon( &screen, x, y, sensors.sqlVoltage[1] );

  x += 20;
  
  if( sensors.checkDataTimeout( &sensors.sqlDateTime[1], 60 * 60 ) )
  {
    sprintf( buf, "OUT:  NO DATA" );
  }
  else
  {
    sprintf( buf, "OUT:  %.1f C  %.0f %%  %.0f L", sensors.sqlTemperature[1], sensors.sqlHumidity[1], sensors.sqlLux[1] );
    switchComa( buf );
  }
  
  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  //sensor timeout - 10 minutes
  if( sensors.checkDataTimeout( &sensors.sqlDateTime[1], 60 * 10 ) )
  {
    font14seg16.charColor = gfColor( 255, 255, 0 );
  }
  else
  {  
    font14seg16.charColor = gfColor( 255, 255, 255 );
  }
  gfPutString( &screen, &font14seg16, x, y, buf );

  x = 160;
  y = 230;
  
  sprintf( buf, "%.2f uSV/h", sensors.sqlRadiation );

  switchComa( buf );
  
  
  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  //sensor timeout - 10 minutes
  if( sensors.checkDataTimeout( &sensors.sqlRadiationDateTime, 60 * 10 ) )
  {
    font14seg16.charColor = gfColor( 255, 255, 0 );
  }
  else
  {  
    font14seg16.charColor = gfColor( 255, 255, 255 );
  }

  //radiation over the usual value warning has precedence over sensor timeout
  if( sensors.sqlRadiation >= 0.5 )
  {
    font14seg16.charColor = gfColor( 255, 0, 0 );
  }
  

  gfPutString( &screen, &font14seg16, x, y, buf );

  
  
  
  sensors.owWeatherDescription.toCharArray( buf, sizeof( buf ) - 1 );

  x = 240 - ( ( strlen( buf ) * 16 ) / 2 );
  y = 295;
  
  font14seg16.charColor = shadowColor;
  gfPutString( &screen, &font14seg16, x + 2, y + 2, buf );
  gfPutString( &screen, &font14seg16, x - 2, y - 2, buf );

  font14seg16.charColor = gfColor( 255, 255, 255 );
  gfPutString( &screen, &font14seg16, x, y, buf );


  return 0;
}
