
#include "ms5611.h"

MS5611_StatusTypeDef MS5611_Init(MS5611_HandleTypeDef *dev){

	 if(HAL_I2C_IsDeviceReady(dev->i2c, dev->I2C_ADDRESS, 1, 1000) != HAL_OK){

			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	 }

	MS5611_Get_CalibCoeff(dev);

		return MS5611_OK;
}


//MS5611 are reset and if it's not completed successfully the sensor is initialized one more time
MS5611_StatusTypeDef MS5611_Reset(MS5611_HandleTypeDef *dev){

	uint8_t ResetCom = 0x1E;

	if((HAL_I2C_Mem_Write(dev->i2c, dev->I2C_ADDRESS, dev->I2C_ADDRESS, 1, &ResetCom, 1, 1000)) != HAL_OK){

			if((MS5611_Init(dev)) == MS5611_OK){

				return MS5611_OK;

			}

	}
	else{

		return MS5611_OK;

	}

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

//ADC Registers will be reading for raw pressure and temperature values
MS5611_StatusTypeDef MS5611_ReadRaw_Press_Temp(MS5611_HandleTypeDef *dev){

}

MS5611_StatusTypeDef MS5611_Calc_Temp(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.dT   = ((dev->ClcPrms.D2 - dev->Clb_Cf.C5) - pow(2,8));
	dev->ClcPrms.TEMP = (20 + (dev->ClcPrms.dT * dev->Clb_Cf.C6));


	if(dev->ClcPrms.TEMP < 20){

		dev->ClcPrms.TEMP2 = dev->ClcPrms.dT / pow(2,31);
		dev->ClcPrms.OFF2  = (5 * pow((dev->ClcPrms.TEMP - 2000),2) / 2);
		dev->ClcPrms.SENS2 = (5 * pow((dev->ClcPrms.TEMP - 2000),2) / 4);

				if(dev->ClcPrms.TEMP < -15){

					dev->ClcPrms.OFF2  = dev->ClcPrms.OFF2 + 7 * pow((dev->ClcPrms.TEMP + 1500),2);
					dev->ClcPrms.SENS2 = (dev->ClcPrms.SENS2 + 11 * pow((dev->ClcPrms.TEMP + 1500),2) / 2);
					MS5611_ScndOrd_Calc_Press_Temp(dev);

				}
				else{

					MS5611_ScndOrd_Calc_Press_Temp(dev);

				}


	}
	else{

		/*!İf the temperature higher than 20 celcius */
		dev->ClcPrms.TEMP2 = 0;
		dev->ClcPrms.OFF2  = 0;
		dev->ClcPrms.SENS2 = 0;
		MS5611_ScndOrd_Calc_Press_Temp(dev);

	}

	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_Calc_Press(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.OFF  = (dev->Clb_Cf.C2 * pow(2,16)) + (((dev->Clb_Cf.C4 ) * (dev->ClcPrms.dT)) / pow(2,7));
	dev->ClcPrms.SENS = ((dev->Clb_Cf.C1 * pow(2,15)) + (dev->Clb_Cf.C3 + dev->ClcPrms.dT) / pow(2,8));
	dev->ClcPrms.P 	  = (((dev->ClcPrms.D1 * dev->ClcPrms.SENS / pow(2,21)) - dev->ClcPrms.OFF)/pow(2,15));

	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_ScndOrd_Calc_Press_Temp(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.TEMP = dev->ClcPrms.TEMP - dev->ClcPrms.TEMP2;
	dev->ClcPrms.OFF  = dev->ClcPrms.OFF - dev->ClcPrms.OFF2;
	dev->ClcPrms.SENS = dev->ClcPrms.SENS - dev->ClcPrms.SENS2;
	return MS5611_OK;

}




