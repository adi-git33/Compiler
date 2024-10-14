#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Language.tab.h"

// Start-Process ./parse -RedirectStandardInput exampleProg1.txt -NoNewWindow -Wait
// Start-Process ./parse -RedirectStandardInput exampleProg2.txt -NoNewWindow -Wait

extern int yylex();
extern int yylineno;
extern char* yytext;
extern int yyparse(void);

void yyerror (char *s) {fprintf (stderr, "%s\n", s); exit(1);} 

int main(void)
{
    yydebug = 0;
    fprintf(stdout, "#include <stdio.h>\n");
    fprintf(stdout, "#include <stdlib.h>\n");
    fprintf(stdout, "#include <string.h>\n\n");
    fprintf(stdout, "int main()\n");
    fprintf(stdout, "{\n");

    yyparse();

    fprintf(stdout, "}\n");
    return 0;
}
