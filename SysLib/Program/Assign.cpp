//********************************************************* 
//	Assign.cpp - Assign a Data Item
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Assign
//---------------------------------------------------------

bool User_Program::Assign (void)
{
	const char *target_types [] = {"Structure", 
		"Integer Field", "Decimal Field", "String Field", 
		"Integer Variable", "Decimal Variable", "String Variable", 
		"Record Number"};

	File_Data *file = 0;

	switch (cmd_ptr->type) {

		case DATA:
			if (s->type != STRUCT_DATA) goto error;

			file = &file_data [cmd_ptr->token];
			file->file->Copy_Fields (*file_data [s->lvalue].file);
			file->out_flag = 1;
			break;

		case INTEGER:
		case FLOAT:
			if (s->type == INT_DATA) {
				file = &file_data [cmd_ptr->token];
				file->file->Put_Field (cmd_ptr->value, s->lvalue);
			} else if (s->type == FLOAT_DATA) {
				file = &file_data [cmd_ptr->token];
				file->file->Put_Field (cmd_ptr->value, s->fvalue);
			} else {
				goto error;
			}
			file->out_flag = 1;
			break;

		case STRING:
			if (s->type != STRING_DATA) goto error;

			file = &file_data [cmd_ptr->token];
			file->file->Put_Field (cmd_ptr->value, *(s->str_ptr));
			file->out_flag = 1;
			break;
	
		case LVARIABLE:
			if (s->type == INT_DATA) {
				lvariable [cmd_ptr->value] = s->lvalue;
			} else if (s->type == FLOAT_DATA) {
				int value;

				if (s->fvalue > MAX_INTEGER) {
					value = MAX_INTEGER;
				} else if (s->fvalue < -MAX_INTEGER) {
					value = -MAX_INTEGER;
				} else {
					value = (int) s->fvalue;
				}
				lvariable [cmd_ptr->value] = value;
			} else {
				goto error;
			}
			break;
					
		case FVARIABLE:
			if (s->type == INT_DATA) {
				double dvalue = (double) s->lvalue;
				fvariable [cmd_ptr->value] = dvalue;
			} else if (s->type == FLOAT_DATA) {
				fvariable [cmd_ptr->value] = s->fvalue;
			} else {
				goto error;
			}
			break;
				
		case SVARIABLE:
			if (s->type == STRING_DATA) {
				svariable [cmd_ptr->value] = *(s->str_ptr);
			} else {
				goto error;
			}
			break;
		default:
			exe->Error (String ("Assignment to Type %d (Stack %d)") % cmd_ptr->type % stack_index);
			return (false);
			break;
	}
	s = &stack [sindex = 0];
	return (true);

error:
	exe->Error (String ("Illegal Assignment Statement\n\t\t%s = %s") %
		target_types [cmd_ptr->type - DATA] % Data_Type (s->type));
	return (false);
}
