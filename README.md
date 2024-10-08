# MS5611_APP
 MS5611 pressure and temperature sensor application with SMT32 mcu
 work by i2c communication protocole, 
 SPI protocole will be added in soon.

First step ==> enter core files

Second step ==> Find ms5611.h  in Inc file and ms5611.c in Src file

Finally  ==> Adds these two files in your project, and don't forget look at main.c file. 


# FEATURES OF MS5611
• High resolution module, 10 cm

• Fast conversion down to 1 ms

• Low power, 1 µA (standby < 0.15 µA)

• Integrated digital pressure sensor (24 bit ΔΣ ADC)

• Operating range: 10 to 1200 mbar, -40 to +85 °C

• I2C and SPI interface up to 20 MHz



# DESCRIPTION
The MS5611-01BA is a new generation of high resolution altimeter sensors from MEAS Switzerland with SPI and I2C bus interface. 
This barometric pressure sensor is optimized for altimeters and variometers with an
altitude resolution of 10 cm. The sensor module includes a high linearity pressure sensor and an ultra low power
24 bit ΔΣ ADC with internal factory calibrated coefficients. It provides a precise digital 24 Bit pressure and
temperature value and different operation modes that allow the user to optimize for conversion speed and
current consumption. A high resolution temperature output allows the implementation of an
altimeter/thermometer function without any additional sensor.


# FIELD OF APPLICATION
• Mobile altimeter / barometer systems

• Bike computers

• Variometers

• Height sensing for medical alarms

• Indoor navigation


# Important links
•	[MS5611-01BA03-measurement.pdf](https://github.com/YEK-Kayra/MS5611_APP/files/13853362/MS5611-01BA03-measurement.pdf)

• [How to measure absolute pressure.pdf](https://github.com/YEK-Kayra/MS5611_APP/files/13834121/How.to.measure.absolute.pressure.pdf)

• [Example Code MS5611 AN520.pdf](https://github.com/YEK-Kayra/MS5611_APP/files/13853350/Example.Code.MS5611.AN520.pdf)


# Technical Sensor Data Table
![teknikData](https://github.com/YEK-Kayra/MS5611_APP/assets/124110070/77df8082-be08-43d6-8971-a5908d68bc02)



# Attention !
-You have to wait at least 20 milisecond to get datas from the sensor's chip

(1)Raw data read request at selected osrs value

(2)Indicate that you are ready to receive incoming data

(3)Save incoming data to arrays as msb, lsb, xlsb

Gets D1(Raw Pressure) ⇩

```
	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &osrs_4096_D1, 1, 1000); //(1)
	HAL_Delay(20);
	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &adcReadCom , 1, 1000);  //(2)	 
	HAL_Delay(20);
	HAL_I2C_Master_Receive(dev->i2c, dev->I2C_ADDRESS, &RawDataD1[0], 3, 1000);  //(3)	 
	dev->ClcPrms.D1 = (uint32_t)((RawDataD1[0]<<16) | (RawDataD1[1]<<8) | (RawDataD1[2]<<0)); // MSB|LSB|XLSB
```

  



