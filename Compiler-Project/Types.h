#ifndef TYPES_UTIL_H
#define TYPES_UTIL_H

#include <stdlib.h>


typedef char* String;

/*
*	This enumeration defines the attribute type
*/
typedef enum attr_type
{
	INTEGER_TYPE,
	REAL_TYPE,
	VOID_TYPE,
	ERROR_TYPE

} Attr_type;

/*
*	This enumeration defines type of expression
*/
typedef enum exp_type
{
	FUNCT_EXPR,
	ARRAY_EXPR,
	BASIC_EXPR

} Exp_type;

/*
*	This structure holds the integer value of type INT
*/
typedef struct int_type
{	
	unsigned int value;		/* Numeric value of type INT */

} INT;

/*
*	This structure holds the double value of type REAL
*/
typedef struct real_type
{	
	double value;		/* Floating point value of type REAL */

} REAL;

/*
*	This structure holds the size of type ARRAY
*/
typedef struct array
{	
	unsigned int size;		/* Defined ARRAY size */

} ARRAY;

/*
*	This structure holds the value and type of function parameter
*/
typedef struct f_params
{
	Attr_type param_type;		/* Type of parameter */
	String param_name;			/* Parameter name */

} FUNC_PARAM;

/*
*	This structure holds the number of function parameters and parameters list
*/
typedef struct function
{	
	unsigned int params_number_total;		/* Total number of parameters */
	unsigned int params_number_used;		/* Number of parameters that were recognized */
	unsigned int is_declared;				/* Indicate that function was declared */
	FUNC_PARAM *params;						/* Array of parameters */

} FUNCTION;

/*
*	This union holds the above types
*/
typedef union exp_value
{
	INT INTEGER;
	REAL REAL;
	ARRAY ARRAY;
	FUNCTION FUNCTION;

} Exp_value;

#endif // TYPES_UTIL_H