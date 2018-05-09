/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*Letter correspondances for the commands sent/recieved from the sensortag:
"T" - max temp
"t" - min temp
"a" - current temp
"P" - max pressure
"p" - min pressure
"r" - current pressure
"H" - max humidity
"h" - min humidity
"u" - current humidity
"R" - rain threshold
"w" - current rain percentage change
"b" - current battery level
"o" - called to switch off the alarm
"s" - called to check the alarm
*/


/*
 *  ======== emon.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>


//For all the sensors on the sensortag
#include "sensors/SensorI2C.h"
#include "sensors/SensorOpt3001.h"
#include "sensors/SensorBmp280.h"
#include "sensors/SensorHdc1000.h"
#include "sensors/SensorMpu9250.h"
#include "sensors/SensorTmp007.h"


/* Example/Board Header files */
#include "Board.h"

//Allows conversion for delay times
#include <ti/sysbios/knl/Clock.h>
//for battery monitor
#include <driverlib/aon_batmon.h>

//Environment monitor header file
#include "envservice.h"

#define TASKSTACKSIZE   512

//Allows for this to be called to delay for a set amount of ms
#define DELAY_MS(i) (Task_sleep(((i) * 1000) / Clock_tickPeriod))

//for which attribute you want to read
char g_currentReadAttribute;

//defining the variables, setting the actual values here doesn't seem to work
int g_maxtemp=1000000;
int g_mintemp=-1000000;
uint32_t g_maxpress=1000000;
uint32_t g_minpress=0;
uint32_t g_maxhum=1000000;
uint32_t g_minhum=0;
int g_minrain=1000000;
int g_pressureDifference=0;
int g_batteryLevel=0;

int32_t g_temp=0;
int32_t g_tempop2=0;
int32_t g_humd=0;
uint32_t g_pressure=0;
int32_t g_averageTemp=0;

int32_t g_rainPercentage=0;


//for storing alarm reason
int32_t g_lastAlarmValue=0;
char g_lastAlarmType=0;

bool g_buzzerOn;
bool g_buzzerOff;

//First task for the buzzer
Task_Struct tsk0Struct;
UInt8 tsk0Stack[TASKSTACKSIZE];
Task_Handle task;

//Second task for the other environment monitor things
Task_Struct tsk1Struct;
UInt8 tsk1Stack[TASKSTACKSIZE];
Task_Handle task2;


//Copied from project_zero.c for LED used
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;
/*
 * Initial LED pin configuration table
 *   - LEDs Board_LED0 & Board_LED1 are off.
 */
PIN_Config ledPinTable[] = {
  Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};


//For converting an integer to ascii
void my_itoa(int dataIn, uint8_t* result)
{

    for(int i=0;i<10;i++)
    {
        result[i]=0;
    }

    if (dataIn==0)
    {
        result[0] = '0';
        return;
    }



    if (dataIn < 0)
    {
        dataIn =- dataIn;
        *result = '-';
        result++;
    }

    int divisor=1000000;
    while(dataIn/divisor==0)
    {
        divisor=divisor/10;
    }

    while(divisor!=0)
    {
        int digit = dataIn/divisor;

        //+'0' so it is the ascii code rather than actual number
        *result = digit+'0';
        result++;

        dataIn=dataIn-(digit*divisor);
        divisor=divisor/10;
    }

    return;

}



//checking if any of the alarm criteria are met and if so switching on the alarm
Void AlarmTest()
{

    if (g_averageTemp>g_maxtemp)
    {
       g_buzzerOn=true;
       g_lastAlarmValue=g_averageTemp;
       g_lastAlarmType='T';
    }
      if (g_averageTemp<g_mintemp)
      {
         g_buzzerOn=true;
         g_lastAlarmValue=g_averageTemp;
         g_lastAlarmType='t';
      }
      if (g_pressure>g_maxpress)
      {
          g_buzzerOn=true;
          g_lastAlarmValue=g_pressure;
          g_lastAlarmType='P';
      }
      if (g_pressure<g_minpress)
      {
          g_buzzerOn=true;
          g_lastAlarmValue=g_pressure;
          g_lastAlarmType='p';
      }
      if (g_humd>g_maxhum)
      {
          g_buzzerOn=true;
          g_lastAlarmValue=g_humd;
          g_lastAlarmType='H';
      }
      if (g_humd<g_minhum)
      {
          g_buzzerOn=true;
          g_lastAlarmValue=g_humd;
          g_lastAlarmType='h';
      }

      if (g_rainPercentage>g_minrain)
      {
          g_buzzerOn=true;
          g_lastAlarmValue=g_rainPercentage;
          g_lastAlarmType='R';
      }

      if(g_buzzerOn==false)
      {
          g_buzzerOff=false;
      }

      if(g_buzzerOff==true)
      {
      g_buzzerOn=false;
      }

}

//prepares the data to send to the phone
Void updateResult()
{
    es_RESULTVal[1]=',';
    es_RESULTVal[0]=g_currentReadAttribute;
    switch (g_currentReadAttribute)
    {

                case 'T':
                    my_itoa(g_maxtemp,&es_RESULTVal[2]);
                    break;
                case 't':
                    my_itoa(g_mintemp,&es_RESULTVal[2]);
                    break;
                case 'e':

                    my_itoa(g_tempop2,&es_RESULTVal[2]);
                    break;
                case 'm':
                    my_itoa(g_temp,&es_RESULTVal[2]);
                    break;

                case 'a':
                    my_itoa(g_averageTemp,&es_RESULTVal[2]);
                    break;

                case 'P':
                    my_itoa(g_maxpress,&es_RESULTVal[2]);
                    break;

                case 'p':
                    my_itoa(g_minpress,&es_RESULTVal[2]);
                    break;
                case 'r':
                    my_itoa(g_pressure,&es_RESULTVal[2]);
                    break;

                case 'H':
                    my_itoa(g_maxhum,&es_RESULTVal[2]);
                    break;
                case 'h':
                    my_itoa(g_minhum,&es_RESULTVal[2]);
                    break;
                case 'u':
                    my_itoa(g_humd,&es_RESULTVal[2]);
                    break;

                case 'R':
                    my_itoa(g_minrain,&es_RESULTVal[2]);
                    break;


                case 'w':
                    my_itoa(g_rainPercentage,&es_RESULTVal[2]);
                    break;

                case 'o':
                    g_buzzerOff=true;
                    break;

                case 's':
                    es_RESULTVal[2]=g_lastAlarmType;
                    es_RESULTVal[3]=',';
                    my_itoa(g_lastAlarmValue,&es_RESULTVal[4]);
                    break;

                case 'b':
                    my_itoa(g_batteryLevel,&es_RESULTVal[2]);
                    break;

                case 'x':
                    my_itoa(g_pressureDifference,&es_RESULTVal[2]);
                    break;


            }


    //So the buzzer won't buzz again if it's been switched of and still meeting the alarm condition that set if off
    if(g_buzzerOn==false)
    {
        g_buzzerOff=false;
    }

    if(g_buzzerOff==true)
    {
    g_buzzerOn=false;
    }

}

//To read the sensortag sensors
Void sensorRead(UArg arg0, UArg arg1)
{
    //need to initialise values here as initialising globally doesn't work.
    g_maxtemp=50;
    g_mintemp=-10;
    g_maxpress=1200;
    g_minpress=300;
    g_maxhum=100;
    g_minhum=0;
    g_minrain=100;
    g_pressureDifference=0;
    g_batteryLevel=0;

    //To makesure the i2c bus is open
    if (SensorI2C_open())
    {

        SensorBmp280_init();            // Pressure Sensor

        SensorBmp280_enable(true);

        SensorHdc1000_init();   //temp + humidity

        SensorHdc1000_start();

        AONBatMonEnable();

    }
    else
    {

        Log_error0("Error initializing sensors\n");
        System_abort("Error initializing sensors\n");
    }




    //used to store the raw data
    uint16_t rawTemp;
    uint16_t rawHumd;

    uint8_t rawData[8];    //needs 6 bytes, 8 just to be safe

    //For determining change in pressure to decide likelihood of rain
    uint16_t currentIndex = 0;
    int32_t storedPressure[16];

   //array for the stored pressures
    for (int i=0;i<16;i++)
    {
        storedPressure[i]=0;
    }

    //main loop that happens once a minute
    while (1) {

        //switch green LED on
        PIN_setOutputValue(ledPinHandle, Board_LED0, 1);

        //Reading the sensors
        SensorHdc1000_start(); //So that it rereads each reading

        g_batteryLevel = AONBatMonBatteryVoltageGet();

        DELAY_MS(20);     //20ms delay

        SensorHdc1000_read(&rawTemp,&rawHumd);

        DELAY_MS(20);     //takes at least 20ms  to convert

        SensorBmp280_enable(true);

        DELAY_MS(20);

        SensorBmp280_read(rawData);


        //converting the raw values into actual understandable values
        g_temp=((rawTemp*165)/65536)-40;
        g_humd=((rawHumd*100)/65536);


        DELAY_MS(20);     //takes at least 20ms to convert

        //Converting the other values from the other sensor
        SensorBmp280_convert(rawData, &g_tempop2, &g_pressure);


        //for storing pressures for the rain percentage prediction
        int32_t pressureDifference = g_pressure-storedPressure[currentIndex];
        g_pressureDifference=pressureDifference;
        storedPressure[currentIndex]=g_pressure;
        g_pressure /= 100;
        currentIndex++;
        //So that it wraps around if it reaches the 16th position in the array
        currentIndex&=0xF;

        g_rainPercentage=0;

        //Very basic rain percentage prediction
        if(pressureDifference<0)
        {

            if(g_pressure<1020)
            {
                if(g_pressure<870)
                {
                    g_rainPercentage=100;
                }
                else
                {
                    g_rainPercentage=100-((g_pressure-870)*100/(1020-870));

                }
            }
        }

        //calculating the average temperature from the two different temperature sensors for a better reading
        g_averageTemp = (((g_temp)+(g_tempop2/100))/2);


        DELAY_MS(20);

        //switch off the LED
        PIN_setOutputValue(ledPinHandle, Board_LED0, 0);

        //Check if any alarm criteria are met
        AlarmTest();

        //So that it only updates once per minute
        DELAY_MS(59900);

    }

}

//For the buzzer functionality for the alarm (modified from the original pwmled.c)
/*
 *  ======== pwmBuzzerFxn ========
 *  Task periodically increments the PWM duty for the on board LED.
 */
Void pwmLEDFxn(UArg arg0, UArg arg1)
{
    PWM_Handle pwm1;
    PWM_Params params;
    uint16_t   pwmPeriod = 300;      // Period and duty in microseconds
    uint16_t   duty = 0;
    uint16_t   dutyInc = 100;

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;
    pwm1 = PWM_open(Board_PWM2, &params);
   // pwm1 = PWM_open(Board_PWM1, &params);

    if (pwm1 == NULL)
    {
        System_abort("Board_PWM0 did not open");
    }

    /* Loop forever incrementing the PWM duty */
    while(1) {
        if (g_buzzerOn) {

            //switch on the red LED
            PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

            PWM_start(pwm1);
            //for loop so it buzzes for a short time with intervals
            for(int i=0; i<100; i++)
            {
                PWM_setDuty(pwm1, duty);
                duty = (duty + dutyInc);
                if (duty == pwmPeriod || (!duty)) {
                    dutyInc = - dutyInc;
                }

                Task_sleep((UInt) arg0);
            }
        }

        //switch off the red LED
        PIN_setOutputValue(ledPinHandle, Board_LED1, 0);

        //So it is not a continuous buzz for the alarm
        PWM_stop(pwm1);
        Task_sleep(5000000 / Clock_tickPeriod);

    }

}

/*
 *  ======== main ========
 */
//Modified from int main(void) in the original pwmled.c file
int initBuzzer(void)
{
    //Set the buzzer to be off
    g_buzzerOn=false;

    Task_Params tskParams;
    Task_Params tskParams2;

    /* Call board init functions. */
    //Board_initGeneral();    //init pins..now done in main
    //Board_initGPIO();
    Board_initPWM();


    /* Construct Buzzer Task thread */
    Task_Params_init(&tskParams);
    tskParams.stackSize = TASKSTACKSIZE;
    tskParams.stack = &tsk0Stack;
    tskParams.arg0 = 50;
    Task_construct(&tsk0Struct, (Task_FuncPtr)pwmLEDFxn, &tskParams, NULL);

    /* Obtain instance handle */
    task = Task_handle(&tsk0Struct);

    //Second thread for actual environment monitor readings etc
    Task_Params_init(&tskParams2);
    tskParams2.stackSize = TASKSTACKSIZE;
    tskParams2.stack = &tsk1Stack;
    tskParams2.arg0 = 50;
    Task_construct(&tsk1Struct, (Task_FuncPtr)sensorRead, &tskParams2, NULL);


    /* Turn on user LED */
    //GPIO_write(Board_LED0, Board_LED_ON);

    //So the LEds can be switched on/off - copied from project_zero.c
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
      if(!ledPinHandle) {
        Log_error0("Error initializing board LED pins");
        Task_exit();
      }

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    BIOS_start();
    return (0);
}
