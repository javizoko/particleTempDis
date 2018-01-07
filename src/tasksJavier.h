#ifndef TASKSJAVIER_h
#define TASKSJAVIER_h

#include <Particle.h>

/* TAREAS HECHAS POR MI */
#define NUM_TEMP_SENSORS 2
#define TRUE 1
#define FALSE 0
struct TempSensorData
{
  float measuredTemp[NUM_TEMP_SENSORS];
  float measuredAntTemp[NUM_TEMP_SENSORS];
  float deltaTemp[NUM_TEMP_SENSORS];
  int measNumErrors[NUM_TEMP_SENSORS];
  int commNumErrors[NUM_TEMP_SENSORS];
  uint8_t* sensorAddrPointer[NUM_TEMP_SENSORS];
};

struct MeanTempsData
{
  float temperature[NUM_TEMP_SENSORS];
  int measErrors[NUM_TEMP_SENSORS];
  int commErrors[NUM_TEMP_SENSORS];
};

extern TempSensorData TempSensorsData;
extern MeanTempsData MeanTempData;
extern float tempBuffer[NUM_TEMP_SENSORS];
extern int numSamples[NUM_TEMP_SENSORS];

class tasksJavierTemp
{
public:
  void init(void);
  void accumTempVal(float temp0, float temp1);
  float calcMeanValues(int tempIndx);
  int calcNumErrors(int tempIndx);

  void checkValidTempValue (TempSensorData* sensorsData);
  bool meanTemperaturesTask();

private:
  bool checkTempInRange(float tempValue);
  void accumulateMeanValues(int );
  void resetCycleTask(void);

};



#endif //TASKSJAVIER_h
