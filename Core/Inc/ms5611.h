

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
extern float  MS5611_Press;
extern float  MS5611_Temp;
extern float  MS5611_VertAlt;


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

}MS5611_CalibCoef_TypeDef;


typedef struct{

	I2C_HandleTypeDef *i2c;

	uint16_t I2C_ADDRESS;

	MS5611_CalibCoef_TypeDef CalibDatas;

	uint32_t D1;	/*! Digital pressure value */
	uint32_t D2;	/*! Digital temperature value */


}MS5611_HandleTypeDef;


/******************************************************************************
         			#### BMP390 PROTOTYPES OF FUNCTIONS ####
******************************************************************************/

/**
  * @brief  MS5611 sensor initialization.
  * @param  dev_MS5611 general handle.
  * @retval booleans.
  */
_Bool MS5611_Init(MS5611_HandleTypeDef *devMS5611);

/**
  * @brief  Retrieves calibration coefficient data(from C1 to C6) from the MS5611 chip and stores them in CalibDatas.
  * @param  dev_MS5611 general handle.
  * @retval booleans.
  */
_Bool MS5611_Get_CalibCoeff(MS5611_HandleTypeDef *devMS5611);



#endif /* INC_MS5611_H_ */
