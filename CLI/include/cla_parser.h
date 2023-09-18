/*
 * cla_parser.h
 *
 *  Created on: 17-Sep-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLA_PARSER_H_
#define INCLUDE_CLA_PARSER_H_

#include <stdbool.h>

typedef enum CLA_ERROR_CODE
{
	CLA_NO_ERROR          = 0x0000,

	CLA_ERROR_TOO_FEW_ARG = 0x0001,
	CLA_ERROR_UNKNOWN_OPT = 0x0002,
	CLA_ERROR_EXIT_FLAG   = 0x0003,

	CLA_ERROR_MAX
}CLA_ERROR_CODE_T;

#define CLA_HELPSTR_SPACER 10



#endif /* INCLUDE_CLA_PARSER_H_ */
