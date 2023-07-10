/*
 * cli_api.h
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLI_API_H_
#define INCLUDE_CLI_API_H_

#include "dtypes.h"

#define DEBUG_EN 1

#define LOG_SPACER LOGD("*********************************************************\n")

#define CMDFUNC(NAME) void NAME ## _method(void *hdl,  char **args, uint16_t argc)

#define COMMAND(NAME ,ARG_COUNT, HELPSTRING,ADVANCED_HELP)  {CONSOLE_COMMAND, #NAME, #HELPSTRING, NAME ## _method , NULL , ARG_COUNT, #ADVANCED_HELP}
#define PAGE(NAME , HELPSTRING, page_list)  {CONSOLE_PAGE, #NAME, #HELPSTRING, NULL , page_list, 0, ""}

#define PRINTF printf
#define console PRINTF

#define LOG_DEBUG   if(DEBUG_EN) PRINTF
#define LOG_ERROR   PRINTF("ERROR:"); \
				PRINTF

#define LOGD(...) LOG_DEBUG(__VA_ARGS__)

#define LOGE(...) LOG_ERROR(__VA_ARGS__)

typedef void (*interpretorFuncPtr)(pvoid ,uint8_t* , uint16_t);
typedef void (*msgfuncptr)(void *hdl, char **args, uint16_t argc);

#define VERSION "01.00.00"
#define PRINT_HELP_SPACER 10

#define MAX_ARGUMENT 4

typedef struct cli_st cli_ctx , *p_cli_ctx;

typedef enum console_entry_type_t
{
	CONSOLE_PAGE,
	CONSOLE_COMMAND,
	CONSOLE_ENTRY_END
}CONSOLE_ENTRY_TYPE;

typedef struct consolecommands_
{
	CONSOLE_ENTRY_TYPE         type;
	char 			          *commandstr;
	char              		  *helpstr;
	msgfuncptr		           func;
	struct consolecommands_   *next_page;
	uint16_t                   req_arg_count;
	char              		  *advance_helpstr;

}consolecommands , *pconsolecommand;

typedef enum CLI_STATUS
{
	CLI_FAILUE  = -1,
	CLI_SUCCESS = 0
}CLI_STATUS;

typedef struct cli_init_st
{
	pconsolecommand home_page;

	uint16_t receive_buffer_size;
	bool     echo;
}cli_init_t , *p_cli_init_t;

CLI_STATUS cli_init(p_cli_ctx *hdl , p_cli_init_t init_info);
void consoleProcess(p_cli_ctx hdl);
bool enable_echo(p_cli_ctx hdl,bool flag);


bool exit_check(p_cli_ctx hdl);

#endif /* INCLUDE_CLI_API_H_ */
