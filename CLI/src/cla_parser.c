/*
 * cla_parser.c
 *
 *  Created on: 17-Sep-2023
 *      Author: Ajay Krishna R
 */

#include "cla_parser.h"
#include "cla_parser_api.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"


static char *error_string[CLA_ERROR_MAX] =
{
		"Success",
		"Too few argument",
		"Unknown opt"
		"Exit flag true"
};

CLA_ERROR_CODE_T validate_cla(cla_ctx *cla_list ,char** args ,uint16_t argc, bool invoke_flag)
{
	CLA_ERROR_CODE_T status = CLA_NO_ERROR;

	if(NULL != cla_list)
	{

		for(int32_t i=0;i< argc;i++)
		{
			if('-' == args[i][0])
			{
				if('-' == args[i][1])
				{
					status = CLA_ERROR_UNKNOWN_OPT;

					for(p_cla_ctx ctx = cla_list; ctx->cla_alt_string != NULL; ctx++)
					{
						if(strcmp(ctx->cla_alt_string,&args[i][1]))
						{
							status = CLA_NO_ERROR;

							if((i+ctx->arg_count) < argc)
							{
								for(int32_t j = i+1 ; j < i+ctx->arg_count ;j++)
								{
									if('-' == args[j][0])
									{
										status = CLA_ERROR_TOO_FEW_ARG;
										break;
									}
								}
								if (status != CLA_NO_ERROR)
								{
									break;
								} else {
									i = i+ctx->arg_count;
									break;
								}
							}
							else{
								status = CLA_ERROR_TOO_FEW_ARG;
								break;
							}
						}
					}

					if(CLA_NO_ERROR == status)
					{
						/*DO NOTHING*/
					}
					else
					{
						LOGD_CLA(
								"validation failed for argument:%s | Reason:%s\n",
								args[i],error_string[status]);
						break;
					}
				}
				else
				{
					status = CLA_ERROR_UNKNOWN_OPT;

					for(p_cla_ctx ctx = cla_list; ctx->cla_alt_string != NULL; ctx++)
					{
						if(ctx->cla_char == args[i][1])
						{
							status = CLA_NO_ERROR;

							if((i+ctx->arg_count) < argc)
							{
								for(int32_t j = i+1 ; j < i+ctx->arg_count ;j++)
								{
									if('-' == args[j][0])
									{
										status = CLA_ERROR_TOO_FEW_ARG;
										break;
									}
								}
								if (status != CLA_NO_ERROR)
								{
									break;
								} else {
									i = i+ctx->arg_count;
									break;
								}
							}
							else{
								status = CLA_ERROR_TOO_FEW_ARG;
								break;
							}
						}
					}

					if(CLA_NO_ERROR == status)
					{
						/*DO NOTHING*/
					}
					else
					{
						LOGD_CLA(
								"validation failed for argument:%s | Reason:%s\n",
								args[i],error_string[status]);
						break;
					}

				}
			}
			else
			{
				LOGD_CLA("Invalid argument:%s\n",args[i]);
			}
		}
	}
	else
	{
	}

	return status;
}

CLA_ERROR_CODE_T invoke_cla(cla_ctx *cla_list, char **args, uint16_t argc, bool invoke_flag)
{
	CLA_STATUS ret_val = CLA_NO_ERROR;
	bool exit_flag = false;

	if (NULL != cla_list) {

		for (int32_t i = 0; i < argc; i++)
		{
			if ('-' == args[i][0]) {
				if ('-' == args[i][1])
				{

					for (p_cla_ctx ctx = cla_list; ctx->cla_alt_string != NULL;
							ctx++)
					{
						if (strcmp(ctx->cla_alt_string, &args[i][1]))
						{
							ctx->handler(&args[i + 1], ctx->arg_count);
							i = i+ctx->arg_count;
							exit_flag = ctx->exit_flag;

							break;
						}
					}
				}
				else
				{
					for (p_cla_ctx ctx = cla_list; ctx->cla_alt_string != NULL;
							ctx++)
					{
						if (ctx->cla_char == args[i][1])
						{
							ctx->handler(&args[i + 1], ctx->arg_count);
							i = i+ctx->arg_count;
							exit_flag = ctx->exit_flag;
							break;
						}
					}
				}
			} else {
			}

			if(exit_flag == true)
			{
				ret_val = CLA_ERROR_EXIT_FLAG;
				break;
			}
		}
	}
	else {
	}

	return (ret_val);
}


CLA_STATUS cla_parse(cla_ctx *cla_list, char** args , uint16_t  argc)
{
	CLA_STATUS status = CLA_SUCCESS;
	if(NULL != args && argc > 1&& cla_list != NULL)
	{

		if(CLA_NO_ERROR == validate_cla(cla_list,&args[1],(argc-1),false))
		{
			CLA_ERROR_CODE_T invocation_status;

			invocation_status = invoke_cla(cla_list,&args[1],(argc-1),false);

			switch(invocation_status)
			{
				case CLA_NO_ERROR:
					status = CLA_SUCCESS;
					break;
				case CLA_ERROR_UNKNOWN_OPT:
					status = CLA_FAILED;
					break;
				case CLA_ERROR_TOO_FEW_ARG:
					status = CLA_FAILED;
					break;
				case CLA_ERROR_EXIT_FLAG:
					status = CLA_EXIT;
					break;
				case CLA_ERROR_MAX:
					status = CLA_FAILED;
					break;
			}
		}
		else{
			status = CLA_FAILED;
		}
	}

	return status;
}



void cla_print_help(cla_ctx *cla_list)
{
	for (p_cla_ctx ctx = cla_list; ctx->cla_alt_string != NULL;
			ctx++)
	{
		LOGD_CLA("-%c --%s%s%s\n",ctx->cla_char,ctx->cla_alt_string, printFormatSpace(ctx->cla_alt_string,CLA_HELPSTR_SPACER),ctx->help_string);
	}
}
