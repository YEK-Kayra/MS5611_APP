

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
extern int  MS5611_Press;		/*! Pressure data variable 			*/
extern int  MS5611_Temp;		/*! Temperature data variable 		*/
extern int  MS5611_Altitude;

/**!These macros provide to calculate the altitude of BMP280 */
#define SeaLevelPress  101325
#define SeaLevelTemp   288.15
#define GradientTemp   0.0065
#define GravityAccel   9.80665
#define GasCoefficient 287.05


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

	unsigned int C1; 	/*! Pressure sensitivity 							(SENSt1)   */
	unsigned int C2;	/*! Pressure offset 								(OFFt1)    */
	unsigned int C3;	/*! Temperature coefficient of pressure sensitivity (TCS) 	   */
	unsigned int C4;	/*! Temperature coefficient of pressure offset 		(TCO) 	   */
	unsigned int C5;	/*! Reference temperature 20 °C 					(Tref) 	   */
	unsigned int C6;	/*! Temperature coefficient of the temperature 		(TEMPSENS) */

	unsigned int crc;	/*! 4-bit CRC has been implemented to check the data validity in memory*/

}MS5611_CalibrationCoef_TypeDef;

typedef struct{

	uint32_t D1;	/*! Digital raw pressure value */
	uint32_t D2;	/*! Digital raw temperature value */
	double dT;			/*! Difference between actual and reference temperature */
	double TEMP;		/*! Actual temperature (-40…85°C with 0.01°C resolution) */
	double P;			/*! Temperature compensated pressure (10…1200mbar with 0.01mbar resolution) */
	double OFF;			/*! Offset at actual temperature */
	double SENS;		/*! Sensitivity at actual temperature */
	double OFF2;		/*! Offset at actual temperature_2 */
	double SENS2;  		/*! Sensitivity at actual temperature_2 */
	double TEMP2; 		/*! Actual temperature_2 (-40…<20°C with 0.01°C resolution) */

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
  * @brief  Calculates compensated temperature
  * @param  dev_MS5611 general handle.
  * @retval MS5611 Status.
  */
MS5611_StatusTypeDef MS5611_Calc_Temp_Press(MS5611_HandleTypeDef *dev);

#endif /* INC_MS5611_H_ */
