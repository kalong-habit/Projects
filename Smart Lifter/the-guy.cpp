#include <HX711.h>
#include <Stepper.h>

// setting spr for stepper
#define STEPS 200 // type NEMA 17: 1.8 degree
Stepper stepper(STEPS, 4, 5); // pin 4 to direction, pin 5 to step

// setting port for loadcell
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
#define CALLIBRATION_FACTOR -108.9800995024

//setting port for buzzer
const int buzzerPin = 6;

HX711 scale;
int previous_weight = 0;
int weight_threshold[] = {200, 400, 600, 800, 1000};
int step_count = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Initializing the scale...");
  scale.set_scale(CALLIBRATION_FACTOR);
  scale.tare();

  // setting speed for stepper
  stepper.setSpeed(1000);
  
  pinMode(buzzerPin, OUTPUT);

}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(5);
    Serial.print("Weight: ");
    Serial.println(weight);

    for (int i = 0; i < 5; i++) {
      if (weight >= weight_threshold[i] && previous_weight < weight_threshold[i]) {
        tone(buzzerPin, 1000, 1000);
        delay(1000);
        noTone(buzzerPin);     
        stepper.step(20 * STEPS);
        step_count += 1;
      }
      else if (weight < weight_threshold[i] && previous_weight >= weight_threshold[i]) {
        tone(buzzerPin, 1000, 1000);
        delay(1000);
        noTone(buzzerPin); 
        stepper.step(-20 * STEPS);
        step_count = 0;
      }
    }
    previous_weight = weight;
  }
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
