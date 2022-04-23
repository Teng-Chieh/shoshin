#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include <ThreeWire.h>
#include <RtcDS1302.h>
#include "DHT.h"

#define DS1302_SCL              10
#define DS1302_SDA              9
#define DS1302_RST              8

#define DHT_DATA_PIN            7
#define DHT_PWR_PIN             6
#define DHTTYPE                 DHT11

#define BTN_0_PIN               5
#define BTN_1_PIN               4

#define INTERVAL_GET_TIME       500
#define INTERVAL_DHT            500
#define INTERVAL_BTN            100

typedef enum {
    LCD_STATE_MODE_0,
    LCD_STATE_MODE_1,
    LCD_STATE_MODE_2,
    LCD_STATE_MODE_MAX,
} LCD_STATE_E;

typedef struct {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
} rtc_time_st;

typedef struct {
    float temperature;
    float humidity;
} dht_data_st;

typedef struct {
    rtc_time_st lcd_rtc_time;
    dht_data_st lcd_dht_data;
    u8 lcd_state;
    u8 lcd_update;
} lcd_ctl_st;

typedef struct {
    u32 time_lcd;
    u32 time_dht;
    u32 time_btn;
} time_update_st;

typedef struct {
    u8 button_state;
} btn_ctl_st;

lcd_ctl_st lcd_ctl;
btn_ctl_st btn_ctl;
time_update_st time_update;

RtcDateTime rtc_date_time;

DHT dht(DHT_DATA_PIN, DHTTYPE);

LiquidCrystal_PCF8574 lcd(0x27); // set lcd slave addr, 0x27 or 0x3F
ThreeWire myWire(DS1302_SDA, DS1302_SCL, DS1302_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

const char custom[][8] PROGMEM = {                        // Custom character definitions
    { 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 }, // char 1
    { 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }, // char 2
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x03 }, // char 3
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F }, // char 4
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C, 0x18 }, // char 5
    { 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F }, // char 6
    { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F }, // char 7
    { 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }  // char 8
};

const char bigChars[][8] PROGMEM = {
    { 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // Space
    { 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // !
    { 0x05, 0x05, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00 }, // "
    { 0x04, 0xFF, 0x04, 0xFF, 0x04, 0x01, 0xFF, 0x01 }, // #
    { 0x08, 0xFF, 0x06, 0x07, 0xFF, 0x05, 0x00, 0x00 }, // $
    { 0x01, 0x20, 0x04, 0x01, 0x04, 0x01, 0x20, 0x04 }, // %
    { 0x08, 0x06, 0x02, 0x20, 0x03, 0x07, 0x02, 0x04 }, // &
    { 0x05, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '
    { 0x08, 0x01, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00 }, // (
    { 0x01, 0x02, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00 }, // )
    { 0x01, 0x04, 0x04, 0x01, 0x04, 0x01, 0x01, 0x04 }, // *
    { 0x04, 0xFF, 0x04, 0x01, 0xFF, 0x01, 0x00, 0x00 }, // +
    { 0x20, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, //
    { 0x04, 0x04, 0x04, 0x20, 0x20, 0x20, 0x00, 0x00 }, // -
    { 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // .
    { 0x20, 0x20, 0x04, 0x01, 0x04, 0x01, 0x20, 0x20 }, // /
    { 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00 }, // 0
    { 0x01, 0x02, 0x20, 0x04, 0xFF, 0x04, 0x00, 0x00 }, // 1
    { 0x06, 0x06, 0x02, 0xFF, 0x07, 0x07, 0x00, 0x00 }, // 2
    { 0x01, 0x06, 0x02, 0x04, 0x07, 0x05, 0x00, 0x00 }, // 3
    { 0x03, 0x04, 0xFF, 0x20, 0x20, 0xFF, 0x00, 0x00 }, // 4
    { 0xFF, 0x06, 0x06, 0x07, 0x07, 0x05, 0x00, 0x00 }, // 5
    { 0x08, 0x06, 0x06, 0x03, 0x07, 0x05, 0x00, 0x00 }, // 6
    { 0x01, 0x01, 0x02, 0x20, 0x08, 0x20, 0x00, 0x00 }, // 7
    { 0x08, 0x06, 0x02, 0x03, 0x07, 0x05, 0x00, 0x00 }, // 8
    { 0x08, 0x06, 0x02, 0x07, 0x07, 0x05, 0x00, 0x00 }, // 9
    { 0xA5, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // :
    { 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ;
    { 0x20, 0x04, 0x01, 0x01, 0x01, 0x04, 0x00, 0x00 }, // <
    { 0x04, 0x04, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00 }, // =
    { 0x01, 0x04, 0x20, 0x04, 0x01, 0x01, 0x00, 0x00 }, // >
    { 0x01, 0x06, 0x02, 0x20, 0x07, 0x20, 0x00, 0x00 }, // ?
    { 0x08, 0x06, 0x02, 0x03, 0x04, 0x04, 0x00, 0x00 }, // @
    { 0x08, 0x06, 0x02, 0xFF, 0x20, 0xFF, 0x00, 0x00 }, // A
    { 0xFF, 0x06, 0x05, 0xFF, 0x07, 0x02, 0x00, 0x00 }, // B
    { 0x08, 0x01, 0x01, 0x03, 0x04, 0x04, 0x00, 0x00 }, // C
    { 0xFF, 0x01, 0x02, 0xFF, 0x04, 0x05, 0x00, 0x00 }, // D
    { 0xFF, 0x06, 0x06, 0xFF, 0x07, 0x07, 0x00, 0x00 }, // E
    { 0xFF, 0x06, 0x06, 0xFF, 0x20, 0x20, 0x00, 0x00 }, // F
    { 0x08, 0x01, 0x01, 0x03, 0x04, 0x02, 0x00, 0x00 }, // G
    { 0xFF, 0x04, 0xFF, 0xFF, 0x20, 0xFF, 0x00, 0x00 }, // H
    { 0x01, 0xFF, 0x01, 0x04, 0xFF, 0x04, 0x00, 0x00 }, // I
    { 0x20, 0x20, 0xFF, 0x04, 0x04, 0x05, 0x00, 0x00 }, // J
    { 0xFF, 0x04, 0x05, 0xFF, 0x20, 0x02, 0x00, 0x00 }, // K
    { 0xFF, 0x20, 0x20, 0xFF, 0x04, 0x04, 0x00, 0x00 }, // L
    { 0x08, 0x03, 0x05, 0x02, 0xFF, 0x20, 0x20, 0xFF }, // M
    { 0xFF, 0x02, 0x20, 0xFF, 0xFF, 0x20, 0x03, 0xFF }, // N
    { 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00 }, // 0
    { 0x08, 0x06, 0x02, 0xFF, 0x20, 0x20, 0x00, 0x00 }, // P
    { 0x08, 0x01, 0x02, 0x20, 0x03, 0x04, 0xFF, 0x04 }, // Q
    { 0xFF, 0x06, 0x02, 0xFF, 0x20, 0x02, 0x00, 0x00 }, // R
    { 0x08, 0x06, 0x06, 0x07, 0x07, 0x05, 0x00, 0x00 }, // S
    { 0x01, 0xFF, 0x01, 0x20, 0xFF, 0x20, 0x00, 0x00 }, // T
    { 0xFF, 0x20, 0xFF, 0x03, 0x04, 0x05, 0x00, 0x00 }, // U
    { 0x03, 0x20, 0x20, 0x05, 0x20, 0x02, 0x08, 0x20 }, // V
    { 0xFF, 0x20, 0x20, 0xFF, 0x03, 0x08, 0x02, 0x05 }, // W
    { 0x03, 0x04, 0x05, 0x08, 0x20, 0x02, 0x00, 0x00 }, // X
    { 0x03, 0x04, 0x05, 0x20, 0xFF, 0x20, 0x00, 0x00 }, // Y
    { 0x01, 0x06, 0x05, 0x08, 0x07, 0x04, 0x00, 0x00 }, // Z
    { 0xFF, 0x01, 0xFF, 0x04, 0x00, 0x00, 0x00, 0x00 }, // [
    { 0x01, 0x04, 0x20, 0x20, 0x20, 0x20, 0x01, 0x04 }, // Backslash
    { 0x01, 0xFF, 0x04, 0xFF, 0x00, 0x00, 0x00, 0x00 }, // ]
    { 0x08, 0x02, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00 }, // ^
    { 0x20, 0x20, 0x20, 0x04, 0x04, 0x04, 0x00, 0x00 }  // _
};

byte col, row, nb = 0, bc = 0;                            // general
byte bb[8];

void lcd_init()
{
    lcd.begin(16, 2);           // 16 * 2 LCD
    //lcd.begin(20, 4);         // 20 * 4 LCD
    lcd.setBacklight(1);        // lcd back lightlight 0-255

    // create 8 custom characters
    for (nb = 0; nb < 8; nb++ ) {
        for (bc = 0; bc < 8; bc++) bb[bc] = pgm_read_byte( &custom[nb][bc] );
        lcd.createChar ( nb + 1, bb );
    }

    lcd.clear();
    writeBigString("RYAN", 0, 0);
}

void rtc_init()
{
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected()) {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning()) {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    } else if (now > compiled) {
        Serial.println("RTC is newer than compile time. (this is expected)");
    } else if (now == compiled) {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void dht_init()
{
    pinMode(DHT_PWR_PIN, OUTPUT);
    digitalWrite(DHT_PWR_PIN, HIGH);
    Serial.println("DHTxx test!");
    dht.begin();
}

void btn_press_init()
{
    pinMode(BTN_0_PIN, INPUT);
    pinMode(BTN_1_PIN, INPUT);
}

void dht_proc()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

#if 0
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" oC ");
#endif

    lcd_processing_update_dht_data(t, h);
}

void button_press_proc()
{
    unsigned char btn_det = digitalRead(BTN_0_PIN);

    if (btn_det != btn_ctl.button_state) {
        if (btn_det == LOW) {
            lcd_processing_mode_change();
        }

        btn_ctl.button_state = btn_det;
    }
#if 0
    Serial.print("Button status ");
    Serial.print(btn_det ? "HIGH" : "LOW");
    Serial.print(" lcd_mode ");
    Serial.println(lcd_ctl.lcd_state);
#endif
}

void setup ()
{
    Serial.begin(9600);

    lcd_init();
    delay(800);

    rtc_init();
    dht_init();
    btn_press_init();

    lcd_ctl.lcd_state = LCD_STATE_MODE_2;

    rtc_get_time(&rtc_date_time);
    lcd_processing_update_time(rtc_date_time);

    lcd_ctl.lcd_update = 1;
}

void rtc_get_time(RtcDateTime *rtc_date_time)
{
    *rtc_date_time = Rtc.GetDateTime();

#if 0
    printDateTime(*rtc_date_time);
    Serial.println();
#endif

    if (!rtc_date_time->IsValid()) {
        Serial.println("RTC lost confidence in the DateTime!");
    }
}

void loop ()
{
    if(millis() > time_update.time_lcd + INTERVAL_GET_TIME) {
        time_update.time_lcd = millis();

        rtc_get_time(&rtc_date_time);
        lcd_processing_update_time(rtc_date_time);

        if (lcd_ctl.lcd_state == LCD_STATE_MODE_0 || lcd_ctl.lcd_state == LCD_STATE_MODE_2) {
            lcd_ctl.lcd_update = 1;
        }
    }

    if (millis() - time_update.time_dht > INTERVAL_DHT) {
        time_update.time_dht = millis();
        dht_proc();

        if (lcd_ctl.lcd_state == LCD_STATE_MODE_1) {
            lcd_ctl.lcd_update = 1;
        }
    }

    if (millis() - time_update.time_btn > INTERVAL_BTN) {
        time_update.time_btn = millis();
        button_press_proc();
    }

    lcd_processing_proc();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Month(),
               dt.Day(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second() );
    Serial.print(datestring);
}


// writeBigChar: writes big character 'ch' to column x, row y; returns number of columns used by 'ch'
int writeBigChar(char ch, byte x, byte y)
{
    if (ch < ' ' || ch > '_') return 0;                     // If outside table range, do nothing
    nb = 0;                                                 // character byte counter
    for (bc = 0; bc < 8; bc++) {
        bb[bc] = pgm_read_byte( &bigChars[ch - ' '][bc] );  // read 8 bytes from PROGMEM
        if (bb[bc] != 0) nb++;
    }

    bc = 0;
    for (row = y; row < y + 2; row++) {
        for (col = x; col < x + nb / 2; col++ ) {
            lcd.setCursor(col, row);                      // move to position
            lcd.write(bb[bc++]);                          // write byte and increment to next
        }
        //    lcd.setCursor(col, row);
        //    lcd.write(' ');                                 // Write ' ' between letters
    }
    return nb / 2 - 1;                                  // returns number of columns used by char
}

// writeBigString: writes out each letter of string
void writeBigString(char *str, byte x, byte y)
{
    char c;
    while ((c = *str++))
        x += writeBigChar(c, x, y) + 1;
}


// ********************************************************************************** //
//                                      OPERATION ROUTINES
// ********************************************************************************** //
// FREERAM: Returns the number of bytes currently free in RAM
int freeRam(void)
{
    extern int  __bss_end, *__brkval;
    int free_memory;
    if ((int)__brkval == 0) {
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    } else {
        free_memory = ((int)&free_memory) - ((int)__brkval);
    }
    return free_memory;
}

void lcd_processing_update_dht_data(float t, float h)
{
    lcd_ctl.lcd_dht_data.temperature = t;
    lcd_ctl.lcd_dht_data.humidity = h;
}

void lcd_processing_update_time(const RtcDateTime& dt)
{
    lcd_ctl.lcd_rtc_time.year = dt.Year();
    lcd_ctl.lcd_rtc_time.month = dt.Month();
    lcd_ctl.lcd_rtc_time.day = dt.Day();
    lcd_ctl.lcd_rtc_time.hour = dt.Hour();
    lcd_ctl.lcd_rtc_time.minute = dt.Minute();
    lcd_ctl.lcd_rtc_time.second = dt.Second();
}

void lcd_processing_mode_change()
{
    lcd_ctl.lcd_state++;
    if (lcd_ctl.lcd_state >= LCD_STATE_MODE_MAX) {
        lcd_ctl.lcd_state = LCD_STATE_MODE_0;
    }
    lcd_ctl.lcd_update = 1;

    lcd.clear();
}

void lcd_processing_proc()
{
    if (lcd_ctl.lcd_update) {
        lcd_ctl.lcd_update = 0;

        switch (lcd_ctl.lcd_state) {
            case LCD_STATE_MODE_0:

                if(lcd_ctl.lcd_rtc_time.hour > 11) {
                    lcd.setCursor(0, 0);
                    lcd.print("P  ");
                    lcd.setCursor(0, 1);
                    lcd.print("M  ");
                } else {
                    lcd.setCursor(0, 0);
                    lcd.print("A  ");
                    lcd.setCursor(0, 1);
                    lcd.print("M  ");
                }

                char datestring[10];

                snprintf_P(datestring,
                           10,
                           PSTR("%02u:%02u"),
                           lcd_ctl.lcd_rtc_time.hour,
                           lcd_ctl.lcd_rtc_time.minute);

                writeBigString(datestring, 3, 0);
                break;
            case LCD_STATE_MODE_1:

                lcd.setCursor(0, 0);
                lcd.print("Temp:  ");

                lcd.setCursor(7, 0);
                lcd.print(lcd_ctl.lcd_dht_data.temperature);

                lcd.setCursor(12, 0);
                lcd.print(" ");

                lcd.setCursor(13, 0);
                lcd.print((char)223);

                lcd.setCursor(14, 0);
                lcd.print("C ");

                lcd.setCursor(0, 1);
                lcd.print("RH  :  ");

                lcd.setCursor(7, 1);
                lcd.print(lcd_ctl.lcd_dht_data.humidity);

                lcd.setCursor(12, 1);
                lcd.print("  ");

                lcd.setCursor(14, 1);
                lcd.print("%");
                break;

            case LCD_STATE_MODE_2:
                //Year
                lcd.setCursor(0, 0);
                lcd.print(lcd_ctl.lcd_rtc_time.year);
                lcd.print("/");
                //month
                lcd.setCursor(5, 0);
                __lcd_print_datetime(lcd_ctl.lcd_rtc_time.month);
                lcd.print("/");
                //day
                lcd.setCursor(8, 0);
                __lcd_print_datetime(lcd_ctl.lcd_rtc_time.day);
                //
                lcd.setCursor(10, 0);
                lcd.print("      ");
                //hour
                lcd.setCursor(0, 1);
                __lcd_print_datetime(lcd_ctl.lcd_rtc_time.hour);
                lcd.print(":");
                //minute
                lcd.setCursor(3, 1);
                __lcd_print_datetime(lcd_ctl.lcd_rtc_time.minute);
                lcd.print(":");
                //second
                lcd.setCursor(6, 1);
                __lcd_print_datetime(lcd_ctl.lcd_rtc_time.second);
                //
                lcd.setCursor(8, 1);
                lcd.print("        ");
                break;

            default:
                break;
        }
    }
}

void __lcd_print_datetime(uint32_t val)
{
    if (val < 10) {
        lcd.print("0");
        lcd.print(val);
    } else {
        lcd.print(val);
    }
}
