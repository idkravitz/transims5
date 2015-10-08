//*********************************************************
//	Compile.cpp - Compile the User Program
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Compile
//---------------------------------------------------------

bool User_Program::Compile (string &text, bool list_flag)
{
	Strings lines;
	Str_Itr line_itr;

	//---- split the text into lines ----

	String_Ptr (text)->Parse (lines, "\n\r\f");

	for (line_itr = lines.begin (); line_itr != lines.end (); line_itr++) {
		line_num++;

		if (list_flag) {
			exe->Print (1, "\t") << *line_itr;
		}
		line_itr->Clean ();
		if (line_itr->empty ()) continue;

		if (declare_flag) {
			if (!Initial_Declare (*line_itr)) break;
		} else if (table_flag) {
			if (!Initial_Table (*line_itr)) break;
		} else {
			while (Get_Token (*line_itr)) {
				if (!Process_Token ()) break;
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Compile from a file
//---------------------------------------------------------

bool User_Program::Compile (Db_File &script, bool list_flag)
{
	string text;

	script.Clean_Flag (false);

	while (script.Read ()) {
		text = script.Record_String ();

		if (!Compile (text, list_flag)) {
			exe->Error ("Compiling User Program Script");
			return (false);
		}
	}
	text = "END";
	Compile (text, false);

	script.Close ();
	return (true);
}
