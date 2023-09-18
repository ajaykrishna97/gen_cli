/*
 * cla_parser_api.h
 *
 *  Created on: 17-Sep-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLA_PARSER_API_H_
#define INCLUDE_CLA_PARSER_API_H_

#include "dtypes.h"

#define CLA_DEBUG_EN 1

#define PRINTF_CLA printf

#define LOG_DEBUG_CLA   if(CLA_DEBUG_EN) PRINTF_CLA
#define LOG_ERROR_CLA   PRINTF_CLA("CLA_ERROR:"); \
				PRINTF_CLA

#define LOGD_CLA(...) LOG_DEBUG_CLA(__VA_ARGS__)
#define LOGE_CLA(...) LOG_ERROR_CLA(__VA_ARGS__)


#define CLA_HDLR(cla_char) bool cla_char ## _method_CLA(char **args ,uint16_t argc)

#define CLA_ADD(cla_char , cla_str , exit_flag , arg_count, help_str) \
				{cla_char , #cla_str , exit_flag, cla_str ## _method_CLA , arg_count,help_str}

typedef bool (*cla_handler)(char **args , uint16_t argc);


typedef enum CLI_ARG_STATUS
{
	CLA_FAILED = -1,
	CLA_SUCCESS =0,
	CLA_EXIT = 1,
}CLA_STATUS;


typedef struct cla_context_st
{
	char         cla_char;
	char         *cla_alt_string;

	bool         exit_flag;
	cla_handler  handler;
	uint8_t      arg_count;
	char         *help_string;

}cla_ctx, *p_cla_ctx;

CLA_STATUS cla_parse(cla_ctx *cla_list, char** args , uint16_t  argc);
void cla_print_help(cla_ctx *cla_list);


#endif /* INCLUDE_CLA_PARSER_API_H_ */
