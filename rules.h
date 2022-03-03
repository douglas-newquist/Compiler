/*
	Douglas Newquist
*/

#ifndef RULES
enum RULES
{
	R_EMPTY = 1000,
	R_ACCESS1,		   // Name.ID
	R_ACCESS2,		   // Primary.ID
	R_ACCESS3,		   // Array get element at i
	R_ARG_DEF_GROUP,   // Type ID, ..., Type ID
	R_ARG_GROUP,	   // Exp, ..., Exp
	R_ARRAY1,		   // Type[]
	R_ARRAY2,		   // new Type[Exp]
	R_ASSIGN,		   // Name = Exp
	R_BINARY_OP,	   // Exp op Exp
	R_BREAK1,		   // Break
	R_BREAK2,		   // Break ID
	R_CALL1,		   // Name(R_ARG_GROUP)
	R_CALL2,		   // Field(R_ARG_GROUP)
	R_CASE_GROUP,	   // R_CASE, ..., R_CASE
	R_CASE,			   // case Literal R_STATEMENT_GROUP
	R_CLASS_GROUP,	   //
	R_CLASS1,		   // public class ID R_CLASS_GROUP
	R_DEFAULT_CASE,	   //
	R_DEFINE1,		   // Type ID
	R_DEFINE2,		   // Type R_VAR_GROUP
	R_DEFINE3,		   // ID = Exp
	R_EXP_GROUP,	   //
	R_FOR,			   // for(R_EXP_GROUP?; Exp?, R_EXP_GROUP?) R_STATEMENT_GROUP
	R_IF1,			   // if (Exp) R_STATEMENT_GROUP
	R_IF2,			   // R_IF1 else R_STATEMENT_GROUP
	R_IF3,			   // (R_IF1 | R_IF3) else R_IF1
	R_IF4,			   // R_IF3 else R_STATEMENT_GROUP
	R_METHOD1,		   // Type ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_METHOD2,		   // public? static? R_METHOD1
	R_METHOD3,		   // public ID(R_ARG_GROUP) R_STATEMENT_GROUP
	R_NEW1,			   // new Type(R_ARG_GROUP)
	R_RETURN1,		   // Return
	R_RETURN2,		   // Return Exp
	R_STATEMENT_GROUP, // Statement Statement+
	R_SWITCH,		   // switch(Exp) R_CASE_GROUP
	R_UNARY_OP1,	   // op Exp
	R_UNARY_OP2,	   // Exp op
	R_VAR_GROUP,	   // (R_DEFINE3 | ID), ..., (R_DEFINE3 | ID)
	R_WHILE			   // while(Exp) R_STATEMENT_GROUP
};
#endif
