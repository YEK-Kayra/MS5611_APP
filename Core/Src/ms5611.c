
#include "ms5611.h"

MS5611_StatusTypeDef MS5611_Init(MS5611_HandleTypeDef *dev){

	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_Calc_Temp(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.dT   = ((dev->ClcPrms.D2 - dev->Clb_Cf.C5) - pow(2,8));
	dev->ClcPrms.TEMP = (20 + (dev->ClcPrms.dT * dev->Clb_Cf.C6));

	return MS5611_OK;
}

MS5611_StatusTypeDef MS5611_Calc_Press(MS5611_HandleTypeDef *dev){

	dev->ClcPrms.OFF  = (dev->Clb_Cf.C2 * pow(2,16)) + (((dev->Clb_Cf.C4 ) * (dev->ClcPrms.dT)) / pow(2,7));
	dev->ClcPrms.SENS = ((dev->Clb_Cf.C1 * pow(2,15)) + (dev->Clb_Cf.C3 + dev->ClcPrms.dT) / pow(2,8));
	dev->ClcPrms.P 	  = (((dev->ClcPrms.D1 * dev->ClcPrms.SENS / pow(2,21)) - dev->ClcPrms.OFF)/pow(2,15));

	return MS5611_OK;
}



