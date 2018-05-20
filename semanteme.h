#ifndef _SEMANTEME_H
#define _SEMANTEME_H

/***************************************************************
 * 结构体介绍：存放着四元组，包含一个赋值和一个运算语句        *
 * 结构体成员：                                                *
 *         op：存放着操作符                                    *
 *      argv1：存放第一个操作数                                *
 *      argv2：存放第二个操作数                                *
 *     result：存放着结果                                      *
***************************************************************/
struct QUAD
{
    char op[_MAX_LENGTH_];
    char argv1[_MAX_LENGTH_];
    char argv2[_MAX_LENGTH_];
    char result[_MAX_LENGTH_];
};

/***************************************************************
 * 结构体介绍：存放着元素表的基本单元                          *
 * 结构体成员：                                                *
 * Identifier：存放着变量的唯一标识符                          *
 *      value：存放着变量的值，默认为0                         *
***************************************************************/
struct TWOUnit
{
    char Identifier[_MAX_LENGTH_];
    int value;
};

void lrparse(void);

#endif
