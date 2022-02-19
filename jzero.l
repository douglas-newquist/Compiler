%{
    #include "main.h"
    #include "jzero.tab.h"
    #include "errors.h"
%}

%option noyywrap
%option noinput
%option nounput
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

"~"             error(LEX_ERROR, "Illegal operator");
"<<"            error(LEX_ERROR, "Illegal operator");
">>"            error(LEX_ERROR, "Illegal operator");
">>|"           error(LEX_ERROR, "Illegal operator");
"&="            error(LEX_ERROR, "Illegal operator");
"|="            error(LEX_ERROR, "Illegal operator");
"^="            error(LEX_ERROR, "Illegal operator");
">>="           error(LEX_ERROR, "Illegal operator");
"<<="           error(LEX_ERROR, "Illegal operator");
"&"             error(LEX_ERROR, "Illegal operator");
"|"             error(LEX_ERROR, "Illegal operator");
"^"             error(LEX_ERROR, "Illegal operator");
"-+"            error(LEX_ERROR, "Illegal operator");
"?"             error(LEX_ERROR, "Illegal operator");
"?:"            error(LEX_ERROR, "Illegal operator");
ref             error(LEX_ERROR, "Illegal operator");

\{             return token('{');
\}             return token('}');
\(             return token('(');
\)             return token(')');
\[             return token('[');
\]             return token(']');
";"             return token(';');
":"             return token(':');
","             return token(',');

"#"             error(LEX_ERROR, "Illegal character");
"$"             error(LEX_ERROR, "Illegal character");
"@"             error(LEX_ERROR, "Illegal character");
"\\"            error(LEX_ERROR, "Illegal character");
"`"             error(LEX_ERROR, "Illegal character");

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

abstract        error(LEX_ERROR, "Illegal reserved word");
assert          error(LEX_ERROR, "Illegal reserved word");
byte            error(LEX_ERROR, "Illegal reserved word");
catch           error(LEX_ERROR, "Illegal reserved word");
const           error(LEX_ERROR, "Illegal reserved word");
do              error(LEX_ERROR, "Illegal reserved word");
enum            error(LEX_ERROR, "Illegal reserved word");
exports         error(LEX_ERROR, "Illegal reserved word");
extends         error(LEX_ERROR, "Illegal reserved word");
final           error(LEX_ERROR, "Illegal reserved word");
finally         error(LEX_ERROR, "Illegal reserved word");
goto            error(LEX_ERROR, "Illegal reserved word");
implements      error(LEX_ERROR, "Illegal reserved word");
import          error(LEX_ERROR, "Illegal reserved word");
interface       error(LEX_ERROR, "Illegal reserved word");
module          error(LEX_ERROR, "Illegal reserved word");
native          error(LEX_ERROR, "Illegal reserved word");
package         error(LEX_ERROR, "Illegal reserved word");
private         error(LEX_ERROR, "Illegal reserved word");
protected       error(LEX_ERROR, "Illegal reserved word");
requires        error(LEX_ERROR, "Illegal reserved word");
short           error(LEX_ERROR, "Illegal reserved word");
strictfp        error(LEX_ERROR, "Illegal reserved word");
super           error(LEX_ERROR, "Illegal reserved word");
synchronized    error(LEX_ERROR, "Illegal reserved word");
this            error(LEX_ERROR, "Illegal reserved word");
throw           error(LEX_ERROR, "Illegal reserved word");
throws          error(LEX_ERROR, "Illegal reserved word");
transient       error(LEX_ERROR, "Illegal reserved word");
try             error(LEX_ERROR, "Illegal reserved word");
var             error(LEX_ERROR, "Illegal reserved word");
volatile        error(LEX_ERROR, "Illegal reserved word");

"//".*$                 comment();
"/*"([^*]|\*[^/])*"*/"  comment();
^#.*$                   comment();
"/*"([^*]|\*[^/])*      error(LEX_ERROR, "Unterminated multiline comment");

null            return token(LITERAL_NULL);

true            return token(LITERAL_BOOL);
false           return token(LITERAL_BOOL);

0|-?[1-9][0-9]{0,9}         return token(LITERAL_INT);
0[lL]?|-?[1-9][0-9]*[lL]    return token(LITERAL_INT);
[1-9][0-9]{9}[0-9]+         error(LEX_ERROR, "Integer too large");
0[0-9]+                     error(LEX_ERROR, "Octals not supported");
0x[0-9a-fA-F]+              error(LEX_ERROR, "Hexadecimals not supported");
0b[01]+                     error(LEX_ERROR, "Binary not supported");

-?[0-9]+\.?[0-9]*[fF]   return token(LITERAL_DOUBLE);
-?[0-9]*\.[0-9]+[fF]    return token(LITERAL_DOUBLE);
-?[0-9]+\.?[0-9]*[dD]   return token(LITERAL_DOUBLE);
-?[0-9]*\.[0-9]+[dD]    return token(LITERAL_DOUBLE);
[0-9]+\.\.+[0-9]*[fF]   error(LEX_ERROR, "Too many dots in float");
[0-9]+\.[0-9]*          error(LEX_ERROR, "Floats must end with f");
[0-9]*\.[0-9]+          error(LEX_ERROR, "Floats must end with f");

-?[0-9]+[eE][+-]?[0-9]+[fF] return token(LITERAL_DOUBLE);
-?[0-9]+[eE][+-]?[0-9]+[dD] return token(LITERAL_DOUBLE);
[0-9]+[eE][+-]?[0-9]+       error(LEX_ERROR, "Floats must end with f");
[0-9]+[eE][+-][+-]+         error(LEX_ERROR, "Too many signs");
[0-9]+[eE]                  error(LEX_ERROR, "Exponent missing");

[0-9]+[0-9a-zA-Z]*      error(LEX_ERROR, "Unknown number format");

\"([^\\"]|\\.)*\"       return token(LITERAL_STRING);
\"([^\\"]|\\.)*         error(LEX_ERROR, "String was not closed");

'[^\\]'                 return token(LITERAL_CHAR);
'\\.'                   return token(LITERAL_CHAR);
'\\'                    error(LEX_ERROR, "Escape character must be escaped");
'..+'                   error(LEX_ERROR, "Character too long");
'.*'                    error(LEX_ERROR, "Invalid character");
'.{0,3}                 error(LEX_ERROR, "Character not closed");

[a-zA-Z_][a-zA-Z0-9_]*  return token(ID);

.       		        error(LEX_ERROR, "Unknown token");
