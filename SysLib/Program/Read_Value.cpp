//********************************************************* 
//	Read_Value.cpp - read variables from data structures
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Read_Value
//---------------------------------------------------------

bool User_Program::Read_Value (void)
{
	int code;
	File_Data *file;
	Int_Map_Itr map_itr;

	if (++sindex == (int) stack.size ()) {
		Stack_Data stk;
		stack.push_back (stk);
	}
	s = &stack [sindex];
			
	switch (cmd_ptr->type) {
		case DATA:
			s->type = STRUCT_DATA;
			s->lvalue = cmd_ptr->token;
			break;
				
		case INTEGER:
			s->type = INT_DATA;
			file = &file_data [cmd_ptr->token];
			if (cmd_ptr->value < 0) {
				s->lvalue = (file->file->Nested ()) ? 1 : 0;
			} else {
				file->file->Get_Field (cmd_ptr->value, s->lvalue);
			}
			break;
				
		case FLOAT:
			s->type = FLOAT_DATA;
			file = &file_data [cmd_ptr->token];
			file->file->Get_Field (cmd_ptr->value, s->fvalue);
			break;
				
		case STRING:
			s->type = STRING_DATA;
			file = &file_data [cmd_ptr->token];

			code = cmd_ptr->value * num_files + cmd_ptr->token;

			map_itr = svalue_index.find (code);

			if (map_itr == svalue_index.end ()) {
				exe->Error (String ("Svalue Index for Code %d Not Found") % code);
			}
			s->str_ptr = &svalue [map_itr->second];

			file->file->Get_Field (cmd_ptr->value, *(s->str_ptr));
			break;
			
		case LVALUE:
			s->type = INT_DATA;
			s->lvalue = lvalue [cmd_ptr->value];
			break;
			
		case FVALUE:
			s->type = FLOAT_DATA;
			s->fvalue = fvalue [cmd_ptr->value];
			break;
				
		case SVALUE:
			s->type = STRING_DATA;
			s->str_ptr = &svalue [cmd_ptr->value];
			break;

		case LVARIABLE:
			s->type = INT_DATA;
			s->lvalue = lvariable [cmd_ptr->value];
			break;
			
		case FVARIABLE:
			s->type = FLOAT_DATA;
			s->fvalue = fvariable [cmd_ptr->value];
			break;
				
		case SVARIABLE:
			s->type = STRING_DATA;
			s->str_ptr = &svariable [cmd_ptr->value];
			break;

		case RECORD:
			s->type = INT_DATA;
			s->lvalue = record;
			break;

		default:
			exe->Error (String ("Token Type %d was Not Recognized") % cmd_ptr->type);
			return (false);
	}
	return (true);
}
