#include <HCSR04.h>

#define PIR_PIN 8
#define IN1 5
#define IN2 6


HCSR04 hc(10, 11);

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);  
  pinMode(IN1, OUTPUT);      
  pinMode(IN2, OUTPUT);      
  digitalWrite(IN2, LOW);
}

void loop() {
  long distance = hc.dist();  
  int ruch = digitalRead(PIR_PIN);  


  if (ruch == HIGH && distance > 20) {
    digitalWrite(IN1, HIGH);  
   
    Serial.println("Drzwi obracają się...");
  }
  else {
    digitalWrite(IN1, LOW);   
    Serial.println("Drzwi zatrzymane...");
  }


  Serial.print("Odległość: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(100);  
}
