/*
prev and curr hold current and previous averages of readings
 from potentiometers. 0-2 are read from the sliders, and 3 is
 from the trimpot
 */
float prev0 = 0;
float curr0 = 0;

float prev1 = 0;
float curr1 = 0;

float prev2 = 0;
float curr2 = 0;

float prev3 = 0;
float curr3 = 0;

/*
readings holds an array of 50 readings to smooth the input from
 the potentiometers
 */
//float readings0;
//float readings1;
//float readings2;
//float readings3;
float val_readings0[50];

float readings0[50];
float readings1[50];
float readings2[50];
float readings3[50];

float readings0_length = sizeof(readings0)/sizeof(*readings0);
float readings1_length = sizeof(readings1)/sizeof(*readings1);
float readings2_length = sizeof(readings2)/sizeof(*readings2);
float readings3_length = sizeof(readings3)/sizeof(*readings3);

/*
int i is used to increment through each readings array
 
 finger_scale is used to map the analog readings from the Arduino's
 0-1023 scale to the Barrett's finger scale of 0-2.5 radians
 
 spread scale is used to map the analog scale to a the barret 
 spread scale of 0-3.14 radians
 */
int i = 0;

float finger_scale = 2.5/1023;
float spread_scale = 3.14/1023;

/*
buffer holds the string that the Arduino will pass to the python code running
 the hand
 
 avg0-3 are the strings that hold the average from each potentiometer that will
 be written to the buffer string
 */

char buffer[25];

char avg0[5];
char avg1[5];
char avg2[5];
char avg3[5];

/*
Pins for reading potentiometers and sliders.
 */
int potentiometerPin = A1;
int slider1Pin = A4;
int slider2Pin = A3;
int slider3Pin = A2;

/*
Pins for switches for turning the sliders, potentiometer, and entire box on and off.
 */
int potentiometerSwitchPin = 5;
int fingerSwitch1Pin = 2;
int fingerSwitch2Pin = 3;
int fingerSwitch3Pin = 4;
int allSwitchPin = 6;

/*
Switches for turning the sliders and potentiometer on and off.  One switch for turning everything off.
 */
int potentiometerSwitch = 0;
int fingerSwitch1 = 0;
int fingerSwitch2 = 0;
int fingerSwitch3 = 0;
int allSwitch = 0;

/*
Initialize pins for switches
 */
void initializeSwitchPins(){
  pinMode(fingerSwitch1Pin, INPUT);
  pinMode(fingerSwitch2Pin, INPUT);
  pinMode(fingerSwitch3Pin, INPUT);
  pinMode(potentiometerSwitchPin, INPUT);
  pinMode(allSwitchPin, INPUT);
}

/*
Updates the values of all the switches
 */
void checkSwitches(){
  potentiometerSwitch = digitalRead(potentiometerSwitchPin);
  fingerSwitch1 = digitalRead(fingerSwitch1Pin);
  fingerSwitch2 = digitalRead(fingerSwitch2Pin);
  fingerSwitch3 = digitalRead(fingerSwitch3Pin);
  allSwitch = digitalRead(allSwitchPin);
}

/*
Reads the current value of the sensors and adds to the list
 */
void readSensors(){
  val_readings0[i] = analogRead(potentiometerPin);
  readings0[i] = map(val_readings0[i],0,700,0,1023);
  readings1[i] = analogRead(slider1Pin);
  readings2[i] = analogRead(slider2Pin);
  readings3[i] = analogRead(slider3Pin);
}

/*
Prints the current state of the switches -- debugging only
 */
void printSwitchStates(){
  String switchStates;
  switchStates = String(switchStates + "Switches--" + "\n");
  switchStates = String(switchStates + "Potentiometer: " + !potentiometerSwitch + "\n");
  switchStates = String(switchStates + "Finger 1: " + !fingerSwitch1 + "\n");
  switchStates = String(switchStates + "Finger 2: " + !fingerSwitch2 + "\n");
  switchStates = String(switchStates + "Finger 3: " + !fingerSwitch3 + "\n");
  Serial.println(switchStates);
}

/*
Prints the current state of the sliders and potentiometers -- debugging only
 */
void printSliderStates(){
  String sliderStates;
  sliderStates = String(sliderStates + "Sliders -- ");
  sliderStates = String(sliderStates + "Potentiometer: " + curr0 + " ");
  sliderStates = String(sliderStates + "Slider 1: " + curr1 + " ");
  sliderStates = String(sliderStates + "Slider 2: " + curr2 + " ");
  sliderStates = String(sliderStates + "Slider 3: " + curr3 + " ");
  Serial.println(sliderStates);
}

/*
Checks the values and see if they have changed significantly.  Not necessary because noise from pots is likely to cause a meaningful change in hand position.
 Returns true if any of the current averages changes by 3 -- 3 was the value of the old code.  Probably no harm in increase or decreasing this value.
 */
boolean change(){
  if(abs(curr0-prev0)>3 || abs(curr1-prev1)>3 || abs(curr2-prev2)>3 || abs(curr3-prev3)>3){
    return true;
  }
  else{
    return false;
  }
}

/*
Takes in array of floats and length of array, solves for average
 */
float average(float array[], int length){
  int k;
  float sum = 0;
  for(k = 0; k < length; k++){
    sum += array[k];
  }
  return sum/length;
}


/*
intialize Serial communication with 9600 baud rate
 initialize functions to get average and look for change in averages
 */
void setup(){
  Serial.begin(9600);
  float average(float* array, int length);
  boolean change();
  initializeSwitchPins();
}

void loop(){
  // if the whole slidy box is off, just don't do anything -- this will allow for the fingers to be moved.
  // Should the rotational position of the base of the fingers still be sent to hold them in place?
  checkSwitches();
  if (!allSwitch == 0){
    //Serial.println("0,0,0,0");
    return;
  }
  else{
    //Serial.println("Main Switch is on"); 
    //store analog values to readings arrays
    readSensors();
  
    //when array is full, take average
    if(i == 50){
      i = 0;

      // check switches
      checkSwitches();
      //printSwitchStates();
      //printSliderStates();

      // This will cause the slidey box to continue to send whatever the last value was when the switch was changed
      // If the switch starts off, the hand will be told to go to the zero position.
      // If the current position of the hand should be maintained, this will need to be done on the python side and send a message saying the switch has been flipped
      if (!potentiometerSwitch){
        curr3 = average(readings0, readings0_length);
      }
      if (!fingerSwitch1){
        curr1 = average(readings1, readings1_length);
      }
      if (!fingerSwitch2){
        curr2 = average(readings2, readings2_length);
      }
      if (!fingerSwitch3){
        curr0 = average(readings3, readings3_length);
      }
      //printSliderStates();
      //look for significant changes in averages
      if(change()){
        //if change occurs, write new averages to string and write to Serial
        dtostrf(curr0*finger_scale, 4, 2, avg0);
        dtostrf(curr1*finger_scale, 4, 2, avg1);
        dtostrf(curr2*finger_scale, 4, 2, avg2);
        dtostrf(curr3*spread_scale, 4, 2, avg3);
        sprintf(buffer, "%s, %s, %s, %s", avg0, avg1, avg2, avg3);

        Serial.println(buffer);
        prev0 = curr0;
        prev1 = curr1;
        prev2 = curr2;
        prev3 = curr3;
      }
    }
  }
  i++;
  delay(7);
}



