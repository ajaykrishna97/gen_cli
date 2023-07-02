 /*
 * cli.h
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#ifndef INCLUDE_CLI_H_
#define INCLUDE_CLI_H_

#include "string.h"
#include  "helper.h"

#ifdef __linux__
#elif _WIN32
#include <windows.h>
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif

#define MAX_RX_BUF 1024
#define MAX_TX_BUF 1024
#define MAX_CONSOLE_BUF 1024

typedef void (*interpretorFuncPtr)(pvoid ,uint8_t* , uint16_t);
typedef void (*msgfuncptr)(void *hdl);



typedef struct cli_buffer_St
{
	HANDLE 		 con_thread;
	uint32_t     lastChar;
	uint8_t      rxBuf[MAX_RX_BUF];
	uint8_t      txBuf[MAX_TX_BUF];
	uint16_t     rx_p;
	uint16_t     rx_g;
	uint16_t     tx_p;
	uint16_t     tx_g;
	uint16_t     trigger;
	uint16_t     maxRxBufLen;
	uint16_t     maxTxBufLen;
	uint16_t     timeout;

}cli_buf_h , *p_cli_buf_h;

typedef struct consolecommands_
{
	char *commandstr;
	char *helpstr;
	msgfuncptr func;

}consolecommands , *pconsolecommand;

typedef struct consoleHandler_st
{
	uint8_t              buf[MAX_CONSOLE_BUF];
	uint16_t             index;
	uint16_t             maxBufLen;
	uint16_t             timeout;
	uint8_t              echo;
	interpretorFuncPtr   asciiInterpretor;
	interpretorFuncPtr   binaryInterpretor;
	pconsolecommand      consoleCmd;
}consoleHandler , *pconsoleHandler;

typedef struct cli_st
{
	tokener_ctx      token_ctx;
	pconsoleHandler  con;
	cli_buf_h        data_buf;
}cli_ctx , *p_cli_ctx;

#endif /* INCLUDE_CLI_H_ */
