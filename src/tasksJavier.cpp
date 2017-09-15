
/* TAREAS HECHAS POR MI */

/* INCLUDE FILES */
#include "tasksJavier.h"

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
}

void tasksJavierTemp::accumTempVal(float temp0, float temp1)
{
    //Temp0 buffer is incresed
    if((temp0 > MIN_TEMP_DEGREES_VALID) && (temp0 < MAX_TEMP_DEGREES_VALID))
    {
        m_tasksJavierTemp_nSamplesTemp0++;
        m_tasksJavierTemp_bufferTemp0 = m_tasksJavierTemp_bufferTemp0 + temp0;
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
        m_tasksJavierTemp_bufferTemp1 = m_tasksJavierTemp_bufferTemp1 + temp1;
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
            meanTemp = m_tasksJavierTemp_bufferTemp0/m_tasksJavierTemp_nSamplesTemp0;
        }
        else
        {
            meanTemp = -19.0;
        }

        m_tasksJavierTemp_bufferTemp0 = 0.0;
        m_tasksJavierTemp_nSamplesTemp0 = 0.0;
    }

    if(tempIndx == 1)
    {
        if(m_tasksJavierTemp_nSamplesTemp1 > 0)
        {
            meanTemp = m_tasksJavierTemp_bufferTemp1/m_tasksJavierTemp_nSamplesTemp1;
        }
        else
        {
            meanTemp = -19.0;
        }

        m_tasksJavierTemp_bufferTemp1 = 0.0;
        m_tasksJavierTemp_nSamplesTemp1 = 0.0;
    }

    return meanTemp;
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
