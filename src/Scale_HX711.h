#ifndef SCALE_HX711_h
#define SCALE_HX711_h

#define SCALE_HX711_Settings_Address 500
#define SCALE_HX711_Max_Sensor_count 16

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EEPROM.h"


typedef struct _Scale_HX711_Settings//设置信息
{
   long OFFSET;// 归零偏移量
   float SCALE;// 单位换算
}   Scale_HX711_Settings;

typedef struct _Scale_HX711_Sensor//传感器信息
{
  bool Enable=false;//是否启用
  bool isError=false;//是否异常，为多传感器自动屏蔽损坏传感器功能预留
  byte PD_SCK;	// 电源和串口时钟
  byte DOUT;		// 串口数据
  byte GAIN=128;		// 放大系数
  uint8_t GAIN_COUNT=0;//从GAIN转换的需要发送的脉冲次数，由set_sensor自动计算
}   Scale_HX711_Sensor;

class Scale_HX711
{
	private:
    Scale_HX711_Settings settings;//设置
    unsigned int sensor_count=0;//传感器数量
    Scale_HX711_Sensor sensor[SCALE_HX711_Max_Sensor_count];//传感器
    unsigned int EEPROM_Address=SCALE_HX711_Settings_Address;//设置保存地址

	public:
		Scale_HX711(byte dout, byte pd_sck, byte gain = 128);//直接初始化一个传感器
    Scale_HX711(int count);//初始化时设置传感器数量
		Scale_HX711();//直接初始化，需单独设置传感器数量和每个传感器后才能使用

		virtual ~Scale_HX711();

    //保存，读取设置
    void save();
    void load();
    void EEPROM_write_block(unsigned char *memory_block, unsigned int start_address, unsigned int block_size);
    void EEPROM_read_block(unsigned char *memory_block, unsigned int start_address, unsigned int block_size);
    //设置配置信息在EEPROM中的存放地址
    void set_address(unsigned int Address=SCALE_HX711_Settings_Address);
    //设置传感器数量
    void set_sensor_count(int count=1);
    int get_sensor_count();
    //设置传感器参数
    // gain: 128 or 64 for channel A; channel B works with 32 gain factor only
    void set_sensor(unsigned int index,byte dout, byte pd_sck, byte gain = 128);

    //启动
    void begin();
		// check if HX711 is ready
		// from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
		// input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
		bool is_ready(int index);

		// 读取单一传感器，等待芯片完成转换并读数
		long read_one(int index);

		// 读取单一传感器的平均值
		long read_one_avg(int index,int times = 10);

    //读取所有传感器的值，读取times取平均值
    long read(int times = 1);

    //返回去皮重后的值(read(times) - OFFSET)
    long get_value(int times = 1);

    //归零，自动设置OFFSET并保存
		void tare(int times = 20);
    void zero(int times = 20);

    //自动校准，weight 当前称上的重量（g），times 校准次数
    //校准过程：首先调用zero归零去皮重，称上放入砝码，调用adjust(砝码重量)
    void adjust(long weight=1,int times=50);

		// 获取带单位的值 get_value() / SCALE
		float get_units(int times = 1);

		// 设置单位计算常量 SCALE，并保存
		void set_scale(float scale = 1.f);

		// 读取单位计算常量 SCALE
		float get_scale();

		// 设置OFFSET并保存
		void set_offset(long offset = 0);

		// 读取 OFFSET
		long get_offset();

		// 设置芯片 power down
		void power_down();

		// 唤醒芯片
		void power_up();
};

#endif /* SCALE_HX711_h */
