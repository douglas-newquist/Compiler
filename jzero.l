%option noyywrap
%option noinput
%option nounput
    #include "main.h"
    #include "jzero.tab.h"

%%
[ \t\r\f]+      whitespace();
\n		        newline();

"++"            return token(INCREMENT);
"--"            return token(DECREMENT);
"+"             return token('+');
"-"             return token('-');
"*"             return token('*');
"/"             return token('/');
"="             return token('=');
"%"             return token('%');
"&&"            return token(AND);
"!"             return token('!');
"||"            return token(OR);
"."             return token('.');

"=="            return token(EQUALS);
"!="            return token(NOT_EQUAL);
"<"             return token('<');
"<="            return token(LESS_EQUAL);
">"             return token('<');
">="            return token(GREATER_EQUAL);

"~"             yyerror("Illegal operator");
"<<"            yyerror("Illegal operator");
">>"            yyerror("Illegal operator");
">>|"           yyerror("Illegal operator");
"&="            yyerror("Illegal operator");
"|="            yyerror("Illegal operator");
"^="            yyerror("Illegal operator");
">>="           yyerror("Illegal operator");
"<<="           yyerror("Illegal operator");
"&"             yyerror("Illegal operator");
"|"             yyerror("Illegal operator");
"^"             yyerror("Illegal operator");
"-+"            yyerror("Illegal operator");
"?"             yyerror("Illegal operator");
"?:"            yyerror("Illegal operator");
ref             yyerror("Illegal operator");

"{"             return token('{');
"}"             return token('}');
"("             return token('(');
")"             return token(')');
"["             return token('[');
"]"             return token(']');
";"             return token(';');
":"             return token(':');
","             return token(',');

"#"             yyerror("Illegal character");
"$"             yyerror("Illegal character");
"@"             yyerror("Illegal character");
"\\"            yyerror("Illegal character");
"`"             yyerror("Illegal character");

boolean         return token(BOOLEAN);
char            return token(CHAR);
double          return token(DOUBLE);
float           return token(DOUBLE);
int             return token(INT);
long            return token(INT);

break           return token(BREAK);
case            return token(CASE);
class           return token(CLASS);
continue        return token(CONTINUE);
default         return token(DEFAULT);
else            return token(ELSE);
for             return token(FOR);
if              return token(IF);
instanceof      return token(INSTANCEOF);
new             return token(NEW);
public          return token(PUBLIC);
return          return token(RETURN);
static          return token(STATIC);
switch          return token(SWITCH);
void            return token(VOID);
while           return token(WHILE);

abstract        yyerror("Illegal reserved word");
assert          yyerror("Illegal reserved word");
byte            yyerror("Illegal reserved word");
catch           yyerror("Illegal reserved word");
const           yyerror("Illegal reserved word");
do              yyerror("Illegal reserved word");
enum            yyerror("Illegal reserved word");
exports         yyerror("Illegal reserved word");
extends         yyerror("Illegal reserved word");
final           yyerror("Illegal reserved word");
finally         yyerror("Illegal reserved word");
goto            yyerror("Illegal reserved word");
implements      yyerror("Illegal reserved word");
import          yyerror("Illegal reserved word");
interface       yyerror("Illegal reserved word");
module          yyerror("Illegal reserved word");
native          yyerror("Illegal reserved word");
package         yyerror("Illegal reserved word");
private         yyerror("Illegal reserved word");
protected       yyerror("Illegal reserved word");
requires        yyerror("Illegal reserved word");
short           yyerror("Illegal reserved word");
strictfp        yyerror("Illegal reserved word");
super           yyerror("Illegal reserved word");
synchronized    yyerror("Illegal reserved word");
this            yyerror("Illegal reserved word");
throw           yyerror("Illegal reserved word");
throws          yyerror("Illegal reserved word");
transient       yyerror("Illegal reserved word");
try             yyerror("Illegal reserved word");
var             yyerror("Illegal reserved word");
volatile        yyerror("Illegal reserved word");

"//".*$                 comment();
"/*"([^*]|\*[^/])*"*/"  comment();
^#.*$                   comment();
"/*"([^*]|\*[^/])*      yyerror("Unterminated multiline comment");

null            return token(LITERAL_NULL);

true            return token(LITERAL_BOOL);
false           return token(LITERAL_BOOL);

0|[1-9][0-9]{0,9}       return token(LITERAL_INT);
0[lL]?|[1-9][0-9]*[lL]  return token(LITERAL_INT);
[1-9][0-9]{10,}         yyerror("Integer too long");
0[0-9]+                 yyerror("Octals not supported");
0x[0-9a-fA-F]+          yyerror("Hexadecimals not supported");
0b[01]+                 yyerror("Binary not supported");

[0-9]+\.?[0-9]*[fF]     return token(LITERAL_DOUBLE);
[0-9]*\.[0-9]+[fF]      return token(LITERAL_DOUBLE);
[0-9]+\.?[0-9]*[dD]     return token(LITERAL_DOUBLE);
[0-9]*\.[0-9]+[dD]      return token(LITERAL_DOUBLE);
[0-9]+\.\.+[0-9]*[fF]   yyerror("Too many dots in float");
[0-9]+\.[0-9]*          yyerror("Floats must end with f");
[0-9]*\.[0-9]+          yyerror("Floats must end with f");

[0-9]+[eE][+-]?[0-9]+[fF] return token(LITERAL_DOUBLE);
[0-9]+[eE][+-]?[0-9]+   yyerror("Floats must end with f");
[0-9]+[eE][+-][+-]+     yyerror("Too many signs");
[0-9]+[eE]              yyerror("Exponent missing");

[0-9]+[0-9a-zA-Z]*      yyerror("Unknown number format");

\"([^\\"]|\\.)*\"       return token(LITERAL_STRING);
\"([^\\"]|\\.)*         yyerror("String was not closed");

'[^\\]'                 return token(LITERAL_CHAR);
'\\.'                   return token(LITERAL_CHAR);
'\\'                    yyerror("Escape character must be escaped");
'[^']{0,2}              yyerror("Character was not closed");
'.*'                    yyerror("Invalid character");

[a-zA-Z_][a-zA-Z0-9_]*  return token(ID);

.		        yyerror("Unknown token");
