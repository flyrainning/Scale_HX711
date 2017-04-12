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
Serial.println(scale.get_units(5));

scale.zero(); // 复位归零
Serial.println("\nput 100g weight,wait 10s to adjust");
delay(10000);
scale.adjust(100);
Serial.println("adjust finished:");
Serial.print("offset:\t");
Serial.println(scale.get_offset());
Serial.print("scale:\t");
Serial.println(scale.get_scale());

Serial.println("After setting up the scale:");

Serial.print("read: \t\t");
Serial.println(scale.read());

Serial.print("read average: \t\t");
Serial.println(scale.read(20));

Serial.print("get value: \t\t");
Serial.println(scale.get_value(5));

Serial.print("get units: \t\t");
Serial.println(scale.get_units(5));


Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units());
  Serial.print("\traw:\t");
  Serial.print(scale.read());
  Serial.print("\t| average 10 times:\t");
  Serial.println(scale.get_units(10));

  scale.power_down();
  delay(5000);
  scale.power_up();

}
