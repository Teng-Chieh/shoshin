/* 開機只會執行一次 */
void setup()
{
    pinMode(13, OUTPUT);    // sets the digital pin 13 as output (設定PIN13這根腳為Output Mode)
}

/* 重複執行此區段 */
void loop()
{
    digitalWrite(13, HIGH); // sets the digital pin 13 on     (設定PIN13這根腳為輸出HIGH)
    delay(1000);            // waits for a second             (延遲1秒 -> 單位為ms, 1s = 1000ms)  
    digitalWrite(13, LOW);  // sets the digital pin 13 off    (設定PIN13這根腳為輸出LOW)
    delay(1000);            // waits for a second             (延遲1秒 -> 單位為ms, 1s = 1000ms)
}
