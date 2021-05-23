#include <TimeLib.h>

//LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//DHT11
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//LCD
#define BACKLIGHT_PIN     13
//LiquidCrystal_I2C lcd(0x38);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x38, BACKLIGHT_PIN, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//DHT11
#define DHTTYPE    DHT11     // DHT 11
#define DHTPIN 2     // Digital pin connected to the DHT sensor
DHT_Unified dht(DHTPIN, DHTTYPE);

// Creat a set of new characters
const uint8_t charBitmap[][8] = {
  { 0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0 },
  { 0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0 },
  { 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0 },
  { 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0 },
  { 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0 },
  { 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0 },
  { 0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0 },
  { 0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0 }

};

uint32_t delayMS;

typedef enum __lcd_stage {
  lcd_stage_clear,
  lcd_stage_info,
  lcd_stage_dht,
  lcd_stage_dht_1,
  lcd_stage_dht_2,
  lcd_stage_dht_3,
  lcd_stage_dht_4,
  lcd_stage_dht_5,
  lcd_stage_dht_6,
  lcd_stage_dht_7,
  lcd_stage_dht_8,
  lcd_stage_dht_9,
  lcd_stage_max,
} _lcd_stage;

typedef struct _lcd_msg {
  float temperature;
  float humidity;
  bool dht_det_status;
  unsigned int det_times;
  u8 lcd_stage;
  bool lcd_clear;
} lcd_msg;

typedef struct _time_profile_ {
  u32 current_sample_time;
  u32 last_sample_time;
  u8 hours;
  u8 mins;
  u8 secs;
} _time_profile;

lcd_msg lcd_show_msg;
_time_profile time_profile;

void init_lcd()
{
  //LCD
  int charBitmapSize = (sizeof(charBitmap ) / sizeof (charBitmap[0]));

  // Switch on the backlight
  pinMode ( BACKLIGHT_PIN, OUTPUT );
  digitalWrite ( BACKLIGHT_PIN, HIGH );

  lcd.begin(16, 2);              // initialize the lcd

  for ( int i = 0; i < charBitmapSize; i++ )
  {
    lcd.createChar ( i, (uint8_t *)charBitmap[i] );
  }

  lcd.home ();                   // go home
  lcd.print("Hello,ARDUINO");
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("DEMO...........");
  delay ( 1000 );
}

void init_dht()
{
  //DHT11
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temper ature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}
void setup()
{
  Serial.begin(9600);
  memset(&lcd_show_msg, 0, sizeof(lcd_msg));
  memset(&time_profile, 0, sizeof(_time_profile));
  init_lcd();
  init_dht();
}

void show_time_in_lcd(u8 s_cursor_1, u8 s_cursor_2, lcd_msg* lcd_show_msg, _time_profile* t_profile)
{
  String str_time = "";

  if (t_profile->hours < 10)
    str_time += "0";

  str_time += t_profile->hours;
  str_time += ":";

  if (t_profile->mins < 10)
    str_time += "0";

  str_time += t_profile->mins;
  str_time += ":";

  if (t_profile->secs < 10)
    str_time += "0";

  str_time += t_profile->secs;

  lcd.setCursor(s_cursor_1, s_cursor_2);
  lcd.print(str_time);
}

void show_in_lcd(lcd_msg* lcd_show_msg, _time_profile* t_profile)
{
  //Serial.print("lcd_stage:" + String(lcd_show_msg->lcd_stage) + "\n");
  switch (lcd_show_msg->lcd_stage) {
    case lcd_stage_clear:
      lcd.clear();
    case lcd_stage_info:
    case lcd_stage_dht:
    case lcd_stage_dht_1:
    case lcd_stage_dht_2:
    case lcd_stage_dht_3:
    case lcd_stage_dht_4:
    case lcd_stage_dht_5:
    case lcd_stage_dht_6:
    case lcd_stage_dht_7:
    case lcd_stage_dht_8:
    case lcd_stage_dht_9:
      lcd.setCursor(1, 0);
      lcd.print("Ryan");
      show_time_in_lcd(6, 0, lcd_show_msg, t_profile);
      lcd.setCursor(0, 1);
      lcd.print("T:" + String(lcd_show_msg->temperature) + "C " +
                "H:" + String(lcd_show_msg->humidity) + "%");
      break;
    default:
      lcd.setCursor(0, 0);
      lcd.print("ERROR");
      //go to the next line
      lcd.setCursor(0, 1);
      lcd.print("ERROR");
      break;
  }
}

bool detect_temperature_humidity(float* temperture, float* humidity)
{
  sensors_event_t temp_event;
  sensors_event_t humi_event;
  dht.temperature().getEvent(&temp_event);
  dht.humidity().getEvent(&humi_event);

  if (isnan(temp_event.temperature) || isnan(humi_event.relative_humidity)) {
    Serial.println(F("Reading temperature or humidity failed!"));
    return false;
  }
#if 0//def DBG_TEMP_HUMI
  Serial.print(F("Temperature: "));
  Serial.print(temp_event.temperature);
  Serial.println(F("°C"));
  Serial.print(F("Humidity: "));
  Serial.print(humi_event.relative_humidity);
  Serial.println(F("%"));
#endif

  *temperture = temp_event.temperature;
  *humidity = humi_event.relative_humidity;
  return true;
}

void get_time(_time_profile* t_profile)
{
  time_t t = now();
  t_profile->hours = hour(t);
  t_profile->mins = minute(t);
  t_profile->secs = second(t);
}

void loop()
{
  //DHT11
  // Delay between measurements.
  lcd_show_msg.dht_det_status = detect_temperature_humidity(&lcd_show_msg.temperature, &lcd_show_msg.humidity);

  while (1) {
    time_profile.current_sample_time = now();
    if (time_profile.current_sample_time - time_profile.last_sample_time >= 2) {
      lcd_show_msg.dht_det_status = detect_temperature_humidity(&lcd_show_msg.temperature, &lcd_show_msg.humidity);
      time_profile.last_sample_time = time_profile.current_sample_time;
    }

    if (!lcd_show_msg.dht_det_status) {
      lcd_show_msg.temperature = 8888;
      lcd_show_msg.humidity = 8888;
    }

    get_time(&time_profile);
    show_in_lcd(&lcd_show_msg, &time_profile);

    lcd_show_msg.lcd_stage = lcd_show_msg.lcd_stage + 1;
    if (lcd_show_msg.lcd_stage >= lcd_stage_max) {
      lcd_show_msg.lcd_stage = lcd_stage_info;
    }

    //delay(1000);
  }
}
