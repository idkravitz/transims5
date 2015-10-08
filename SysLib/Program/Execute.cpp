///************************************************** 
//	Execute.cpp - Execute a User Program
///**************************************************

#include "User_Program.hpp"

//-----------------------------------------------------------
//	Execute
//-----------------------------------------------------------

int	User_Program::Execute (int _record)
{
	int stat;
	bool test;

	Command_Itr itr, itr0;
	File_Itr file_itr;

	for (file_itr = file_data.begin (); file_itr != file_data.end (); file_itr++) {
		file_itr->out_flag = 0;
	}
	stat = 1;
	s = &stack [sindex = 0];		//---- may need to be 0 ----
	loop = 0;

	if (_record < 0) {
		record++;
	} else {
		record = _record;
	}

	//---- process each command ----

	for (itr = itr0 = command.begin (); itr != command.end (); itr++) {
		cmd_ptr = &(*itr);

		stack_index = (int) (itr - itr0);

		if (sindex > 0) {
			s1 = &stack [sindex - 1];
		} else {
			s1 = 0;
		}
		switch (cmd_ptr->type) {

			case LIMIT:
				if (cmd_ptr->token != EQUALS) {
					exe->Error (String ("Unrecognized Limit Token = %d (stack=%d)") % 
						cmd_ptr->token % stack_index);
					return (-1);
				}
				cmd_ptr = &(*(++itr));
				if (!Assign ()) return (-1);
				break;

			case LOGICAL:
				if (cmd_ptr->token == IF || cmd_ptr->token == WHILE) {
					if (s->type == INT_DATA) {
						test = (s->lvalue != 0);
					} else if (s->type == FLOAT_DATA) {
						test = (s->fvalue != 0.0);
					} else if (s->type == STRING_DATA) {
						test = !s->str_ptr->empty ();
					} else {
						test = false;
					}
					s = &stack [sindex = 1];
					if (test) continue;
				}
				itr = itr0 + cmd_ptr->value;
				break;

			case RELATION:
				if (!Relation ()) return (-1);
				break;

			case MATH:
				if (!Math ()) return (-1);
				break;

			case FUNCTION:
				if (!Function ()) return (-1);
				break;

			case CONVERT:
				if (!Convert ()) return (-1);
				break;

			case DATETIME:
				if (!Date_Time ()) return (-1);
				break;

			case IN_OUT:
				if (!Input_Output ()) return (-1);
				break;

			case TABLE:
				if (!Table ()) return (-1);
				break;

			case RETURN:
				if (s->type == INT_DATA) {
					stat = s->lvalue;
				} else if (s->type == FLOAT_DATA) {
					stat = (int) s->fvalue;
				} else if (s->type == STRING_DATA) {
					stat = (s->str_ptr->empty ()) ? 0 : 1;
				} else {
					stat = 0;
				}
				return (stat);
				break;

			default:
				if (!Read_Value ()) return (-1);
				break;
		}
	}
	return (stat);
}
