// Definicje pinów
const int PIR_PIN = 8;         // Pin czujnika PIR
const int TRIG_PIN = 10;       // Pin Trig czujnika HC-SR04
const int ECHO_PIN = 11;       // Pin Echo czujnika HC-SR04
const int MOTOR_IN1 = 5;       // Pin IN1 sterownika L298N
const int MOTOR_IN2 = 6;       // Pin IN2 sterownika L298N
const int MOTOR_PWM = 9;       // Pin PWM sterownika silnika

// Stałe i zmienne
const int STOP_DISTANCE = 5;   // Minimalna odległość w cm do aktywacji stopu
const int FULL_ROTATION_TIME = 2000; // Czas pełnego obrotu (w ms)
const int SLOW_DOWN_TIME = 400;      // Czas na zwolnienie pod koniec obrotu (w ms)

enum State { IDLE, ROTATING, SLOWING_DOWN, EMERGENCY_STOP };
State currentState = IDLE;

long duration;
int distance;
unsigned long rotationStartTime = 0;

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
  // Aktualizacja odległości
  distance = getDistance();
  int ruch = digitalRead(PIR_PIN);

  switch (currentState) {
    case IDLE:
      if (ruch == HIGH && distance > STOP_DISTANCE) {
        // Rozpoczęcie obrotu
        currentState = ROTATING;
        rotationStartTime = millis();
        Serial.println("Ruch wykryty, rozpoczęcie obrotu.");
      } else if (distance <= STOP_DISTANCE) {
        Serial.println("Osoba za blisko. Oczekiwanie.");
      }
      stopMotor();
      break;

    case ROTATING:
      if (distance <= STOP_DISTANCE) {
        // Awaryjne zatrzymanie
        currentState = EMERGENCY_STOP;
        Serial.println("Awaryjny stop w trakcie obrotu!");
      } else if (millis() - rotationStartTime >= FULL_ROTATION_TIME - SLOW_DOWN_TIME) {
        // Przejście do fazy zwalniania
        currentState = SLOWING_DOWN;
        Serial.println("Zwalnianie obrotu.");
      } else {
        rotateMotor(255); // Pełna prędkość
      }
      break;

    case SLOWING_DOWN:
      if (distance <= STOP_DISTANCE) {
        // Awaryjne zatrzymanie podczas zwalniania
        currentState = EMERGENCY_STOP;
        Serial.println("Awaryjny stop podczas zwalniania!");
      } else if (millis() - rotationStartTime >= FULL_ROTATION_TIME) {
        // Zakończenie obrotu
        currentState = IDLE;
        Serial.println("Pełny obrót zakończony.");
      } else {
        rotateMotor(100); // Zmniejszona prędkość
      }
      break;

    case EMERGENCY_STOP:
      // Zatrzymanie i oczekiwanie na usunięcie przeszkody
      stopMotor();
      if (distance > STOP_DISTANCE) {
        currentState = IDLE;
        Serial.println("Przeszkoda usunięta. Oczekiwanie na ruch.");
      }
      break;
  }

  delay(50); // Minimalne opóźnienie dla stabilności
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
void rotateMotor(int speed) {
  analogWrite(MOTOR_PWM, speed);
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
}

// Funkcja zatrzymania silnika
void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_PWM, 0);
}
