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

int32_t main(void)
{
	int32_t ret_val = -1;

	CLI_HDL hdl = NULL;

	CLI_STATUS status =
				cli_init(&hdl);

	if(status == CLI_SUCCESS)
	{
		ret_val = 0;

	}
	else
	{
		LOGD("CLI init failed\n");
	}

	while(0 == ret_val)
	{
		consoleProcess(hdl);
	}

	return ret_val;
}
