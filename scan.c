#include "globals.h"
#include "scan.h"

extern char strSource[_MAX_LENGTH_];              // 用于保存输入数据
extern FILE *fw;

int gnColumn,                                  
    gnRow,                  /* 行列好，gnColumn表示行号，gnRow表示列号 */
    gnLocate,               /* 下一个字符脚标                          */
    gnLocateStart;          /* 下一个单词的开始位置                    */
struct WORD uWord;

// 用于判断输入是否存在以下关键字
const char KEY_WORDS[20][20] = 
{
    "main","int","char","if","else","for","while","void","EXIT"
};

/**********************************************************************/ // 华丽的分界线

static void Do_Tag(char* strSource);                                     // 处理标识符
static void Do_EndOfTag(char* strSource);                                //   以及关键字

static void Do_EndOfDigit(char* strSource);                              // 处理数字

static void Do_EndOfAdd(char* strSource);                                // '+','+='
static void Do_EndOfSub(char* srtSOurce);                                // '-','-='
static void Do_EndOfMul(char* strSource);                                // '*','*='
static void Do_EndOfDiv(char* strSource);                                // '/','/='

static void Do_EndOfLParen(char* strSource);                             // '('
static void Do_EndOfRParen(char* strSource);                             // ')'
static void Do_EndOfLeftBracket1(char* strSource);                       // '['
static void Do_EndOfLeftBracket2(char* strSource);                       // '{'
static void Do_EndOfRightBracket1(char* strSource);                      // ']'
static void Do_EndOfRightBracket2(char* strSource);                      // '}' 

static void Do_EndOfColon(char* strSource);                              // ':'
static void Do_EndOfSemicolon(char* strSource);                          // ';'
static void Do_EndOfComma(char* strSource);                              // ','

static void Do_EndOfOR(char* strSource);                                 // '|'
static void Do_EndOfAND(char* strSource);                                // '&'
static void Do_EndOfNOR(char* strSource);                                // '^'

static void Do_EndOfEqual(char* strSource);                              // '=','=='
static void Do_EndOfMore(char* strSource);                               // '>','>='
static void Do_EndOfLess(char* strSource);                               // '<','<='
static void Do_EndOfEnd(char* strSource);                                // '\0'作为程序的结束

/**********************************************************************/ // 分界线结束

void init()
{
    gnColumn      = 1;
    gnRow         = 0;
    gnLocate      = 0;
    gnLocateStart = 0;
}

/******************************************************************
 * 函数原型： int IsDigit(char chInpot);                          *
 * 函数功能： 该函数用于判断当前的字符是不是数字                  *
 * 函数返回： 1代表是，0代表不是                                  *
******************************************************************/
static int IsDigit(char chInput)
{
    if(chInput >= '0' && chInput <= '9') return 1;
    else return 0;
}

/*****************************************************************
 * 函数原型： int IsChar(char chInput);                          *
 * 函数功能： 该函数用于判断当前字符是不是字母                   *
 * 函数返回： 1表示是，0表示不是                                 *
*****************************************************************/
static int IsChar(char chInput)
{
    if(chInput >= 'a' && chInput <= 'z') return 1;
    else return 0;
}

/****************************************************************
 * 函数原型：void Do_Start(char* strSource);                    *
 * 函数功能：读取单词，并给予单词识别码的主要函数               *
****************************************************************/
static void Do_Start(char *strSource)
{
    gnLocateStart = gnLocate;  
    
    switch(strSource[gnLocate]){
    case '+': Do_EndOfAdd(strSource);          break;            // solve '+' or '+='
    case '-': Do_EndOfSub(strSource);          break;            // solve '-' or '-='
    case '*': Do_EndOfMul(strSource);          break;            // solve '*' or '*='
    case '/': Do_EndOfDiv(strSource);          break;            // solve '/' or '/='
    case '(': Do_EndOfLParen(strSource);       break;            // solve '('
    case ')': Do_EndOfRParen(strSource);       break;            // solve ')'
    case '{': Do_EndOfLeftBracket2(strSource); break;            // solve '{'
    case '}': Do_EndOfRightBracket2(strSource);break;            // solve '}'
    case '[': Do_EndOfLeftBracket1(strSource); break;            // solve '['
    case ']': Do_EndOfRightBracket2(strSource);break;            // solve ']'
    case ',': Do_EndOfComma(strSource);        break;            // solve ','
    case ';': Do_EndOfSemicolon(strSource);    break;            // solve ';'
    case ':': Do_EndOfColon(strSource);        break;            // solve ':'
    case '=': Do_EndOfEqual(strSource);        break;            // solve '=' or '=='
    case '>': Do_EndOfMore(strSource);         break;            // solve '>' or '>='
    case '<': Do_EndOfLess(strSource);         break;            // solve '<' or '<='
    case '#': Do_EndOfEnd(strSource);          break;            // solve '#'
    case '\0':Do_EndOfEnd(strSource);          break;            // solve '\0'
    case '|': Do_EndOfOR(strSource);           break;            // solve '|'
    case '&': Do_EndOfAND(strSource);          break;            // solve '&'
    case '^': Do_EndOfNOR(strSource);          break;            // solve '^'
    default:
        /*
         * First the judge will goto a function named 'Do_Tag' if the current character is a char 
         * Secondly the judge will goto a function named 'Do_EndOfDigit' if the current character is a digit
         * Other the judge will goto a function named 'Do_start' after doing some process
        */
        if(IsChar(strSource[gnLocate]))
        {
            Do_Tag(strSource);
        }
        else if(IsDigit(strSource[gnLocate]))
        {
            uWord.value.num = strSource[gnLocate] - '0';
            Do_EndOfDigit(strSource);
        }
        else
        {
            if(strSource[gnLocate] != ' '
               && strSource[gnLocate] != '\t'
               && strSource[gnLocate] != '\n'
               && strSource[gnLocate] != '\r')
                printf("%c --> Error!\n",strSource[gnLocate]);
            
            if(strSource[gnLocate] == '\n' || strSource[gnLocate] == '\r'){
                gnColumn++;
                gnRow = 0;
            }
            else if(strSource[gnLocate] == '\t') gnColumn += _TAB_LENGTH;
            else gnRow++;

            gnLocate++;
            Do_Start(strSource);
        }
    } 
}

/****************************************************************
 * 函数原型：void Do_Tag(char* strSource);                      *
 * 函数功能：获取一个关键字或者标识符                           *
****************************************************************/
static void Do_Tag(char* strSource)
{
    gnLocate++;
    gnRow++;
    
    if(IsChar(strSource[gnLocate]) || IsDigit(strSource[gnLocate])){
        Do_Tag(strSource);
    }
    else Do_EndOfTag(strSource);

    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfTag(char* strSource);                *
 * 函数功能：对所获得的单词分类                                *
 *           10以下表示该单词为关键字，10表示为标识符          *
***************************************************************/
static void Do_EndOfTag(char* strSource)
{
    int nLoop;
    
    uWord.syn = _SYN_ID;
    
    strncpy(uWord.value.str,strSource + gnLocateStart,gnLocate - gnLocateStart);
    uWord.value.str[gnLocate - gnLocateStart] = '\0';
    
    nLoop = 0;
    while(strcmp(KEY_WORDS[nLoop],_KEY_WORD_END)){
        if(!strcmp(KEY_WORDS[nLoop],uWord.value.str)){
            uWord.syn = nLoop + 1;
            break;
        }
        nLoop = nLoop + 1;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_Digit(char* strSource);                   *
 * 函数功能：获取数字                                          *
***************************************************************/
static void Do_EndOfDigit(char* strSource)
{
    gnLocate++;
    gnRow++;
    if(IsDigit(strSource[gnLocate])){
        uWord.value.num = uWord.value.num * 10 + strSource[gnLocate] - '0';
        Do_EndOfDigit(strSource);
    }
    else
        uWord.syn = _SYN_NUM;
    
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfAdd(char* strSource);                *
 * 函数功能：读取+或者+=符号                                   *
***************************************************************/
static void Do_EndOfAdd(char* strSource)
{
    uWord.syn = _SYN_ADD; 
    uWord.value.str[0] = '+';
    uWord.value.str[1] = 0;    

    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_ADD_ASSIGN;
        gnLocate++;
        gnRow++;
    }
    
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfSub(char* strSource);                *
 * 函数功能：读取-或者-=符号                                   *
***************************************************************/
static void Do_EndOfSub(char* strSource)
{
    uWord.syn = _SYN_SUB;
    uWord.value.str[0] = '-';    
    uWord.value.str[1] = 0;

    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_SUB_ASSIGN;

        gnLocate++;
        gnRow++;
    }
    
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfMul(char* strSource);                *
 * 函数功能：读取*或者*=符号                                   *
***************************************************************/
static void Do_EndOfMul(char* strSource)
{
    uWord.syn = _SYN_MUL;
    uWord.value.str[0] = '*';
    uWord.value.str[1] = 0;
    
    gnLocate++;
    gnRow++;
    
    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_MUL_ASSIGN;

        gnLocate++;
        gnRow++;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfDiv(char* strSource);                *
 * 函数功能：读取/或者/=符号                                   *
***************************************************************/
static void Do_EndOfDiv(char* strSource)
{
    uWord.syn = _SYN_DIV;
    uWord.value.str[0] = '/';
    uWord.value.str[1] = 0;
    
    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_DIV_ASSIGN;

        gnLocate++;
        gnRow++;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfLParen(char* strSource);             *
 * 函数功能：读取(符号                                         *
***************************************************************/
static void Do_EndOfLParen(char* strSource)
{
    uWord.value.str[0] = '(';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_LPAREN;

    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfRParen(char* strSource);             *
 * 函数功能：读取)符号                                         *
***************************************************************/
static void Do_EndOfRParen(char* strSource)
{
    uWord.value.str[0] = ')';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_RPAREN;

    gnLocate++;
    gnRow++;
    return; 
}

/***************************************************************
 * 函数原型：void Do_EndOfLeftBracket1(char* strSource);       *
 * 函数功能：读取[符号                                         *
***************************************************************/
static void Do_EndOfLeftBracket1(char* strSource)
{
    uWord.value.str[0] = '[';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_LEFTBRACKET1;

    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfRightBracket1(char* strSource);      *
 * 函数功能：读取]符号                                         *
***************************************************************/
static void Do_EndOfRightBracket1(char* strSource)
{
    uWord.value.str[0] = ']';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_RIGHTBRACKET1;
    
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfLeftBracket2(char* strSource);       *
 * 函数功能：读取{符号                                         *
***************************************************************/
static void Do_EndOfLeftBracket2(char* strSource)
{
    uWord.value.str[0] = '{';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_LEFTBRACKET2;
    
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfRightBracket2(char* strSource);      *
 * 函数功能：读取}符号                                         *
***************************************************************/
static void Do_EndOfRightBracket2(char* strSource)
{
    uWord.value.str[0] = '}';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_RIGHTBRACKET2;
    
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfColon(char* strSource);              *
 * 函数功能：读取:符号                                         *
***************************************************************/
static void Do_EndOfColon(char* strSource)
{
    uWord.value.str[0] = ':';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_COLON;

    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfComma(char* strSource);              *
 * 函数功能：读取,符号                                         *
***************************************************************/
static void Do_EndOfComma(char* strSource)
{
    uWord.value.str[0] = ',';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_COMMA;
    
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfSemicolon(char* strSource);          *
 * 函数功能：读取;符号                                         *
***************************************************************/
static void Do_EndOfSemicolon(char* strSource)
{
    uWord.value.str[0] = ';';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_SEMICOLON;
   
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfOR(char* strSource);                 *
 * 函数功能：读取|或者||符号                                   *
***************************************************************/
static void Do_EndOfOR(char* strSource)
{
    uWord.syn = _SYN_OR;
    uWord.value.str[0] = '|';
    uWord.value.str[1] = 0;

    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '|')
    {
        uWord.value.str[1] = '|';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_OP_OR;

        gnLocate++;
        gnRow++;
    }

    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfAND(char* strSource);                *
 * 函数功能：读取&或者&&符号                                   *
***************************************************************/
static void Do_EndOfAND(char* strSource)
{
    uWord.syn = _SYN_AND;
    uWord.value.str[0] = '&';
    uWord.value.str[1] = 0;

    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '&')
    {
        uWord.value.str[1] = '&';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_OP_AND;

        gnLocate++;
        gnRow++;
    }

    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfNOR(char* strSource);                *
 * 函数功能：读取^符号                                   *
***************************************************************/
static void Do_EndOfNOR(char* strSource)
{
    uWord.syn = _SYN_NOR;
    uWord.value.str[0] = '^';
    uWord.value.str[1] = 0;

    gnLocate++;
    gnRow++;

/*
    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_SUB_ASSIGN;

        gnLocate++;
        gnRow++;
    }
*/
    return;
}



/***************************************************************
 * 函数原型：void Do_EndOfEqual(char* strSource);              *
 * 函数功能：读取=或者==符号                                   *
***************************************************************/
static void Do_EndOfEqual(char* strSource)
{
    uWord.value.str[0] = '=';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_ASSIGN;
    
    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_EQ;
       
        gnLocate++;
        gnRow++;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfMore(char* strSource);               *
 * 函数功能：读取>或者>=符号                                   *
***************************************************************/
static void Do_EndOfMore(char* strSource)
{
    uWord.value.str[0] = '>';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_LG;
    
    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_GE;
   
        gnLocate++; 
        gnRow++;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfLess(char* strSource);               *
 * 函数功能：读取<或者<=符号                                   *
***************************************************************/
static void Do_EndOfLess(char* strSource)
{
    uWord.value.str[0] = '<';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_LT;

    gnLocate++;
    gnRow++;

    if(strSource[gnLocate] == '=')
    {
        uWord.value.str[1] = '=';
        uWord.value.str[2] = 0;
        uWord.syn = _SYN_LE;

        gnLocate++;
        gnRow++;
    }
    return;
}

/***************************************************************
 * 函数原型：void Do_EndOfEnd(char* strSource);                *
 * 函数功能：读取#符号                                         *
***************************************************************/
static void Do_EndOfEnd(char* strSource)
{
    uWord.value.str[0] = '#';
    uWord.value.str[1] = 0;
    uWord.syn = _SYN_END;
   
    gnLocate++;
    gnRow++;
    return;
}

/***************************************************************
 * 函数原型：void Print_WORD(const struct WORD word);          *
 * 函数功能：输出指定单词，可供外部文件使用                    *
***************************************************************/
void Print_WORD(const struct WORD word)
{
    if(word.syn != 20) printf("%s -> %d\n",word.value.str,word.syn);
    else printf("%d -> %d\n",word.value.num,word.syn);
}

/***************************************************************
 * 函数原型：void scan(void);                                  *
 * 函数功能：读取单词的主要函数，可供外部文件使用              *
***************************************************************/
void scan(void)
{
    Do_Start(strSource);
}
