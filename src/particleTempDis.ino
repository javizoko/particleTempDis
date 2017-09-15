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
}

void loop()
{

    //Enable D7 led. Start conversion
    digitalWrite(led1, HIGH);

    //Increase main loop samples
    samplesMainLoop++;

    //Request temperature conversion
    dallas.requestTemperatures();
    // get the temperature in Celcius
    float temp0 = dallas.getTempCByIndex(0);
    float temp1 = dallas.getTempCByIndex(1);

    //Request to accumulate mean
    tasksTemp.accumTempVal(temp0,temp1);

    if(samplesMainLoop >= oneSecSamples)
    {
        /* One second task */
        float meanTemp0 = -127.0;
        int numErrors0 = 0;
        float meanTemp1 = -127.0;
        int numErrors1 = 0;

        //Generate actual date Excel format
        String actualDate = generateExcelDate();
        //Calculate mean temperature
        meanTemp0= tasksTemp.calcMeanValues(0);
        meanTemp1= tasksTemp.calcMeanValues(1);
        //Calculate number of errors
        numErrors0 = tasksTemp.calcNumErrors(0);
        numErrors1 = tasksTemp.calcNumErrors(1);

        //Publish
        Particle.publish("DateAndTemp", actualDate + ";" +  String(meanTemp0) + ";" + String(numErrors0) + ";" +  String(meanTemp1) + ";" + String(numErrors1));

        //Reset samples
        samplesMainLoop = 0;

    }

  //Publish at loop sampling speed
  Particle.publish("InstTemp0", String(temp0));
  Particle.publish("InstTemp1", String(temp1));

  if((temp0 > -20.0) && (temp1 > -20.0))
  {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.print("Temperatu.");
    oled.setCursor(0,20);
    oled.print("Int: "); oled.print(String(int(temp0))); oled.print("."); oled.print(String(int((temp0 - int(temp0))*10.0))); oled.print("C");
    oled.setCursor(0,40);
    oled.print("Ext: "); oled.print(String(int(temp1))); oled.print("."); oled.print(String(int((temp1 - int(temp1))*10.0))); oled.print("C");

    oled.setTextColor(BLACK, WHITE); // 'inverted' text

    oled.display();
  }
  //Disable D7 led. End conversion
  digitalWrite(led1, LOW);
  delay(5000);
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
