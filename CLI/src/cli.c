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
static void printhelpstring(pconsolecommand cmd);

#ifdef __linux__
void handle_get_char(void ctx)
#elif _WIN32
DWORD WINAPI handle_get_char(LPVOID lpParam)
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif
{
	p_cli_ctx con = (p_cli_ctx)lpParam;


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
	LOGD("CLI version 01\n");
}

CMDFUNC(ver)
{
	printVersion();
}

CMDFUNC(help)
{
	p_cli_ctx con = (p_cli_ctx)hdl;

	printhelpstring(con->con->consoleCmd);

}


consolecommands uartCliCmd[] =                                                          //UART CLI commands
{
		COMMAND(ver             , This prints softwares current version),
		COMMAND(help            , command help string),
		{NULL}
};


CLI_STATUS cli_init(CLI_HDL *hdl)
{
	CLI_STATUS ret_val = CLI_FAILUE;

	p_cli_ctx main_ctx = calloc(1 , sizeof(cli_ctx));

	consoleInit(main_ctx,1024,1000,1,uartCliCmd);
	main_ctx->data_buf.con_thread = CreateThread(NULL, 0, handle_get_char, main_ctx, 0, 0);

	if (NULL == main_ctx->data_buf.con_thread )
	{
		LOGD("CON thread init failed \n");
	}
	else{
		*hdl = (CLI_HDL)main_ctx;
		LOGD("CON thread init success \n");
		ret_val = CLI_SUCCESS;
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
		ctx->con->consoleCmd = cmd;
		ctx->con->index = 0;

		ctx->con->asciiInterpretor = consoleAsciiInterpretor;
		ctx->con->binaryInterpretor = consoleBinaryInterpretor;

		ctx->data_buf.maxRxBufLen = MAX_RX_BUF;
		ctx->data_buf.maxTxBufLen = MAX_TX_BUF;

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
		if(NULL != conH->con->asciiInterpretor)
		{
			conH->con->asciiInterpretor(conH ,conH->con->buf , conH->con->index);
			conH->con->index = 0;
		}
	}
	else
	{
	}

	return;
}


void consoleAsciiInterpretor(pvoid ctx ,uint8_t* dBuf , uint16_t dLen)
{
	p_cli_ctx con = (p_cli_ctx)ctx;
	char* tok;

	tok = strtok((char *)dBuf, "\n");

	if (NULL != tok) {
		con->token_ctx.tokencount++;
		for (pconsolecommand iter = con->con->consoleCmd; iter->commandstr;
				iter++) {
			if (strcmp(iter->commandstr, tok) == 0) {
				iter->func(ctx);
				return;
			}
		}
	}

	LOGE("SYNTAX ERROR\n");
}

void consoleBinaryInterpretor(void *hdl ,uint8_t* dBuf , uint16_t dLen)
{

}

static void printhelpstring(pconsolecommand cmd)
{
	for(pconsolecommand cmditer = &cmd[0]; cmditer->commandstr;cmditer++)
	{
		console("%s %s:%s\n" , cmditer->commandstr,printFormatSpace(cmditer->commandstr,12),cmditer->helpstr);
	}

	LOGD("\n");
}

