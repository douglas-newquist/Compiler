%option noyywrap
%option noinput
%option nounput
    #include "main.h"
    #include "tokens.h"

%%
<<EOF>>         return EOF;
[ \t\r\f]       whitespace();
\n		        newline();

"++"            return INCREMENT;
"--"            return DECREMENT;
"+"             return ADD;
"-"             return SUB;
"*"             return MULT;
"/"             return DIV;
"="             return EQUAL;
"%"             return MODULO;
"&&"            return AND;
"!"             return NOT;
"||"            return OR;
"."             return DOT;

"=="            return EQUALS;
"!="            return NOT_EQUAL;
"<"             return LESS;
"<="            return LESS_EQUAL;
">"             return GREATER;
">="            return GREATER_EQUAL;

"~"             error(ILLEGAL, "Illegal operator");
"<<"            error(ILLEGAL, "Illegal operator");
">>"            error(ILLEGAL, "Illegal operator");
">>|"           error(ILLEGAL, "Illegal operator");
"&="            error(ILLEGAL, "Illegal operator");
"|="            error(ILLEGAL, "Illegal operator");
"^="            error(ILLEGAL, "Illegal operator");
">>="           error(ILLEGAL, "Illegal operator");
"<<="           error(ILLEGAL, "Illegal operator");
"&"             error(ILLEGAL, "Illegal operator");
"|"             error(ILLEGAL, "Illegal operator");
"^"             error(ILLEGAL, "Illegal operator");
"-+"            error(ILLEGAL, "Illegal operator");
"?"             error(ILLEGAL, "Illegal operator");
"?:"            error(ILLEGAL, "Illegal operator");
ref             error(ILLEGAL, "Illegal operator");

"{"             return OPEN_CURLY;
"}"             return CLOSE_CURLY;
"("             return OPEN_PAREN;
")"             return CLOSE_PAREN;
"["             return OPEN_SQUARE;
"]"             return CLOSE_SQUARE;
";"             return SEMICOLON;
":"             return COLON;
","             return COMMA;

"#"             error(ILLEGAL, "Illegal character");
"$"             error(ILLEGAL, "Illegal character");
"@"             error(ILLEGAL, "Illegal character");
"\\"            error(ILLEGAL, "Illegal character");
"`"             error(ILLEGAL, "Illegal character");

boolean         return BOOLEAN;
char            return CHAR;
float           return FLOAT;
int             return INT;
long            return LONG;

break           return BREAK;
case            return CASE;
class           return CLASS;
continue        return CONTINUE;
default         return DEFAULT;
double          return DOUBLE;
else            return ELSE;
for             return FOR;
if              return IF;
instanceof      return INSTANCEOF;
new             return NEW;
public          return PUBLIC;
return          return RETURN;
static          return STATIC;
switch          return SWITCH;
void            return VOID;
while           return WHILE;

abstract        error(ILLEGAL, "Illegal reserved word");
assert          error(ILLEGAL, "Illegal reserved word");
byte            error(ILLEGAL, "Illegal reserved word");
catch           error(ILLEGAL, "Illegal reserved word");
const           error(ILLEGAL, "Illegal reserved word");
do              error(ILLEGAL, "Illegal reserved word");
enum            error(ILLEGAL, "Illegal reserved word");
exports         error(ILLEGAL, "Illegal reserved word");
extends         error(ILLEGAL, "Illegal reserved word");
final           error(ILLEGAL, "Illegal reserved word");
finally         error(ILLEGAL, "Illegal reserved word");
goto            error(ILLEGAL, "Illegal reserved word");
implements      error(ILLEGAL, "Illegal reserved word");
import          error(ILLEGAL, "Illegal reserved word");
interface       error(ILLEGAL, "Illegal reserved word");
module          error(ILLEGAL, "Illegal reserved word");
native          error(ILLEGAL, "Illegal reserved word");
package         error(ILLEGAL, "Illegal reserved word");
private         error(ILLEGAL, "Illegal reserved word");
protected       error(ILLEGAL, "Illegal reserved word");
requires        error(ILLEGAL, "Illegal reserved word");
short           error(ILLEGAL, "Illegal reserved word");
strictfp        error(ILLEGAL, "Illegal reserved word");
super           error(ILLEGAL, "Illegal reserved word");
synchronized    error(ILLEGAL, "Illegal reserved word");
this            error(ILLEGAL, "Illegal reserved word");
throw           error(ILLEGAL, "Illegal reserved word");
throws          error(ILLEGAL, "Illegal reserved word");
transient       error(ILLEGAL, "Illegal reserved word");
try             error(ILLEGAL, "Illegal reserved word");
var             error(ILLEGAL, "Illegal reserved word");
volatile        error(ILLEGAL, "Illegal reserved word");

"//".*$                 comment();
"/*"([^*]|\*[^/])*"*/"  comment();
^#.*$                   comment();
"/*"([^*]|\*[^/])*      error(TOKEN_ERROR, "Unterminated multiline comment");

true            return LITERAL_BOOL;
false           return LITERAL_BOOL;

0|[1-9][0-9]{0,9}       return LITERAL_INT;
0[lL]?|[1-9][0-9]*[lL]  return LITERAL_LONG;
[1-9][0-9]{10,}         error(INVALID_LITERAL, "Integer too long");
0[0-9]+                 error(INVALID_LITERAL, "Octals not supported");
0x[0-9a-fA-F]+          error(INVALID_LITERAL, "Hexadecimals not supported");
0b[01]+                 error(INVALID_LITERAL, "Binary not supported");

[0-9]+\.?[0-9]*[fF]     return LITERAL_FLOAT;
[0-9]*\.[0-9]+[fF]      return LITERAL_FLOAT;
[0-9]+\.?[0-9]*[dD]     return LITERAL_DOUBLE;
[0-9]*\.[0-9]+[dD]      return LITERAL_DOUBLE;
[0-9]+\.\.+[0-9]*[fF]   error(INVALID_LITERAL, "Too many dots in float");
[0-9]+\.[0-9]*          error(INVALID_LITERAL, "Floats must end with f");
[0-9]*\.[0-9]+          error(INVALID_LITERAL, "Floats must end with f");

[0-9]+[eE][+-]?[0-9]+[fF] return LITERAL_FLOAT;
[0-9]+[eE][+-]?[0-9]+   error(INVALID_LITERAL, "Floats must end with f");
[0-9]+[eE][+-][+-]+     error(INVALID_LITERAL, "Too many signs");
[0-9]+[eE]              error(INVALID_LITERAL, "Exponent missing");

[0-9]+[0-9a-zA-Z]*      error(INVALID_LITERAL, "Unknown number format");

\"([^\\"]|\\.)*\"       return LITERAL_STRING;
\"([^\\"]|\\.)*         error(INVALID_LITERAL, "String was not closed");

'[^\\]'                 return LITERAL_CHAR;
'\\.'                   return LITERAL_CHAR;
'\\'                    error(INVALID_LITERAL, "Escape character must be escaped");
'[^']{0,2}              error(INVALID_LITERAL, "Character was not closed");
'.*'                    error(INVALID_LITERAL, "Invalid character");

[a-zA-Z_][a-zA-Z0-9_]*  return ID;

.		        error(UNKNOWN_TOKEN, "Unknown token");
