
// Definicje pinów
const int PIR_PIN = 8;         // Pin czujnika PIR
const int TRIG_PIN = 10;        // Pin Trig czujnika HC-SR04
const int ECHO_PIN = 11;        // Pin Echo czujnika HC-SR04
const int MOTOR_IN1 = 5;       // Pin IN1 sterownika L298N
const int MOTOR_IN2 = 6;       // Pin IN2 sterownika L298N
const int MOTOR_PWM = 9;       // Pin PWM sterownika silnika

// Stałe i zmienne
const int STOP_DISTANCE = 5;  // Minimalna odległość w cm do aktywacji stopu
const int FULL_ROTATION_TIME = 5000; // Czas pełnego obrotu (w ms)
const int SLOW_DOWN_TIME = 400; // Czas na zwolnienie pod koniec obrotu (w ms)

long duration;
int distance;
bool isEmergencyStop = false;
unsigned long rotationStartTime = 0;
bool isRotating = false;

void setup() {
  // Ustawienia pinów
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  // Start komunikacji
  Serial.begin(9600);
}

void loop() {
  // Sprawdzenie odległości
  distance = getDistance();
  int ruch = digitalRead(PIR_PIN);
 if (ruch == HIGH) {
    // Wykryto ruch, sprawdzenie odległości
    
   if (ruch == HIGH) {
    // Wykryto ruch, sprawdzenie odległości
    if (distance <= STOP_DISTANCE) {
      // Awaryjny stop
      stopMotor();
      Serial.println("Awaryjny stop! Osoba za blisko.");
    } else {
      // Aktywacja drzwi
      if (!isRotating) {
        isRotating = true;
        rotationStartTime = millis();
      }
      controlMotor();
      Serial.println("Ruch wykryty, obrót drzwi.");
    }
  } else {
    // Brak ruchu, drzwi zatrzymane
    stopMotor();
    Serial.println("Brak ruchu. Drzwi zatrzymane.");
  }
 }
  delay(100); // Minimalne opóźnienie dla stabilności
}

// Funkcja pomiaru odległości
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2; // Odległość w cm
}

// Funkcja obrotu silnika
void controlMotor() {
  unsigned long elapsedTime = millis() - rotationStartTime;

  if (elapsedTime >= FULL_ROTATION_TIME) {
    // Zakończenie obrotu
    stopMotor();
    isRotating = false;
    Serial.println("Pełny obrót zakończony.");
  } else if (elapsedTime >= FULL_ROTATION_TIME - SLOW_DOWN_TIME) {
    // Zwalnianie pod koniec obrotu
    analogWrite(MOTOR_PWM, 100); // Ustawienie niższej prędkości (wartość PWM 0-255)
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    Serial.println("Zwalnianie...");
  } else {
    // Normalna prędkość
    analogWrite(MOTOR_PWM, 255); // Maksymalna prędkość
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  }
}

// Funkcja zatrzymania silnika
void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
}
