
/* TAREAS HECHAS POR MI */

/* INCLUDE FILES */
#include "tasksJavier.h"
#include <math.h>

/* DEFINES */
#define MIN_TEMP_DEGREES_VALID (float)-20.0
#define MAX_TEMP_DEGREES_VALID (float)50.0

/* PRIVATE VARS */
//Temp0 vars
int m_tasksJavierTemp_nSamplesTemp0 = 0;
int m_tasksJavierTemp_nErrorsTemp0 = 0;
float m_tasksJavierTemp_bufferTemp0 = 0.0;

//Temp1 vars
int m_tasksJavierTemp_nSamplesTemp1 = 0;
int m_tasksJavierTemp_nErrorsTemp1 = 0;
float m_tasksJavierTemp_bufferTemp1 = 0.0;

int fiveSecTast = 0;
int samplesOneMinute = 60000/5000;

/* PUBLIC VARS */
TempSensorData TempSensorsData;
MeanTempsData MeanTempData;
float tempBuffer[NUM_TEMP_SENSORS];
int numSamples[NUM_TEMP_SENSORS];

/* ADDRESES */
uint8_t externalSensorAddress[8] = {0x22,0x6C,0x3E,0x57,0x00,0x00,0x00,0x53};
uint8_t internalSensorAddress[8] = {0x22,0x03,0x09,0x57,0x00,0x00,0x00,0x61};
uint8_t connectorSensorAddress[8] = {0x22,0x24,0x24,0x57,0x00,0x00,0x00,0x46};

void tasksJavierTemp::init(void)
{
    //Temp0 buffer initialization
    m_tasksJavierTemp_nSamplesTemp0 = 0;
    m_tasksJavierTemp_nErrorsTemp0 = 0;
    m_tasksJavierTemp_bufferTemp0 = 0.0;

    //Temp1 buffer initialization
    m_tasksJavierTemp_nSamplesTemp1 = 0;
    m_tasksJavierTemp_nErrorsTemp1 = 0;
    m_tasksJavierTemp_bufferTemp1 = 0.0;

    for (int i = 0; i < NUM_TEMP_SENSORS; i++)
    {
      TempSensorsData.measuredTemp[i] = (float)-35.0 + (float)i;
      TempSensorsData.measuredAntTemp[i] = (float)-35.0 + (float)i;
      TempSensorsData.deltaTemp[i] = 0;
      TempSensorsData.measNumErrors[i] = 0;
      TempSensorsData.commNumErrors[i] = 0;

      tempBuffer[i] = 0;
      numSamples[i] = 0;
    }

    //Addreses initialization
    TempSensorsData.sensorAddrPointer[0] = &internalSensorAddress[0];
    TempSensorsData.sensorAddrPointer[1] = &externalSensorAddress[0];
}

void tasksJavierTemp::accumTempVal(float temp0, float temp1)
{
    //Temp0 buffer is incresed
    if((temp0 > MIN_TEMP_DEGREES_VALID) && (temp0 < MAX_TEMP_DEGREES_VALID))
    {
        m_tasksJavierTemp_nSamplesTemp0++;
        m_tasksJavierTemp_bufferTemp0 += temp0;
    }
    else
    {
        //Error in temperature measurement
        m_tasksJavierTemp_nErrorsTemp0++;
    }

    //Temp1 buffer is incresed
    if((temp1 > MIN_TEMP_DEGREES_VALID) && (temp1 < MAX_TEMP_DEGREES_VALID))
    {
        m_tasksJavierTemp_nSamplesTemp1++;
        m_tasksJavierTemp_bufferTemp1 += temp1;
    }
    else
    {
        //Error in temperature measurement
        m_tasksJavierTemp_nErrorsTemp1++;
    }

}

float tasksJavierTemp::calcMeanValues(int tempIndx)
{
    float meanTemp = 0;

    if(tempIndx == 0)
    {
        if(m_tasksJavierTemp_nSamplesTemp0 > 0)
        {
            meanTemp = (float)((float)m_tasksJavierTemp_bufferTemp0/(float)m_tasksJavierTemp_nSamplesTemp0);
        }
        else
        {
            meanTemp = (float)-19.0;
        }

        m_tasksJavierTemp_bufferTemp0 = (float)0.0;
        m_tasksJavierTemp_nSamplesTemp0 = (float)0.0;
    }

    if(tempIndx == 1)
    {
        if(m_tasksJavierTemp_nSamplesTemp1 > 0)
        {
            meanTemp = (float)((float)m_tasksJavierTemp_bufferTemp1/(float)m_tasksJavierTemp_nSamplesTemp1);
        }
        else
        {
            meanTemp = (float)-19.0;
        }

        m_tasksJavierTemp_bufferTemp1 = (float)0.0;
        m_tasksJavierTemp_nSamplesTemp1 = (float)0.0;
    }

    return (float)meanTemp;
}

int tasksJavierTemp::calcNumErrors(int tempIndx)
{
    int numErrors = 0;

    if(tempIndx == 0)
    {
        numErrors = m_tasksJavierTemp_nErrorsTemp0;
        m_tasksJavierTemp_nErrorsTemp0 = 0;
    }

    if(tempIndx == 1)
    {
        numErrors = m_tasksJavierTemp_nErrorsTemp1;
        m_tasksJavierTemp_nErrorsTemp1 = 0;
    }

    return numErrors;
}


/* NEW IMPLEMENTATION */
void tasksJavierTemp::checkValidTempValue (TempSensorData* sensorsData)
{
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    if(!checkTempInRange(sensorsData->measuredTemp[i]))
    {
      sensorsData->measNumErrors[i]++;
      sensorsData->measuredTemp[i] = sensorsData->measuredAntTemp[i];
    }
/*
    //Calculate deltaTemp
    sensorsData->deltaTemp[i] = fabs(sensorsData->measuredTemp[i] - sensorsData->measuredAntTemp[i]);

    //Difference between samples bigger to 2.0
    if(sensorsData->deltaTemp[i] > 2.0)
    {
      float otherSensorTemp = 0;
      if(i < (NUM_TEMP_SENSORS-1))
        otherSensorTemp = sensorsData->measuredTemp[i+1];
      else
        otherSensorTemp = sensorsData->measuredTemp[0];

      //Check temperature equal to other sensor
      if(sensorsData->measuredTemp[i] == otherSensorTemp)
      {
        //Error
        sensorsData->measuredTemp[i] = sensorsData->measuredAntTemp[i];
        sensorsData->commNumErrors[i]++;
      }
    }
*/
    sensorsData->measuredAntTemp[i] = sensorsData->measuredTemp[i];
  }

}

bool tasksJavierTemp::checkTempInRange(float tempValue)
{
  if((tempValue > -40.0) && (tempValue < 60.0))
    return TRUE;
  else
    return FALSE;
}

bool tasksJavierTemp::meanTemperaturesTask()
{
  fiveSecTast++;

  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    accumulateMeanValues(i);
  }

  if(fiveSecTast >= samplesOneMinute)
  {
    for (int i = 0; i < NUM_TEMP_SENSORS; i++)
    {
      MeanTempData.temperature[i] = tempBuffer[i]/(float)numSamples[i];
      MeanTempData.measErrors[i] = TempSensorsData.measNumErrors[i];
      MeanTempData.commErrors[i] = TempSensorsData.commNumErrors[i];
    }

    resetCycleTask();

    fiveSecTast = 0;
    return TRUE;
  }
  else
    return FALSE;
}

void tasksJavierTemp::accumulateMeanValues(int i)
{
  if(TempSensorsData.measuredTemp[i] > (float)-40.0)
  {
    tempBuffer[i] += TempSensorsData.measuredTemp[i];
    numSamples[i]++;
  }
}

void tasksJavierTemp::resetCycleTask()
{
  for (int i = 0; i < NUM_TEMP_SENSORS; i++)
  {
    TempSensorsData.measNumErrors[i] = 0;
    TempSensorsData.commNumErrors[i] = 0;
    tempBuffer[i] = 0;
    numSamples[i] = 0;
  }
}
