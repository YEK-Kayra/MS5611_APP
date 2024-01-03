
#include "ms5611.h"

/******************************************************************************
         			#### MS5611 VARIABLES ####
******************************************************************************/
/*! Pressure oversampling rates*/
uint8_t osrs_256_D1  = 0x40;
uint8_t osrs_512_D1  = 0x42;
uint8_t osrs_1024_D1 = 0x44;
uint8_t osrs_2048_D1 = 0x46;
uint8_t osrs_4096_D1 = 72;

/*! Temperature oversampling rates*/
uint8_t osrs_256_D2  = 0x50;
uint8_t osrs_512_D2  = 0x52;
uint8_t osrs_1024_D2 = 0x54;
uint8_t osrs_2048_D2 = 0x56;
uint8_t osrs_4096_D2 = 88;

/*! Command value for request getting raw pressure and temperature value*/
uint8_t adc_read = 0;

/******************************************************************************
         			#### MS5611 FUNCTIONS ####
******************************************************************************/
MS5611_StatusTypeDef MS5611_Init(MS5611_HandleTypeDef *dev){

	MS5611_Reset(dev);
	 if(HAL_I2C_IsDeviceReady(dev->i2c, dev->I2C_ADDRESS, 1, 10000) != HAL_OK){

			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	 }

	MS5611_Reset(dev);
	HAL_Delay(10);

	MS5611_Get_CalibCoeff(dev);

		return MS5611_OK;
}



MS5611_StatusTypeDef MS5611_Reset(MS5611_HandleTypeDef *dev){

	uint8_t ResetCom = 0x1E;

	HAL_I2C_Mem_Write(dev->i2c, dev->I2C_ADDRESS, dev->I2C_ADDRESS, 1, &ResetCom, 1, 1000);
	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_Get_CalibCoeff(MS5611_HandleTypeDef *dev){

	uint8_t CalibCoefAddrss[7] = {0xA2, //C1
								  0xA4,	//C2
								  0xA6, //C3
								  0xA8, //C4
								  0xAA, //C5
								  0xAC, //C6
								  0xAE}; /*! The last address is for CRC*/

	uint8_t TempryCalibCoefVal[2] = {0};
	uint8_t CalibCoefVal[14] = {0};
	uint8_t cnt = 0;


	for(cnt = 0; cnt <= 6 ; cnt++){

		HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &CalibCoefAddrss[cnt], 1, 1000);

		for(int cnt_2 = 0; cnt_2 <= 1 ; cnt_2++){

			HAL_I2C_Master_Receive(dev->i2c, dev->I2C_ADDRESS, &TempryCalibCoefVal[cnt_2], 2, 1000);

		}

		CalibCoefVal[(cnt*2)]   = TempryCalibCoefVal[0];
		CalibCoefVal[(cnt*2+1)] = TempryCalibCoefVal[1];

	}
	cnt = 0;

	dev->Clb_Cf.C1  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.C2  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.C3  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.C4  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.C5  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.C6  = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;
	dev->Clb_Cf.crc = ((CalibCoefVal[cnt]<<8) | CalibCoefVal[cnt+1]); cnt+=2;

return MS5611_OK;
}


MS5611_StatusTypeDef MS5611_ReadRaw_Press_Temp(MS5611_HandleTypeDef *dev){

	uint8_t RawDataD1[3]  = {0}; /*! D1 = Raw pressure value that will be compensated at other functions*/
	uint8_t RawDataD2[3]  = {0}; /*! D2 = Raw temperature value that will be compensated at other functions*/

	/**
	 * (1)Raw data read request at selected osrs value
	 * (2)Indicate that you are ready to receive incoming data
	 * (3)Save incoming data to arrays as msb, lsb, xlsb
	 */
	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &osrs_4096_D1, 1, 1000); //(1)
	HAL_Delay(20);
	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &adc_read , 1, 1000);	 //(2)
	HAL_Delay(20);
	HAL_I2C_Master_Receive(dev->i2c, dev->I2C_ADDRESS, &RawDataD1[0], 3, 1000);	 //(3)
	dev->ClcPrms.D1 = (uint32_t)((RawDataD1[0]<<16) | (RawDataD1[1]<<8) | (RawDataD1[2]<<0)); // MSB|LSB|XLSB

	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &osrs_4096_D2, 1, 1000); //(1)
	HAL_Delay(20);
	HAL_I2C_Master_Transmit(dev->i2c, dev->I2C_ADDRESS, &adc_read , 1, 1000);  	 //(2)
	HAL_Delay(20);
	HAL_I2C_Master_Receive(dev->i2c, dev->I2C_ADDRESS, &RawDataD2[0], 3, 1000);  //(3)
	dev->ClcPrms.D2 = (uint32_t)((RawDataD2[0]<<16) | (RawDataD2[1]<<8) | (RawDataD2[2]<<0)); // MSB|LSB|XLSB

	return MS5611_OK;

}

MS5611_StatusTypeDef MS5611_Calc_Temp_Press(MS5611_HandleTypeDef *dev){

	MS5611_ReadRaw_Press_Temp(dev);

	/*! Calculate 1st order temperature and pressure  according to MS5611 1st order algorithm */
	dev->ClcPrms.dT   = dev->ClcPrms.D2 - dev->Clb_Cf.C5 * pow(2,8);
	dev->ClcPrms.TEMP = 2000 + dev->ClcPrms.dT * dev->Clb_Cf.C6 / pow(2,23);
	dev->ClcPrms.OFF  = dev->Clb_Cf.C2 * pow(2,16) + (dev->Clb_Cf.C4 * dev->ClcPrms.dT) / pow(2,7);
	dev->ClcPrms.SENS = dev->Clb_Cf.C1 * pow(2,15) + (dev->Clb_Cf.C3 * dev->ClcPrms.dT) / pow(2,8);
	dev->ClcPrms.P	  = 2 * (dev->ClcPrms.D1 * dev->ClcPrms.SENS / pow(2,21) - dev->ClcPrms.OFF) / pow(2,15);

	/*! Calculate 2st order temperature and pressure  according to MS5611 2st order algorithm */
	if(dev->ClcPrms.TEMP < 2000){

		/*! Low Temperature */
		dev->ClcPrms.TEMP2 = (dev->ClcPrms.dT * dev->ClcPrms.dT) / pow(2,31);
		dev->ClcPrms.OFF2  = 5 * ((dev->ClcPrms.TEMP - 2000) * (dev->ClcPrms.TEMP - 2000)) / pow(2,1);
		dev->ClcPrms.SENS2 = 5 * ((dev->ClcPrms.TEMP - 2000) * (dev->ClcPrms.TEMP - 2000)) / pow(2,2);

				if(dev->ClcPrms.TEMP < -1500){
					/*! Very Low Temperature */
					dev->ClcPrms.OFF2  = dev->ClcPrms.OFF2 + 7 * ((dev->ClcPrms.TEMP + 1500) * (dev->ClcPrms.TEMP + 1500));
					dev->ClcPrms.SENS2 = dev->ClcPrms.SENS2 + 11 * ((dev->ClcPrms.TEMP + 1500) * (dev->ClcPrms.TEMP + 1500)) / pow(2,1);

					dev->ClcPrms.TEMP = dev->ClcPrms.TEMP - dev->ClcPrms.TEMP2;
					dev->ClcPrms.OFF  = dev->ClcPrms.OFF - dev->ClcPrms.OFF2;
					dev->ClcPrms.SENS = dev->ClcPrms.SENS - dev->ClcPrms.SENS2;

				}
				else{

					dev->ClcPrms.TEMP = dev->ClcPrms.TEMP - dev->ClcPrms.TEMP2;
					dev->ClcPrms.OFF  = dev->ClcPrms.OFF - dev->ClcPrms.OFF2;
					dev->ClcPrms.SENS = dev->ClcPrms.SENS - dev->ClcPrms.SENS2;

				}


	}
	else{
		/*! High Temperature */
		dev->ClcPrms.TEMP2 = 0;
		dev->ClcPrms.OFF2  = 0;
		dev->ClcPrms.SENS2 = 0;

		dev->ClcPrms.TEMP = dev->ClcPrms.TEMP - dev->ClcPrms.TEMP2;
		dev->ClcPrms.OFF  = dev->ClcPrms.OFF - dev->ClcPrms.OFF2;
		dev->ClcPrms.SENS = dev->ClcPrms.SENS - dev->ClcPrms.SENS2;

	}
	MS5611_Altitude = (SeaLevelTemp / GradientTemp) * (1 - pow((dev->ClcPrms.P / SeaLevelPress),((GasCoefficient * GradientTemp)/GravityAccel)));
	MS5611_Press = dev->ClcPrms.P ;
	MS5611_Temp  = dev->ClcPrms.TEMP ;

	return MS5611_OK;
}
