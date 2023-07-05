/*
 * cli_api.h
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLI_API_H_
#define INCLUDE_CLI_API_H_

#include "dtypes.h"

typedef void (*interpretorFuncPtr)(pvoid ,uint8_t* , uint16_t);
typedef void (*msgfuncptr)(void *hdl);

#define VERSION "01.00.00"
#define PRINT_HELP_SPACER 10

typedef enum console_entry_type_t
{
	CONSOLE_PAGE,
	CONSOLE_COMMAND,
	CONSOLE_END
}CONSOLE_ENTRY_TYPE;

typedef struct consolecommands_
{
	CONSOLE_ENTRY_TYPE         type;
	char 			          *commandstr;
	char              		  *helpstr;
	msgfuncptr		           func;
	struct consolecommands_   *next_page;

}consolecommands , *pconsolecommand;

#define CMDFUNC(NAME) void NAME ## _method(void *hdl)

#define COMMAND(NAME , HELPSTRING)  {CONSOLE_COMMAND, #NAME, #HELPSTRING, NAME ## _method , NULL}
#define PAGE(NAME , HELPSTRING, page_list)  {CONSOLE_PAGE, #NAME, #HELPSTRING, NULL , page_list}


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
