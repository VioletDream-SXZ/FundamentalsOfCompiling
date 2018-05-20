#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************************
 * 关键字识别码                     *
************************************/
#define _SYN_MAIN  1   /*   main   */
#define _SYN_INT   2   /*   int    */
#define _SYN_CHAR  3   /*   char   */  
#define _SYN_IF    4   /*   if     */
#define _SYN_ELSE  5   /*   else   */
#define _SYN_FOR   6   /*   for    */
#define _SYN_WHILE 7   /*   while  */

#define _SYN_ID    10   /*  标识符识别码  */
#define _SYN_NUM   20   /*  数字识别码    */

/************************************
 * 运算符识别码                     *
************************************/
#define _SYN_ASSIGN            21    /* = */       
#define _SYN_ADD               22    /* + */
#define _SYN_SUB               23    /* - */
#define _SYN_MUL               24    /* * */
#define _SYN_DIV               25    /* / */
#define _SYN_LPAREN            26    /* ( */
#define _SYN_RPAREN            27    /* ) */
#define _SYN_LEFTBRACKET1      28    /* [ */
#define _SYN_RIGHTBRACKET1     29    /* ] */
#define _SYN_LEFTBRACKET2      30    /* { */
#define _SYN_RIGHTBRACKET2     31    /* } */
#define _SYN_COMMA             32    /* , */
#define _SYN_COLON             33    /* : */
#define _SYN_SEMICOLON         34    /* ; */

#define _SYN_LG                35    /*  >  */
#define _SYN_LT                36    /*  <  */
#define _SYN_GE                37    /* > = */
#define _SYN_LE                38    /* < = */
#define _SYN_EQ                39    /* = = */
#define _SYN_NE                40    /* ! = */
#define _SYN_END               1000  /* END */
#define _SYN_ERROR             -1    /*ERROR*/

#define _SYN_ADD_ASSIGN        50    /* + = */
#define _SYN_SUB_ASSIGN        51    /* - = */
#define _SYN_MUL_ASSIGN        52    /* * = */
#define _SYN_DIV_ASSIGN        53    /* / = */

#define _SYN_OR                54    /* | */
#define _SYN_AND               55    /* & */
#define _SYN_NOR               56    /* ^ */

#define _SYN_OP_OR             57    /* || */
#define _SYN_OP_AND            58    /* && */

#define _MAX_LENGTH_           1023  /* 数组定义的最大长度 */

union WORDCONTENT
{
    char str[_MAX_LENGTH_];
    int num;
};

struct WORD
{
    int syn;
    union WORDCONTENT value;
};

#endif
