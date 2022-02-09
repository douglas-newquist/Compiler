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

"~"             yyerror(ILLEGAL, "Illegal operator");
"<<"            yyerror(ILLEGAL, "Illegal operator");
">>"            yyerror(ILLEGAL, "Illegal operator");
">>|"           yyerror(ILLEGAL, "Illegal operator");
"&="            yyerror(ILLEGAL, "Illegal operator");
"|="            yyerror(ILLEGAL, "Illegal operator");
"^="            yyerror(ILLEGAL, "Illegal operator");
">>="           yyerror(ILLEGAL, "Illegal operator");
"<<="           yyerror(ILLEGAL, "Illegal operator");
"&"             yyerror(ILLEGAL, "Illegal operator");
"|"             yyerror(ILLEGAL, "Illegal operator");
"^"             yyerror(ILLEGAL, "Illegal operator");
"-+"            yyerror(ILLEGAL, "Illegal operator");
"?"             yyerror(ILLEGAL, "Illegal operator");
"?:"            yyerror(ILLEGAL, "Illegal operator");
ref             yyerror(ILLEGAL, "Illegal operator");

"{"             return OPEN_CURLY;
"}"             return CLOSE_CURLY;
"("             return OPEN_PAREN;
")"             return CLOSE_PAREN;
"["             return OPEN_SQUARE;
"]"             return CLOSE_SQUARE;
";"             return SEMICOLON;
":"             return COLON;
","             return COMMA;

"#"             yyerror(ILLEGAL, "Illegal character");
"$"             yyerror(ILLEGAL, "Illegal character");
"@"             yyerror(ILLEGAL, "Illegal character");
"\\"            yyerror(ILLEGAL, "Illegal character");
"`"             yyerror(ILLEGAL, "Illegal character");

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

abstract        yyerror(ILLEGAL, "Illegal reserved word");
assert          yyerror(ILLEGAL, "Illegal reserved word");
byte            yyerror(ILLEGAL, "Illegal reserved word");
catch           yyerror(ILLEGAL, "Illegal reserved word");
const           yyerror(ILLEGAL, "Illegal reserved word");
do              yyerror(ILLEGAL, "Illegal reserved word");
enum            yyerror(ILLEGAL, "Illegal reserved word");
exports         yyerror(ILLEGAL, "Illegal reserved word");
extends         yyerror(ILLEGAL, "Illegal reserved word");
final           yyerror(ILLEGAL, "Illegal reserved word");
finally         yyerror(ILLEGAL, "Illegal reserved word");
goto            yyerror(ILLEGAL, "Illegal reserved word");
implements      yyerror(ILLEGAL, "Illegal reserved word");
import          yyerror(ILLEGAL, "Illegal reserved word");
interface       yyerror(ILLEGAL, "Illegal reserved word");
module          yyerror(ILLEGAL, "Illegal reserved word");
native          yyerror(ILLEGAL, "Illegal reserved word");
package         yyerror(ILLEGAL, "Illegal reserved word");
private         yyerror(ILLEGAL, "Illegal reserved word");
protected       yyerror(ILLEGAL, "Illegal reserved word");
requires        yyerror(ILLEGAL, "Illegal reserved word");
short           yyerror(ILLEGAL, "Illegal reserved word");
strictfp        yyerror(ILLEGAL, "Illegal reserved word");
super           yyerror(ILLEGAL, "Illegal reserved word");
synchronized    yyerror(ILLEGAL, "Illegal reserved word");
this            yyerror(ILLEGAL, "Illegal reserved word");
throw           yyerror(ILLEGAL, "Illegal reserved word");
throws          yyerror(ILLEGAL, "Illegal reserved word");
transient       yyerror(ILLEGAL, "Illegal reserved word");
try             yyerror(ILLEGAL, "Illegal reserved word");
var             yyerror(ILLEGAL, "Illegal reserved word");
volatile        yyerror(ILLEGAL, "Illegal reserved word");

"//".*$                 comment();
"/*"([^*]|\*[^/])*"*/"  comment();
^#.*$                   comment();
"/*"([^*]|\*[^/])*      yyerror(TOKEN_ERROR, "Unterminated multiline comment");

true            return LITERAL_BOOL;
false           return LITERAL_BOOL;

0|[1-9][0-9]{0,9}       return LITERAL_INT;
0[lL]?|[1-9][0-9]*[lL]  return LITERAL_LONG;
[1-9][0-9]{10,}         yyerror(INVALID_LITERAL, "Integer too long");
0[0-9]+                 yyerror(INVALID_LITERAL, "Octals not supported");
0x[0-9a-fA-F]+          yyerror(INVALID_LITERAL, "Hexadecimals not supported");
0b[01]+                 yyerror(INVALID_LITERAL, "Binary not supported");

[0-9]+\.?[0-9]*[fF]     return LITERAL_FLOAT;
[0-9]*\.[0-9]+[fF]      return LITERAL_FLOAT;
[0-9]+\.?[0-9]*[dD]     return LITERAL_DOUBLE;
[0-9]*\.[0-9]+[dD]      return LITERAL_DOUBLE;
[0-9]+\.\.+[0-9]*[fF]   yyerror(INVALID_LITERAL, "Too many dots in float");
[0-9]+\.[0-9]*          yyerror(INVALID_LITERAL, "Floats must end with f");
[0-9]*\.[0-9]+          yyerror(INVALID_LITERAL, "Floats must end with f");

[0-9]+[eE][+-]?[0-9]+[fF] return LITERAL_FLOAT;
[0-9]+[eE][+-]?[0-9]+   yyerror(INVALID_LITERAL, "Floats must end with f");
[0-9]+[eE][+-][+-]+     yyerror(INVALID_LITERAL, "Too many signs");
[0-9]+[eE]              yyerror(INVALID_LITERAL, "Exponent missing");

[0-9]+[0-9a-zA-Z]*      yyerror(INVALID_LITERAL, "Unknown number format");

\"([^\\"]|\\.)*\"       return LITERAL_STRING;
\"([^\\"]|\\.)*         yyerror(INVALID_LITERAL, "String was not closed");

'[^\\]'                 return LITERAL_CHAR;
'\\.'                   return LITERAL_CHAR;
'\\'                    yyerror(INVALID_LITERAL, "Escape character must be escaped");
'[^']{0,2}              yyerror(INVALID_LITERAL, "Character was not closed");
'.*'                    yyerror(INVALID_LITERAL, "Invalid character");

[a-zA-Z_][a-zA-Z0-9_]*  return ID;

.		        yyerror(UNKNOWN_TOKEN, "Unknown token");
