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
#include "cla_parser_api.h"

static void  print_cla_help(void);

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

CMDFUNC(echo)
{
	p_cli_ctx ctx = (p_cli_ctx) hdl;

	if(NULL != ctx)
	{

		int32_t echo_flag = 0;

		if(0 != sscanf(args[0],"%d",&echo_flag))
		{
			enable_echo(ctx, (bool)echo_flag);
			LOGD("Done\n");

		}else{
			LOGD("INVALID ARGUMENT\n");
		}

	}
	else{

	}
}

consolecommands build_info_page[] =
{
		COMMAND(time         ,0      , This prints softwares build time,time),
		COMMAND(date         ,0      , This prints softwares build date,date),
		{CONSOLE_ENTRY_END}
};

consolecommands console_home[] =
{
		COMMAND(ver          ,0     , This prints softwares current version                ,ver),
		COMMAND(echo         ,1     , Enables command echo                                 ,echo [FLAG] | FLAG:1-enable 0-disable),
		PAGE   (buildinfo           , list commands related to buildinfo                   ,build_info_page),
		{CONSOLE_ENTRY_END}
};



CLA_HDLR(help)
{
	print_cla_help();
	return true;
}


CLA_HDLR(version)
{
	printVersion();
	return true;
}


cla_ctx cla_list[] =
{
		CLA_ADD('h',help   ,1,0,"prints help string for all command line options"),
		CLA_ADD('v',version,1,0,"give current version of the sw"),
};


static void print_cla_help(void)
{
	cla_print_help(cla_list);
}

int32_t main(int argc,char *args[])
{
	int32_t ret_val = -1;

	p_cli_ctx hdl = NULL;
	cli_init_t  initializer;
	CLA_STATUS status = CLA_FAILED;

	status = cla_parse(cla_list,args,argc);

	if(status == CLA_SUCCESS)
	{
		printVersion();

		initializer.echo 				= false;
		initializer.receive_buffer_size = 1024;
		initializer.home_page 			= console_home;

		CLI_STATUS status 				= cli_init(&hdl,&initializer);

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
	}

	return ret_val;
}
