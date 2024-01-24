//IDE config
//Board: ESP32 Dev module
//flash size: 16MB
//psram: enabled
//partition scheme: 16m flash ( 2mb app, 12.5mb fatfs )
//FATFS upload (not used)
//https://github.com/lorol/arduino-esp32fs-plugin
//https://www.mischianti.org/2021/04/06/esp32-integrated-ffat-fat-exfat-filesystem-6/
//#include "FFat.h"

//https://techtutorialsx.com/2017/05/09/esp32-running-code-on-a-specific-core/

//https://minecraft.fandom.com/wiki/List_of_block_textures
//https://www.freertos.org/implementation/a00008.html
//https://deepbluembedded.com/esp32-change-cpu-speed-clock-frequency/
//https://www.reddit.com/r/esp32/comments/f529hf/results_comparing_the_speeds_of_different_gpio/

//esp32 module
//https://pl.aliexpress.com/item/32977375539.html?spm=a2g0o.store_pc_allProduct.8148356.68.400917764XdMZl&pdp_npi=3%40dis%21PLN%2142%2C65%20z%C5%82%2142%2C65%20z%C5%82%21%21%21%21%21%40211b88ec16841705237218187e6ada%2112000031557142204%21sh%21PL%21745629082

//CAUTION: D1 diode in display has to be removed, pads soldered together https://github.com/Bodmer/TFT_eSPI/discussions/898


#define NDEBUG

#include <WiFi.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S_kbv.h"

#include "ESPDateTime.h"

#include "FS.h"
#include "SD.h"

#include <qbkGf.h>

#include <JSON_Decoder.h>
#include <OpenWeather.h>

#include "CSensors.h"

#include "ui.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

TaskHandle_t taskGetForecastHandle;


CSensors  sensors;
volatile ushort taskGetForecastFinished;

/*#include "qbkGf/osAlloc.h"
#include "qbkGf/osFile.h"

#include "qbkGf/gfTypes.h"
#include "qbkGf/gfBitmap.h"
#include "qbkGf/gfDrawing.h"
#include "qbkGf/gfFont.h"
#include "qbkGf/gfGouraud.h"
*/

// These are 'flexible' lines that can be changed
#define TFT_CS 25 //10
#define TFT_DC 26 //9
#define TFT_RST 35 //8 // RST can be set to -1 if you tie it to Arduino's reset
//TFT_MOSI - 23
//TFT_MISO - 19
//TFT_SCLK - 18

Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, TFT_RST);





tgfBitmap              screen;
tgfTextOverlay         con;
tgfFont                font8;
tgfFont                font14seg16;
tgfFont                font7seg24;
tgfBitmap              bg1;
tgfBitmap              bg2;
tgfBitmap              weatherIcons;

int IRAM_ATTR redrawLCD( tgfBitmap *bmp )
{
  int i;
  uint32_t *fb;
  uint32_t pixel;
  
//  SPI.beginTransaction( SPISettings( 40000000, MSBFIRST, SPI_MODE0 ) );

  tft.startWrite();  
  tft.setAddrWindow( 0, 0, 480, 320 );

  fb = (uint32_t *)bmp->buffer;


  for( i = 0; i < 76800; i++ )
  {
      pixel = *fb++;
      
      SPI.write32( ( ( pixel & 0xffff0000 ) >> 16 ) | ( ( pixel & 0xffff ) << 16 ) );
  }


  tft.endWrite();
   
 // SPI.endTransaction();
  
  return 0;
}

void taskGetForecast( void *param )
{
  
  vTaskDelay( 1/ portTICK_RATE_MS );
  
  
  sensors.owGetData();
  sensors.sqlGetData();

  taskGetForecastFinished = 1; 

  vTaskDelete( NULL );

}

void setup() 
{
  int                    i;
  tosFile                in;
  void                  *psRam; 
  int                    rv;
  char                   buf[256];
  unsigned short int    *screenFrameBuffer;  
  
  Serial.begin( 115200 );

  SD.begin();

  pinMode( TFT_CS, OUTPUT );

  tft.begin( 40000000 );
  
  tft.setRotation( 3 );

  Serial.printf( "PSRAM size: %d\n", ESP.getFreePsram() );

  Serial.printf( "osAlloc init: " );  
  psRam = ps_malloc( 1048576 * 3.92 );

  if( !psRam )
  {
    Serial.printf("PSram alloc failed!\n");
    do{}while( 1 );
    
  }
  else
  {
    Serial.printf("PSram alloc ok\n");    
  }
  osAllocInit( psRam, 1048576 * 3.95 );
  

  //Alloc and init screen bitmaps  
  screenFrameBuffer = ( unsigned short int *)osAlloc( 480*320*2 );
  if( !screenFrameBuffer )
  {
    Serial.printf( "ERROR: Can't alloc screen frame buffer.\n" );
    do{}while( 1 );
    
  }

  screen.width              = 480;
  screen.height             = 320;
  screen.flags              = NULL;
  screen.transparentColor   = 0;
  screen.buffer             = screenFrameBuffer;

  //clear screen
  gfFillRect( &screen, 0, 0, screen.width - 1, screen.height - 1, gfColor( 128, 128, 128 ) );
  redrawLCD( &screen );

  
  //fixed width raw font
  font8.type              = GF_FONT_TYPE_MONO_BITMAP_FIXED_WIDTH;
  font8.width             = 8;
  font8.height            = 8;
  font8.charColor         = gfColor( 0xff, 0xff, 0xff );
  font8.backgroundColor   = gfColor( 0x0, 0x0, 0x0 );
  font8.firstChar         = 0x0;
  font8.lastChar          = 0xff;
  font8.charWidths        = NULL;
  font8.charBuffer        = osAlloc( 2048 );

  if ( !osFOpen( &in, "/font8x8atari.fnt", OS_FILE_READ ) )
  {
    ulong nbr;
    
    osFRead( &in, (uint8_t*)font8.charBuffer, 2048, &nbr );
    osFClose( &in );
    
  }else{
    
    Serial.printf( "ERROR: Can't load font8x8atari.fnt\n" );

    gfFillRect( &screen, 0, 0, screen.width - 1, screen.height - 1, gfColor( 255, 0, 0 ) );
    redrawLCD( &screen );

    delay ( 1000 );
    ESP.restart();
    
  }

  //init console
  con.type      = GF_TEXT_OVERLAY_TYPE_SOFTWARE;
  con.flags     = 0;
  con.width     = screen.width / font8.width;
  con.height    = screen.height / font8.height;
  con.font      = &font8;
  con.textBuffer  = (uchar*)osAlloc( con.width * con.height * 2 );


  con.textAttributes = 0x0f;  //7f transparent bg
  con.flags |= GF_TEXT_OVERLAY_FLAG_SHOW_CURSOR;

  gfFillRect( &screen, 0, 0, screen.width - 1, screen.height - 1, gfColor( 0, 0, 0 ) );
  toCls( &con );

  toPrint( &con, (char*)"\nSensorsBaseST7796S WebGate B20231106\n\n" );
  toPrint( &con, (char*)"Loading assets: " );

  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );

  rv = 0;

  digitalWrite( TFT_CS, 1 );
  
  rv |= gfLoadFontFS( &font14seg16, (char*)"/14seg_16.gfn" );
  rv |= gfLoadFontFS( &font7seg24, (char*)"/7seg_24.gfn" );
  rv |= gfLoadBitmapFS( &weatherIcons, (char*)"/weaticon.gbm" );

  rv |= gfLoadBitmapFS( &bg1, (char*)"/clearday.gbm" );
//  rv |= gfLoadBitmapFS( &bg1, (char*)"/clearnight.gbm" );
//  rv |= gfLoadBitmapFS( &bg1, (char*)"/clearnight2.gbm" );
//  rv |= gfLoadBitmapFS( &bg1, (char*)"/cloudyday.gbm" );

  if( !rv )
  {
    toPrint( &con, (char*)"ok\n" );    
  }
  else
  {
    toPrint( &con, (char*)"error\n" );    
  }

  weatherIcons.flags            |= GF_BITMAP_FLAG_TRANSPARENT;
  weatherIcons.transparentColor  = gfColor( 0, 0, 0 );

  font7seg24.flags              |= GF_FONT_FLAG_TRANSPARENT;
  font14seg16.flags             |= GF_FONT_FLAG_TRANSPARENT;

  
  toPrint( &con, (char*)"Connecting to WiFi:" );
    
  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );

  WiFi.begin( "ssid" , "password" );


  i = 0;
  
  while( WiFi.status() != WL_CONNECTED ) 
  {
    delay( 100 );

    i++;

    if( i > 10 * 60 * 2 )
    {

        ESP.restart();

    }
  }


  toPrint( &con, (char*)"ok\nGetting time:" );
  
  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );


  DateTime.setServer( "ntp.icm.edu.pl" );
  DateTime.setTimeZone( "CET-1CEST,M3.5.0,M10.5.0/3" );
  DateTime.begin();
  
  if( !DateTime.isTimeValid() ) 
  {
    toPrint( &con, (char*)"error\n");
  } 
  else 
  {
    sprintf( buf, "%s\n", DateTime.toString().c_str() );
    toPrint( &con, buf );
  }

  toPrint( &con, (char*)"Fetching first forecast:" );

  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );


  sensors.owGetData();
  taskGetForecastFinished = 1;
  
  toPrint( &con, (char*)"done\n" );


  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );


  toPrint( &con, (char*)"Fetching first sql data:" );

  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );

  sensors.sqlGetData();

  toPrint( &con, (char*)"done\n" );

  gfDrawTextOverlay( &screen, &con, 0, 0 );
  redrawLCD( &screen );

  uiInit();

  delay( 1000 );
}



void loop( void ) 
{
  static int  wifiDisconnectTimer = 0;
  
  uiMainScreen();

  redrawLCD( &screen );

  if(( ( sensors.owReadAttemptMillis + 1000 * 60 * 5 ) < millis() ) && ( taskGetForecastFinished ) )
  {
    Serial.printf( "Getting forecast\n" );
    taskGetForecastFinished = 0;
    xTaskCreate( taskGetForecast, "taskGetForecast", 40000, NULL, 10, &taskGetForecastHandle );
  
  }
  
  delay( 100 );

  if( WiFi.status() != WL_CONNECTED )
  {
    wifiDisconnectTimer++;
  }
  else
  {
    wifiDisconnectTimer = 0;
  }


  if( wifiDisconnectTimer >= 10 * 60 * 5 )
  {
    ESP.restart();
  }
  

}
