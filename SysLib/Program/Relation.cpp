//*********************************************************
//	Relation.cpp - logical relationships
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Relation
//---------------------------------------------------------

bool User_Program::Relation (void)
{
	int l1, l2;
	double f1, f2;
	String str1, str2;

	if (s->type == STRUCT_DATA || s1->type == STRUCT_DATA) goto error;

	//---- process a NOT statement ---

	if (cmd_ptr->token == NOT) {
		if (s->type == INT_DATA) {
			s->lvalue = !(s->lvalue);
		} else if (s->type == FLOAT_DATA) {
			s->fvalue = !(s->fvalue);
		} else {
			goto error;
		}
		return (true);
	}

	//---- check for string relationships ----

	if (s1->type == STRING_DATA || s->type == STRING_DATA) {
		if (s1->type == STRING_DATA) {
			str1 = *(s1->str_ptr);
		} else if (s1->type == FLOAT_DATA) {
			str1 ("%.2lf") % s1->fvalue;
		} else {
			str1 ("%d") % s1->lvalue;
		}
		if (s->type == STRING_DATA) {
			str2 = *(s->str_ptr);
		} else if (s->type == FLOAT_DATA) {
			str2 ("%.2lf") % s->fvalue;
		} else {
			str2 ("%d") % s->lvalue;
		}
		switch (cmd_ptr->token) {
			case EQ:
				l1 = (str1 == str2);
				break;
			case NEQ:
				l1 = (str1 != str2);
				break;
			case LT:
				l1 = (str1 < str2);
				break;
			case LE:
				l1 = (str1 <= str2);
				break;
			case GT:
				l1 = (str1 > str2);
				break;
			case GE:
				l1 = (str1 >= str2);
				break;
			case AND:
				l1 = (str1 == str2);
				break;
			case OR:
				l1 = (str1 != str2);
				break;
			default:
				goto token_error;
				break;
		}
		
	} else if (s1->type == FLOAT_DATA || s->type == FLOAT_DATA) {

		//---- process floating point relationships ----

		if (s1->type == FLOAT_DATA) {
			f1 = s1->fvalue;
		} else {
			f1 = s1->lvalue;
		}
		if (s->type == FLOAT_DATA) {
			f2 = s->fvalue;
		} else {
			f2 = s->lvalue;
		}
		
		switch (cmd_ptr->token) {
			case EQ:
				l1 = (f1 == f2);
				break;
			case NEQ:
				l1 = (f1 != f2);
				break;
			case LT:
				l1 = (f1 < f2);
				break;
			case LE:
				l1 = (f1 <= f2);
				break;
			case GT:
				l1 = (f1 > f2);
				break;
			case GE:
				l1 = (f1 >= f2);
				break;
			case AND:
				l1 = (f1 && f2);
				break;
			case OR:
				l1 = (f1 || f2);
				break;
			default:
				goto token_error;
				break;
		}

	} else {

		//---- process integer relationships ----

		l1 = s1->lvalue;
		l2 = s->lvalue;

		switch (cmd_ptr->token) {
			case EQ:
				l1 = (l1 == l2);
				break;
			case NEQ:
				l1 = (l1 != l2);
				break;
			case LT:
				l1 = (l1 < l2);
				break;
			case LE:
				l1 = (l1 <= l2);
				break;
			case GT:
				l1 = (l1 > l2);
				break;
			case GE:
				l1 = (l1 >= l2);
				break;
			case AND:
				l1 = (l1 && l2);
				break;
			case OR:
				l1 = (l1 || l2);
				break;
			default:
				goto token_error;
				break;
		}
	}
	s = &stack [--sindex];

	s->type = INT_DATA;
	s->lvalue = l1;
	return (true);

error:
	exe->Error (String ("Relationship \"%s\" does not support %s Data") % 
		Token_Name (RELATION, cmd_ptr->token) % Data_Type (s->type));
	return (false);

token_error:
	exe->Error (String ("Unrecognized Relationship Token = %d") % cmd_ptr->token);
	return (false);
}
