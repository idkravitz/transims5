//*************************************************** 
//	Input_Output.cpp - input-output functions
//***************************************************

#include "User_Program.hpp"

//-----------------------------------------------------------
//	Input_Output
//-----------------------------------------------------------

bool User_Program::Input_Output (void)
{
	Stack_Data *s2, *s3;
	int fh, len, offset, max;
	File_Data *file;
	String buffer, *str_ptr;
	char *record;

	if (s->type == STRUCT_DATA) goto error;

	if (cmd_ptr->token == INPUT || cmd_ptr->token == OUTPUT) {

		if (s->type != INT_DATA || s1->type != STRUCT_DATA) goto error;

		fh = s1->lvalue;
		file = &file_data [fh];
		record = file->file->Record_String ();
		offset = s->lvalue;

		if (cmd_ptr->token == INPUT) {
			//s1->lvalue = file [fh]->Read (record, size, offset);
		} else {
			//s1->lvalue = file [fh]->Write (record, &offset);
		}
		s1->type = INT_DATA;

	} else {

		if (s1->type != STRING_DATA) goto error;

		if (cmd_ptr->token == PRINT || cmd_ptr->token == LIST || cmd_ptr->token == FORMAT) {

			//----- get the format syntax ----
			
			buffer = *(s1->str_ptr);

			//---- format the data ---

			switch (s->type) {
				case INT_DATA:
					buffer % s->lvalue;
					break;
				case FLOAT_DATA:
					buffer % s->fvalue;
					break;
				case STRING_DATA:
					buffer % *(s->str_ptr);
					break;
				default:
					break;
			}
			if (cmd_ptr->token == FORMAT) {
				str_ptr = &svalue [0];
				*str_ptr = buffer;
				s1->str_ptr = str_ptr;
			} else {
				s = &stack [--sindex];

				if (cmd_ptr->token == PRINT) {
					exe->Print (1, buffer);
				} else {
					exe->Show_Message (buffer);
				}
			}

		} else if (cmd_ptr->token == FREAD || cmd_ptr->token == FWRITE) {

			s2 = &stack [sindex - 2];
			s3 = &stack [sindex - 3];

			if (s3->type != STRUCT_DATA || s2->type != INT_DATA) goto error;

			fh = s3->lvalue;
			file = &file_data [fh];
			len = file->file->Record_Size ();
			record = file->file->Record_String ();
			offset = s2->lvalue;

			if (cmd_ptr->token == FREAD) {

				//---- read ascii data ----

				//max = FIELD_BUFFER;
				//max = Max_Field_Width ();
				//if (offset + max > len) goto record_error;
				
				//str_ncpy (token, sizeof (token), record + offset, max);

				//token [max] = '\0';

				switch (s->type) {
					case INT_DATA:
						//if (sscanf (token, buffer, &(s->lvalue)) != 1) s->lvalue = 0;
						break;
					case FLOAT_DATA:
						//if (sscanf (token, buffer, &(s->fvalue)) != 1) s->fvalue = 0.0;
						break;
					case STRING_DATA:
						//if (sscanf (token, buffer, &(s->svalue)) != 1) s->svalue = token;
						break;
				}
				*s3 = *s;

			} else {

				//---- write ascii data ----

				str_ptr = s1->str_ptr;

				switch (s->type) {
					case INT_DATA:
						buffer (*str_ptr) % s->lvalue;
						break;
					case FLOAT_DATA:
						buffer (*str_ptr) % s->fvalue;
						break;
					case STRING_DATA:
						buffer (*str_ptr) % *(s->str_ptr);
						break;
				}
				max = (int) buffer.size ();

				if (offset + max > len) goto record_error;

				memcpy (record + offset, buffer.c_str (), max);
				file->out_flag = 1;
					
				s3->type = INT_DATA;
				s3->lvalue = offset + max;
			}
			sindex -= 2;
			s = s2;

		} else {
			exe->Error (String ("Input-Output Token %d was Unrecognized") % cmd_ptr->token);
			return (false);
		}
	}
	s = &stack [--sindex];
	return (true);

record_error:
	exe->Error (String ("ASCII Record Offset %d-%d is Out-of-Range (0-%d)") % offset % (offset + max) % len);
	return (false);

error:
	exe->Error (String ("Input-Output Function \"%s\" does not support %s Data") %
		Token_Name (IN_OUT, cmd_ptr->token) % Data_Type (s->type));
	return (false);
}

