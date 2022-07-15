/***********************************************************************
 * Project      :     tiny32_WATER-FLOW-METER_getFlowrate
 * Description  :     get water flow rate(m^3) from WATER-FLOW-METER [1-252]
 * Hardware     :     tiny32         
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     14/07/2022
 * Revision     :     1.0
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     admin@innovation.co.th
 * TEL          :     +66 82-380-3299
 ***********************************************************************/
#include <Arduino.h>
#include <tiny32_v3.h>

tiny32_v3 mcu; //define object

uint8_t id = 1; //current address of WATER-FLOW-METER, You can change here if it differance

void setup()
{
  Serial.begin(115200);
  Serial.printf("\r\n**** tiny32_WATER-FLOW-METER_getFlowrate ****\r\n");
  mcu.library_version();
  mcu.WATER_FLOW_METER_begin(RXD2,TXD2);
  mcu.buzzer_beep(2); //buzzer 2 beeps
}

void loop()
{
    float _flowrate = mcu.WATER_FLOW_METER_flowrate(id);
    Serial.printf("water flow rate = %0.2f m^3/h\r\n",_flowrate);
    vTaskDelay(1000);

}