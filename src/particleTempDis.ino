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

void setup()
{
  pinMode(led1, OUTPUT);
  // setup the library
  dallas.begin();
  //Initialize Javier's tasks
  tasksTemp.init();
  Time.zone(+2);
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
  }

  tasksTemp.checkValidTempValue(&TempSensorsData);
  endOfCycle = tasksTemp.meanTemperaturesTask();

  if(endOfCycle)
  {
    //Generate actual date Excel format
    String actualDate = generateExcelDate();

    //Publish
    Particle.publish("DateAndTemp", actualDate + ";" +  String(MeanTempData.temperature[0]) + ";" + String(MeanTempData.measErrors[0]) + ";" + String(MeanTempData.commErrors[0]) + ";" +  String(MeanTempData.temperature[1]) + ";" + String(MeanTempData.measErrors[1]) + ";" + String(MeanTempData.commErrors[1]));
  }

  //PRINT LCD
  {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.print("Temperatu.");
    oled.setCursor(0,20);
    oled.print("Int: "); oled.print(String(int(MeanTempData.temperature[0]))); oled.print("."); oled.print(String(int((MeanTempData.temperature[0] - int(MeanTempData.temperature[0]))*10.0))); oled.print("C");
    oled.setCursor(0,40);
    oled.print("Ext: "); oled.print(String(int(MeanTempData.temperature[1]))); oled.print("."); oled.print(String(int((MeanTempData.temperature[1] - int(MeanTempData.temperature[1]))*10.0))); oled.print("C");

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
