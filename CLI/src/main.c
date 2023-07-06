/*
 * main.c
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "cli_api.h"


static void printVersion(void)
{
	LOGD("\n");
	LOG_SPACER;

	LOGD("Generic CLI version %s\n",VERSION);
	LOGD("BUILD DATE: %s | BUILD TIME %s\n",__DATE__,__TIME__);

	LOG_SPACER;
	LOGD("\n");
}

CMDFUNC(ver)
{
	printVersion();
}

CMDFUNC(time)
{
	LOGD("%s\n",__TIME__);
}

CMDFUNC(date)
{
	LOGD("%s\n",__DATE__);
}

consolecommands build_info_page[] =
{
		COMMAND(time             , This prints softwares current version),
		COMMAND(date             , command help string),
		{CONSOLE_ENTRY_END}
};

consolecommands console_home[] =
{
		COMMAND(ver             , This prints softwares current version                 ),
		PAGE   (buildinfo       , command help string,                   build_info_page),
		{CONSOLE_ENTRY_END}
};

int32_t main(void)
{
	int32_t ret_val = -1;

	p_cli_ctx hdl = NULL;
	cli_init_t  initializer;

	printVersion();

	initializer.echo 				= false;
	initializer.receive_buffer_size = 1024;
	initializer.home_page 			= console_home;

	CLI_STATUS status 				=cli_init(&hdl,&initializer);

	if(status == CLI_SUCCESS)
	{
		ret_val = 0;

	}
	else
	{
		LOGE("CLI INIT FAILED\n");
	}

	while(0 == exit_check(hdl))
	{
		consoleProcess(hdl);

	}

	return ret_val;
}
