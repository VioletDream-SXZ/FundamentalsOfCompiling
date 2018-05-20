#include "globals.h"
#include "scan.h"
#include "semanteme.h"

//****************************引用外部定义************************************
struct QUAD pQUAD[1000];                          /*    存放四元组的数组    */
extern struct WORD uWord;                         /*    当前位置的单词      */
extern int gnColumn,gnRow;                        /*    行列号              */
extern char* strSource;                           /*    源程序              */
//****************************************************************************

//*******************华丽的分界线，函数声明区域*******************************

static void Remove(void);
static char* Factor(void);
static void LocateError(int nColumn,int nRow);
static void error(char* strError);
void Match(int syn,char* strError);
static void gen(char* op,char* argv1,char* argv2,char* result);
void PrintQuaternion(void);
static char* Newtemp(void);
static void bp(int p,int t);
static char* Term(void);
static void Statement(int* nChain);
static void Statement_Sequence(int* nChain);
static void Statement_Block(int* nChain);
static void Parse(void);
static char* Expression1(void);
void lrparse(void);

//****************************分界线结束**************************************

//****************************该文件参数**************************************
static int ERROR = 0;                             /*      当前行错误       */
static int ALLERROR = 0;                          /*      总错误个数       */
int QUADnum;                                      /*      表达式总个数     */
int nSuffix;                                      /*      临时变量后缀     */
int nfc;          
int ntc;
FILE* fw;                                         /*   打印结果的文件指针  */
const char* strResultFileName = "result.txt";     /*   打印结果的文件名称  */
const char* strTableFileName  = "table.txt";      /*   打印表格的文件名称  */
//****************************************************************************

/***************************************************************
 * 函数原型：void Remove()                                     *
 * 函数功能：当遇到首单词不是"if","while"或者是变量标识符的时候*
 *           需要将整一行去掉而做的处理                        *
***************************************************************/
static void Remove(void)
{
    while(1){
   	scan();
        if(uWord.syn == _SYN_SEMICOLON)
        {
            scan();
            return;
        }
    	if(uWord.syn == _SYN_RIGHTBRACKET2)
     	    return;
        if(uWord.syn == _SYN_END) 
            return;
    }
}

/***************************************************************
 * 函数原型：void LocateError(int nColumn,int nRow);           *
 * 函数功能：输出错误所在的行列号                              *
***************************************************************/
static void LocateError(int nColumn,int nRow)
{
    fprintf(fw,"Col: %d\tRow: %d --> ",nColumn,nRow);
}

/***************************************************************
 * 函数原型：void error(char* strError)                        *
 * 函数功能：输出错误提示                                      *
***************************************************************/
static void error(char* strError)
{
    LocateError(gnColumn,gnRow);
    fprintf(fw,"%s\n",strError);
}

/***************************************************************
 * 函数原型：void Match(int syn,char* strError);               *
 * 函数功能：判断是否满足指定输入，不满足输出错误提示          *
***************************************************************/
void Match(int syn,char* strError)
{
    if(syn == uWord.syn) scan();
    else 
    {
        error(strError);
        ALLERROR++;
    }
    return;
}

/**********************************************************************
 * 函数原型：void gen(char* op,char* argv1,char* argv2,char* result); *
 * 函数功能：根据需求生成四元式                                       *
**********************************************************************/
static void gen(char* op,char* argv1,char* argv2,char* result)
{
    strcpy(pQUAD[QUADnum].op,op);
    strcpy(pQUAD[QUADnum].argv1,argv1);
    strcpy(pQUAD[QUADnum].argv2,argv2);
    strcpy(pQUAD[QUADnum].result,result);
    QUADnum++;
    return;
}

/***************************************************************
 * 函数原型：void PrintQuaternion(void)                        *
 * 函数功能：输出所有的四元式，用于测试或实验要求              *
***************************************************************/
void PrintQuaternion(void)
{
    /**
     * 对于该输出进行说明：
     *     本身操作符有 +,-,*,/ = >,<,>=,<= j
     *     对于复制运算符，需要将格式转化成: result = argv1
     *     对于条件运算符，需要将格式转化成: if argv1 op argv2 jump result
     *     对于跳转运算符，需要将格式转化成：jump result
     *     对于其他运算符，需要将格式转化成: result = argv1 op argv2
    **/
    int nLoop;
    for(nLoop = 1;nLoop < QUADnum;nLoop++)
    {
        if(!strcmp(pQUAD[nLoop].op,"="))
            fprintf(fw,"%d: %s %s %s\n",nLoop,pQUAD[nLoop].result,pQUAD[nLoop].op,pQUAD[nLoop].argv1);
        else if(!strcmp(pQUAD[nLoop].op,"j"))
            fprintf(fw,"%d: jump %s\n",nLoop,pQUAD[nLoop].result);
        else if(!strcmp(pQUAD[nLoop].op,">") || !strcmp(pQUAD[nLoop].op,">=") ||
                !strcmp(pQUAD[nLoop].op,"<") || !strcmp(pQUAD[nLoop].op,"<=") ||
                !strcmp(pQUAD[nLoop].op,"==") || !strcmp(pQUAD[nLoop].op,"!="))
            fprintf(fw,"%d: if %s %s %s jump %s\n",nLoop,pQUAD[nLoop].argv1,pQUAD[nLoop].op,pQUAD[nLoop].argv2,pQUAD[nLoop].result);
        else 
            fprintf(fw,"%d: %s = %s %s %s\n",nLoop,pQUAD[nLoop].result,pQUAD[nLoop].argv1,pQUAD[nLoop].op,pQUAD[nLoop].argv2);
    }
    // printf("%d: %s\t=\t%s\t%s\t%s",nLoop,pQUAD[nLoop].result,pQUAD[nLoop].argv1,pQUAD[nLoop].op,pQUAD[nLoop].argv2);
}

/***************************************************************
 * 函数原型：char* Newtemp(void)                               *
 * 函数功能：生成一个临时变量名，如T1,T2等                     *
 * 函数返回：返回一个函数内部静态数组，防止内存泄露            *
***************************************************************/
static char* Newtemp(void)
{
    static char strTempID[_MAX_LENGTH_];
    sprintf(strTempID,"T%d",nSuffix++);
    return strTempID;
}

/***************************************************************
 * 函数原型：int merh(int p1,int p2)                           *
 * 函数功能：将p1和p2端合并                                    *
 * 函数返回：如果存在p2，返回p2断点，否则返回p1                *
***************************************************************/
static int merg(int p1,int p2)
{
    int p,nResult;
    
    if(p2 == 0) nResult = p1;
    else
    {
        nResult = p = p2;
        while(atoi(pQUAD[p].result))
        {
            p = atoi(pQUAD[p].result);
            sprintf(pQUAD[p].result,"%d",p1);
        }
    }
    return nResult;
}

/***************************************************************
 * 函数原型：void bp(int p,int t)                              *
 * 函数功能：将跳转步数回调到p的命令中                         *
***************************************************************/
static void bp(int p,int t)
{
    int w,q = p;
    while(q){
        w = atoi(pQUAD[q].result);
        sprintf(pQUAD[q].result,"%d",t);
        q = w;
    }
}


static char* Expression(void)
{
    char opp[_MAX_LENGTH_],eplace1[_MAX_LENGTH_],eplace2[_MAX_LENGTH_];
    static char eplace[_MAX_LENGTH_];
    char* temp = Expression1();
    strcpy(eplace,temp);
    strcpy(eplace1,eplace);
    while(uWord.syn == _SYN_OR || uWord.syn == _SYN_AND)
    {
        sprintf(opp,"%s",uWord.value.str);
        scan();
        temp = Expression1();
        strcpy(eplace2,temp);
        temp = Newtemp();
        strcpy(eplace,temp);
        gen(opp,eplace1,eplace2,eplace);
        strcpy(eplace1,eplace);
    }
    return eplace;
}

/***************************************************************
 * 函数原型：char* Expression(void)                            *
 * 函数功能：处理程序的'+'和'-'，                              *
 *           按照优先级，越大递归层数越深                      *
 * 函数返回：一个新的表达式，可以为数字，可以为标识符          *
***************************************************************/
static char* Expression1(void)
{
    char opp[_MAX_LENGTH_],eplace1[_MAX_LENGTH_],eplace2[_MAX_LENGTH_];
    static char eplace[_MAX_LENGTH_];                                    /* 设置为static是为了防止之后回收不了内存导致内存泄露 */
    char* temp = Term();
    strcpy(eplace,temp);
    strcpy(eplace1,eplace);
    while(uWord.syn == _SYN_ADD || uWord.syn == _SYN_SUB)                /* + - */
    {
        sprintf(opp,"%s",uWord.value.str);
        scan();                                                          /* 记录：在此读取写一个单词 */
        temp = Term();
        strcpy(eplace2,temp);
        temp = Newtemp();
        strcpy(eplace,temp);
        gen(opp,eplace1,eplace2,eplace);
        strcpy(eplace1,eplace);
    }
    return eplace;
}

/***************************************************************
 * 函数原型：char* Term(void)                                  *
 * 函数功能：处理程序的'*'和'/'                                *
 * 函数返回：一个新的表达式，可以为数字，可以为标识符          *
***************************************************************/
static char* Term(void)
{
    char opp[10],eplace1[_MAX_LENGTH_],eplace2[_MAX_LENGTH_];
    static char eplace[_MAX_LENGTH_];
    char* temp ;
    temp = Factor();
    strcpy(eplace,temp);
    strcpy(eplace1,eplace);
    while(uWord.syn == _SYN_MUL || uWord.syn == _SYN_DIV)               /* * / */
    {
        sprintf(opp,"%s",uWord.value.str);
        scan();
        temp = Factor();
        strcpy(eplace2,temp);
        temp = Newtemp();
        strcpy(eplace,temp);
        gen(opp,eplace1,eplace2,eplace);
        strcpy(eplace1,eplace);
    }
    return eplace;
}

/***************************************************************
 * 函数原型：char* Factor(void)                                *
 * 函数功能：处理程序的标识符，数字和括号'(',')'               *
 * 函数返回：一个新的表达式，可以为数字，可以为标识符          *
***************************************************************/
static char* Factor(void)
{
    static char eplace[_MAX_LENGTH_];
    if(uWord.syn == _SYN_ID || uWord.syn == _SYN_NUM)                   // 判断是否为标识符或者数字
    {
        if(uWord.syn == _SYN_ID) sprintf(eplace,"%s",uWord.value.str);  // 如果是标识符，给予它标识符的名称
        else sprintf(eplace,"%d",uWord.value.num);                      // 如果是数字，给予它数值的大小
        scan();
    }
    else if(_SYN_LPAREN == uWord.syn)                                   // 括号分支，原本不加判断的话会死递归
    {   
        char* temp;                                                
        Match(_SYN_LPAREN,"error: Maybe missing number or paren!"); 
        temp = Expression();
        strcpy(eplace,Expression());
        Match(_SYN_RPAREN,"error: Missing right paren!");
    }
    else                                                                // 其他不合法的字符，恢复操作，当做实际上是存在的继续运行
    {
        ALLERROR++;
        error("error：There are illegal symbols!");
        sprintf(eplace,"error");
    }
    
    return eplace;
}

/***************************************************************
 * 函数原型：void COndition(int* etc,int* efc)                 *
 * 函数功能：                                                  *
 *     该函数用于读取if语句里面的条件判断                      *
 *     目前支持的条件判断详细请看globals.h里面的定义           *
***************************************************************/
static void Condition(int* etc,int* efc)
{
    char opp[10],eplace1[_MAX_LENGTH_],eplace2[_MAX_LENGTH_],result[_MAX_LENGTH_];
    char strTemp[4];
    char* temp;  
   
    temp = Expression();
    strcpy(eplace1,temp);
    if((uWord.syn <= _SYN_NE && uWord.syn >= _SYN_LG) || 
       (uWord.syn <= _SYN_OP_AND && uWord.syn >= _SYN_OP_OR)){               // 判断当前操作数是否为关系运算符
        sprintf(opp,"%s",uWord.value.str);
        scan();
        temp = Expression();
        strcpy(eplace2,temp);
        *etc = QUADnum;
        *efc = QUADnum + 1;
        sprintf(strTemp,"%s",opp);
        gen(strTemp,eplace1,eplace2,"0");                                   // 生成条件判断，可作为下面的判断前提
        gen("j","","","0");                                                 // 生成跳转指令，指示条件为否是跳转的位置
    }
    else
    {
        error("error: Missing relationship operation!");
        ALLERROR++;
    }
}

/***************************************************************
 * 函数原型：void Statement(int* nChain)                       *
 * 函数参数：                                                  *
 *           int* nChain                                       *
 * 函数功能：判断程序类型(赋值，分支，循环)，                  *
 *           并从指定类型进行扫描                              *
***************************************************************/
static void Statement(int* nChain)
{
    char strTemp[_MAX_LENGTH_],eplace[_MAX_LENGTH_];
    char* temp;
    int nChainTemp,nWQUAD;
    int syn;
    int nf,nt;                                                   // 该变量不可以为全局变量，防止内部if语句覆盖外部if语句的状态
    switch(uWord.syn){
    case _SYN_ID:                                                // 如果为标识符，即为赋值语句，该语言分析器只能分析整数
        strcpy(strTemp,uWord.value.str);
        scan();
        if(_SYN_ASSIGN == uWord.syn)                                             // 判断'='
        {
            scan();
            temp = Expression();
            strcpy(eplace,temp);
            // Match(_SYN_SEMICOLON,"error: Missing ';'");
            gen("=",eplace,"",strTemp);
            *nChain = 0;
        }
        else if(uWord.syn <= _SYN_DIV_ASSIGN && uWord.syn >= _SYN_ADD_ASSIGN)    // 判断'+=','-=','*=','/='
        {
            syn = uWord.syn;
            scan();
            temp = Expression();
            strcpy(eplace,temp);
            // Match(_SYN_SEMICOLON,"error: Missing ';'");
            temp = Newtemp();
            // 生成表达式 temp = strTemp syn eplace (syn表示符号)
            if(syn == _SYN_ADD_ASSIGN) gen("+",strTemp,eplace,temp);
            else if(syn == _SYN_SUB_ASSIGN) gen("-",strTemp,eplace,temp);
            else if(syn == _SYN_MUL_ASSIGN) gen("*",strTemp,eplace,temp);
            else gen("/",strTemp,eplace,temp);
            // 生成表达式strTemp = temp
            gen("=",temp,"",strTemp);
            *nChain = 0;
        }
        else
        {
            error("error: Missing '=','+=','-=','*=' or '/='");
            ALLERROR++;
        }
       
        // 处理程序的','
        if(uWord.syn == _SYN_COMMA)
        {
            scan();
            Statement(nChain);
        }
        else
            Match(_SYN_SEMICOLON,"error: Missing ';'");

        break;
    case _SYN_IF:
        Match(_SYN_IF,"error: Missing 'if'");
        Match(_SYN_LPAREN,"error: Missing '('");
        Condition(&nt,&nf);                                       // nt当前指令行数 nf下一条指令行数
        bp(nt,QUADnum);                                           // 将nt之后的第二条指令回填到nt中
        Match(_SYN_RPAREN,"error: Missing ')'");
	Match(_SYN_LEFTBRACKET2,"error: Missing '{'");
        Statement_Block(&nChainTemp);
        Match(_SYN_RIGHTBRACKET2,"error: Missing '}'");
        *nChain = merg(nChainTemp,nf);                           //  将p1和p2合并
        break;
    case _SYN_WHILE:
        Match(_SYN_WHILE,"error: Missing 'while'");
        nWQUAD = QUADnum;                                        // nWQUAD保存当前指令行数，供以后填入
        Match(_SYN_LPAREN,"error: Missing '('");
        Condition(&nt,&nf);                                      // nt当前指令行数  nf下一条指令行数
        bp(nt,QUADnum);                                          // 将之后的命令填入进入跳转的循环中
        Match(_SYN_RPAREN,"error: Missing ')'");
        Match(_SYN_LEFTBRACKET2,"error: Missing '{'");
        Statement_Block(&nChainTemp);
        Match(_SYN_RIGHTBRACKET2,"error: Missing '}'");
        bp(nChainTemp,nWQUAD);                                   // 将判断为非跳转行数填入其中，此外还能回溯到之前的判断点
        sprintf(strTemp,"%d",nWQUAD);
        gen("j","","",strTemp);                                  // 循环返回跳转
        *nChain = nf;                                            // 返回下一条指令地址，供以后跳转输入
        break;
    default:                                                     // 不过任何语句的入口即为错误，错误处理直接去掉整个句子
        error("error: The sentence is error!Maybe missing 'if','identifier' or 'while'!");
        ERROR = 1;
        Remove();
    }
}

static void Statement_Sequence(int* nChain)
{
    Statement(nChain);
    while(uWord.syn == _SYN_ID || uWord.syn == _SYN_IF || uWord.syn == _SYN_WHILE){
        if(ERROR) return;
        bp(*nChain,QUADnum);                                    // 指令回填，将当前指令行数填入循环或者分支语句中
        Statement(nChain);
    }   
    if(!ERROR) bp(*nChain,QUADnum);                             // 指令回填，将当前指令行数填入循环或者分支语句中
    return;
}

static void Statement_Block(int* nChain)
{
    Statement_Sequence(nChain);
}

/***************************************************************
 * 程序处理入口四个函数Parse,Parse1,Parse2,Parse3              *
 *-------------------------------------------------------------*
 * 函数原型：void Parse(void)                                  *
 * 函数功能：提供对外统一接口，直接实现程序的扫描操作          *
 *-------------------------------------------------------------*
 * 函数原型：void Parse1(void)                                 *
 * 函数功能：处理程序的前半部分，程序需包含以下前缀            *
 *           int main(){                                       *
 *-------------------------------------------------------------*
 * 函数原型：void Parse2(int* nChain)                          *
 * 函数功能：实现源程序的语句分析和错误恢复处理                *
 *-------------------------------------------------------------*
 * 函数原型：void Parse3(void)                                 *
 * 函数功能：处理程序的后缀，程序在结束的时候需要包含一些内容  *
 *           }(#)                                              *
***************************************************************/
static void Parse1(void)
{
    scan();
    Match(_SYN_INT,"error: missing 'int' before 'main'");
    Match(_SYN_MAIN,"error: undefined reference to `main'");
    Match(_SYN_LPAREN,"error: missing '('");
    Match(_SYN_RPAREN,"error: missing ')'");
    Match(_SYN_LEFTBRACKET2,"error: Missing '{'");
}

static void Parse2(int* nChain)
{
    while(uWord.syn != _SYN_END && uWord.syn != _SYN_RIGHTBRACKET2)
    {
        Statement_Block(nChain);
        
        // 对于递归错误处理，总错误有ALLERROR决定
        if(ERROR)
        {
            ERROR = 0;
            ALLERROR++;
        }
    }
    return;
}

static void Parse3(void)
{
    Match(_SYN_RIGHTBRACKET2,"error: Missing '}'");                                    
    if(uWord.syn != _SYN_END) fprintf(fw,"error: The source file is abnormal end!\n");
    else if(!ALLERROR)
        fprintf(fw,"success!\n");  
}

static void Parse(void)
{
    int nChain;
    Parse1();
    Parse2(&nChain);
    Parse3();
}

/***************************************************************
 * 函数原型:void lrparse(void)                                 *
 * 函数功能：外部文件的函数借口                                *
***************************************************************/
void lrparse(void)
{
    memset(pQUAD,0,sizeof(pQUAD));
    nSuffix = 0;
    nfc = ntc = QUADnum = 1;
    fw = fopen(strResultFileName,"w");                          // 打开结果保存的文件
    Parse();                                                    // 开始扫描源程序
    fclose(fw);                                                 // 关闭结果保存的文件
    
    if(!ALLERROR)                                               // 如果不存在错误，将程序所有的四元组放入table.txt文件里面
    {   
        fw = fopen(strTableFileName,"w");
        PrintQuaternion();        
        fclose(fw);
    }
    else                                                        // 如果存在错误，则将源文件内容清空
    {
        fw = fopen(strTableFileName,"w");
        fprintf(fw,"\n");
        fclose(fw);
    }
}
