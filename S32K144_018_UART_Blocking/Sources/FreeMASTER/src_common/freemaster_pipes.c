/*******************************************************************************
 *
 * Copyright 2014-2015 NXP Semiconductor, Inc.
 *
 * This software is owned or controlled by NXP Semiconductor.
 * Use of this software is governed by the NXP FreeMASTER License
 * distributed with this Material.
 * See the LICENSE file distributed for more details.
 *
 ****************************************************************************
 *
 * @brief  FreeMASTER loss-less "stream" communication
 *
 * This file contains the FreeMASTER PIPE implementation, whcih enables a
 * lossless data I/O streams between PC and embedded application
 *
 *******************************************************************************/

#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_protocol.h"

#if FMSTR_USE_PIPES && (!FMSTR_DISABLE)

#if FMSTR_USE_PIPE_PRINTF_VARG
#include <stdarg.h>
#endif

/***********************************
 *  local types
 ***********************************/

/* runtime flags */
typedef union
{
    FMSTR_FLAGS all;

    struct
    {
        unsigned bExpectOdd : 1;   /* now expecting even round */
        unsigned bInComm : 1;      /* in protocol handler now */
        unsigned bLocked : 1;      /* data buffer access is locked */
    } flg;

} FMSTR_PIPE_FLAGS;

typedef union
{
    FMSTR_FLAGS all;

    struct
    {
        unsigned bIsFull : 1;     /* buffer is full (wp==rp, but not empty) */
    } flg;

} FMSTR_PIPE_BUFF_FLAGS;

typedef struct
{
    FMSTR_ADDR pBuff;
    FMSTR_PIPE_SIZE nSize;
    FMSTR_PIPE_SIZE nWP;
    FMSTR_PIPE_SIZE nRP;
    FMSTR_PIPE_BUFF_FLAGS flags;
} FMSTR_PIPE_BUFF;

typedef struct
{
    /* pipe configuration */
    FMSTR_PIPE_BUFF rx;
    FMSTR_PIPE_BUFF tx;
    FMSTR_PIPE_PORT nPort;
    FMSTR_PPIPEFUNC pCallback;

    /* runtime information */
    FMSTR_PIPE_FLAGS flags;
    FMSTR_U8 nLastBytesReceived;

    /* residue buffer for 8-bit handling on DSP56F8xx platform */
#if FMSTR_CFG_BUS_WIDTH > 1
    FMSTR_U16 rd8Resid;
    FMSTR_U16 wr8Resid;
#endif

#if FMSTR_USE_PIPE_PRINTF
    char printfBuff[FMSTR_PIPES_PRINTF_BUFF_SIZE];
    FMSTR_SIZE8 printfBPtr;
#endif

} FMSTR_PIPE;

/* Pipe ITOA formatting flags */
typedef union
{
    FMSTR_FLAGS all;

    struct
    {
        unsigned upperc : 1;     /* uppercase HEX letters */
        unsigned zeroes : 1;     /* prepend with zeroes rather than spaces */
        unsigned showsign : 1;   /* show sign always (even if plus) */
        unsigned negative : 1;   /* sign determined during itoa */
        unsigned signedtype : 1; /* type is signed (used with va_list only) */
        unsigned isstring : 1;   /* formatting %s */

    } flg;

} FMSTR_PIPE_ITOA_FLAGS;

/* Pipe printf context */
typedef struct
{
    FMSTR_PIPE_ITOA_FLAGS flags;
    FMSTR_U8 radix;
    FMSTR_U8 dtsize;
    FMSTR_SIZE8 alen;

} FMSTR_PIPE_PRINTF_CTX;

typedef FMSTR_BOOL (*FMSTR_PIPE_ITOA_FUNC)(FMSTR_HPIPE, const void*, FMSTR_PIPE_PRINTF_CTX*);

/***********************************
 *  local variables
 ***********************************/

static FMSTR_PIPE pcm_pipes[FMSTR_MAX_PIPES_COUNT];

/**********************************************************************
 *  local macros
 **********************************************************************/

#define FMSTR_PIPE_ITOAFMT_BIN  0U
#define FMSTR_PIPE_ITOAFMT_OCT  1U
#define FMSTR_PIPE_ITOAFMT_DEC  2U
#define FMSTR_PIPE_ITOAFMT_HEX  3U
#define FMSTR_PIPE_ITOAFMT_CHAR 4U

#define FMSTR_IS_DIGIT(x) (((x)>='0') && ((x)<='9'))
#define FMSTR_I2XU(x) (((x)<10) ? ((x)+'0') : ((x)-10+'A'))
#define FMSTR_I2XL(x) (((x)<10) ? ((x)+'0') : ((x)-10+'a'))

/**********************************************************************
 *  local functions
 **********************************************************************/

static FMSTR_PIPE* FMSTR_FindPipe(FMSTR_PIPE_PORT nPort);
static FMSTR_PIPE_SIZE FMSTR_PipeGetBytesFree(FMSTR_PIPE_BUFF* pbuff);
static FMSTR_PIPE_SIZE FMSTR_PipeGetBytesReady(FMSTR_PIPE_BUFF* pbuff);
static void FMSTR_PipeDiscardBytes(FMSTR_PIPE_BUFF* pbuff, FMSTR_SIZE8 count);
static FMSTR_BPTR FMSTR_PipeReceive(FMSTR_BPTR pMessageIO, FMSTR_PIPE* pp, FMSTR_SIZE8 size);
static FMSTR_BPTR FMSTR_PipeTransmit(FMSTR_BPTR pMessageIO, FMSTR_PIPE* pp, FMSTR_SIZE8 size);
static FMSTR_BOOL FMSTR_PipeIToAFinalize(FMSTR_HPIPE hpipe, FMSTR_PIPE_PRINTF_CTX* pctx);

static FMSTR_BOOL FMSTR_PipePrintfOne(FMSTR_HPIPE hpipe, const char* pszFmt, void* parg, FMSTR_PIPE_ITOA_FUNC pItoaFunc);

static FMSTR_BOOL FMSTR_PipePrintfAny(FMSTR_HPIPE hpipe, va_list* parg, FMSTR_PIPE_PRINTF_CTX* pctx);
static FMSTR_BOOL FMSTR_PipePrintfV(FMSTR_HPIPE hpipe, const char* pszFmt, va_list* parg);

/**********************************************************************
 *  local "inline" functions (may also be static on some platforms)
 **********************************************************************/

FMSTR_INLINE FMSTR_BOOL FMSTR_PipePrintfFlush(FMSTR_HPIPE hpipe);
FMSTR_INLINE FMSTR_BOOL FMSTR_PipePrintfPutc(FMSTR_HPIPE hpipe, char c);

/**************************************************************************
 *
 * @brief  Initialization of pipes layer
 *
 ******************************************************************************/

void FMSTR_InitPipes(void)
{
    FMSTR_INDEX i;

    for(i=0; i<FMSTR_MAX_PIPES_COUNT; i++)
        pcm_pipes[i].nPort = 0;
}

/**************************************************************************
 *
 * @brief  API: Opening the pipe
 *
 ******************************************************************************/

FMSTR_HPIPE FMSTR_PipeOpen(FMSTR_PIPE_PORT nPort, FMSTR_PPIPEFUNC pCallback,
                           FMSTR_ADDR pRxBuff, FMSTR_PIPE_SIZE nRxSize,
                           FMSTR_ADDR pTxBuff, FMSTR_PIPE_SIZE nTxSize)
{
    FMSTR_PIPE* pp = &pcm_pipes[0];
    FMSTR_INDEX ifree = -1;
    FMSTR_INDEX i;

    for(i=0; i<FMSTR_MAX_PIPES_COUNT; i++, pp++)
    {
        /* find first free pipe */
        if(pp->nPort == 0 && ifree < 0)
            ifree = i;
        /* look for existing pipe with the same port */
        if(pp->nPort == nPort)
            break;
    }

    /* pipe not found */
    if(i >= FMSTR_MAX_PIPES_COUNT)
    {
        /* create new pipe */
        if(ifree >= 0)
            pp = &pcm_pipes[ifree];
        /* no slot for creating pipe */
        else
            return NULL;
    }

    /* disable pipe (just in case the interrupt would come now) */
    pp->nPort = 0;

    /* initialize pipe */
    pp->rx.pBuff = pRxBuff;
    pp->rx.nSize = nRxSize;
    pp->rx.nWP = 0;
    pp->rx.nRP = 0;
    pp->rx.flags.all = 0;
    pp->tx.pBuff = pTxBuff;
    pp->tx.nSize = nTxSize;
    pp->tx.nWP = 0;
    pp->tx.nRP = 0;
    pp->tx.flags.all = 0;
    pp->flags.all = 0;
    pp->pCallback = pCallback;

#if FMSTR_USE_PIPE_PRINTF
    pp->printfBPtr = 0;
#endif

    /* activate pipe for the new port */
    pp->nPort = nPort;

    return (FMSTR_HPIPE) pp;
}

/**************************************************************************
 *
 * @brief  PIPE API: Close pipe
 *
 ******************************************************************************/

void FMSTR_PipeClose(FMSTR_HPIPE hpipe)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;

    /* un-initialize pipe */
    if(pp != NULL)
        pp->nPort = 0;
}

/**************************************************************************
 *
 * @brief  PIPE API: Write data to a pipe
 *
 ******************************************************************************/

FMSTR_PIPE_SIZE FMSTR_PipeWrite(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE length, FMSTR_PIPE_SIZE granularity)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_PIPE_BUFF* pbuff = &pp->tx;
    FMSTR_PIPE_SIZE total = FMSTR_PipeGetBytesFree(pbuff);
    FMSTR_PIPE_SIZE s;

    /* when invalid address is given, just return number of bytes free */
    if(addr)
    {
        /* only fill the free space */
        if(length > total)
            length = total;

        /* obey granularity */
        if(granularity > 1)
        {
            length /= granularity;
            length *= granularity;
        }

        /* return value */
        total = length;

        /* valid length only */
        if(length > 0)
        {
            /* total bytes available in the rest of buffer */
            s = (FMSTR_PIPE_SIZE) ((pbuff->nSize - pbuff->nWP) * FMSTR_CFG_BUS_WIDTH);
            if(s > length)
                s = length;

            /* get the bytes */
            FMSTR_CopyMemory(pbuff->pBuff + pbuff->nWP, addr, (FMSTR_SIZE8) s);
            addr += s / FMSTR_CFG_BUS_WIDTH;

            /* advance & wrap pointer */
            pbuff->nWP += s / FMSTR_CFG_BUS_WIDTH;
            if(pbuff->nWP >= pbuff->nSize)
                pbuff->nWP = 0;

            /* rest of frame to a (wrapped) beggining of buffer */
            length -= (FMSTR_SIZE8) s;
            if(length > 0)
            {
                FMSTR_CopyMemory(pbuff->pBuff + pbuff->nWP, addr, (FMSTR_SIZE8) length);
                pbuff->nWP += length / FMSTR_CFG_BUS_WIDTH;
            }

            /* buffer got full? */
            if(pbuff->nWP == pbuff->nRP)
                pbuff->flags.flg.bIsFull = 1;
        }
    }

    return total;
}

/**************************************************************************
 *
 * @brief  PIPE API: Put zero-terminated string into pipe. Succeedes only
 * if full string fits into the output buffer and return TRUE if so.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePuts(FMSTR_HPIPE hpipe, const char* pszStr)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_PIPE_BUFF* pbuff = &pp->tx;
    FMSTR_PIPE_SIZE free = FMSTR_PipeGetBytesFree(pbuff);
    FMSTR_PIPE_SIZE slen = (FMSTR_PIPE_SIZE) FMSTR_StrLen((FMSTR_ADDR)pszStr);

    if(slen > free)
        return FMSTR_FALSE;

    FMSTR_PipeWrite(hpipe, (FMSTR_ADDR)pszStr, slen, 0);
    return FMSTR_TRUE;
}

/**************************************************************************
 *
 * @brief  PRINTF-like formatting functions
 *
 *****************************************************************************/

#if FMSTR_USE_PIPE_PRINTF

/**************************************************************************
 *
 * @brief  Flush pipe's printf formatting buffer into pipe output buffer
 *
 *****************************************************************************/

FMSTR_INLINE FMSTR_BOOL FMSTR_PipePrintfFlush(FMSTR_HPIPE hpipe)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_BOOL ok = FMSTR_TRUE;

    if(pp->printfBPtr)
    {
        FMSTR_SIZE8 sz = (FMSTR_SIZE8) FMSTR_PipeWrite(hpipe, (FMSTR_ADDR)pp->printfBuff, (FMSTR_PIPE_SIZE)pp->printfBPtr, 0);

        /* all characters could NOT be printed */
        if(sz != pp->printfBPtr)
            ok = FMSTR_FALSE;

        /* not a cyclic buffer, must start over anyway (also if error) */
        pp->printfBPtr = 0;
    }

    return ok;
}

/**************************************************************************
 *
 * @brief  Put one character into pipe's printf formating buffer
 *
 *****************************************************************************/

FMSTR_INLINE FMSTR_BOOL FMSTR_PipePrintfPutc(FMSTR_HPIPE hpipe, char c)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;

    if(pp->printfBPtr >= FMSTR_PIPES_PRINTF_BUFF_SIZE)
    {
        if(!FMSTR_PipePrintfFlush(hpipe))
            return FMSTR_FALSE;
    }

    pp->printfBuff[pp->printfBPtr++] = c;
    return FMSTR_TRUE;
}

/**************************************************************************
 *
 * @brief  This function finishes the number formatting, adds spacing, signs
 *         and reverses the string (the input is comming in reversed order)
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeIToAFinalize(FMSTR_HPIPE hpipe, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_SIZE8 bptr, minlen, i, bhalf;
    char z, sgn;

    /* buffer pointer into local variable */
    bptr = pp->printfBPtr;

    /* if anything goes wrong, throw our prepared string away */
    pp->printfBPtr = 0;

    /* zero may come as an empty string from itoa procedures */
    if(bptr == 0)
    {
        pp->printfBuff[0] = '0';
        bptr = 1;
    }

    /* first strip the zeroes put by itoa */
    while(bptr > 1 && pp->printfBuff[bptr-1] == '0')
        bptr--;

    /* determine sign to print */
    if(pctx->flags.flg.negative)
    {
        sgn = '-';

        /* minus need to be shown always */
        pctx->flags.flg.showsign = 1U;
    }
    else
    {
        /* plus will be shown if flg.showsign was set by caller */
        sgn = '+';
    }

    /* unsigned types can not print sign */
    if(!pctx->flags.flg.signedtype)
        pctx->flags.flg.showsign = 0U;

    /* calculate minimum buffer length needed */
    minlen = bptr;
    if(pctx->flags.flg.showsign)
        minlen++;

    /* will it fit? */
    if(FMSTR_PIPES_PRINTF_BUFF_SIZE < minlen)
        return FMSTR_FALSE;

    /* required length should never exceed the buffer length */
    if(pctx->alen > FMSTR_PIPES_PRINTF_BUFF_SIZE)
        pctx->alen = FMSTR_PIPES_PRINTF_BUFF_SIZE;

    /* choose prefix character (zero, space or sign-extension OCT/HEX/BIN) */
    if(pctx->flags.flg.zeroes)
    {
        z = '0';

        /* sign extend? */
        if(pctx->flags.flg.negative)
        {
            switch(pctx->radix)
            {
            case FMSTR_PIPE_ITOAFMT_BIN:
                z = '1';
                break;
            case FMSTR_PIPE_ITOAFMT_OCT:
                z = '7';
                break;
            case FMSTR_PIPE_ITOAFMT_HEX:
                z = pctx->flags.flg.upperc ? 'F' : 'f';
                break;
            }
        }

        /* the sign will be in front of added zeroes */
        if(pctx->flags.flg.showsign)
            pctx->alen--;
    }
    else
    {
        z = ' ';

        /* sign should be in front of the number */
        if(pctx->flags.flg.showsign)
        {
            pp->printfBuff[bptr++] = sgn;
            pctx->flags.flg.showsign = 0; /* prevent it to be added again below  */
        }
    }

    /* now fill to required len */
    while(bptr < pctx->alen)
        pp->printfBuff[bptr++] = z;

    /* add the sign if needed */
    if(pctx->flags.flg.showsign)
        pp->printfBuff[bptr++] = sgn;

    /* buffer contains this number of characters */
    pp->printfBPtr = bptr;

    /* now reverse the string and feed it to pipe */
    bhalf = bptr/2;
    bptr--;
    for(i=0; i<bhalf; i++)
    {
        z = pp->printfBuff[i];
        pp->printfBuff[i] = pp->printfBuff[bptr-i];
        pp->printfBuff[bptr-i] = z;
    }

    return FMSTR_TRUE;
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeU8ToA(FMSTR_HPIPE hpipe, const FMSTR_U8* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_U8 arg = *parg;
    FMSTR_U8 tmp;
    FMSTR_INDEX i;

    switch(pctx->radix)
    {
    case FMSTR_PIPE_ITOAFMT_CHAR:
        pp->printfBuff[pp->printfBPtr++] = (char)arg;
        break;

    case FMSTR_PIPE_ITOAFMT_BIN:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 8)
            return FMSTR_FALSE;

        for(i=0; arg && i<8; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 1) + '0');
            arg >>= 1;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_OCT:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 3)
            return FMSTR_FALSE;

        for(i=0; arg && i<3; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 7) + '0');
            arg >>= 3;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_DEC:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 3)
            return FMSTR_FALSE;

        for(i=0; arg && i<3; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg % 10) + '0');
            arg /= 10;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_HEX:
    default:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 2)
            return FMSTR_FALSE;

        for(i=0; arg && i<2; i++)
        {
            tmp = arg & 15;
            pp->printfBuff[pp->printfBPtr++] = (char)(pctx->flags.flg.upperc ? FMSTR_I2XU(tmp) : FMSTR_I2XL(tmp));
            arg >>= 4;
        }
        break;
    }

    return FMSTR_PipeIToAFinalize(hpipe, pctx);
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeS8ToA(FMSTR_HPIPE hpipe, const FMSTR_S8* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_S8 arg = *parg;

    if(arg < 0)
    {
        pctx->flags.flg.negative = 1U;

        /* if sign will be shown, then negate the number */
        if(pctx->flags.flg.signedtype)
            arg *= -1;
    }

    return FMSTR_PipeU8ToA(hpipe, (const FMSTR_U8*)&arg, pctx);
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeU16ToA(FMSTR_HPIPE hpipe, const FMSTR_U16* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_U16 arg = *parg;
    FMSTR_U16 tmp;
    FMSTR_INDEX i;

    switch(pctx->radix)
    {
    case FMSTR_PIPE_ITOAFMT_CHAR:
        pp->printfBuff[pp->printfBPtr++] = (char)arg;
        break;

    case FMSTR_PIPE_ITOAFMT_BIN:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 16)
            return FMSTR_FALSE;

        for(i=0; arg && i<16; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 1) + '0');
            arg >>= 1;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_OCT:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 6)
            return FMSTR_FALSE;

        for(i=0; arg && i<6; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 7) + '0');
            arg >>= 3;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_DEC:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 5)
            return FMSTR_FALSE;

        for(i=0; arg && i<5; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg % 10) + '0');
            arg /= 10;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_HEX:
    default:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 4)
            return FMSTR_FALSE;

        for(i=0; arg && i<4; i++)
        {
            tmp = arg & 15;
            pp->printfBuff[pp->printfBPtr++] = (char)(pctx->flags.flg.upperc ? FMSTR_I2XU(tmp) : FMSTR_I2XL(tmp));
            arg >>= 4;
        }
        break;
    }

    return FMSTR_PipeIToAFinalize(hpipe, pctx);
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeS16ToA(FMSTR_HPIPE hpipe, const FMSTR_S16* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_S16 arg = *parg;

    if(arg < 0)
    {
        pctx->flags.flg.negative = 1U;

        /* if sign will be shown, then negate the number */
        if(pctx->flags.flg.signedtype)
            arg *= -1;
    }

    return FMSTR_PipeU16ToA(hpipe, (const FMSTR_U16*)&arg, pctx);
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeU32ToA(FMSTR_HPIPE hpipe, const FMSTR_U32* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_U32 arg = *parg;
    FMSTR_U32 tmp;
    FMSTR_INDEX i;

    switch(pctx->radix)
    {
    case FMSTR_PIPE_ITOAFMT_CHAR:
        pp->printfBuff[pp->printfBPtr++] = (char)arg;
        break;

    case FMSTR_PIPE_ITOAFMT_BIN:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 32)
            return FMSTR_FALSE;

        for(i=0; arg && i<32; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 1) + '0');
            arg >>= 1;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_OCT:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 11)
            return FMSTR_FALSE;

        for(i=0; arg && i<11; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg & 7) + '0');
            arg >>= 3;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_DEC:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 10)
            return FMSTR_FALSE;

        for(i=0; arg && i<10; i++)
        {
            pp->printfBuff[pp->printfBPtr++] = (char)((arg % 10) + '0');
            arg /= 10;
        }
        break;

    case FMSTR_PIPE_ITOAFMT_HEX:
    default:
        if(FMSTR_PIPES_PRINTF_BUFF_SIZE < 8)
            return FMSTR_FALSE;

        for(i=0; arg && i<8; i++)
        {
            tmp = arg & 15;
            pp->printfBuff[pp->printfBPtr++] = (char)(pctx->flags.flg.upperc ? FMSTR_I2XU(tmp) : FMSTR_I2XL(tmp));
            arg >>= 4;
        }
        break;
    }

    return FMSTR_PipeIToAFinalize(hpipe, pctx);
}

/**************************************************************************
 *
 * @brief  This function formats the argument into the temporary printf buffer
 *         It is granted by the caller that the buffer is empty before calling.
 *
 *****************************************************************************/

static FMSTR_BOOL FMSTR_PipeS32ToA(FMSTR_HPIPE hpipe, const FMSTR_S32* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_S32 arg = *parg;

    if(arg < 0)
    {
        pctx->flags.flg.negative = 1U;

        /* if sign will be shown, then negate the number */
        if(pctx->flags.flg.signedtype)
            arg *= -1;
    }

    return FMSTR_PipeU32ToA(hpipe, (const FMSTR_U32*)&arg, pctx);
}

/**************************************************************************
 *
 * @brief  This function parses the printf format and sets the context
 *         structure properly.
 *
 * @return The function returns the pointer to end of format string handled
 *
 *****************************************************************************/

static const char* FMSTR_PipeParseFormat(const char* pszFmt, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    pctx->flags.all = 0;

    /* skip percent sign */
    if(*pszFmt == '%')
        pszFmt++;

    /* show sign always? */
    if(*pszFmt == '+')
    {
        pctx->flags.flg.showsign = 1U;
        pszFmt++;
    }

    /* prefix with zeroes? */
    if(*pszFmt == '0')
    {
        pctx->flags.flg.zeroes = 1U;
        pszFmt++;
    }

    /* parse length */
    pctx->alen = 0;
    while(FMSTR_IS_DIGIT(*pszFmt))
    {
        pctx->alen *= 10;
        pctx->alen += *pszFmt - '0';
        pszFmt++;
    }

    /* default data type is 'int' */
    pctx->dtsize = sizeof(int);

    /* parse dtsize modifier */
    switch(*pszFmt)
    {
        /* short modifier (char for hh)*/
    case 'h':
        pctx->dtsize = sizeof(short);
        pszFmt++;

        /* one more 'h' means 'char' */
        if(*pszFmt == 'h')
        {
            pctx->dtsize = sizeof(char);
            pszFmt++;
        }
        break;

    case 'l':
        pctx->dtsize = sizeof(long);
        pszFmt++;
        break;
    }

    /* now finaly concluding to format letter */
    switch(*pszFmt++)
    {
        /* HEXADECIMAL */
    case 'X':
        pctx->flags.flg.upperc = 1U;
        /* falling thru case */

        /* hexadecimal */
    case 'x':
        pctx->radix = FMSTR_PIPE_ITOAFMT_HEX;
        break;

        /* octal */
    case 'o':
        pctx->radix = FMSTR_PIPE_ITOAFMT_OCT;
        break;

        /* binary */
    case 'b':
        pctx->radix = FMSTR_PIPE_ITOAFMT_BIN;
        break;

        /* decimal signed */
    case 'd':
    case 'i':
        pctx->flags.flg.signedtype = 1U;
        /* falling thru case */

        /* decimal unsigned */
    case 'u':
        pctx->radix = FMSTR_PIPE_ITOAFMT_DEC;
        break;

        /* character */
    case 'c':
        pctx->radix = FMSTR_PIPE_ITOAFMT_CHAR;
        pctx->dtsize = sizeof(char);
        break;

        /* string */
    case 's':
        pctx->flags.flg.isstring = 1U;
        pctx->dtsize = sizeof(void*);
        break;

    }

    return pszFmt;
}

/**************************************************************************
 *
 * @brief  Printf with one argument passed by pointer.
 *
 ******************************************************************************/

static FMSTR_BOOL FMSTR_PipePrintfOne(FMSTR_HPIPE hpipe, const char* pszFmt, void* parg, FMSTR_PIPE_ITOA_FUNC pItoaFunc)
{
    FMSTR_BOOL ok = FMSTR_TRUE;
    FMSTR_PIPE_PRINTF_CTX ctx;

    while(*pszFmt && ok)
    {
        if(*pszFmt == '%')
        {
            pszFmt++;

            if(*pszFmt == '%')
            {
                FMSTR_PipePrintfPutc(hpipe, '%');
                pszFmt++;
                continue;
            }

            /* empty the pipe's temporary buffer */
            ok = FMSTR_PipePrintfFlush(hpipe);

            if(ok)
            {
                pszFmt = FMSTR_PipeParseFormat(pszFmt, &ctx);

                if(ctx.flags.flg.isstring)
                {
                    const char* psz = (const char*) parg;
                    FMSTR_PipePuts(hpipe, psz ? psz : "NULL");
                }
                else
                {
                    ok = pItoaFunc(hpipe, parg, &ctx);
                }
            }
        }
        else
        {
            ok = FMSTR_PipePrintfPutc(hpipe, *pszFmt++);
        }
    }

    return ok && FMSTR_PipePrintfFlush(hpipe);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfU8(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U8 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeU8ToA);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfS8(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S8 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeS8ToA);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfU16(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U16 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeU16ToA);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfS16(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S16 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeS16ToA);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfU32(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U32 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeU32ToA);
}

/**************************************************************************
 *
 * @brief  PIPE API: Format argument into the pipe output stream. The format
 *         follows the standard printf format. The leading '%' is optional.
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintfS32(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S32 arg)
{
    return FMSTR_PipePrintfOne(hpipe, pszFmt, &arg, (FMSTR_PIPE_ITOA_FUNC)FMSTR_PipeS32ToA);
}


/**************************************************************************
 *
 * @brief  Format va_list argument into the pipe output stream. This function
 *         is called as a part of our printf routine.
 *
 ******************************************************************************/

static FMSTR_BOOL FMSTR_PipePrintfAny(FMSTR_HPIPE hpipe, va_list* parg, FMSTR_PIPE_PRINTF_CTX* pctx)
{
    FMSTR_BOOL ok = FMSTR_FALSE;

    switch(pctx->dtsize)
    {
    case 1:
        if(pctx->flags.flg.signedtype)
        {
            FMSTR_S8 arg = (FMSTR_S8)va_arg(*parg, int);
            ok = FMSTR_PipeS8ToA(hpipe, &arg, pctx);
        }
        else
        {
            FMSTR_U8 arg = (FMSTR_U8)va_arg(*parg, unsigned);
            ok = FMSTR_PipeU8ToA(hpipe, &arg, pctx);
        }
        break;

    case 2:
        if(pctx->flags.flg.signedtype)
        {
            FMSTR_S16 arg = (FMSTR_S16)va_arg(*parg, int);
            ok = FMSTR_PipeS16ToA(hpipe, &arg, pctx);
        }
        else
        {
            FMSTR_U16 arg = (FMSTR_U16)va_arg(*parg, unsigned);
            ok = FMSTR_PipeU16ToA(hpipe, &arg, pctx);
        }
        break;

    case 4:
        if(pctx->flags.flg.signedtype)
        {
            FMSTR_S32 arg = (FMSTR_S32)va_arg(*parg, long);
            ok = FMSTR_PipeS32ToA(hpipe, &arg, pctx);
        }
        else
        {
            FMSTR_U32 arg = (FMSTR_U32)va_arg(*parg, unsigned long);
            ok = FMSTR_PipeU32ToA(hpipe, &arg, pctx);
        }
        break;
    }

    return ok;
}

/**************************************************************************
 *
 * @brief  Printf with va_list arguments prepared.
 *
 *         This function is not declared static (may be reused as global),
 *         but public prototype is not available (not to force user to
 *         have va_list defined.
 *
 ******************************************************************************/

static FMSTR_BOOL FMSTR_PipePrintfV(FMSTR_HPIPE hpipe, const char* pszFmt, va_list* parg)
{
    FMSTR_BOOL ok = FMSTR_TRUE;
    FMSTR_PIPE_PRINTF_CTX ctx;

    while(*pszFmt && ok)
    {
        if(*pszFmt == '%')
        {
            pszFmt++;

            if(*pszFmt == '%')
            {
                ok = FMSTR_PipePrintfPutc(hpipe, '%');
                pszFmt++;
            }
            else
            {
                ok = FMSTR_PipePrintfFlush(hpipe);

                if(ok)
                {
                    pszFmt = FMSTR_PipeParseFormat(pszFmt, &ctx);

                    if(ctx.flags.flg.isstring)
                    {
                        const char* psz = va_arg(*parg, char*);
                        FMSTR_PipePuts(hpipe, psz ? psz : "NULL");
                    }
                    else
                    {
                        ok = FMSTR_PipePrintfAny(hpipe, parg, &ctx);
                    }
                }
            }
        }
        else
        {
            ok = FMSTR_PipePrintfPutc(hpipe, *pszFmt++);
        }
    }

    return ok && FMSTR_PipePrintfFlush(hpipe);
}

#if FMSTR_USE_PIPE_PRINTF_VARG

/**************************************************************************
 *
 * @brief  PIPE API: The printf into the pipe
 *
 ******************************************************************************/

FMSTR_BOOL FMSTR_PipePrintf(FMSTR_HPIPE hpipe, const char* pszFmt, ...)
{
    FMSTR_BOOL ok;

    va_list args;
    va_start(args, pszFmt);
    ok = FMSTR_PipePrintfV(hpipe, pszFmt, &args);
    va_end(args);

    return ok;
}

#endif /* FMSTR_USE_PIPE_PRINTF_VARG */
#endif /* FMSTR_USE_PIPE_PRINTF */

/**************************************************************************
 *
 * @brief  PIPE API: Read data from a pipe
 *
 ******************************************************************************/

FMSTR_PIPE_SIZE FMSTR_PipeRead(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE length, FMSTR_PIPE_SIZE granularity)
{
    FMSTR_PIPE* pp = (FMSTR_PIPE*) hpipe;
    FMSTR_PIPE_BUFF* pbuff = &pp->rx;
    FMSTR_PIPE_SIZE total = FMSTR_PipeGetBytesReady(pbuff);
    FMSTR_PIPE_SIZE s;

    /* when invalid address is given, return number of bytes available */
    if(addr)
    {
        /* round length to bus width */
        length /= FMSTR_CFG_BUS_WIDTH;
        length *= FMSTR_CFG_BUS_WIDTH;

        /* only fetch what we have cached */
        if(length > total)
            length = total;

        /* obey granularity */
        if(granularity > 1)
        {
            length /= granularity;
            length *= granularity;
        }

        /* return value */
        total = length;

        /* rest of cyclic buffer */
        if(length > 0)
        {
            /* total bytes available in the rest of buffer */
            s = (FMSTR_PIPE_SIZE) ((pbuff->nSize - pbuff->nRP) * FMSTR_CFG_BUS_WIDTH);
            if(s > length)
                s = length;

            /* put bytes */
            FMSTR_CopyMemory(addr, pbuff->pBuff + pbuff->nRP, (FMSTR_SIZE8) s);
            addr += s / FMSTR_CFG_BUS_WIDTH;

            /* advance & wrap pointer */
            pbuff->nRP += s / FMSTR_CFG_BUS_WIDTH;
            if(pbuff->nRP >= pbuff->nSize)
                pbuff->nRP = 0;

            /* rest of frame to a (wrapped) beggining of buffer */
            length -= (FMSTR_SIZE8) s;
            if(length > 0)
            {
                FMSTR_CopyMemory(addr, pbuff->pBuff + pbuff->nRP, (FMSTR_SIZE8) length);
                pbuff->nRP += length / FMSTR_CFG_BUS_WIDTH;
            }

            /* buffer for sure not full now */
            pbuff->flags.flg.bIsFull = 0;
        }
    }

    return total;
}

/**************************************************************************
 *
 * @brief  Find pipe by port number
 *
 ******************************************************************************/

static FMSTR_PIPE* FMSTR_FindPipe(FMSTR_PIPE_PORT nPort)
{
    FMSTR_PIPE* pp = &pcm_pipes[0];
    FMSTR_INDEX i;

    for(i=0; i<FMSTR_MAX_PIPES_COUNT; i++, pp++)
    {
        /* look for existing pipe with the same port */
        if(pp->nPort == nPort)
            return pp;
    }

    return NULL;
}

/**************************************************************************
 *
 * @brief  Get number of bytes free in the buffer
 *
 ******************************************************************************/

static FMSTR_PIPE_SIZE FMSTR_PipeGetBytesFree(FMSTR_PIPE_BUFF* pbuff)
{
    FMSTR_PIPE_SIZE free;

    if(pbuff->flags.flg.bIsFull)
    {
        free = 0;
    }
    else if(pbuff->nWP < pbuff->nRP)
    {
        free = (FMSTR_PIPE_SIZE)(pbuff->nRP - pbuff->nWP);
    }
    else
    {
        free = (FMSTR_PIPE_SIZE)(pbuff->nSize - pbuff->nWP + pbuff->nRP);
    }

    return (FMSTR_PIPE_SIZE)(free * FMSTR_CFG_BUS_WIDTH);
}

static FMSTR_PIPE_SIZE FMSTR_PipeGetBytesReady(FMSTR_PIPE_BUFF* pbuff)
{
    FMSTR_PIPE_SIZE full;

    if(pbuff->flags.flg.bIsFull)
    {
        full = (FMSTR_PIPE_SIZE)pbuff->nSize;
    }
    else if(pbuff->nWP >= pbuff->nRP)
    {
        full = (FMSTR_PIPE_SIZE)(pbuff->nWP - pbuff->nRP);
    }
    else
    {
        full = (FMSTR_PIPE_SIZE)(pbuff->nSize - pbuff->nRP + pbuff->nWP);
    }

    return (FMSTR_PIPE_SIZE)(full * FMSTR_CFG_BUS_WIDTH);
}

static void FMSTR_PipeDiscardBytes(FMSTR_PIPE_BUFF* pbuff, FMSTR_SIZE8 count)
{
    FMSTR_PIPE_SIZE rest = (FMSTR_PIPE_SIZE)(pbuff->nSize - pbuff->nRP);
    FMSTR_PIPE_SIZE total, rp = 0;
    FMSTR_PIPE_SIZE discard;

    total = FMSTR_PipeGetBytesReady(pbuff);
    discard = (FMSTR_PIPE_SIZE) (count > total ? total : count);
    discard /= FMSTR_CFG_BUS_WIDTH;

    if(discard > 0)
    {
        /* will RP wrap? */
        if(rest <= discard)
        {
            rp = (FMSTR_PIPE_SIZE) (discard - rest);
        }
        else
        {
            rp = (FMSTR_PIPE_SIZE) (pbuff->nRP + discard);
        }

        /* buffer is for sure not full */
        pbuff->flags.flg.bIsFull = 0;
    }

    pbuff->nRP = rp;
}

/* get data from frame into our Rx buffer, we are already sure it fits */

static FMSTR_BPTR FMSTR_PipeReceive(FMSTR_BPTR pMessageIO, FMSTR_PIPE* pp, FMSTR_SIZE8 size)
{
    FMSTR_PIPE_BUFF* pbuff = &pp->rx;
    FMSTR_PIPE_SIZE s;

    if(size > 0)
    {
        /* total bytes available in the rest of buffer */
        s = (FMSTR_PIPE_SIZE) ((pbuff->nSize - pbuff->nWP) * FMSTR_CFG_BUS_WIDTH);
        if(s > (FMSTR_PIPE_SIZE) size)
            s = (FMSTR_PIPE_SIZE) size;

        /* get the bytes */
        pMessageIO = FMSTR_CopyFromBuffer(pbuff->pBuff + pbuff->nWP, pMessageIO, (FMSTR_SIZE8) s);

        /* advance & wrap pointer */
        pbuff->nWP += s / FMSTR_CFG_BUS_WIDTH;
        if(pbuff->nWP >= pbuff->nSize)
            pbuff->nWP = 0;

        /* rest of frame to a (wrapped) beginning of buffer */
        size -= (FMSTR_SIZE8) s;
        if(size > 0)
        {
            pMessageIO = FMSTR_CopyFromBuffer(pbuff->pBuff + pbuff->nWP, pMessageIO, size);
            pbuff->nWP += size / FMSTR_CFG_BUS_WIDTH;
        }

        /* buffer got full? */
        if(pbuff->nWP == pbuff->nRP)
            pbuff->flags.flg.bIsFull = 1;
    }

    return pMessageIO;
}

/* put data into the comm buffer, we are already sure it fits, buffer's RP is not modified */

static FMSTR_BPTR FMSTR_PipeTransmit(FMSTR_BPTR pMessageIO, FMSTR_PIPE* pp, FMSTR_SIZE8 size)
{
    FMSTR_PIPE_BUFF* pbuff = &pp->tx;
    FMSTR_PIPE_SIZE s, nRP = pbuff->nRP;

    if(size > 0)
    {
        /* total bytes available in the rest of buffer */
        s = (FMSTR_PIPE_SIZE) ((pbuff->nSize - nRP) * FMSTR_CFG_BUS_WIDTH);
        if(s > (FMSTR_PIPE_SIZE) size)
            s = (FMSTR_PIPE_SIZE) size;

        /* put bytes */
        pMessageIO = FMSTR_CopyToBuffer(pMessageIO, pbuff->pBuff + nRP, (FMSTR_SIZE8) s);

        /* advance & wrap pointer */
        nRP += s / FMSTR_CFG_BUS_WIDTH;
        if(nRP >= pbuff->nSize)
            nRP = 0;

        /* rest of frame to a (wrapped) beggining of buffer */
        size -= (FMSTR_SIZE8) s;
        if(size > 0)
        {
            pMessageIO = FMSTR_CopyToBuffer(pMessageIO, pbuff->pBuff + nRP, size);
        }
    }

    return pMessageIO;
}


/**************************************************************************
 *
 * @brief  Handling PIPE commands
 *
 * @param  pMessageIO - original command (in) and response buffer (out)
 *
 ******************************************************************************/

FMSTR_BPTR FMSTR_PipeFrame(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_U8 nFrameLen, nByte, nPort;
    FMSTR_PIPE* pp;

    /* skip command byte */
    pMessageIO = FMSTR_SkipInBuffer(pMessageIO, 1);

    /* get frame length */
    pMessageIO = FMSTR_ValueFromBuffer8(&nFrameLen, pMessageIO);

    /* need at least port number and tx-discard bytes */
    if(nFrameLen < 1)
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_PIPEERR);

    /* get port number and even/odd flag */
    pMessageIO = FMSTR_ValueFromBuffer8(&nPort, pMessageIO);

    /* get pipe by port */
    pp = FMSTR_FindPipe((FMSTR_PIPE_PORT)(nPort & 0x7f));

    /* pipe port must exist (i.e. be open) */
    if(!pp)
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_PIPEERR);

    /* data-in are valid only in "matching" request (even to even, odd to odd) */
    if(nPort & 0x80)
    {
        if(!pp->flags.flg.bExpectOdd)
            nFrameLen = 0;
        else
            pp->flags.flg.bExpectOdd = !pp->flags.flg.bExpectOdd;
    }
    else
    {
        if(pp->flags.flg.bExpectOdd)
            nFrameLen = 0;
        else
            pp->flags.flg.bExpectOdd = !pp->flags.flg.bExpectOdd;
    }

    /* process received data */
    if(nFrameLen > 0)
    {
        /* first byte tells me how many output bytes can be discarded from my
           pipe-transmit buffer (this is how PC acknowledges how many bytes it
           received and saved from the last response) */
        pMessageIO = FMSTR_ValueFromBuffer8(&nByte, pMessageIO);

        /* discard bytes from pipe's transmit buffer */
        if(nByte)
            FMSTR_PipeDiscardBytes(&pp->tx, nByte);

        /* next come (nFrameLen-2) bytes to be received */
        if(nFrameLen > 2)
        {
            /* how many bytes may I accept? */
            FMSTR_PIPE_SIZE rxFree = FMSTR_PipeGetBytesFree(&pp->rx);
            /* how many bytes PC want to push? */
            FMSTR_U8 rxToRead = (FMSTR_U8)(nFrameLen - 2);

            /* round to bus width */
            rxToRead /= FMSTR_CFG_BUS_WIDTH;
            rxToRead *= FMSTR_CFG_BUS_WIDTH;

            /* get the lower of the two numbers */
            if(rxFree < (FMSTR_PIPE_SIZE)rxToRead)
                rxToRead = (FMSTR_U8)rxFree;

            /* get frame data */
            pMessageIO = FMSTR_PipeReceive(pMessageIO, pp, rxToRead);

            /* this is the number to be returned to PC to inform it how
               many bytes it may discard in his transmit buffer */
            pp->nLastBytesReceived = rxToRead;
        }
        else
        {
            /* no bytes received */
            pp->nLastBytesReceived = 0;
        }
    }

    /* now call the pipe's handler, it may read or write data */
    if(pp->pCallback)
    {
        pp->flags.flg.bInComm = 1;
        pp->pCallback((FMSTR_HPIPE) pp);
        pp->flags.flg.bInComm = 0;
    }

    /* now put our output data */
    {
        /* how many bytes are waiting to be sent? */
        FMSTR_PIPE_SIZE txAvail = FMSTR_PipeGetBytesReady(&pp->tx);
        /* how many bytes I can safely put? */
        FMSTR_U8 txToSend = FMSTR_COMM_BUFFER_SIZE - 3;

        /* round to bus width */
        txToSend /= FMSTR_CFG_BUS_WIDTH;
        txToSend *= FMSTR_CFG_BUS_WIDTH;

        /* get the lower of two values */
        if(txAvail < (FMSTR_PIPE_SIZE)txToSend)
            txToSend = (FMSTR_U8)txAvail;

        /* build frame length */
        nFrameLen = (FMSTR_U8)(txToSend + 2);

        /* send pipe's transmit data back */
        pResponse = FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK | FMSTR_STSF_VARLEN);
        pResponse = FMSTR_ValueToBuffer8(pResponse, nFrameLen);
        pResponse = FMSTR_ValueToBuffer8(pResponse, nPort);

        /* inform PC how many bytes it may discard from its pipe's transmit buffer */
        nByte = pp->nLastBytesReceived;
        pResponse = FMSTR_ValueToBuffer8(pResponse, nByte);

        /* put data */
        if(txToSend)
            pResponse = FMSTR_PipeTransmit(pResponse, pp, txToSend);
    }

    return pResponse;
}

#else /* FMSTR_USE_PIPES && (!FMSTR_DISABLE) */

/* implement void pipe-API functions */

FMSTR_HPIPE FMSTR_PipeOpen(FMSTR_PIPE_PORT nPort, FMSTR_PPIPEFUNC pCallback,
                           FMSTR_ADDR pRxBuff, FMSTR_PIPE_SIZE nRxSize,
                           FMSTR_ADDR pTxBuff, FMSTR_PIPE_SIZE nTxSize)
{
    FMSTR_UNUSED(nPort);
    FMSTR_UNUSED(pCallback);
    FMSTR_UNUSED(pRxBuff);
    FMSTR_UNUSED(nRxSize);
    FMSTR_UNUSED(pTxBuff);
    FMSTR_UNUSED(nTxSize);

    return NULL;
}

void FMSTR_PipeClose(FMSTR_HPIPE hpipe)
{
    FMSTR_UNUSED(hpipe);
}

FMSTR_SIZE FMSTR_PipeWrite(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE size, FMSTR_PIPE_SIZE granularity)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(addr);
    FMSTR_UNUSED(size);
    FMSTR_UNUSED(granularity);

    return 0U;
}

FMSTR_SIZE FMSTR_PipeRead(FMSTR_HPIPE hpipe, FMSTR_ADDR addr, FMSTR_PIPE_SIZE size, FMSTR_PIPE_SIZE granularity)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(addr);
    FMSTR_UNUSED(size);
    FMSTR_UNUSED(granularity);

    return 0U;
}

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_PIPES  && (!FMSTR_DISABLE) */

#if (FMSTR_DISABLE) || (!(FMSTR_USE_PIPES)) || (!(FMSTR_USE_PIPE_PRINTF))

FMSTR_BOOL FMSTR_PipePrintfU8(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U8 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_PipePrintfS8(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S8 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_PipePrintfU16(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U16 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_PipePrintfS16(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S16 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_PipePrintfU32(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_U32 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_PipePrintfS32(FMSTR_HPIPE hpipe, const char* pszFmt, FMSTR_S32 arg)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);
    FMSTR_UNUSED(arg);

    return FMSTR_FALSE;
}

#endif /* (!(FMSTR_USE_PIPES)) || (!(FMSTR_USE_PIPE_PRINTF)) */

#if (FMSTR_DISABLE) || (!(FMSTR_USE_PIPES)) || (!(FMSTR_USE_PIPE_PRINTF_VARG))

FMSTR_BOOL FMSTR_PipePrintf(FMSTR_HPIPE hpipe, const char* pszFmt, ...)
{
    FMSTR_UNUSED(hpipe);
    FMSTR_UNUSED(pszFmt);

    return FMSTR_FALSE;
}

#endif /* (!(FMSTR_USE_PIPES)) || (!(FMSTR_USE_PIPE_PRINTF_VARG)) */
