//********************************************************* 
//	Print_Commands.cpp - Write Commands to the Printer
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Print_Commands
//---------------------------------------------------------

void User_Program::Print_Commands (bool header)
{
	int i;
	Command_Itr cmd_itr;
	File_Data *file;

	String text;
	
	const char *type_text [] = {
		"Error",
		"Assign",
		"Logical",
		"Relation",
		"Math",
		"Function",
		"Convert",
		"Date-Time",
		"In/Output",
		"Table",
		"Return",
		"Declare",
		"Structure",
		"Integer",
		"Real",
		"String",
		"Integer",
		"Real",
		"String",
		"Integer",
		"Real",
		"String",
		"Record"
	};

	//---- print the page header ----

	if (!exe->Break_Check ((int) command.size () + 5)) {
		exe->Print (1);
		exe->Page_Header ();
	}
	if (header) {
		exe->Print (1, "Command Processing Stack");
		exe->Print (1);
	}

	//---- process the command stack ----

	for (i=1, cmd_itr = command.begin (); cmd_itr != command.end (); cmd_itr++, i++) {

		switch (cmd_itr->type) {
			case LOGICAL:
				if (cmd_itr->token == IF || cmd_itr->token == WHILE) {
					text ("If False, Jump to %d") % (cmd_itr->value + 2);
				} else {
					text ("Jump to %d") % (cmd_itr->value + 2);
				}
				break;

			case RELATION:
			case FUNCTION:
			case CONVERT:
			case DATETIME:
			case IN_OUT:
				text (Token_Name (cmd_itr->type, cmd_itr->token));
				break;

			case MATH:
			case LIMIT:
				if (cmd_itr->token == NEGATIVE) {
					text ("Negative");
				} else {
					text (Token_Name (cmd_itr->type, cmd_itr->token));
				}
				break;

			case TABLE:
				if (cmd_itr->token == ITAB) {
					text ("ITAB%d") % cmd_itr->value;
				} else if (cmd_itr->token == RTAB) {
					text ("RTAB%d") % cmd_itr->value;
				} else {
					text ("STAB%d") % cmd_itr->value;
				}
				break;

			case RETURN:
				text ("Integer");
				break;

			case DATA:
				file = &(file_data [cmd_itr->token]);
				text (file->file->File_ID ());
				break;

			case INTEGER:
			case FLOAT:
			case STRING:
				file = &(file_data [cmd_itr->token]);
				text (file->file->File_ID ());
				text += ".";
				if (cmd_itr->value < 0) {
					text += "NEST";
				} else {
					text += file->file->Field (cmd_itr->value)->Name ();
				}
				break;

			case RECORD:
				text ("Integer");
				break;

			case LVALUE:
				text ("%d") % lvalue [cmd_itr->value];
				break;

			case FVALUE:
				text ("%lf") % fvalue [cmd_itr->value];
				break;

			case SVALUE:
				text = '"';
				text += svalue [cmd_itr->value];
				text += '"';
				break;

			case LVARIABLE:
			case FVARIABLE:
			case SVARIABLE:
				text = Variable_Name (cmd_itr->type, cmd_itr->value);

				if (text.empty ()) {
					exe->Error (String ("type=%d, value=%d") % cmd_itr->type % cmd_itr->value);
				}
				break;

			default:
				text ("??? %d") % cmd_itr->token;
				break;
	
		}

		//---- print the line ----

		if (header && exe->Break_Check (1)) {
			exe->Print (1, "Command Processing Stack");
			exe->Print (1);
		}
		exe->Print (1, String ("%3d) %-10.10s   %s") % i % type_text [cmd_itr->type] % text);	
	}
	exe->Print (1, String ("%3d) End          1") % i);
	exe->Print (1);
}	
	 
/**********************************************************|***********************************************************

	Command Processing Stack

	xxx) ssssssssss   sssssssssssssssssssssss

***********************************************************|***********************************************************/ 
