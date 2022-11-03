#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <lvgl.h>
#include <Arduino.h>
#include <demos/lv_demos.h>
#include <ui.h>
#include <math.h>
static const uint16_t screenWidth  = 160;
static const uint16_t screenHeight = 80;
int32_t bar_value=0;
int8_t wifi_line[10];
int32_t wifi_gain;
float distance=0;
// lvgl buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth,screenHeight);  // 调用自定义库
const char *wifi_SSID="YXDZ_ESP32";  //保存AP的名称信息
const char *wifi_Password="ESP321234";  //保存AP的密码信息



void TFT_Init(void){
  tft.init();//屏幕初始化
  // 将“光标”设置在显示屏的左上角（0,0），然后选择字体4
  tft.setCursor(0, 0, 2);
  // 将字体颜色设置为白色，背景为黑色
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setRotation(4);
  tft.fillScreen(TFT_BLACK);//填充屏幕
  tft.println("Hello world");
}
void Wifi_SAT_Init(void){
  WiFi.begin("JinHai","12123434");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("waiting");    
    delay(500);
  }
}
void WiFi_Info(void){
    Serial.print("\nIP位址:");
 
  //WiFi.localIP()显示本机ip
  Serial.println(WiFi.localIP());
 
  Serial.print("WiFi RSSI: ");
 
  //WiFi.RSSI()显示信号强度
  Serial.println(WiFi.RSSI());
}
void Wifi_AP_Init(void){
  WiFi.softAP(wifi_SSID,wifi_Password);  //设置AP模式热点的名称和密码，密码可不填则发出的热点为无密码热点
}
//================================================================
#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif
/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}
void setup() {
  // put your setup code here, to run once:
  String LVGL_Arduino = "Hello Arduino! ";
  Serial.begin(115200);
  Serial.println("Hello ESP32!!");
  //wifi 
  Wifi_SAT_Init();
  lv_init();
#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif
  tft.init();   
  tft.setRotation(4);
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );
  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );
  ui_init();
  
  Serial.println( "Setup done" );
  delay(300);
}

void loop() {
  
  //10次均值滤波
  float temp;
  int j,sum;
  for(j=9;j>=1;j--){
    wifi_line[j]=wifi_line[j-1];
  }
  //最新的信号强度值存入队列
  wifi_line[0]=100+WiFi.RSSI();
  for(j=0;j<=9;j++){
    sum+=wifi_line[j];
  }
  wifi_gain=sum/10;
  sum=0;
  //计算距离：
  temp=(-60.46-wifi_gain+100)/14.3;
  distance=pow(10,temp);
  Serial.printf("signal:%d temp:%f\n",wifi_gain,temp);
  lv_timer_handler();
  set_bar_value(wifi_gain,wifi_gain);
  
  delay(5);
}