/*
	Douglas Newquist
*/

#ifndef RULES
enum RULES
{
	R_EMPTY = 1000,
	R_ACCESS1,			 // Name.ID
	R_ACCESS2,			 // Primary.ID
	R_ACCESS3,			 // Array get element at i
	R_ARG_DEF_GROUP,	 // Type ID, ..., Type ID
	R_ARG_GROUP,		 // Exp, ..., Exp
	R_ARRAY1,			 // Type[]
	R_ARRAY2,			 // new Type[Exp]
	R_ASSIGN,			 // Name = Exp
	R_BINARY_OP,		 // Exp op Exp
	R_BREAK1,			 // Break
	R_BREAK2,			 // Break ID
	R_CALL1,			 // Name(R_ARG_GROUP)
	R_CALL2,			 // Field(R_ARG_GROUP)
	R_CASE_GROUP,		 // R_CASE, ..., R_CASE
	R_CASE,				 // case Literal R_STATEMENT_GROUP
	R_CLASS_GROUP,		 //
	R_CLASS1,			 // public class ID R_CLASS_GROUP
	R_DEFAULT_CASE,		 //
	R_DEFINE1,			 // Type ID
	R_DEFINE2,			 // Type R_VAR_GROUP
	R_DEFINE3,			 // ID = Exp
	R_DEFINE4,			 //
	R_EXP_GROUP,		 //
	R_FIELD,			 // public static R_VAR_GROUP
	R_FOR,				 // for(R_EXP_GROUP?; Exp?, R_EXP_GROUP?) R_STATEMENT_GROUP
	R_IF1,				 // if (Exp) R_STATEMENT_GROUP
	R_IF2,				 // R_IF1 else R_STATEMENT_GROUP
	R_IF3,				 // (R_IF1 | R_IF3) else R_IF1
	R_IF4,				 // R_IF3 else R_STATEMENT_GROUP
	R_METHOD1,			 // Type ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_METHOD2,			 // public? static? R_METHOD1
	R_METHOD3,			 // public ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_NEW1,				 // new Type(R_ARG_GROUP)
	R_OP1_DECREMENT,	 // a--
	R_OP1_INCREMENT,	 // a++
	R_OP1_NEGATE,		 // -a
	R_OP1_NOT,			 // !a
	R_OP2_ADD,			 // a + b
	R_OP2_AND,			 // a && b
	R_OP2_DIV,			 // a / b
	R_OP2_EQUALS,		 // a == b
	R_OP2_GREATER_EQUAL, // a >= b
	R_OP2_GREATER,		 // a > b
	R_OP2_LESS_EQUAL,	 // a <= b
	R_OP2_LESS,			 // a < b
	R_OP2_MOD,			 // a % b
	R_OP2_MULT,			 // a * b
	R_OP2_NOT_EQUAL,	 // a != b
	R_OP2_OR,			 // a || b
	R_OP2_SUB,			 // a- b
	R_RETURN1,			 // Return
	R_RETURN2,			 // Return Exp
	R_STATEMENT_GROUP,	 // Statement Statement+
	R_SWITCH,			 // switch(Exp) R_CASE_GROUP
	R_VAR_GROUP,		 // (R_DEFINE3 | ID), ..., (R_DEFINE3 | ID)
	R_WHILE,			 // while(Exp) R_STATEMENT_GROUP
};
#endif
