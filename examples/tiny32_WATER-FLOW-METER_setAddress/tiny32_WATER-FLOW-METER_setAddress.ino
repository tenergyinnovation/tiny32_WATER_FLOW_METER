/***********************************************************************
 * Project      :     tiny32_WATER-FLOW-METER_setAddress
 * Description  :     set Address for WATER-FLOW-METER [1-252]
 * Hardware     :     tiny32         
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     14/07/2022
 * Revision     :     1.0
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     admin@innovation.co.th
 * TEL          :     +66 82-308-3299
 ***********************************************************************/
#include <Arduino.h>
#include <tiny32_v3.h>

tiny32_v3 mcu; //define object

uint8_t id = 1; //current address of WATER-FLOW-METER, You can change here if it differance
uint8_t new_id = 2; //new address of WATER-FLOW-METER, You can change here if it differance

void setup()
{
  Serial.begin(115200);
  Serial.printf("\r\n**** tiny32_WATER-FLOW-METER_setAddress[1-252] ****\r\n");
  mcu.library_version();
  mcu.WATER_FLOW_METER_begin(RXD2,TXD2);
  mcu.buzzer_beep(2); //buzzer 2 beeps
}

void loop()
{

  Serial.printf("Info: Old ID: %d\r\n",id);
  Serial.printf("Info: New ID: %d\r\n",new_id);
  _response_id = mcu.WATER_FLOW_METER_SetAddress(id,new_id);

  /* check status */
  if(_response_id != -1)
  {
    Serial.printf("Info: Success to new address : %d\r\n",_response_id);
  }
  else
  {
    Serial.printf("Error: can't change address\r\n");
  }

  while(1);
}