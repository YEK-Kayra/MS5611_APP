

#ifndef MS5611_H_
#define MS5611_H_


/******************************************************************************
         			#### MS5611 INCLUDES ####
******************************************************************************/

#include "main.h"
#include "stdbool.h"
#include "stdint.h"
#include "math.h"


/******************************************************************************
         			#### MS5611 EXTERNAL VARIABLES ####
******************************************************************************/

extern I2C_HandleTypeDef hi2c1;
extern float  MS5611_Press;		/*! Pressure data variable 			*/
extern float  MS5611_Temp;		/*! Temperature data variable 		*/
extern float  MS5611_VertAlt;	/*! Vertical Altitude data variable */


/******************************************************************************
         				#### MS5611 ENUMS ####
******************************************************************************/

typedef enum
{

  MS5611_OK   	= 0,
  MS5611_ERROR  = 1

}MS5611_StatusTypeDef;

/******************************************************************************
         				#### MS5611 STRUCTURES ####
******************************************************************************/

typedef struct{

	uint16_t C1; 	/*! Pressure sensitivity 							(SENSt1)   */
	uint16_t C2;	/*! Pressure offset 								(OFFt1)    */
	uint16_t C3;	/*! Temperature coefficient of pressure sensitivity (TCS) 	   */
	uint16_t C4;	/*! Temperature coefficient of pressure offset 		(TCO) 	   */
	uint16_t C5;	/*! Reference temperature 20 °C 					(Tref) 	   */
	uint16_t C6;	/*! Temperature coefficient of the temperature 		(TEMPSENS) */

}MS5611_CalibrationCoef_TypeDef;

typedef struct{

	uint32_t D1;	/*! Digital pressure value */
	uint32_t D2;	/*! Digital temperature value */

	int32_t dT;		/*! Difference between actual and reference temperature */
	int32_t TEMP;	/*! Actual temperature (-40…85°C with 0.01°C resolution) */
	int32_t P;		/*! Temperature compensated pressure (10…1200mbar with 0.01mbar resolution) */

	int64_t OFF;	/*! Offset at actual temperature */
	int64_t SENS;	/*! Sensitivity at actual temperature */

}MS5611_CalculationParams_TypeDef;


typedef struct{

	I2C_HandleTypeDef *i2c;

	uint16_t I2C_ADDRESS;

	MS5611_CalibrationCoef_TypeDef Clb_Cf;
	MS5611_CalculationParams_TypeDef ClcPrms;

}MS5611_HandleTypeDef;


/******************************************************************************
         			#### BMP390 PROTOTYPES OF FUNCTIONS ####
******************************************************************************/

/**
  * @brief  MS5611 sensor initialization.
  * @param  dev_MS5611 general handle.
  * @retval booleans.
  */
MS5611_StatusTypeDef MS5611_Init(MS5611_HandleTypeDef *dev);

/**
  * @brief  Retrieves calibration coefficient data(from C1 to C6) from the MS5611 chip and stores them in CalibDatas.
  * @param  dev_MS5611 general handle.
  * @retval booleans.
  */
MS5611_StatusTypeDef MS5611_Get_CalibCoeff(MS5611_HandleTypeDef *dev);

MS5611_StatusTypeDef MS5611_Calc_Temp(MS5611_HandleTypeDef *dev);
MS5611_StatusTypeDef MS5611_Calc_Press(MS5611_HandleTypeDef *dev);



#endif /* INC_MS5611_H_ */
