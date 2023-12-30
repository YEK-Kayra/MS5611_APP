
#include "ms5611.h"

MS5611_StatusTypeDef MS5611_Init(MS5611_HandleTypeDef *dev){

	 if(HAL_I2C_IsDeviceReady(dev->i2c, dev->I2C_ADDRESS, 1, 1000) != HAL_OK){

			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	 }

	MS5611_Get_CalibCoeff(dev);

	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_Get_CalibCoeff(MS5611_HandleTypeDef *dev){

	uint8_t ClbCoef_Arr[12] = {0};
	uint8_t cnt = 0;
	uint8_t StartClbAddrs = 0xA2;

	HAL_I2C_Mem_Read(dev->i2c, dev->I2C_ADDRESS, StartClbAddrs, 1, ClbCoef_Arr , 12, 1000);

	dev->Clb_Cf.C1   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.C2   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.C3   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.C4   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.C5   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.C6   = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;
	dev->Clb_Cf.crc  = (uint16_t)((ClbCoef_Arr[cnt]<<8) | (ClbCoef_Arr[cnt+1]));  cnt+=2;

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

MS5611_StatusTypeDef MS5611_ScndOrd_Calc_Press_Temp(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.TEMP = dev->ClcPrms.TEMP - dev->ClcPrms.TEMP2;
	dev->ClcPrms.OFF  = dev->ClcPrms.OFF - dev->ClcPrms.OFF2;
	dev->ClcPrms.SENS = dev->ClcPrms.SENS - dev->ClcPrms.SENS2;
	return MS5611_OK;

}
MS5611_StatusTypeDef MS5611_Calc_Press(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.OFF  = (dev->Clb_Cf.C2 * pow(2,16)) + (((dev->Clb_Cf.C4 ) * (dev->ClcPrms.dT)) / pow(2,7));
	dev->ClcPrms.SENS = ((dev->Clb_Cf.C1 * pow(2,15)) + (dev->Clb_Cf.C3 + dev->ClcPrms.dT) / pow(2,8));
	dev->ClcPrms.P 	  = (((dev->ClcPrms.D1 * dev->ClcPrms.SENS / pow(2,21)) - dev->ClcPrms.OFF)/pow(2,15));

	return MS5611_OK;
}



