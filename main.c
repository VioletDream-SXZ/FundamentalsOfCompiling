#include "scan.h"
#include "globals.h"
#include "semanteme.h"
char strSource[_MAX_LENGTH_];
extern int gnLocate;
extern struct WORD uWord;

int main()
{
#ifdef DSCAN
    freopen("data.txt","r",stdin);
    init();
    memset(strSource,0,sizeof(strSource));
    scanf("%[^#]s",strSource);
    strSource[strlen(strSource)] = '#';
    while(strSource[gnLocate] != '#')
    {   
        scan();
        Print_WORD(uWord);
    }
#endif

#ifdef DLRPARSE
    freopen("data.txt","r",stdin);
    init();
    memset(strSource,0,sizeof(strSource));
    scanf("%[^#]s",strSource);
    strSource[strlen(strSource)] = '#';
    lrparse();
#endif
}
