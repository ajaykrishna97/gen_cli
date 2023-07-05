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
#include "cli_api.h"

#ifdef __linux__
#include <pthread.h>
#elif _WIN32
#include <windows.h>
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif

#define MAX_PAGE_HISTORY 16

#define MAX_RX_BUF 1024
#define MAX_TX_BUF 1024
#define MAX_CONSOLE_BUF 1024

#define STAR_SPACER LOGD("*********************************************************\n")

typedef struct cli_buffer_St
{

#ifdef __linux__
    pthread_t 	 con_thread;
#elif _WIN32
	HANDLE 		 con_thread;
#else
#error "PLATFORM NOT SELECTED WIN/LINUX"
#endif
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



typedef struct page_handler_st
{
	uint16_t history_count;
	pconsolecommand page_history[MAX_PAGE_HISTORY];

	pconsolecommand home_page;
	pconsolecommand current_page;

}console_page_st, *p_console_page_st;

typedef struct consoleHandler_st
{
	uint8_t                buf[MAX_CONSOLE_BUF];
	uint16_t               index;
	uint16_t               maxBufLen;
	uint16_t               timeout;
	uint8_t                echo;
	interpretorFuncPtr     asciiInterpretor;
	interpretorFuncPtr     binaryInterpretor;
	console_page_st        consolePageHolder;
	pconsolecommand        houseKeepingCmd;
}consoleHandler , *pconsoleHandler;

typedef struct cli_st
{
	tokener_ctx      token_ctx;
	pconsoleHandler  con;
	cli_buf_h        data_buf;
}cli_ctx , *p_cli_ctx;

#endif /* INCLUDE_CLI_H_ */
