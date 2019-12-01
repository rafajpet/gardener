#include "SSD1306.h"


uint8_t LED_PIN = 15; 

SSD1306 display(0x3c, 5, 4);

char buffer[20];
void setup() {
    Serial.begin(115200); 
    Serial.println("Start switch application");
    display.init(); 
    display.flipScreenVertically(); 
    display.setFont(ArialMT_Plain_16); 
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    pinMode(LED_PIN, OUTPUT);
}
 
void loop() {
    if(Serial.available()){
        char input = Serial.read();
        Serial.print("You typed: " );
        sprintf(buffer, "Switch: %c", input);
        Serial.println(input);
        display.drawString(15, 15, buffer);
        display.display();
        display.clear();
        if (input == '1') {
            digitalWrite(LED_PIN, HIGH);  
        } else {
            digitalWrite(LED_PIN, LOW);  
        }
    }
}
