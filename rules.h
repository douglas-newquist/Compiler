/*
	Douglas Newquist
*/

#ifndef RULES
enum RULES
{
	R_EMPTY = 1000,
	R_ACCESS1,		 // Name.ID
	R_ACCESS2,		 // Primary.ID
	R_ACCESS3,		 // Array get element at i
	R_AND,			 // Exp && Exp
	R_ARG_DEF_GROUP, // Type ID, ..., Type ID
	R_ARG_GROUP,	 // Exp, ..., Exp
	R_ARRAY1,		 // Type[]
	R_ARRAY2,		 // new Type[Exp]
	R_ASSIGN,		 // Name = Exp
	R_BREAK1,		 // Break
	R_BREAK2,		 // Break ID
	R_CALL1,		 // Name(R_ARG_GROUP)
	R_CALL2,		 // Field(R_ARG_GROUP)
	R_CASE_GROUP,	 // R_CASE, ..., R_CASE
	R_CASE,			 // case Literal R_STATEMENT_GROUP
	R_CLASS_GROUP,
	R_CLASS1, // public class ID R_CLASS_GROUP
	R_DEFAULT_CASE,
	R_DEFINE1, // Type ID
	R_DEFINE2, // Type R_VAR_GROUP
	R_DEFINE3, // ID = Exp
	R_EQUALS,  // Exp == Exp
	R_EXP_GROUP,
	R_FOR,
	R_GROUP,
	R_IF1,		 // if (Exp) R_STATEMENT_GROUP
	R_IF2,		 // R_IF1 else R_STATEMENT_GROUP
	R_IF3,		 // (R_IF1 | R_IF3) else R_IF1
	R_IF4,		 // R_IF3 else R_STATEMENT_GROUP
	R_METHOD1,	 // public static Type ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_METHOD2,	 // public ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_NEW1,		 // new Type(R_ARG_GROUP)
	R_NOT_EQUAL, // Exp != Exp
	R_OR,		 // Exp || Exp
	R_RETURN1,	 // Return
	R_RETURN2,	 // Return Exp
	R_STATEMENT_GROUP,
	R_STEP,		 // Name++ or Name--
	R_SWITCH,	 // switch(Exp) R_CASE_GROUP
	R_VAR_GROUP, // (R_DEFINE3 | ID), ..., (R_DEFINE3 | ID)
	R_WHILE		 // while(Exp) R_STATEMENT_GROUP
};
#endif
