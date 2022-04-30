void setup()
{
    pinMode(3, OUTPUT);         //LED1
    pinMode(4, OUTPUT);         //LED2
    pinMode(5, OUTPUT);         //LED3
    pinMode(6, OUTPUT);         //LED4
    pinMode(A3, OUTPUT);        //LED5
    pinMode(A2, OUTPUT);        //LED6
    pinMode(A1, OUTPUT);        //LED7
    pinMode(A0, OUTPUT);        //LED8
}

void loop()
{
    digitalWrite(3, LOW);          //LED1
    digitalWrite(4, LOW);          //LED2
    digitalWrite(5, LOW);          //LED3
    digitalWrite(6, LOW);          //LED4
    digitalWrite(A3, LOW);         //LED5
    digitalWrite(A2, LOW);         //LED6
    digitalWrite(A1, LOW);         //LED7
    digitalWrite(A0, LOW);         //LED8
    delay(1000);
    digitalWrite(3, HIGH);           //LED1
    digitalWrite(4, HIGH);           //LED2
    digitalWrite(5, HIGH);           //LED3
    digitalWrite(6, HIGH);           //LED4
    digitalWrite(A3, HIGH);          //LED5
    digitalWrite(A2, HIGH);          //LED6
    digitalWrite(A1, HIGH);          //LED7
    digitalWrite(A0, HIGH);          //LED8
    delay(1000);
}

