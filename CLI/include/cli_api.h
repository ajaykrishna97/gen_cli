/*
 * cli_api.h
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLI_API_H_
#define INCLUDE_CLI_API_H_

#define CMDFUNC(NAME) void NAME ## _method(void *hdl)
#define COMMAND(NAME , HELPSTRING)  { #NAME, #HELPSTRING, NAME ## _method }

#define PRINTF printf
#define console PRINTF

#define DEBUG_EN 1

#define LOG_DEBUG   if(DEBUG_EN) PRINTF
#define LOG_ERROR   PRINTF("ERROR:"); \
				PRINTF

#define LOGD(...) LOG_DEBUG(__VA_ARGS__)
#define LOGE(...) LOG_ERROR(__VA_ARGS__)


typedef enum CLI_STATUS
{
	CLI_FAILUE  = -1,
	CLI_SUCCESS = 0
}CLI_STATUS;

typedef void *CLI_HDL;

CLI_STATUS cli_init(CLI_HDL *hdl);
void consoleProcess(CLI_HDL hdl);

#endif /* INCLUDE_CLI_API_H_ */
