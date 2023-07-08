/*
 * helper.h
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#ifndef HELPER_INCLUDE_HELPER_H_
#define HELPER_INCLUDE_HELPER_H_

#include "dtypes.h"
#include "stdio.h"
#include "cli_api.h"

typedef struct tokener_st
{
	u8     *argptr;
	u16     arglen;

	char   *command;
	char   *argument[MAX_ARGUMENT+1];
	u8      argument_count;

}tokener_ctx , *ptokener_ctx;

bool  settokensparams(ptokener_ctx ctx,u8 *s, u16 a);
u8*   getnexttoken(ptokener_ctx ctx);
void  printstream(const char *Ptr, u16 Length);
void  reversecopy(pu8 dst, pu8 src, u16 n);
void  reverseeui64(pu8 src, pu8 dst);
char *printFormatSpace(char *str , uint8_t spaces);


#endif /* HELPER_INCLUDE_HELPER_H_ */
