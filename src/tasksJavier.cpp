
/* TAREAS HECHAS POR MI */

/* INCLUDE FILES */
#include "tasksJavier.h"
#include <math.h>

/* DEFINES */
#define MIN_TEMP_DEGREES_VALID (float)-30.0
#define MAX_TEMP_DEGREES_VALID (float)60.0
#define SIZE_MOV_WIN_TEMP_ARRAYS 24

/* PRIVATE VARS */
//Temp0 vars
int fiveSecTast = 0;
int samplesOneMinute = 60000/5000;

/* PUBLIC VARS */
TempSensorData TempSensorsData;
MeanTempsData MeanTempData;

typedef float MovWinTempArray[SIZE_MOV_WIN_TEMP_ARRAYS];
MovWinTempArray MovWinTempData[NUM_TEMP_SENSORS];
movWinDataStr MovWinFilterStr[NUM_TEMP_SENSORS];

/* ADDRESES */
uint8_t externalSensorAddress[8] = {0x22,0x6C,0x3E,0x57,0x00,0x00,0x00,0x53};
uint8_t internalSensorAddress[8] = {0x22,0x03,0x09,0x57,0x00,0x00,0x00,0x61};
uint8_t connectorSensorAddress[8] = {0x22,0x24,0x24,0x57,0x00,0x00,0x00,0x46};

void tasksJavierTemp::init(void)
{
    for (int i = 0; i < NUM_TEMP_SENSORS; i++)
    {
      TempSensorsData.measuredTemp[i] = (float)-35.0 + (float)i;
      TempSensorsData.measuredAntTemp[i] = (float)-35.0 + (float)i;
      TempSensorsData.measNumErrors[i] = 0;

      initMovWinFilter(&MovWinFilterStr[i],&MovWinTempData[i][0], SIZE_MOV_WIN_TEMP_ARRAYS);
    }

    //Addreses initialization
    TempSensorsData.sensorAddrPointer[0] = &internalSensorAddress[0];
    TempSensorsData.sensorAddrPointer[1] = &externalSensorAddress[0];
}

/* NEW IMPLEMENTATION */
bool tasksJavierTemp::checkValidTempValue (TempSensorData* sensorsData)
{
  bool validTempReaded = TRUE;
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    if(!checkTempInRange(sensorsData->measuredTemp[i]))
    {
      validTempReaded = FALSE;
      sensorsData->measNumErrors[i]++;
      sensorsData->measuredTemp[i] = sensorsData->measuredAntTemp[i];
    }
    sensorsData->measuredAntTemp[i] = sensorsData->measuredTemp[i];
  }
  return validTempReaded;
}

bool tasksJavierTemp::checkTempInRange(float tempValue)
{
  if((tempValue > MIN_TEMP_DEGREES_VALID) && (tempValue < MAX_TEMP_DEGREES_VALID))
    return TRUE;
  else
    return FALSE;
}

bool tasksJavierTemp::meanTemperaturesTask()
{
  fiveSecTast++;

  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    movWinFilter(&MovWinFilterStr[i],&MovWinTempData[i][0],TempSensorsData.measuredTemp[i]);
    MeanTempData.temperatureMovWin[i] = MovWinFilterStr[i].accumValue/(float)MovWinFilterStr[i].size;
  }

  if(fiveSecTast >= samplesOneMinute)
  {
    for (int i = 0; i < NUM_TEMP_SENSORS; i++)
    {
      MeanTempData.measErrors[i] = TempSensorsData.measNumErrors[i];
    }

    resetCycleTask();

    fiveSecTast = 0;
    return TRUE;
  }
  else
    return FALSE;
}

void tasksJavierTemp::resetCycleTask()
{
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    TempSensorsData.measNumErrors[i] = 0;
  }
}

void tasksJavierTemp::movWinFilter(movWinDataStr* movWinData, float* arrayData, float actualValue)
{
  movWinData->accumValue -= *(arrayData + movWinData->index);
  *(arrayData + movWinData->index) = actualValue;
  movWinData->accumValue += actualValue;

  movWinData->index++;
  if(movWinData->index >= movWinData->size)
    movWinData->index = 0;
}

void tasksJavierTemp::initMovWinFilter(movWinDataStr* movWinData, float* arrayData, int sizeOfArray)
{
  movWinData->index = 0;
  movWinData->accumValue = 0.0;
  movWinData->size = sizeOfArray;

  //Initialize array to 0s
  for (int i = 0; i < movWinData->size; i++)
  {
    *(arrayData + i) = (float)0.0;
  }
}

void tasksJavierTemp::initMovWinFilterToValue(movWinDataStr* movWinData, float* arrayData, int sizeOfArray, float initDesiredValue)
{
  movWinData->index = 0;
  movWinData->accumValue = initDesiredValue * (float)sizeOfArray;
  movWinData->size = sizeOfArray;

  //Initialize array to 0s
  for (int i = 0; i < movWinData->size; i++)
  {
    *(arrayData + i) = (float)initDesiredValue;
  }
}

void tasksJavierTemp::initializeTempBuffers (void)
{
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    initMovWinFilterToValue(&MovWinFilterStr[i],&MovWinTempData[i][0], SIZE_MOV_WIN_TEMP_ARRAYS,TempSensorsData.measuredTemp[i]);
  }
}
