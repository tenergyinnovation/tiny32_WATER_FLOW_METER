/***********************************************************************
 * Project      :     tiny32_WATER-FLOW-METER_accumulated
 * Description  :     set and get water flow rate(m^3) accumulated from WATER-FLOW-METER
 * Hardware     :     tiny32_v3         
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     14/07/2022
 * Revision     :     1.0
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     admin@innovation.co.th
 * TEL          :     +66 82-380-3299
 ***********************************************************************/
#include <Arduino.h>
#include <tiny32_v3.h>
#include <EEPROM.h>

/**************************************/
/*        define object variable      */
/**************************************/
tiny32_v3 mcu; //define object

uint8_t id = 1; //current address of WATER-FLOW-METER, You can change here if it differance



/**************************************/
/*       Constand define value        */
/**************************************/

/**************************************/
/*       eeprom address define        */
/**************************************/
#define EEPROM_SIZE 1024
#define FLOW_RATE_ACCU_EEP      100  //4-byte [100-103]

/**************************************/
/*        define global variable      */
/**************************************/
float flow_rate; //ตัวแปรสำหรับเก็บค่าอัตราการไหลของน้ำ flow rate unit: m^3
float flow_rate_accu; //ตัวแปรสำหรับเก็บค่าปริมาณน้ำที่ใช้ไป Q: m^3




/****************************************************/
/*  define multi-tasking function [multitasking]    */
/****************************************************/
void Flow_rate_accu_Task(void *pvParameters);
TaskHandle_t Flow_rate_accu_var_Task = NULL;

/**************************************/
/*           define function          */
/**************************************/
#define RELAY_LED(time1,time2){mcu.BlueLED(1); vTaskDelay(time1); mcu.BlueLED(0); vTaskDelay(time2);} //Relay Time + LED blink
bool reset_flow_accu(void); //Reset water accumulte value from eeprom
bool stop_flow_rate_Task(void); //Stop multi tasking for read flow rate and accumulated water
bool resume_flow_rate_Task(void); //resume multi tasking for read flow rate and accumulated water

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/

void setup()
{
  Serial.begin(115200);
  Serial.printf("\r\n**** tiny32_WATER-FLOW-METER_accumulated ****\r\n");
  mcu.library_version();

  /*** eeprom reading ***/
  Serial.printf("Info: Initial eeprom ....");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
      Serial.println("\r\nError: failed to initialise EEPROM"); delay(1000000);
  }
  else
  {

      /*** flow_rate_accu ***/
      if(EEPROM.read(FLOW_RATE_ACCU_EEP)==0xFF && EEPROM.read(FLOW_RATE_ACCU_EEP+1)==0xFF && EEPROM.read(FLOW_RATE_ACCU_EEP+2)==0xFF && EEPROM.read(FLOW_RATE_ACCU_EEP+3)==0xFF)
      {
        flow_rate_accu = 0;  // new EEPROM, write typical voltage
        EEPROM.writeFloat(FLOW_RATE_ACCU_EEP,flow_rate_accu);
        EEPROM.commit();
      }
      else
      {
        flow_rate_accu = EEPROM.readFloat(FLOW_RATE_ACCU_EEP);
      }
      
      Serial.println("done");
  }
  Serial.printf("Info: flow_rate_accu[eeprom] => %.2f m^3\r\n",flow_rate_accu);

  mcu.WATER_FLOW_METER_begin(RXD2,TXD2);
  xTaskCreatePinnedToCore(Flow_rate_accu_Task,"Flow_rate_accu_Task",2048,NULL,2,&Flow_rate_accu_var_Task,0);
  mcu.buzzer_beep(2); //buzzer 2 beeps
}


 /***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{

}


 /***********************************************************************
 * FUNCTION:    Flow_rate_accu_Task
 * DESCRIPTION: Multitasking for accumulate water volume, running 
 *              every 1 second
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void Flow_rate_accu_Task(void *pvParameters)
{
  (void) pvParameters;
  static uint16_t _count = 0;
  static unsigned long _millis = 0;
  static float _flow_rate_accu_prev = 0;
  for(;;)
  {

    Serial.printf("Debug: Flow_rate_accu_Task spend time : %d mSec\r\n",millis() - _millis); _millis=millis(); //สำหรับตรวจสอบเวลาใน loop
    flow_rate = mcu.WATER_FLOW_METER_flowrate(id);
    flow_rate = 1;
    Serial.printf("Info: flow rate = %.2f m^2/h\r\n",flow_rate);
    flow_rate_accu += (flow_rate/3600);
    RELAY_LED(10,990);

    if(_count++ >= 30) //record to eeprom every 30 second
    {
        if(flow_rate_accu != _flow_rate_accu_prev) //program will not write eeprom without changed value
        {
          Serial.printf("\r\n*************************\r\n");
          Serial.printf("info: Q = %.2f m^3\r\n",flow_rate_accu);
          EEPROM.writeFloat(FLOW_RATE_ACCU_EEP,flow_rate_accu);
          EEPROM.commit();
          Serial.printf("\r\n*************************\r\n");
        }
        _count=0;
    }

    _flow_rate_accu_prev = flow_rate_accu;
  }
}


 /***********************************************************************
 * FUNCTION:    reset_flow_accu
 * DESCRIPTION: Reset water accumulte value from eeprom
 * PARAMETERS:  nothing
 * RETURNED:    true/false
 ***********************************************************************/
bool reset_flow_accu(void)
{

  flow_rate_accu = 0;
  EEPROM.writeFloat(FLOW_RATE_ACCU_EEP,flow_rate_accu);
  EEPROM.commit();
  Serial.printf("Info: Reset water accumulate ... successed\r\n");
  return 1;
}

 /***********************************************************************
 * FUNCTION:    stop_flow_rate_Task
 * DESCRIPTION: Stop multi tasking for read flow rate and accumulated water
 * PARAMETERS:  nothing
 * RETURNED:    true/false
 ***********************************************************************/
bool stop_flow_rate_Task(void)
{
  vTaskSuspend(Flow_rate_accu_var_Task);
  Serial.printf("Info: Stop water flow rate tasking... successed\r\n");
  return 1;
}

 /***********************************************************************
 * FUNCTION:    stop_flow_rate_Task
 * DESCRIPTION: Resume multi tasking for read flow rate and accumulated water
 * PARAMETERS:  nothing
 * RETURNED:    true/false
 ***********************************************************************/
bool resume_flow_rate_Task(void)
{
    vTaskResume(Flow_rate_accu_var_Task);
    Serial.printf("Info: Resume water flow rate tasking... successed\r\n");
    return 1;
}
