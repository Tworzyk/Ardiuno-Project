
// Definicje pinów
const int PIR_PIN = 8;         // Pin czujnika PIR
const int TRIG_PIN = 10;        // Pin Trig czujnika HC-SR04
const int ECHO_PIN = 11;        // Pin Echo czujnika HC-SR04
const int MOTOR_IN1 = 5;       // Pin IN1 sterownika L298N
const int MOTOR_IN2 = 6;       // Pin IN2 sterownika L298N

// Stałe i zmienne
const int STOP_DISTANCE = 5;  // Minimalna odległość w cm do aktywacji stopu
long duration;
int distance;
bool isEmergencyStop = false;

void setup() {
  // Ustawienia pinów
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  // Start komunikacji
  Serial.begin(9600);
}

void loop() {
  // Sprawdzenie odległości
  distance = getDistance();
int ruch = digitalRead(PIR_PIN); 
 if (ruch == HIGH) {
    // Wykryto ruch, sprawdzenie odległości
    
    if (distance <= STOP_DISTANCE) {
        // Awaryjny stop
        stopMotor();
        Serial.println("Awaryjny stop! Osoba za blisko.");
    } else {
        // Aktywacja drzwi
        rotateMotor();
        Serial.println("Ruch wykryty, obrót drzwi.");
    }
} else {
    // Brak ruchu, drzwi zatrzymane
    stopMotor();
    Serial.println("Brak ruchu. Drzwi zatrzymane.");
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
void rotateMotor() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
}

// Funkcja zatrzymania silnika
void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}
