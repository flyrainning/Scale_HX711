#include <Arduino.h>

#include "Scale_HX711.h"

Scale_HX711 scale;


void setup() {

Serial.begin(115200);
Serial.println("Scale_HX711 Demo");

Serial.println("Initializing the scale");

//设置2个传感器
scale.set_sensor_count(2);
//设置每个传感器的pin (index,dout,pd_sck,gain=128)
scale.set_sensor(0,A0,A1);
scale.set_sensor(1,A2,A3);
scale.begin();

Serial.println("Before setting up the scale:");
Serial.print("read: \t\t");
Serial.println(scale.read());

Serial.print("read average: \t\t");
Serial.println(scale.read(20));

Serial.print("get value: \t\t");
Serial.println(scale.get_value(5));

Serial.print("get units: \t\t");
Serial.println(scale.get_units(5), 1);

scale.zero(); // reset the scale to 0
Serial.println("\nput 100g weight,wait 10s to adjust");
delay(10000);
scale.adjust(100); // this value is obtained by calibrating the scale with known weights; see the README for details
Serial.println("adjust finished:");
Serial.print("offset:\t");
Serial.println(scale.get_offset());
Serial.print("scale:\t");
Serial.println(scale.get_scale());

Serial.println("After setting up the scale:");

Serial.print("read: \t\t");
Serial.println(scale.read());                 // print a raw reading from the ADC

Serial.print("read average: \t\t");
Serial.println(scale.read(20));       // print the average of 20 readings from the ADC

Serial.print("get value: \t\t");
Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

Serial.print("get units: \t\t");
Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
          // by the SCALE parameter set with set_scale

Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\traw:\t");
  Serial.print(scale.read());
  Serial.print("\t| average 10 times:\t");
  Serial.println(scale.get_units(10), 1);

  scale.power_down();			        // put the ADC in sleep mode
  delay(5000);
  scale.power_up();

}
