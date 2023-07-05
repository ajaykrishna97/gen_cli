/*
 * cli.c
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#include  "cli.h"
#include  "cli_api.h"
#include  "helper.h"
#include  "stdlib.h"

static bool consoleInit(p_cli_ctx ctx ,uint16_t bufLen, uint16_t timeout, uint16_t echo,
		pconsolecommand cmd);
static void printhelpstring(p_cli_ctx ctx, pconsolecommand cmd);

static void add_to_history(p_cli_ctx ctx ,pconsolecommand page);
static void set_current_page(p_cli_ctx ctx, pconsolecommand page, bool print_help,
		bool update_history);
static void handle_page_call(p_cli_ctx ctx ,pconsolecommand page);


#ifdef __linux__
void* handle_get_char(void *ctx)
#elif _WIN32
DWORD WINAPI handle_get_char(LPVOID ctx)
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif
{
	p_cli_ctx con = (p_cli_ctx)ctx;

	while (1)
	{
		con->data_buf.rxBuf[con->data_buf.rx_p] = getchar();
		con->data_buf.rx_p = (con->data_buf.rx_p + 1);

		if (con->data_buf.rx_p == con->data_buf.maxRxBufLen) {
			con->data_buf.rx_p = 0;
		}

	}
}

int getCharConsole(p_cli_ctx ctx)
{
	p_cli_ctx con = (p_cli_ctx)ctx;

	if(con->data_buf.rx_g == con->data_buf.rx_p) return (-1);

	int ch =  con->data_buf.rxBuf[con->data_buf.rx_g];
	con->data_buf.rx_g = (con->data_buf.rx_g +1);

	if(con->data_buf.rx_g == con->data_buf.maxRxBufLen)  con->data_buf.rx_g = 0;

	return (ch);
}


void printVersion(void)
{
	LOGD("\n");
	STAR_SPACER;

	LOGD("Generic CLI version %s\n",VERSION);
	LOGD("BUILD DATE: %s | BUILD TIME %s\n",__DATE__,__TIME__);
}

CMDFUNC(ver)
{
	printVersion();
}

CMDFUNC(help)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	printhelpstring(ctx, ctx->con->consolePageHolder.current_page);

}

CMDFUNC(back)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	ctx->con->consolePageHolder.history_count = ctx->con->consolePageHolder.history_count
			?ctx->con->consolePageHolder.history_count-1:(MAX_PAGE_HISTORY-1);
	set_current_page(ctx,
			ctx->con->consolePageHolder.page_history[ctx->con->consolePageHolder.history_count],
			true,false);

}

CMDFUNC(home)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	set_current_page(ctx, ctx->con->consolePageHolder.home_page,true,true);

	ctx->con->consolePageHolder.current_page = ctx->con->consolePageHolder.home_page;
	printhelpstring(ctx, ctx->con->consolePageHolder.current_page);

}

CMDFUNC(history)
{

}

CMDFUNC(time)
{
	LOGD("%s\n",__TIME__);
}

CMDFUNC(date)
{
	LOGD("%s\n",__DATE__);
}

consolecommands housekeeping_cmd[] =
{
		COMMAND(help          , print help of current page),
		COMMAND(home          , To return back to home page),
		COMMAND(back          , Return to previous page),
		{CONSOLE_END}
};

consolecommands build_info_page[] =
{
		COMMAND(time             , This prints softwares current version),
		COMMAND(date             , command help string),
		{CONSOLE_END}
};

consolecommands console_home[] =
{
		COMMAND(ver             , This prints softwares current version                 ),
		PAGE(buildinfo          , command help string,                   build_info_page),
		{CONSOLE_END}
};


CLI_STATUS cli_init(CLI_HDL *hdl)
{
	CLI_STATUS ret_val = CLI_FAILUE;

	p_cli_ctx main_ctx = calloc(1, sizeof(cli_ctx));

	consoleInit(main_ctx, 1024, 1000, 0, console_home);

#ifdef __linux__
	pthread_create(&main_ctx->data_buf.con_thread,NULL,handle_get_char,main_ctx);
	if (0 == main_ctx->data_buf.con_thread) {
#elif _WIN32
	main_ctx->data_buf.con_thread = CreateThread(NULL, 0, handle_get_char,
			main_ctx, 0, 0);
	if (NULL == main_ctx->data_buf.con_thread) {
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif

		LOGD("CON thread init failed \n");
	} else {
		*hdl = (CLI_HDL) main_ctx;
		ret_val = CLI_SUCCESS;

		printVersion();
		printhelpstring(main_ctx, main_ctx->con->consolePageHolder.current_page);
	}

	ret_val = CLI_SUCCESS;

	return ret_val;
}

void consoleAsciiInterpretor (pvoid  ctx  ,uint8_t* dBuf , uint16_t dLen);
void consoleBinaryInterpretor(pvoid  ctx  ,uint8_t* dBuf , uint16_t dLen);

static bool consoleInit(p_cli_ctx ctx ,uint16_t bufLen, uint16_t timeout, uint16_t echo,
		pconsolecommand cmd)
{

	ctx->con  	  = calloc(1 , sizeof(consoleHandler));

	if (NULL != ctx->con && NULL != ctx) {

		ctx->con->maxBufLen = MAX_CONSOLE_BUF;
		ctx->con->echo = echo;
		ctx->con->timeout = timeout;
		ctx->con->consolePageHolder.home_page = cmd;
		ctx->con->consolePageHolder.current_page = NULL;
		ctx->con->index = 0;

		ctx->con->asciiInterpretor = consoleAsciiInterpretor;
		ctx->con->binaryInterpretor = consoleBinaryInterpretor;

		ctx->data_buf.maxRxBufLen = MAX_RX_BUF;
		ctx->data_buf.maxTxBufLen = MAX_TX_BUF;

		ctx->con->houseKeepingCmd = housekeeping_cmd;

		set_current_page(ctx,ctx->con->consolePageHolder.home_page,false,true);

	}
	else
	{
		LOGE("CLI INIT FAILED\n");
	}

	return (true);
}

void consoleProcess(CLI_HDL hdl)
{
	p_cli_ctx conH = (p_cli_ctx)hdl;

	int ch = getCharConsole(conH);

	if(ch == -1){ return; }

	conH->con->buf[conH->con->index] = ch;
	conH->con->index = conH->con->index + 1;

	if(conH->con->index > conH->con->maxBufLen)
	{
		conH->con->index =0;
	}

	if(conH->con->echo) LOGD("%c",ch);

	if(ch == 8 || ch == 127)
	{
		conH->con->index = conH->con->index - 2;
		LOGD("\b \b");
	}
	else if(ch == '\n' || ch == '\r')
	{
		if(NULL != conH->con->asciiInterpretor && (conH->con->index > 1))
		{
			conH->con->asciiInterpretor(conH ,conH->con->buf , conH->con->index);
			conH->con->index = 0;
		}
		else
		{
			conH->con->index = 0;
		}
	}
	else
	{
	}

	return;
}


static void add_to_history(p_cli_ctx ctx ,pconsolecommand page)
{
	if (NULL != page && NULL != ctx)
	{
		printf("adding to history\n");
		ctx->con->consolePageHolder.
			page_history[ctx->con->consolePageHolder.history_count] = page;

		ctx->con->consolePageHolder.history_count++;

		if(ctx->con->consolePageHolder.history_count == MAX_PAGE_HISTORY)
		{
			ctx->con->consolePageHolder.history_count = 0;
		}
		else{

		}
	}
}

static void set_current_page(p_cli_ctx ctx, pconsolecommand new_page, bool print_help,
		bool update_history)
{
	if (NULL != new_page && NULL != ctx)
	{
		if (true == update_history)
		{
			if(NULL != ctx->con->consolePageHolder.current_page)
			add_to_history(ctx, ctx->con->consolePageHolder.current_page);

		}

		ctx->con->consolePageHolder.current_page = new_page;

		if(true == print_help)
		{
			printhelpstring(ctx, new_page);
		}
		else{

		}

	} else {

		printf("PAGE NOT FOUND\n");
	}
}

static void handle_page_call(p_cli_ctx ctx ,pconsolecommand page)
{
	if(NULL != page && NULL != ctx)
	{
		set_current_page(ctx, page->next_page,true,true);
		printhelpstring(ctx, page->next_page);
	}
	else
	{
		printf("page not found null\n");
	}
}

void consoleAsciiInterpretor(pvoid context ,uint8_t* dBuf , uint16_t dLen)
{
	p_cli_ctx ctx = (p_cli_ctx)context;
	char* tok;

	tok = strtok((char *)dBuf, "\n");

	if (NULL != tok)
	{
		ctx->token_ctx.tokencount++;
		for (pconsolecommand iter = ctx->con->consolePageHolder.current_page
				; iter->type != CONSOLE_END;
				iter++)
		{
			if (strcmp(iter->commandstr, tok) == 0)
			{
				if(CONSOLE_PAGE == iter->type)
				{
					handle_page_call(ctx , iter);
					return;
				}
				else{
					if (iter->func != NULL)
					{
						iter->func(ctx);
						return;
					}
				}
			}
		}

		for (pconsolecommand iter = ctx->con->houseKeepingCmd
				; iter->type != CONSOLE_END;
				iter++)
		{
			if (strcmp(iter->commandstr, tok) == 0)
			{
				if(CONSOLE_PAGE == iter->type)
				{
					handle_page_call(ctx , iter);
					return;
				}
				else{
					if (iter->func != NULL)
					{
						iter->func(ctx);
						return;
					}
				}
			}
		}


	}
	LOGE("SYNTAX ERROR\n");
}

void consoleBinaryInterpretor(void *hdl ,uint8_t* dBuf , uint16_t dLen)
{

}

static void printhelpstring(p_cli_ctx ctx, pconsolecommand cmd)
{
	STAR_SPACER;

	LOGD("CHOSE FROM BELOW MENU:\n\n");
	for(pconsolecommand cmditer = &cmd[0]; CONSOLE_END != cmditer->type
			;cmditer++)
	{
		if(cmditer->type == CONSOLE_PAGE){
		console("%s %s:%s\n" , cmditer->commandstr,printFormatSpace(cmditer->commandstr,PRINT_HELP_SPACER),cmditer->helpstr);
		}
	}
	LOGD("\n");

	for (pconsolecommand cmditer = &cmd[0]; CONSOLE_END != cmditer->type;
			cmditer++)
	{
		if (cmditer->type == CONSOLE_COMMAND)
		{
			console("%s %s:%s\n", cmditer->commandstr,
					printFormatSpace(cmditer->commandstr, PRINT_HELP_SPACER),
					cmditer->helpstr);
		}
	}

	LOGD("\n");

	for (pconsolecommand cmditer = ctx->con->houseKeepingCmd; CONSOLE_END != cmditer->type;
			cmditer++)
	{
		console("%s %s:%s\n" , cmditer->commandstr,
				printFormatSpace(cmditer->commandstr,PRINT_HELP_SPACER),cmditer->helpstr);
	}

	STAR_SPACER;

}

