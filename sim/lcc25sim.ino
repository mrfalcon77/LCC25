/* Pro Micro Test Code
   by: Stanislav Sokolov 
   Spectral Molecular Imaging Inc
   date: February 25, 2017
   version: 0.1
   license: Public Domain - please use this code however you'd like.
   It's provided as a learning tool.

   This code is provided to Simulate THORLABS LCC25 Liquid Crystal Controller device
   (https://www.thorlabs.com/thorproduct.cfm?partnumber=LCC25) 
   using SparkFun ProMicro's board (ATmega32U4 based) within a sketch
   (https://www.sparkfun.com/products/12587). Tested on Pro Micro - 3.3V/8MHz model, 
   should work with Pro Micro - 5V/16MHz or any Arduino based board.
   Only limited command set of LCC25 controller is implemented 
*/

int RXLED = 17;                 // The RX LED has a defined Arduino pin
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
int inByte = 0;                 // incoming serial byte

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 250;           // interval at which to blink (milliseconds)
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String cmd[] = {"*idn", "volt1", "volt2", "extern", "freq", "mode", "enable", "min", "max", "dwell", "increment", "test"}; //Array of supported commands
//boolean goodcmd = false;        // used in parsing commands. Shows whether command is recognized
int cmdfunc = -1;               // corresponds to supported command function. -1 corresponds to unrecognized commands

//Struct contains internal LCC25 param values to be stored and updated
struct LCC25
{
  float volt1;      //current Voltage1
  float volt2;      //current Voltage2
  int exter;        //current modulation mode - 0 for internal and 1 for external
  float freq;       //current modulation frequency between 5 and 150 Hz
  int mode;         //current output mode - 0 for Modulation, 1 for Voltage1, 2 - Voltage2
  int enable;       //Output - is disabled(0) or enabled(1)
  float minv;       //current starting voltage level for LC Test Mode Set
  float maxv;       //current ending voltage level for LC Test Mode Set
  int dwell;        //current dwell time for LC Test Mode
  float increment;  //current voltage step increment for LC Test Mode
};

LCC25 lcc = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
void setup()
{
  pinMode(RXLED, OUTPUT);  // Set RX LED as an output
  // TX LED is set as an output behind the scenes

  Serial.begin(115200); //This pipes to the serial monitor
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  Serial.println("LCC25 THORLABS Simulator");
}

void process(int cmdlet)
{
  switch (cmdlet) {
    case 0: //*idn?
      if (inputString.length() == 5 && inputString[4] == '?')
      {
        Serial.print("THORLABS LCC25\r");
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 1: //volt1
      if (inputString.length() == 6 && inputString[5] == '?')
      {
        Serial.print("\r" + String(lcc.volt1, 3) + '\r');
      }
      else if (inputString.length() > 6 && inputString[5] == '=' )
      {
        lcc.volt1 = inputString.substring(6).toFloat();
        interval = lcc.volt1*100 + 100;
        Serial.print("\rvolt1=" + String(lcc.volt1, 3) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 2: //volt2
      if (inputString.length() == 6 && inputString[5] == '?')
      {
        Serial.print("\r" + String(lcc.volt2, 3) + '\r');
      }
      else if (inputString.length() > 6 && inputString[5] == '=' )
      {
        lcc.volt2 = inputString.substring(6).toFloat();
        Serial.print("\rvolt2=" + String(lcc.volt2, 3) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 3: //extern
      if (inputString.length() == 7 && inputString[6] == '?')
      {
        Serial.print("\rextern=" + String(lcc.exter) + '\r');
      }
      else if (inputString.length() == 8 && (inputString.endsWith("=1" ) || inputString.endsWith("=0" )) )
      {
        lcc.exter = inputString.substring(7).toInt();
        Serial.print("\rextern=" + String(lcc.exter) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 4: //freq
      if (inputString.length() == 5 && inputString[4] == '?')
      {
        Serial.print("\r" + String(lcc.freq, 2) + '\r');
      }
      else if (inputString.length() > 5 && inputString[4] == '=' )
      {
        lcc.freq = inputString.substring(5).toFloat();
        Serial.print("\rfreq=" + String(lcc.freq, 2) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 5: //mode
      if (inputString.length() == 5 && inputString[4] == '?')
      {
        Serial.print("\rmode=" + String(lcc.mode) + '\r');
      }
      else if (inputString.length() == 6 && (inputString.endsWith("=1" ) || inputString.endsWith("=0" ) || inputString.endsWith("=2" )) )
      {
        lcc.mode = inputString.substring(5).toInt();
        Serial.print("\rmode=" + String(lcc.mode) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 6: //enable
      if (inputString.length() == 7 && inputString[6] == '?')
      {
        Serial.print("\renable=" + String(lcc.enable) + '\r');
      }
      else if (inputString.length() == 8 && (inputString.endsWith("=1" ) || inputString.endsWith("=0" )) )
      {
        lcc.enable = inputString.substring(7).toInt();
        Serial.print("\renable=" + String(lcc.enable) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 7: //min
      if (inputString.length() == 4 && inputString[3] == '?')
      {
        Serial.print("\rmin=" + String(lcc.minv, 3) + '\r');
      }
      else if (inputString.length() > 5 && inputString[3] == '=' )
      {
        lcc.minv = inputString.substring(4).toFloat();
        Serial.print("\rmin=" + String(lcc.minv, 3) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 8: //max
      if (inputString.length() == 4 && inputString[3] == '?')
      {
        Serial.print("\rmax=" + String(lcc.maxv, 3) + '\r');
      }
      else if (inputString.length() > 5 && inputString[3] == '=' )
      {
        lcc.maxv = inputString.substring(4).toFloat();
        Serial.print("\rmax=" + String(lcc.maxv, 3) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 9: //dwell
      if (inputString.length() == 6 && inputString[5] == '?')
      {
        Serial.print("\rdwell=" + String(lcc.dwell) + '\r');
      }
      else if (inputString.length() > 6 && inputString[5] == '=' )
      {
        lcc.dwell = inputString.substring(6).toInt();
        Serial.print("\rdwell=" + String(lcc.dwell) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 10: //increment
      if (inputString.length() == 10 && inputString[9] == '?')
      {
        Serial.print("\rincrement=" + String(lcc.increment, 3) + '\r');
      }
      else if (inputString.length() > 10 && inputString[9] == '=' )
      {
        lcc.increment = inputString.substring(10).toFloat();
        Serial.print("\rincrement=" + String(lcc.increment, 3) + '\r');
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;
    case 11: //test
      if (inputString.length() == 4 )
      {
        Serial.print("\rtest + '\r'");
      }
      else
      {
        Serial.print("Command error CMD_NOT_DEFINED\r");
      }
      break;


    default:
      // Unsupported cmd:
      Serial.print("Command error CMD_NOT_DEFINED\r");
  }
}

void loop() {
  // if we get a valid byte:
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\r') {
      stringComplete = true;
    }
  }   // print the string when a newline arrives:
  if (stringComplete) {
 //   goodcmd = false;
    cmdfunc = -1;
    inputString.trim();
    for (int i = 0; i < 12; i++) {
      //if (inputString.equals(cmd[i]))
      if (inputString.startsWith(cmd[i], 0))
      {
 //       goodcmd = true;
        cmdfunc = i;
        break;
      }
    }
    process(cmdfunc);

    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  digitalWrite(RXLED, ledState);   // set the LED on
  TXLED0; //TX LED is not tied to a normally controlled pin
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(RXLED, ledState);
  }
  TXLED1;
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
/*void serialEvent() {
  Serial.write('^');
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  }*/
