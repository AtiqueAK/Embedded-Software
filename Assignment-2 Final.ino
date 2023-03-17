#include <B31DGMonitor.h>
B31DGCyclicExecutiveMonitor monitor;
// Trace program solution for cyclic executive example
// Author: Mauro Dragone

#define FRAME_DURATION_MS 4     // 4ms
int led1=13; //output port for LED of task 1 
int t2_freq=12;//input port from signal generator to measure task-2 frequency
int t3_freq=14;//input port from signal geneerator to measure task-3 frequency
int pot_t4=27;//input port from potentiometer to show analog frequency
int error_led=26;//output port to blink the led for error from potentiometer

unsigned long frameTime = 0;//Initializing frameTimer
unsigned long frameCounter = 0;//Initializing frameCounter

//----------------------------------------------------------------------------------------------------------------------------------------------
void setup(void)
{
  monitor.startMonitoring();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Ready");
  pinMode(led1, OUTPUT); // set pin 2 as output for Task 1
  pinMode(t2_freq, INPUT); // set pin 2 as input for Task 2
  pinMode(t3_freq, INPUT); // set pin 2 as input for Task 3
  pinMode(pot_t4, INPUT); // set pin 2 as input for Task 4
  pinMode(error_led, OUTPUT); //Led pin output for Task 4
  // Initialize readings array with 0's

    
}
//----------------------------------------------------------------------------------------------------------------------------------------------


  // Increase frame counter and reset it after 10 frames


void frame() {
  unsigned int slot = frameCounter % 10;

   switch (slot) {
     case 0: JobTask1();JobTask3();break;
     case 1: JobTask1();JobTask2();break;
     case 2: JobTask1();JobTask3();break;
     case 3: JobTask1();JobTask4();break;
     case 4: JobTask1();JobTask3();break;
     case 5: JobTask1();JobTask2();break;
     case 6: JobTask1();JobTask3();break;
     case 7: JobTask1();JobTask4();break;
     case 8: JobTask1();JobTask3();break;
     case 9: JobTask1();    
}
}



//----------------------------------------------------------------------------------------------------------------------------------------------
void loop(void) // Single time slot function of the Cyclic Executive (repeating)
{
  /*
  unsigned long bT = micros();
    JobTask4();
  
  unsigned long timeItTook = micros()-bT;
  Serial.print("Duration SerialOutput Job = ");
  Serial.print(timeItTook);
  exit(0);
  */
frame();// TO-DO: wait the next frame
JobTask5();

}

// Task 1, takes 1ms
void JobTask1(void) 
{
  monitor.jobStarted(1);
  Serial.println("Task 1 Started");
  digitalWrite(led1, HIGH); // set pin 2 high for 200us
  delayMicroseconds(200);
  digitalWrite(led1, LOW); // set pin 2 low for 50us
  delayMicroseconds(50);
  digitalWrite(led1, HIGH); // set pin 2 high for 30us
  delayMicroseconds(30);
  digitalWrite(led1, LOW); // set pin 2 low for remaining period
  //delayMicroseconds(1720); // wait for 4ms minus the time spent in the loop
  monitor.jobEnded(1);
  Serial.println("Task 1 Done");
} 

// Task 2, takes 3ms
void JobTask2(void) 
{
   monitor.jobStarted(2);
   Serial.println("Task 2 Started");
   int count = 0;
   count += pulseIn(t2_freq, HIGH); // measure the pulse width of the input signal which is high
  count = count*2;//Pulse width*2 to calculate positive and negetive pulse as whole waveform
  float frequency = 1000000.0 / (count ); //SAMPLES); // calculate frequency in Hz
  frequency = constrain(frequency, 333, 1000); // bound frequency between 333 and 1000 Hz
  int scaled_frequency = map(frequency, 333, 1000, 0, 99); // scale frequency between 0 and 99 for
  Serial.println("Frequency_1:"); // output frequency value to serial port
  Serial.println(frequency); // output frequency value to serial port
  //delayMicroseconds(800); // wait for 20ms minus the time spent in the loop
  Serial.println("Task 2 Done");
  monitor.jobEnded(2); 
} 

// Task 3, takes 3ms
void JobTask3(void) 
{
  monitor.jobStarted(3);
  int count2 = 0;
  count2 += pulseIn(t3_freq, HIGH); // measure the pulse width of the input signal that is high
  count2 = count2*2; //Pulse width*2 to calculate positive and negetive pulse as whole waveform
  float frequency2 = 1000000.0 / (count2); // SAMPLES); // calculate frequency in Hz
  frequency2 = constrain(frequency2, 500, 1000); // bound frequency between 500 and 1000 Hz
  int scaled_frequency2 = map(frequency2, 500, 1000, 0, 99); // scale frequency between 0 and 99
  Serial.println("Frequency_2:"); // output frequency value to serial port
  Serial.println(frequency2); // output frequency value to serial port
  //delayMicroseconds(920); // wait for 8ms minus the time spent in the loop
  monitor.jobEnded(3);
} 


// Task 4, takes 1ms
void JobTask4(void) 
{
  monitor.jobStarted(4);
  const int maxAnalogIn = 1023;
  const int numReadings = 4;
  int readings[numReadings];
  int index = 0;
  int total = 0;
  int filteredValue = 0;
  for (int i = 0; i < numReadings; i++) 
  {
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
  if (index >= numReadings)
   {
    index = 0;
  }
  // Compute the filtered value as the average of the readings
  filteredValue = total / numReadings;
  // If the filtered value is greater than half of the maximum range, turn on the LED
  if (filteredValue > maxAnalogIn / 2) {
    digitalWrite(error_led, HIGH);
    Serial.println("error led HIGH");
 
  } else {
    digitalWrite(error_led, LOW);

  }
  // Send the filtered value to the serial port
  Serial.println(filteredValue);
  monitor.jobEnded(4);
}

  // Task 5
void JobTask5(void) 
{
  int task2Freq = 0;
  int task3Freq = 0;
    // Measure the frequency of Task 2 signal
  task2Freq = pulseIn(t2_freq, HIGH, 20000) == 0 ? 0 : 1000000 / pulseIn(t2_freq, HIGH, 20000);
  // Scale and bound the frequency between to 0-99
  task2Freq = map(t2_freq, 333, 1000, 0, 99);
  // Measure the frequency of Task 3 signal
  task3Freq = pulseIn(t3_freq, HIGH, 8000) == 0 ? 0 : 1000000 / pulseIn(t2_freq, HIGH, 8000);
  // Scale and bound the frequency value between 0-99
  task3Freq = map(t3_freq, 500, 1000, 0, 99);
  // Send the frequency values to the serial port
  Serial.println(task2Freq);//To print frequency of given waveform of Task2
  Serial.println(task3Freq);//To print frequency of given waveform of Task3
}