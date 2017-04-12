#include "Scale_HX711.h"

#if ARDUINO_VERSION <= 106
    // "yield" is not implemented as noop in older Arduino Core releases, so let's define it.
    // See also: https://stackoverflow.com/questions/34497758/what-is-the-secret-of-the-arduino-yieldfunction/34498165#34498165
    void yield(void) {};
#endif

Scale_HX711::Scale_HX711(byte dout, byte pd_sck, byte gain) {
  set_sensor_count(1);
  set_sensor(0,dout, pd_sck, gain);
}
Scale_HX711::Scale_HX711(int count) {
  set_sensor_count(count);
}
Scale_HX711::Scale_HX711() {
}

Scale_HX711::~Scale_HX711() {
}
void Scale_HX711::save() {
  EEPROM_write_block((unsigned char*)&settings, EEPROM_Address, sizeof(Scale_HX711_Settings));
}
void Scale_HX711::load() {
  EEPROM_read_block((unsigned char*)&settings, EEPROM_Address, sizeof(Scale_HX711_Settings));
}
void Scale_HX711::EEPROM_write_block(unsigned char *memory_block, unsigned int start_address, unsigned int block_size)
{
   unsigned char Count = 0;
   for (Count=0; Count<block_size; Count++){
       EEPROM.write(start_address + Count, memory_block[Count]);
   }
}
void Scale_HX711::EEPROM_read_block(unsigned char *memory_block, unsigned int start_address, unsigned int block_size)
{
   unsigned char Count = 0;
   for (Count=0; Count<block_size; Count++){
       memory_block[Count]= EEPROM.read(start_address + Count);
   }
}
void Scale_HX711::set_address(unsigned int Address){
  EEPROM_Address=Address;
}
void Scale_HX711::set_sensor_count(int count) {
  if (count<=SCALE_HX711_Max_Sensor_count){
    sensor_count=count;
  }
}
int Scale_HX711::get_sensor_count() {
  return sensor_count;
}
void Scale_HX711::set_sensor(unsigned int index,byte dout, byte pd_sck, byte gain) {
  if (index<sensor_count){
    sensor[index].Enable=true;
    sensor[index].DOUT=dout;
    sensor[index].PD_SCK=pd_sck;
    sensor[index].GAIN=gain;
    switch (gain) {
  		case 128:		// channel A, gain factor 128
  			sensor[index].GAIN_COUNT = 1;
  			break;
  		case 64:		// channel A, gain factor 64
  			sensor[index].GAIN_COUNT = 3;
  			break;
  		case 32:		// channel B, gain factor 32
  			sensor[index].GAIN_COUNT = 2;
  			break;
  	}
  }
}
void Scale_HX711::begin() {
  load();
  for (size_t i = 0; i < sensor_count; i++) {
    pinMode(sensor[i].PD_SCK, OUTPUT);
  	pinMode(sensor[i].DOUT, INPUT);
    digitalWrite(sensor[i].PD_SCK, LOW);
    read_one(i);

  }
}
bool Scale_HX711::is_ready(int index) {
	return digitalRead(sensor[index].DOUT) == LOW;
}

long Scale_HX711::read_one(int index) {
  //检测状态
  if (!sensor[index].Enable) return 0;
	// wait for the chip to become ready
	while (!is_ready(index)) {
		// Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
		yield();
	}

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// pulse the clock pin 24 times to read the data
	data[2] = shiftIn(sensor[index].DOUT, sensor[index].PD_SCK, MSBFIRST);
	data[1] = shiftIn(sensor[index].DOUT, sensor[index].PD_SCK, MSBFIRST);
	data[0] = shiftIn(sensor[index].DOUT, sensor[index].PD_SCK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < sensor[index].GAIN_COUNT; i++) {
		digitalWrite(sensor[index].PD_SCK, HIGH);
		digitalWrite(sensor[index].PD_SCK, LOW);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);
}

long Scale_HX711::read_one_avg(int index,int times) {
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += read_one(index);
		yield();
	}
	return sum / times;
}
long Scale_HX711::read(int times) {
  long allsum = 0;
  long sum;

	for (byte i = 0; i < times; i++) {
    sum = 0;
    for (size_t i = 0; i < sensor_count; i++) {
      sum +=read_one(i);
      yield();
    }
    allsum+=sum;
	}
	return allsum / times;
}
long Scale_HX711::get_value(int times) {
	return read(times) - settings.OFFSET;
}
void Scale_HX711::zero(int times) {
	long sum = read(times);
	set_offset(sum);
}
void Scale_HX711::adjust(long weight,int times){
  long now_read=get_value(times);
  float scale=now_read / weight;
  set_scale(scale);
}
void Scale_HX711::tare(int times) {
	zero(times);
}

float Scale_HX711::get_units(int times) {
	return get_value(times) / settings.SCALE;
}

void Scale_HX711::set_scale(float scale) {
	settings.SCALE = scale;
  save();
}

float Scale_HX711::get_scale() {
	return settings.SCALE;
}

void Scale_HX711::set_offset(long offset) {
	settings.OFFSET = offset;
  save();
}

long Scale_HX711::get_offset() {
	return settings.OFFSET;
}

void Scale_HX711::power_down() {
  for (size_t i = 0; i < sensor_count; i++) {
    digitalWrite(sensor[i].PD_SCK, LOW);
    digitalWrite(sensor[i].PD_SCK, HIGH);
    sensor[i].Enable=false;
  }

}

void Scale_HX711::power_up() {
  for (size_t i = 0; i < sensor_count; i++) {
    digitalWrite(sensor[i].PD_SCK, LOW);
    sensor[i].Enable=true;
    read_one_avg(i,5);
  }
}
