 
                                       
#include <SmartMatrix3.h>
#include "colorwheel.c"
#include "gimpbitmap.h"

#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24

const uint8_t kMatrixWidth = 64; // known working: 32, 64, 96, 128
//const uint8_t kMatrixHeight = 16;       // known working: 16, 32, 48, 64
const uint8_t kMatrixHeight = 32;  

const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
//const uint8_t kPanelType = SMARTMATRIX_HUB75_16ROW_MOD8SCAN;
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

//const int defaultBrightness = 17*(255/100);    // full brightness
const int defaultBrightness = 45*(255/100);    // dim: 15% brightness
const int defaultScrollOffset = 8;
//const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {0x40, 0, 0};

const int ledPin = 13;

void drawBitmap(int16_t x, int16_t y, const gimp32x32bitmap* bitmap) {
  for(unsigned int i=0; i < bitmap->height; i++) {
    for(unsigned int j=0; j < bitmap->width; j++) {
      rgb24 pixel = { bitmap->pixel_data[(i*bitmap->width + j)*3 + 0],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 1],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 2] };

      backgroundLayer.drawPixel(x + j, y + i, pixel);
    }
  }
}

int touchPin = 1;
int touchReading = 0;
boolean isStarted = false;

void setup() {

  pinMode(12, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(38400);

  matrix.addLayer(&backgroundLayer); 
  matrix.addLayer(&scrollingLayer); 
  matrix.addLayer(&indexedLayer); 
  matrix.begin();

  matrix.setBrightness(defaultBrightness);

  scrollingLayer.setOffsetFromTop(defaultScrollOffset);

  backgroundLayer.enableColorCorrection(true);
}

/*
#define DEMO_DRAWING_PIXELS   0

#define DEMO_FILL_SCREEN       0

#define DEMO_MONO_BITMAP        0

#define DEMO_BRIGHTNESS        0

#define DEMO_INDEXED_LAYER     0
*/

void loop() {
    int i, j;
    unsigned long currentMillis;
    touchReading = touchRead(touchPin);  
    Serial.println(touchReading); 

 if (touchReading < 2000) {
//if (digitalRead(12) == HIGH) {
        scrollingLayer.setColor({0xff, 0xff, 0xff});
        scrollingLayer.setMode(wrapForward);
        scrollingLayer.setSpeed(19);
        scrollingLayer.setFont(font8x13);
        
        scrollingLayer.start(" Fortunes Told ", 1);
       // scrollingLayer.start( "Past Present Future",1);

       // const uint transitionTime = 3000;
        const uint transitionTime = 10000;
       
        backgroundLayer.fillScreen({0, 0, 0});
       // backgroundLayer.swapBuffers();
        indexedLayer.fillScreen(0);
        indexedLayer.swapBuffers();

        currentMillis = millis();

        while (millis() - currentMillis < transitionTime && touchReading < 2000) {
            touchReading = touchRead(touchPin);
            int x0, y0;

            rgb24 color;
            float fraction = ((float)millis() - currentMillis) / ((float)transitionTime);

            (millis() - currentMillis < transitionTime / 3) ;
                color.red = 127.0 - 255.0 * fraction * random(7);
              
                color.blue = 127.0 * fraction * random(7);
                color.green = 0;
            

            for (i = 0; i < 3; i++) {
                x0 = random(matrix.getScreenWidth());
                y0 = random(matrix.getScreenHeight());

                backgroundLayer.drawPixel(x0, y0, color);
                // delay(77);


                
                    for (i = 0; i < 12; i++) {
                          x0 = random(matrix.getScreenWidth());
                           y0 = random(matrix.getScreenHeight());

                           color.red =0;
                           color.blue = 0;
                           color.green = 0;
                           backgroundLayer.drawPixel(x0, y0, color);
                
                           delay(17);
                    }
            backgroundLayer.swapBuffers();
            }
        }
    }

   else {
 
        const uint transitionTime = 19000;
        //const uint transitionTime = 41000;

        const int testBitmapWidth = 13;
        const int testBitmapHeight = 13;
       


         uint8_t testBitmap[] = {
            
     
            ___XXXXX, XXX_____,
            ___XXXXX, XXX_____,
            ___XX___, XXX_____,
            ___XX___, XXX_____,
            ___XXX__, XXX_____,
            _______X, X_______,
            _______X, X_______,
            _______X, X_______,
            _______X, X_______,
            ________, ________,
            _______X, X_______,         
            _______X, X_______,
            ________, ________,
           
           
      
            
           
          
           
        };


       

        int pattern = random(0,27);
      
        currentMillis = millis();

        scrollingLayer.start("                      ", 1);
        scrollingLayer.setSpeed(37);

        backgroundLayer.fillScreen({0, 0, 0});
        backgroundLayer.swapBuffers();

        while (millis() < currentMillis + transitionTime/2) {
            backgroundLayer.drawMonoBitmap(random(matrix.getScreenWidth() + testBitmapWidth) - testBitmapWidth/2,
                                  random(matrix.getScreenHeight() + testBitmapHeight) - testBitmapHeight/2,
                                  testBitmapWidth, testBitmapHeight, {(uint8_t)random(256), (uint8_t)random(128), (uint8_t)random(256)}, testBitmap);
            backgroundLayer.swapBuffers();
     
      
            delay(73);
        }

         
    
        switch (pattern){

            case(0):
         
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
         
            scrollingLayer.start("A childhood friend will appear", 1);
            break;

            case(1):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
        
            scrollingLayer.start("You are trapped in the everdayNESS", 1);

            case(2):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
         
            scrollingLayer.start("You will be told three lies", 1);
            break;
            
            case(3):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
     
            scrollingLayer.start("At a crossroads, you will find a small object", 1);
            break;

            case(4):

          
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
         
            scrollingLayer.start("A dream of water will lead you to the truth", 1);
            break;

            case(5):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
      
            scrollingLayer.start("a winding path through the city, walk slowly", 1);
            break;

            case(6):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
           
            scrollingLayer.start("You are taking the fastest route", 1);
            break;

            case(7):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
          
            scrollingLayer.start("Nothing can compare to your experience", 1);
            break;

            case(8):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
           
            scrollingLayer.start("Do  Not  Fear Self Improvement", 1);
            break;

            case(9):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
     
            
            scrollingLayer.start("???????  $$$$$$$   !!!!!!!", 1);
            break;

            case(10):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();

            scrollingLayer.start("Bravado will stand in for bravery", 1);
            break;

            case(11):

                     
           
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
           
            scrollingLayer.start("Easy money is coming your way (watch out!)", 1);
            break;

            case(12):  
            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
          
            scrollingLayer.start("a child has message for you", 1);
            break;

            case(13):

            
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
         
            scrollingLayer.start("gather 5 stones,sleep with them under your pillow", 1);
            break;

            case(14):

            
            //scrollingLayer.start("         ", 1);
            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("beware of a man with a three legged dog", 1);
            break;

            case(15):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("You are the past present future", 1);
            break;

            case(16):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("the leader has not yet arrived", 1);
            break;

            
            case(17):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("Take a different approach", 1);
            break;

            
            case(18):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("this is the last responsible moment", 1);
            break;

            
            case(19):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("you have arrived", 1);
            break;

                
            case(20):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("a lost item will soon be found", 1);
            break;

                
            case(21):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("get there early get there first", 1);
            break;

                
            case(22):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("we are here to go!", 1);
            break;

                
            case(23):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("there is a chance of rain", 1);
            break;


            case(24):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("have your pockets ready", 1);
            break;

            case(25):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("wait for the slow burn", 1);
            break;

            case(26):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("you will get what you negotiate", 1);
            break;

            case(27):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("your password will expire soon", 1);
            break;

            case(28):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("the space is the event", 1);
            break;

            case(29):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("you are blinded by the brightness", 1);
            break;

            case(30):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("flattery will go far tonight", 1);
            break;

            case(31):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("someone has Googled you recently", 1);
            break;

            case(32):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("the fortune you seek is elsewhere", 1);
            break;

            case(33):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("your hand will help another", 1);
            break;

            case(34):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("listen to the trees, they have the answer", 1);
            break;

            case(35):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("a woman with red hair will tell you a secret", 1);
            break;

            case(36):

            backgroundLayer.fillScreen({0,0,0});
            backgroundLayer.setBrightness(17);
            backgroundLayer.swapBuffers();
            scrollingLayer.start("step nimbly over the river before it engulfs you", 1);
            break;

            
        }
    
        scrollingLayer.setColor({0xff, 0x00, 0xff});
        scrollingLayer.setMode(wrapForward);
       // scrollingLayer.setSpeed(13);
        scrollingLayer.setSpeed(25);
        scrollingLayer.setFont(font8x13);
      
        indexedLayer.swapBuffers();

        delay(177);
   

        currentMillis = millis();

        while (millis() - currentMillis < transitionTime) {
            int x0, y0;
            bool opaque;

            for (i = 0; i < 7; i++) {
                x0 = random(matrix.getScreenWidth());
                y0 = random(matrix.getScreenHeight());
                 opaque = random(0,2);
                indexedLayer.drawPixel(x0, y0, opaque);
                
                //delay(7);
                delay(47);
            }

            x0 = random(matrix.getScreenWidth());
            y0 = random(matrix.getScreenHeight());

            //indexedLayer.drawMonoBitmap(x0, y0, testBitmapWidth, testBitmapHeight, 1, testBitmap_alt);

            indexedLayer.swapBuffers();
            delay(3);

   }
  }
}   
    
    


