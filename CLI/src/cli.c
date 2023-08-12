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

static bool consoleInit(p_cli_ctx ctx ,uint16_t bufLen, uint16_t echo,
		pconsolecommand cmd);
static void printhelpstring(p_cli_ctx ctx, pconsolecommand cmd);

static void add_to_history(p_cli_ctx ctx ,pconsolecommand page);
static void set_current_page(p_cli_ctx ctx, pconsolecommand page, bool print_help,
		bool update_history);
static void handle_page_call(p_cli_ctx ctx ,pconsolecommand page);

static bool argument_tokeniser(p_cli_ctx hdl, uint8_t* dBuf , uint16_t dLen);

static bool add_to_cmd_history(p_cli_ctx ctx ,uint8_t *dBuf, uint16_t dLen);

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
	p_cli_ctx con = ctx;

	if(con->data_buf.rx_g == con->data_buf.rx_p) return (-1);

	int ch =  con->data_buf.rxBuf[con->data_buf.rx_g];
	con->data_buf.rx_g = (con->data_buf.rx_g +1);

	if(con->data_buf.rx_g == con->data_buf.maxRxBufLen)  con->data_buf.rx_g = 0;

	return (ch);
}


CMDFUNC(help)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	printhelpstring(ctx, ctx->con->consolePageHolder.current_page);

}

CMDFUNC(back)
{
	p_cli_ctx ctx = (p_cli_ctx) hdl;

	ctx->con->consolePageHolder.history_count = ctx->con->consolePageHolder.history_count
			?ctx->con->consolePageHolder.history_count-1:(MAX_PAGE_HISTORY-1);
	set_current_page(ctx,
			ctx->con->consolePageHolder.
			page_history_info[ctx->con->consolePageHolder.history_count].page_history,
			true,false);

	ctx->con->consolePageHolder.
				page_history_info[ctx->con->consolePageHolder.history_count]
								  .page_history = NULL;

}

CMDFUNC(home)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	set_current_page(ctx, ctx->con->consolePageHolder.home_page,true,true);
	ctx->con->consolePageHolder.current_page = ctx->con->consolePageHolder.home_page;

}

CMDFUNC(quit)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;
	ctx->quit_flag = true;

}

CMDFUNC(history)
{
	p_cli_ctx ctx = (p_cli_ctx)hdl;

	int iter = ctx->con->consolePageHolder.cmd_history.cmd_hist_get;

	for(int i = 0; i < MAX_HISTORY_STORE_COUNT;i++)
	{
		LOGD("%d)%s",(i+1),ctx->con->consolePageHolder.cmd_history.history[iter].command);
		iter++;

		if(iter == MAX_HISTORY_STORE_COUNT){
			iter = 0;
		}

		if(iter == ctx->con->consolePageHolder.cmd_history.cmd_hist_put)
		{
			break;
		}
	}

}

consolecommands housekeeping_cmd[] =
{
		COMMAND(help    ,0, print help of current page,help),
		COMMAND(history ,0, exit the application,quit),
		COMMAND(home    ,0, To return back to home page,home),
		COMMAND(back    ,0, Return to previous page,back),
		COMMAND(quit    ,0, exit the application,quit),

		{CONSOLE_ENTRY_END}
};


CLI_STATUS cli_init(p_cli_ctx *hdl, p_cli_init_t init_info)
{
	CLI_STATUS ret_val = CLI_FAILUE;

	if (NULL != hdl && NULL != init_info)
	{
		p_cli_ctx main_ctx = calloc(1, sizeof(cli_ctx));

		consoleInit(main_ctx, init_info->receive_buffer_size, init_info->echo , init_info->home_page);

		if (NULL != main_ctx)
		{
#ifdef __linux__
	pthread_create(&main_ctx->data_buf.con_thread,NULL,handle_get_char,main_ctx);
	if (0 == main_ctx->data_buf.con_thread) {
#elif _WIN32
			main_ctx->data_buf.con_thread = CreateThread(NULL, 0,
					handle_get_char, main_ctx, 0, 0);
			if (NULL == main_ctx->data_buf.con_thread) {
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif

				LOGD("CON thread init failed \n");
			} else {
				*hdl = main_ctx;
				ret_val = CLI_SUCCESS;

				printhelpstring(main_ctx,
						main_ctx->con->consolePageHolder.current_page);
			}

			ret_val = CLI_SUCCESS;
		} else {
			/* DO NOTHING*/
		}
	} else
	{
		/* DO NOTHING*/
	}
	return ret_val;
}

void consoleAsciiInterpretor (pvoid  ctx  ,uint8_t* dBuf , uint16_t dLen);
void consoleBinaryInterpretor(pvoid  ctx  ,uint8_t* dBuf , uint16_t dLen);

static bool consoleInit(p_cli_ctx ctx ,uint16_t bufLen, uint16_t echo,
		pconsolecommand cmd)
{

	ctx->con  	  = calloc(1 , sizeof(consoleHandler));

	if (NULL != ctx->con && NULL != ctx) {

		ctx->con->maxBufLen = MAX_CONSOLE_BUF;
		ctx->con->echo = echo;
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


static bool handle_bang_command(p_cli_ctx ctx, uint8_t *dBuf, uint16_t dLen)
{

	char *tok;
	int32_t index = -1;
	char buffer[MAX_COMMAND_STRING_SIZE];


	if(dBuf[0] == '!')
	{
		index = (!ctx->con->consolePageHolder.cmd_history.cmd_hist_put)?
				MAX_HISTORY_STORE_COUNT:
				(ctx->con->consolePageHolder.cmd_history.cmd_hist_put-1);

		if (index < MAX_HISTORY_STORE_COUNT
				&& index
						< ctx->con->consolePageHolder.cmd_history.command_count) {
			strncpy(buffer,
					ctx->con->consolePageHolder.cmd_history.history[index].command,
					ctx->con->consolePageHolder.cmd_history.history[index].dLen);

			ctx->con->asciiInterpretor(ctx, (uint8_t*) buffer,
					ctx->con->consolePageHolder.cmd_history.history[index].dLen);

			return 1;
		}
	}
	else
	{
		settokensparams(&ctx->token_ctx, dBuf, dLen);
		argument_tokeniser(ctx, dBuf, dLen);

		tok = ctx->token_ctx.command;
		if (0 != sscanf(tok, "%d", &index)) {
			index = index - 1;
			index = (ctx->con->consolePageHolder.cmd_history.cmd_hist_get+index)
					%MAX_HISTORY_STORE_COUNT;

			if (index < MAX_HISTORY_STORE_COUNT
					&& index
							< ctx->con->consolePageHolder.cmd_history.command_count) {
				strncpy(buffer,
						ctx->con->consolePageHolder.cmd_history.history[index].command,
						ctx->con->consolePageHolder.cmd_history.history[index].dLen);

				ctx->con->asciiInterpretor(ctx, (uint8_t*) buffer,
						ctx->con->consolePageHolder.cmd_history.history[index].dLen);

				return 1;
			}

		}
	}

	return 0;
}


void consoleProcess(p_cli_ctx hdl)
{
	p_cli_ctx conH = hdl;

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
			if(conH->con->buf[0] == '!' && conH->con->index > 2)
			{
				handle_bang_command(conH, &conH->con->buf[1], (conH->con->index-1));

			}
			else
			{
				add_to_cmd_history(conH, conH->con->buf, conH->con->index);
				conH->con->asciiInterpretor(conH ,conH->con->buf , conH->con->index);
			}
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
		ctx->con->consolePageHolder.total_commands++;

		ctx->con->consolePageHolder.page_history_info[ctx->con->consolePageHolder.history_count].
			page_history = page;

		ctx->con->consolePageHolder.
		page_history_info[ctx->con->consolePageHolder.history_count].command_index
			= ctx->con->consolePageHolder.total_commands;

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
	}
	else
	{
		printf("page not found null\n");
	}
}

static bool add_to_cmd_history(p_cli_ctx ctx, uint8_t *dBuf, uint16_t dLen) {
	if (dLen > MAX_COMMAND_STRING_SIZE) {
		LOGD("Command too big not adding to history\n");
		return 0;
	}

	if ((ctx->con->consolePageHolder.cmd_history.cmd_hist_put
			== ctx->con->consolePageHolder.cmd_history.cmd_hist_get)
			&& ctx->con->consolePageHolder.cmd_history.full_flag) {
		ctx->con->consolePageHolder.cmd_history.cmd_hist_get++;

		if (ctx->con->consolePageHolder.cmd_history.cmd_hist_get
				>= MAX_HISTORY_STORE_COUNT) {
			ctx->con->consolePageHolder.cmd_history.cmd_hist_get = 0;
		}
	}

	memset(
			ctx->con->consolePageHolder.cmd_history.history[ctx->con->consolePageHolder.cmd_history.cmd_hist_put].command,
			0, MAX_COMMAND_STRING_SIZE);
	strncpy(
			ctx->con->consolePageHolder.cmd_history.history[ctx->con->consolePageHolder.cmd_history.cmd_hist_put].command,
			(char*) dBuf, dLen);

	ctx->con->consolePageHolder.cmd_history.history[ctx->con->consolePageHolder.cmd_history.cmd_hist_put].dLen =
			dLen;

	ctx->con->consolePageHolder.cmd_history.command_count++;
	ctx->con->consolePageHolder.cmd_history.cmd_hist_put++;

	if (ctx->con->consolePageHolder.cmd_history.cmd_hist_put
			>= MAX_HISTORY_STORE_COUNT) {
		ctx->con->consolePageHolder.cmd_history.cmd_hist_put = 0;
	}

	if (ctx->con->consolePageHolder.cmd_history.cmd_hist_put
			== ctx->con->consolePageHolder.cmd_history.cmd_hist_get) {
		ctx->con->consolePageHolder.cmd_history.full_flag = 1;
	}

	return 1;
}

void consoleAsciiInterpretor(pvoid context, uint8_t *dBuf, uint16_t dLen)
{
	p_cli_ctx ctx = context;
	char *tok;

	settokensparams(&ctx->token_ctx, dBuf, dLen);

	argument_tokeniser(ctx, dBuf, dLen);
	tok = ctx->token_ctx.command;

	if (NULL != tok) {
		for (pconsolecommand iter = ctx->con->consolePageHolder.current_page;
				iter->type != CONSOLE_ENTRY_END; iter++)
		{
			if (strcmp(iter->commandstr, tok) == 0) {
				if (CONSOLE_PAGE == iter->type) {

					handle_page_call(ctx, iter);
					return;
				} else {
					if (iter->func != NULL
							&& (iter->req_arg_count
									== ctx->token_ctx.argument_count))
					{
						iter->func(ctx,ctx->token_ctx.argument,ctx->token_ctx.argument_count);
						return;
					} else {
						if (iter->req_arg_count
								< ctx->token_ctx.argument_count) {
							LOGD("too many arguments\n");
						} else {
							LOGD("too few arguments\n");

						}
						LOGD("USAGE:%s\n",iter->advance_helpstr);
						return;
					}
				}
			}
		}

		for (pconsolecommand iter = ctx->con->houseKeepingCmd;
				iter->type != CONSOLE_ENTRY_END; iter++) {
			if (strcmp(iter->commandstr, tok) == 0) {
				if (CONSOLE_PAGE == iter->type) {

					handle_page_call(ctx, iter);
					return;
				} else {
					if (iter->func != NULL
							&& (iter->req_arg_count
									== ctx->token_ctx.argument_count)) {
						iter->func(ctx,ctx->token_ctx.argument,ctx->token_ctx.argument_count);
						return;
					} else {
						if (iter->req_arg_count
								< ctx->token_ctx.argument_count) {
							LOGD("too many arguments\n");
						} else {
							LOGD("too few arguments\n");

						}
						LOGD("USAGE:%s\n",iter->advance_helpstr);
						return;
					}
				}
			}
		}
	} else {
	}

	LOGD("INVALID INPUT\n");

}

void consoleBinaryInterpretor(void *hdl ,uint8_t* dBuf , uint16_t dLen)
{

}

static void printhelpstring(p_cli_ctx ctx, pconsolecommand cmd)
{
	LOG_SPACER;

	LOGD("CHOSE FROM BELOW MENU:\n\n");
	for(pconsolecommand cmditer = &cmd[0]; CONSOLE_ENTRY_END != cmditer->type
			;cmditer++)
	{
		if(cmditer->type == CONSOLE_PAGE){
		console("%s %s:%s\n" , cmditer->commandstr,printFormatSpace(cmditer->commandstr,PRINT_HELP_SPACER),cmditer->helpstr);
		}
	}
	LOGD("\n");

	for (pconsolecommand cmditer = &cmd[0]; CONSOLE_ENTRY_END != cmditer->type;
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

	for (pconsolecommand cmditer = ctx->con->houseKeepingCmd; CONSOLE_ENTRY_END != cmditer->type;
			cmditer++)
	{
		console("%s %s:%s\n" , cmditer->commandstr,
				printFormatSpace(cmditer->commandstr,PRINT_HELP_SPACER),cmditer->helpstr);
	}

	LOG_SPACER;

}

bool exit_check(p_cli_ctx hdl)
{
	bool ret_val = true;

	if(NULL != hdl)
	{
		ret_val = hdl->quit_flag;
	}

	return(ret_val);
}

static bool argument_tokeniser(p_cli_ctx hdl, uint8_t* dBuf , uint16_t dLen)
{
	bool ret_val = false;

	settokensparams(&hdl->token_ctx,dBuf,dLen);

	hdl->token_ctx.command = (char *)getnexttoken(&hdl->token_ctx);

	for(int32_t i =0 ; i < MAX_ARGUMENT ; i++)
	{
		hdl->token_ctx.argument[i] = (char *)getnexttoken(&hdl->token_ctx);
		if(NULL != hdl->token_ctx.argument[i])
		{
			hdl->token_ctx.argument_count++;
		}
		else{
			break;
		}
	}

	return ret_val;
}

bool enable_echo(p_cli_ctx hdl,bool flag)
{
	bool ret_val = false;

	if(NULL != hdl)
	{
		hdl->con->echo = flag;
	}

	return(ret_val);
}



