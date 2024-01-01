

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
extern int32_t  MS5611_Press;		/*! Pressure data variable 			*/
extern int32_t  MS5611_Temp;		/*! Temperature data variable 		*/

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

	uint16_t crc;	/*! 4-bit CRC has been implemented to check the data validity in memory*/

}MS5611_CalibrationCoef_TypeDef;

typedef struct{

	uint32_t D1;	/*! Digital raw pressure value */
	uint32_t D2;	/*! Digital raw temperature value */

	int32_t dT;		/*! Difference between actual and reference temperature */
	int32_t TEMP;	/*! Actual temperature (-40…85°C with 0.01°C resolution) */
	int32_t P;		/*! Temperature compensated pressure (10…1200mbar with 0.01mbar resolution) */

	int64_t OFF;	/*! Offset at actual temperature */
	int64_t SENS;	/*! Sensitivity at actual temperature */

	int64_t OFF2;	/*! Offset at actual temperature_2 */
	int64_t SENS2;  /*! Sensitivity at actual temperature_2 */
	int32_t TEMP2; 	/*! Actual temperature_2 (-40…<20°C with 0.01°C resolution) */

}MS5611_CalculationParams_TypeDef;


typedef struct{

	I2C_HandleTypeDef *i2c;

	uint16_t I2C_ADDRESS;

	MS5611_CalibrationCoef_TypeDef Clb_Cf;
	MS5611_CalculationParams_TypeDef ClcPrms;

}MS5611_HandleTypeDef;


/******************************************************************************
         			#### MS5611 PROTOTYPES OF FUNCTIONS ####
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
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_Get_CalibCoeff(MS5611_HandleTypeDef *dev);


/**
  * @brief  MS5611 are reset and if it's not completed successfully the sensor is initialized one more time
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_Reset(MS5611_HandleTypeDef *dev);


/**
  * @brief  ADC Registers will be reading for raw pressure and temperature values
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_ReadRaw_Press_Temp(MS5611_HandleTypeDef *dev);


/**
  * @brief  Optimisation function(if celsius degrees is under 20)
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_ScndOrd_Calc_Press_Temp(MS5611_HandleTypeDef *dev);


/**
  * @brief  Calculates compensated temperature
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_Calc_Temp(MS5611_HandleTypeDef *dev);


/**
  * @brief  Calculates compensated pressure
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_Calc_Press(MS5611_HandleTypeDef *dev);


/**
  * @brief  It is the function to execute MS5611
  * @param  dev_MS5611 general handle.
  * @param 	*press will get the actual pressure value
  * @param  *temp  will get the actual temperature value
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_ReadValues(MS5611_HandleTypeDef *dev, int32_t *press, int32_t *temp);

#endif /* INC_MS5611_H_ */
