/*
 * helper.c
 *
 *  Created on: 01-Jul-2023
 *      Author: Ajay Krishna R
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "helper.h"
#include "dtypes.h"

bool settokensparams(ptokener_ctx ctx,u8 *s, u16 a)
{
	bool ret_val = true;

	if(NULL != s)
	{
		ctx->argptr = s;
		ctx->arglen = a;
		ctx->argument_count = 0;

		ret_val = true;
	}
	else{

	}

	return ret_val;

}

u8* getnexttoken(ptokener_ctx ctx)
{
	uint8_t *p = ctx->argptr;
	uint8_t *rtoken;
	if(*p == '\0')
	{
		return NULL;
	}
	while(*p == ' ')
	{
		p++;
	}

	if(*p == '\n' || *p == '\r') return NULL;
	rtoken = p;
	for(int i = 0 ; i< ctx->arglen ; i++)
	{
		if(*p == ' ')
		{
			*p = '\0';
			ctx->argptr = p+1;
			return (rtoken);
		}
		else if( *p == '\n' || *p == '\r')
		{
			*p = '\0';
			ctx->argptr = p;
			return (rtoken);
		}
		else{
			p++;
		}
	}
	return NULL;
}

const char *cabuf = "0123456789ABCDEF";

void printstream(const char *Ptr, u16 Length)
{

	int i=0;
	while(i<Length) {
		u8 c = *Ptr++; i++;
		putchar(cabuf[c>>4]);
		putchar(cabuf[c&15]);
		putchar(' ');
		if((i&15)==0) {
			putchar('\n');
		}
	}
	putchar('\n');
}

void reversecopy(pu8 dst, pu8 src, u16 n) {
	pu8 s = (pu8)src + n - 1;
	pu8 d = (pu8)dst;
	while (n>0) {
		*d++ = *s--;
		n--;
	}
}

void reverseeui64(pu8 src, pu8 dst)
{
	pu8 p = dst;
	*p++ = src[7];
	*p++ = src[6];
	*p++ = src[5];
	*p++ = src[4];
	*p++ = src[3];
	*p++ = src[2];
	*p++ = src[1];
	*p++ = src[0];

}

#define MAX_SPACES 32
char spaceBuf[MAX_SPACES] = {0};

char *printFormatSpace(char *str , uint8_t spaces)
{
	if(spaces > MAX_SPACES) spaces = MAX_SPACES;

	if(spaces < strlen(str)) spaces = strlen(str);

	memset((void *)spaceBuf ,0 ,32);
	memset((void *)spaceBuf ,0x20 ,(spaces - strlen(str)));

	return (spaceBuf);
}





