#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VL53L0X.h>

//OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  
#define SCREEN_ADDRESS 0x3C 

// buzzer pin (@ D3 on arduino)
#define buzzerPin 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
VL53L0X sensor;

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT); //initialize buzzer

  //initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  //startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();

  //initialize sensor
  Wire.begin();
  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize VL53L0X sensor!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sensor Error!");
    display.display();
    while (1) {}
  }


  sensor.startContinuous();

  delay(1000);
}

void loop() {
  //read distance from VL53L0X (in millimeters) this is for raw data @ bottom
  uint16_t distance_mm = sensor.readRangeContinuousMillimeters();
  float distance_inches = distance_mm / 25.4;

  //this checks for sensor timeout
  if (sensor.timeoutOccurred()) {
    Serial.print("VL53L0X TIMEOUT");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Sensor Timeout!");
    display.display();
    return;
  }

  //display text on OLED
  display.clearDisplay();

  //title @ top of display
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Distance Sensor");

  //distance text @ middle of display (main text)
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(distance_inches, 2);
  display.println(" in");

  //additional info (distance in cm) in smaller text on bottom
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Raw: ");
  display.print(distance_mm / 10.0, 1);  // convert to cm with 1 decimal
  display.println(" cm");

  display.display();

  //print to serial (for debugging)
  Serial.print("Distance: ");
  Serial.print(distance_inches);
  Serial.println(" inches");


  //BUZZER CODE
  
  //buzz if object is within 1 inch
  if (distance_inches <= 1.0) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  delay(100);
}
