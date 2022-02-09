/*
	Douglas Newquist
*/

#define TYPE_SHIFT 8
#define RESERVED (1 << TYPE_SHIFT)
#define OPERATOR (2 << TYPE_SHIFT)
#define PUNCTUATION (3 << TYPE_SHIFT)
#define COMPARATOR (4 << TYPE_SHIFT)
#define LITERAL (5 << TYPE_SHIFT)
#define OTHER (6 << TYPE_SHIFT)

// Generic tokens
#define ID (OTHER | 1)

// Operators
#define EQUAL (OPERATOR | 1)
#define ADD (OPERATOR | 2)
#define SUB (OPERATOR | 3)
#define MULT (OPERATOR | 4)
#define DIV (OPERATOR | 5)
#define INCREMENT (OPERATOR | 6)
#define DECREMENT (OPERATOR | 7)
#define MODULO (OPERATOR | 8)
#define AND (OPERATOR | 9)
#define NOT (OPERATOR | 10)
#define OR (OPERATOR | 11)
#define DOT (OPERATOR | 12)
#define CAST (OPERATOR | 14)

// Punctuation
#define OPEN_CURLY (PUNCTUATION | 1)
#define CLOSE_CURLY (PUNCTUATION | 2)
#define OPEN_PAREN (PUNCTUATION | 3)
#define CLOSE_PAREN (PUNCTUATION | 4)
#define OPEN_SQUARE (PUNCTUATION | 5)
#define CLOSE_SQUARE (PUNCTUATION | 6)
#define SEMICOLON (PUNCTUATION | 7)
#define COLON (PUNCTUATION | 8)
#define COMMA (PUNCTUATION | 9)

// Comparators
#define EQUALS (COMPARATOR | 1)
#define NOT_EQUAL (COMPARATOR | 2)
#define LESS (COMPARATOR | 3)
#define GREATER (COMPARATOR | 4)
#define LESS_EQUAL (COMPARATOR | 5)
#define GREATER_EQUAL (COMPARATOR | 6)

// Reserved words
#define INT (RESERVED | 1)
#define FLOAT (RESERVED | 2)
#define LONG INT
#define CHAR (RESERVED | 4)
#define BOOLEAN (RESERVED | 5)
#define BREAK (RESERVED | 6)
#define CASE (RESERVED | 7)
#define CLASS (RESERVED | 8)
#define CONTINUE (RESERVED | 9)
#define DEFAULT (RESERVED | 10)
#define DOUBLE FLOAT
#define IF (RESERVED | 11)
#define ELSE (RESERVED | 12)
#define FOR (RESERVED | 13)
#define INSTANCEOF (RESERVED | 14)
#define NEW (RESERVED | 15)
#define PUBLIC (RESERVED | 16)
#define RETURN (RESERVED | 17)
#define STATIC (RESERVED | 18)
#define SWITCH (RESERVED | 19)
#define VOID (RESERVED | 20)
#define WHILE (RESERVED | 21)

// Literals
#define LITERAL_INT (LITERAL | 1)
#define LITERAL_LONG LITERAL_INT
#define LITERAL_DOUBLE (LITERAL | 2)
#define LITERAL_FLOAT LITERAL_DOUBLE
#define LITERAL_CHAR (LITERAL | 3)
#define LITERAL_STRING (LITERAL | 4)
#define LITERAL_BOOL (LITERAL | 5)
