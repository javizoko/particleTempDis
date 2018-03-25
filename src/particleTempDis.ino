// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_SSD1306.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_GFX.h"

// This #include statement was automatically added by the Particle IDE.
#include "tasksJavier.h"

// This #include statement was automatically added by the Particle IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "DS18.h"

// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"


//Use I2C with OLED RESET pin on D4
#define OLED_RESET D4
Adafruit_SSD1306 oled(OLED_RESET);

//JSS test led propio
int led1 = D7;

// -----------------
// Read temperature
// -----------------

// Data wire is plugged into port 0 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(D6);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&oneWire);

//Setup Javier's temperature tasks instance
tasksJavierTemp tasksTemp;

/* PRIVATE VARS */
//Task times vars
int mainLoopDelayMs = 5000;
int oneSecSamples = 60000/mainLoopDelayMs;
int samplesMainLoop = 0;
float tempInt, tempExt;

bool resolutionIsSet = FALSE;
bool tempReadIsValid = FALSE;
bool firstValidTempMeasureIsDone = FALSE;

void setup()
{
  pinMode(led1, OUTPUT);
  // setup the library
  dallas.begin();
  //Initialize Javier's tasks
  tasksTemp.init();
  Time.zone(+1);
  //Setup display
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  //Serial begin
  Serial.begin(115200);
}

void loop()
{
  bool endOfCycle = FALSE;

  //Enable D7 led. Start conversion
  digitalWrite(led1, HIGH);

  //Set resulution of the sensors
  if(resolutionIsSet == FALSE)
  {
    bool setOk = TRUE;
    for (int i = 0; i < NUM_TEMP_SENSORS; i++)
    {
      if(!dallas.setResolution(TempSensorsData.sensorAddrPointer[i],(uint8_t)10))
        setOk = FALSE;
    }

    if(setOk)
      resolutionIsSet = TRUE;
  }

  //Increase main loop samples
  samplesMainLoop++;

  //Request & get temperature
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    dallas.requestTemperaturesByAddress(TempSensorsData.sensorAddrPointer[i]);
    TempSensorsData.measuredTemp[i] = dallas.getTempC(TempSensorsData.sensorAddrPointer[i]);
    Serial.print("Temp Sensor: ");
    Serial.println(i);
    Serial.println(TempSensorsData.measuredTemp[i]);
    delay(100);
  }

  tempReadIsValid = tasksTemp.checkValidTempValue(&TempSensorsData);

  if(tempReadIsValid && !firstValidTempMeasureIsDone)
  {
    //Initialize moving windows.
    tasksTemp.initializeTempBuffers();
  }

  if(tempReadIsValid)
    firstValidTempMeasureIsDone = TRUE;

  if(firstValidTempMeasureIsDone)
  {
    endOfCycle = tasksTemp.meanTemperaturesTask();

    if(endOfCycle)
    {
      //Generate actual date Excel format
      String actualDate = generateExcelDate();

      //Publish
      Particle.publish("DateAndTemp", actualDate + ";" +  String(MeanTempData.temperature[0]) + ";" + String(MeanTempData.measErrors[0]) + ";" + String(MeanTempData.commErrors[0]) + ";" +  String(MeanTempData.temperature[1]) + ";" + String(MeanTempData.measErrors[1]) + ";" + String(MeanTempData.commErrors[1]));
      Serial.println("Publish now...");
    }
    //PRINT TEMPERATURES IN DISPLAY
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.print("TEMPERATURA MEDIA MIN");
    oled.setCursor(0,10);
    oled.print("Interior: "); oled.print(String(int(MeanTempData.temperatureMovWin[0]))); oled.print("."); oled.print(String(int((MeanTempData.temperatureMovWin[0] - int(MeanTempData.temperatureMovWin[0]))*10.0))); oled.print("C");
    oled.setCursor(0,20);
    oled.print("Exterior: "); oled.print(String(int(MeanTempData.temperatureMovWin[1]))); oled.print("."); oled.print(String(int((MeanTempData.temperatureMovWin[1] - int(MeanTempData.temperatureMovWin[1]))*10.0))); oled.print("C");
    oled.setCursor(0,35);
    oled.print("TEMPERATURAS INST");
    oled.setCursor(0,45);
    oled.print("Int: "); oled.print(String(int(TempSensorsData.measuredTemp[0]))); oled.print("."); oled.print(String(int((TempSensorsData.measuredTemp[0] - int(TempSensorsData.measuredTemp[0]))*10.0))); oled.print("C "); oled.print("-Err:");  oled.print(String(int(TempSensorsData.measNumErrors[0])));
    oled.setCursor(0,55);
    oled.print("Ext: "); oled.print(String(int(TempSensorsData.measuredTemp[1]))); oled.print("."); oled.print(String(int((TempSensorsData.measuredTemp[1] - int(TempSensorsData.measuredTemp[1]))*10.0))); oled.print("C "); oled.print("-Err:");  oled.print(String(int(TempSensorsData.measNumErrors[1])));

    oled.setTextColor(BLACK, WHITE); // 'inverted' text
    oled.display();
  }
  else
  {
    //First valid temperature measurement is not already get...
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.print("Esperando lectura valida de temperatura...");
    oled.setCursor(0,35);
    oled.print("TEMPERATURAS INST");
    oled.setCursor(0,45);
    oled.print("Int: "); oled.print(String(int(TempSensorsData.measuredTemp[0]))); oled.print("."); oled.print(String(int((TempSensorsData.measuredTemp[0] - int(TempSensorsData.measuredTemp[0]))*10.0))); oled.print("C "); oled.print("-Err:");  oled.print(String(int(TempSensorsData.measNumErrors[0])));
    oled.setCursor(0,55);
    oled.print("Ext: "); oled.print(String(int(TempSensorsData.measuredTemp[1]))); oled.print("."); oled.print(String(int((TempSensorsData.measuredTemp[1] - int(TempSensorsData.measuredTemp[1]))*10.0))); oled.print("C "); oled.print("-Err:");  oled.print(String(int(TempSensorsData.measNumErrors[1])));
    oled.setTextColor(BLACK, WHITE); // 'inverted' text
    oled.display();
  }

  //Disable D7 led. End conversion
  digitalWrite(led1, LOW);
  delay(mainLoopDelayMs);
}

String generateExcelDate(void)
{
  String dateExcel;
  //Test print time
  int day = Time.day();
  int month = Time.month();
  int year = Time.year();
  int hour = Time.hour();
  int minute =  Time.minute();
  int second =  Time.second();

  dateExcel = String(day) + "/" + String(month) + "/" + String(year) + " " + String(hour) + ":" + String(minute) + ":" + String(second);

  return dateExcel;
}
