// Initalizing Libraries for LCD Display
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  

const int trigPin = 11; // HC-SR04 Trigger Pin 
const int echoPin = 12; // HC-SR04 Echo Pin
const int alarmLight = 9; // LED Alarm Pin
const int alarmSound = 8; // Piezo Pin (Connected to Slide switch)
const int setDistanceButton = 2; // Push Button Pin
const int currentDistanceButton = 3; // Push Button Pin
const int potPin = A3; // Potentiometer Pin

long duration; // For computation of distance from Ultrasonic Distance Sensor
int distance; // Current Distance Value
int safetyDistance = 15; // Default threshold distance
int currentDisplay = 1; // Helps with switching display logic
int potVal; // Value from Potentiometer
int normalizedPotVal; // Normalized value from potentiometer (1-100)

void setup()
{
    lcd.init();
    pinMode(alarmSound, OUTPUT);
    pinMode(alarmLight, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(setDistanceButton, INPUT);
    digitalWrite(setDistanceButton, HIGH);
    pinMode(currentDistanceButton, INPUT);
    digitalWrite(currentDistanceButton, HIGH);
    Serial.begin(9600);
    Serial.flush();
}

void loop()
{
    potVal = analogRead(potPin); // Reading from 10k Potentiometer Pin, gives an integer value of 0 - 682 
    

    normalizedPotVal = map(potVal, 0, 682, 1, 100); // Maps the range of [0, 682] to [1, 100]
    normalizedPotVal = constrain(normalizedPotVal, 1, 100); // Compresses the range of [0, 682] to [1, 100]

    Serial.print("Normalized Potentiometer Value: "); 
    Serial.println(normalizedPotVal);
    
     // Check for 'set' button press
    if (digitalRead(setDistanceButton) == LOW) {
        delay(10); // Simple debounce
        if (digitalRead(setDistanceButton) == LOW) { // Check again after debounce
        Serial.println("Set Button Clicked");
        currentDisplay = 2; // Switch to display 2, or Set Distance Display. 
        lcd.clear();
        while (digitalRead(setDistanceButton) == LOW); // Wait for button release
        }
    }

    // Check for 'back' button press
    if (digitalRead(currentDistanceButton) == LOW) {
        delay(10); // Simple debounce
        if (digitalRead(currentDistanceButton) == LOW) { // Check again after debounce
        Serial.println("Back Button Clicked");
        if(currentDisplay = 2){
            safetyDistance = normalizedPotVal;
        }
        currentDisplay = 1; // Switch to display 1, or Current Distance Display
        lcd.clear();
        while (digitalRead(currentDistanceButton) == LOW); // Wait for button release
        }
    }
        
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = ((duration / 2) / 29.1) + (distance / 25);



    if (distance <= safetyDistance) // Checks if the current distance is less than or equal to the set safetyDistance
    {
        digitalWrite(alarmLight, HIGH); // Lights up Alarm LED 
        digitalWrite(alarmSound, HIGH); // Buzzes up Piezo Alarm
    }
    else
    {
        digitalWrite(alarmSound, LOW); // Turns Alarm LED off
        digitalWrite(alarmLight, LOW); // Turns Piezo Alarm off
    }

    if (currentDisplay == 1) { // Checks if current display is set to Current Distance Screen (or 1)
        displayDistance(); // Call Current Distance function 
    } else if (currentDisplay == 2) { // Checks if current display is set to Set Distance Screen (or 2)
        setDistanceScreen(); // Call Set Distance function
    }
}
void setDistanceScreen(){
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Set Distance");
    lcd.setCursor(4,1);
    lcd.print("    ");
    lcd.setCursor(4,1);
    lcd.print(normalizedPotVal);
    lcd.setCursor(10, 1);
    lcd.print("cm");
    delay(150);
}

void displayDistance(){
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Current Distance");
    lcd.setCursor(4,1);
    lcd.print("      ");
    lcd.setCursor(4,1);
    lcd.print(distance, DEC);
    lcd.setCursor(10, 1);
    lcd.print("cm");
    delay(150);
}
