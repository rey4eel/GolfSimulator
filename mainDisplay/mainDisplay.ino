#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include "MessageService.h"
#include "Message.h"
 

#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define VIOLET      0xEC1D 
#define GRAY        0x8410 


// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=6, XM=A2, YP=A1, YM=7; //ID=0x5310
const int TS_LEFT = 951, TS_RT = 198, TS_TOP = 174, TS_BOT = 198;
const int displayWidth = 480;
const int dispayHeight = 320;
int currentPage;
int pixel_x, pixel_y;     //Touch_getXY() updates global vars


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;
MessageService msgService;


void setup(void)
{
    uint16_t ID = tft.readID();
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            
    tft.fillScreen(BLACK);
    Serial1.begin(9600);
    drawHome();
    currentPage = 0;
}

void loop(void)
{
    msgService.startListner();
    bool down = Touch_getXY();

    if (currentPage == 0)
    {
        if (btnPressed(60, 80, 360, 40, down)) {
            btnPressedAnimation(60, 80, 360, 40, 8);
            drawPlay();
            currentPage = 1;
        }       
        else if (btnPressed(60, 130, 360, 40, down)) {
            btnPressedAnimation(60, 130, 360, 40, 8);
            drawIo();
            currentPage = 2;
        } 
        else if (btnPressed(60, 180, 360, 40, down)) {
            btnPressedAnimation(60, 180, 360, 40, 8);
            drawService();
            currentPage = 3;            
        }
    }    
    
    if (currentPage == 1) {
        if(btnPressed(5, 5, 70, 30, down)) {
            btnPressedAnimation(5, 5, 70, 30, 8); 
            drawHome();
        }
        if(btnPressed(40, 80, 150, 40, down)) { // btn1
            btnPressedAnimation(40, 80, 150, 40, 8); 
        }
        if(btnPressed(40, 130, 150, 40, down)) { // btn2
            btnPressedAnimation(40, 130, 150, 40, 8); 
        }
        if(btnPressed(40, 180, 150, 40, down)) { // btn3
            btnPressedAnimation(40, 180, 150, 40, 8); 
        }
        if(btnPressed(40, 230, 150, 40, down)) { // btn4
            btnPressedAnimation(40, 230, 150, 40, 8); 
//            getDataFromStage();
       Serial1.write(1);

        }
    }

    if (currentPage == 2) {
        if(btnPressed(5, 5, 70, 30, down)) { // Back
            btnPressedAnimation(5, 5, 70, 30, 8); 
            drawHome();
        }
    }

    if (currentPage == 3) {
        if(btnPressed(5, 5, 70, 30, down)) {
            btnPressedAnimation(5, 5, 70, 30, 8); 
            drawHome();
        }
    }
}

void addButton(int16_t x0, int16_t y0, int16_t width, int16_t height, uint16_t color,uint16_t textColor,int16_t xText, int16_t yText, String text) {
    tft.fillRoundRect(x0, y0, width, height, 8, color);
    tft.drawRoundRect(x0, y0, width, height, 8, WHITE);
    tft.setTextColor(textColor);
    tft.setCursor(xText, yText);
    tft.print(text);
}

void addIoButton(int16_t x0, int16_t y0, uint16_t color,uint16_t textColor, String text) {
    tft.fillCircle(x0, y0, 20, color);
    tft.drawCircle(x0, y0, 20, WHITE);
    tft.setTextColor(textColor);
    tft.setCursor(x0 + 35, y0 - 5);
    tft.print(text);
}

bool btnPressed(int btnX0, int btnY0, int btnWidth, int btnHeight, bool pressed) {
    return pixel_x > btnX0 && pixel_x < btnY0 + btnWidth && pixel_y > btnY0 && pixel_y < btnY0 + btnHeight && pressed;
}

void btnPressedAnimation(int16_t x0, int16_t y0, int16_t width, int16_t height, int16_t borderRadius) {
    delay(70);
    tft.drawRoundRect(x0, y0, width, height, borderRadius, GREEN);
    delay(70);
    tft.drawRoundRect(x0, y0, width, height, borderRadius, WHITE);
}

void drawLoading() {
    tft.fillScreen(BLACK);

}

void drawPlay() {
    tft.fillScreen(BLACK);
    addButton(5, 5, 70, 30, BLUE, BLACK, 15, 15, "Back");
    addButton(40, 80, 150, 40, VIOLET, BLACK, 80, 95, "Flat");
    addButton(40, 130, 150, 40, VIOLET, BLACK, 80, 145, "Driver");
    addButton(40, 180, 150, 40, VIOLET, BLACK, 80, 195, "Tee");
    addButton(40, 230, 150, 40, VIOLET, BLACK, 80, 245, "Grass");
}

void drawIo() {
    tft.fillScreen(BLACK);
    addButton(5, 5, 70, 30, BLUE, BLACK, 15, 15, "Back");
    addIoButton(30, 100, GRAY, WHITE, "Feeder Sensor");
    addIoButton(30, 160, GRAY, WHITE, "Feeder Homing");

    addIoButton(280, 100, GRAY, WHITE, "Middle Sensor");
    addIoButton(280, 160, GRAY, WHITE, "Middle Homing");

    addIoButton(30, 220, GRAY, WHITE, "Tee Sensor");
    addIoButton(30, 280, GRAY, WHITE, "Tee Homing");
}

void getDataFromStage() {
    delay(50);
    char body[6] {'0','0','0','0','0','0'};
    char * request = msgService.buildRequest(MessageType::KA, body);
    msgService.sendRequest(request);
    free(request);
}

void drawService() {
    tft.fillScreen(BLACK);
    addButton(5, 5, 70, 30, BLUE, BLACK, 15, 15, "Back");
}

void drawHome()
{
    currentPage = 0;
    tft.fillScreen(BLACK);
    tft.drawRoundRect(1, 0, 479, 320, 8, WHITE);     //Page border

    tft.setCursor(150, 20);
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(WHITE);
    tft.print("Golf Simulator!");

    addButton(60, 80, 360, 40, RED, BLACK, 105, 95, "Play");
    addButton(60, 130, 360, 40, RED, BLACK, 105, 145, "I/O control");
    addButton(60, 180, 360, 40, RED, BLACK, 105, 195, "Service");
}

void drawCentreString(const String &buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    tft.setCursor(x - w / 2, y);
    tft.print(buf);
}

bool Touch_getXY()
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.y, 951, 198, 0, displayWidth);
        pixel_y = map(p.x, 174, 918, 0, dispayHeight);
    }
    return pressed;
}
