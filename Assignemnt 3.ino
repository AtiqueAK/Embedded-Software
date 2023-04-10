#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>


int led1 = 13;            //output port for LED of task 1
int t2_freq = 12;         //input port from signal generator to measure task-2 frequency
int t3_freq = 14;         //input port from signal geneerator to measure task-3 frequency
int pot_t4 = 27;          //input port from potentiometer to show analog frequency
int error_led = 26;       //output port to blink the led for error from potentiometer
const int buttonPin = 5;  //button pin for task 6 & 7
const int ledPin = 4;     //led pin for task 7
float frequency;          // variable declaration for task 2
float frequency2;         // variable declaration for task 3
SemaphoreHandle_t xButtonSemaphore; //variable declaration of button semaphore to assigned protection from 2 task
SemaphoreHandle_t xFrequencyMutex; //variable declaration of Mutex semaphore to assigned protection from 3 task
// Function prototypes

void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);
void Task4(void *pvParameters);
void Task5(void *pvParameters);
void Task6(void *pvParameters);
void Task7(void *pvParameters);

// Timer function
void outputTimerCallback(TimerHandle_t xTimer) {
  // Your timer callback logic here.
}

void setup(void) {
  xButtonSemaphore = xSemaphoreCreateBinary(); //creating binary semaphore for button acess
  xFrequencyMutex = xSemaphoreCreateMutex(); //creating Mutex semaphore to use in different task
  pinMode(led1, OUTPUT);             // set pin 2 as output for Task 1
  pinMode(t2_freq, INPUT);           // set pin 2 as input for Task 2
  pinMode(t3_freq, INPUT);           // set pin 2 as input for Task 3
  pinMode(pot_t4, INPUT);            // set pin 2 as input for Task 4
  pinMode(error_led, OUTPUT);        //Led pin output for Task 4
  pinMode(buttonPin, INPUT_PULLUP);  //Button pin input for Task 6
  pinMode(ledPin, OUTPUT);           //LED pin output for Task6

  // Initialize readings array with 0's
  Serial.begin(9600);
  
  // Task creation
  xTaskCreate(Task1, "Task1", 1024, NULL, 3, NULL);
  xTaskCreate(Task2, "Task2", 1024, NULL, 3, NULL);
  xTaskCreate(Task3, "Task3", 1024, NULL, 3, NULL);
  xTaskCreate(Task4, "Task4", 1024, NULL, 2, NULL);
  xTaskCreate(Task5, "Task5", 1024, NULL, 1, NULL);
  xTaskCreate(Task6, "Task6", 1024, NULL, 2, NULL);
  xTaskCreate(Task7, "Task7", 1024, NULL, 2, NULL);

  // Timer creation
  TimerHandle_t outputTimer = xTimerCreate("OutputTimer", pdMS_TO_TICKS(4), pdTRUE, (void *)0, outputTimerCallback);
  xTimerStart(outputTimer, 0);
}

void loop() {
  // Empty - all tasks
}

// Task 1, takes 0.9ms
void Task1(void *pvParameters) //Starting of Task 1 from assignment 2
{
  for (;;) {
    digitalWrite(led1, HIGH);  // set pin 2 high for 200us
    delayMicroseconds(200);
    digitalWrite(led1, LOW);  // set pin 2 low for 50us
    delayMicroseconds(50);
    digitalWrite(led1, HIGH);  // set pin 2 high for 30us
    delayMicroseconds(30);
    digitalWrite(led1, LOW);  // set pin 2 low for remaining period
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void Task2(void *pvParameters) //Starting of Task 2 from assignment 2
{
  for (;;) {
#define SAMPLES 10  // number of signal samples to take
    int count = 0;
    for (int i = 0; i < SAMPLES; i++) {
      count += pulseIn(t2_freq, HIGH);  // measure the pulse width of the input signal
    }
    count = count * 2;
    xSemaphoreTake(xFrequencyMutex, portMAX_DELAY);
    frequency = 1000000.0 / (count / SAMPLES);    // calculate frequency in Hz
    xSemaphoreGive(xFrequencyMutex);
    frequency = constrain(frequency, 333, 1000);  // bound frequency between 333 and 1000 Hz
  }
}

void Task3(void *pvParameters) //Starting of Task 3 from assignment 2
{
  for (;;) {
#define SAMPLES 8  // number of signal samples to take
    int count2 = 0;
    for (int i = 0; i < SAMPLES; i++) {
      count2 += pulseIn(t3_freq, HIGH);  // measure the pulse width of the input signal
    }
    count2 = count2 * 2;
    xSemaphoreTake(xFrequencyMutex, portMAX_DELAY);
    frequency2 = 1000000.0 / (count2 / SAMPLES);    // calculate frequency in Hz
    xSemaphoreGive(xFrequencyMutex);
    frequency2 = constrain(frequency2, 500, 1000);  // bound frequency between 500 and 1000 Hz
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void Task4(void *pvParameters)  //Starting of Task 4 from assignment 2
{
  for (;;) {
    const int maxAnalogIn = 1023;
    const int numReadings = 4;
    int readings[numReadings];
    int index = 0;
    int total = 0;
    int filteredValue = 0;
    for (int i = 0; i < numReadings; i++) {
      readings[i] = 0;
    }
    // Read  the analog input value
    int analogValue = analogRead(pot_t4);
    // Subtract the oldest reading from the total
    total -= readings[index];
    // Add the new reading to the total
    total += analogValue;
    // Store the new reading in the readings array
    readings[index] = analogValue;
    // Increment the index
    index++;
    // Wrap the index if it exceeds the number of readings
    if (index >= numReadings) {
      index = 0;
    }
    // Compute the filtered value as the average of the readings
    filteredValue = total / numReadings;
    // If the filtered value is greater than half of the maximum range, turn on the LED
    if (filteredValue > maxAnalogIn / 2) {
      digitalWrite(error_led, HIGH);

    } else {
      digitalWrite(error_led, LOW); // If the filtered value is lower than half of the maximum range, turn off the LED
    }
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void Task5(void *pvParameters)  //starting of Task 5 from assignment 2
{
  for (;;) {
    xSemaphoreTake(xFrequencyMutex, portMAX_DELAY);
    int task2Freq = map(frequency, 333, 1000, 0, 99); //map the saved frequency from task 2 between 0 to 99
    int task3Freq = map(frequency2, 500, 1000, 0, 99); //map the saved frequency from task 3 between 0 to 99
    xSemaphoreGive(xFrequencyMutex);
    Serial.println(task2Freq);
    Serial.println(task3Freq);
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void Task6(void *pvParameters) //Starting of Task 6 from assignment 3
{
  int buttonState = digitalRead(buttonPin);
  int lastButtonState = buttonState;
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceTime = 50;

  for (;;) {
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceTime) {
      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == LOW) {
          digitalWrite(ledPin, !digitalRead(ledPin));
          xSemaphoreGive(xButtonSemaphore);
        }
      }
    }

    lastButtonState = reading;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void Task7(void *pvParameters) {
  bool ledState = false;

  for (;;) {
    if (xSemaphoreTake(xButtonSemaphore, portMAX_DELAY) == pdTRUE) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
    }
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}