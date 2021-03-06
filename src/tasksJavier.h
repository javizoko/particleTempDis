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
  int measNumErrors[NUM_TEMP_SENSORS];
  uint8_t* sensorAddrPointer[NUM_TEMP_SENSORS];
};

struct MeanTempsData
{
  float temperatureMovWin[NUM_TEMP_SENSORS];
  int measErrors[NUM_TEMP_SENSORS];
};

extern TempSensorData TempSensorsData;
extern MeanTempsData MeanTempData;

struct movWinDataStr{
  int index;
  int size;
  float accumValue;
};

class tasksJavierTemp
{
public:
  void init(void);
  void accumTempVal(float temp0, float temp1);

  bool checkValidTempValue (TempSensorData* sensorsData);
  bool meanTemperaturesTask();
  void initializeTempBuffers(void);

private:
  bool checkTempInRange(float tempValue);
  void resetCycleTask(void);
  void initMovWinFilter(movWinDataStr* movWinData, float* arrayData, int sizeOfArray);
  void movWinFilter(movWinDataStr* movWinData, float* arrayData, float actualValue);
  void initMovWinFilterToValue(movWinDataStr* movWinData, float* arrayData, int sizeOfArray, float initDesiredValue);

};



#endif //TASKSJAVIER_h
