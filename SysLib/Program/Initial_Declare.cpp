//*********************************************************
//	Initial_Declare.cpp - interpret a list of variables
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Initial_Declare
//---------------------------------------------------------

bool User_Program::Initial_Declare (String &line)
{
	int i, j, len, lnum;
	double fnum;
	bool range;
	String token, sval;
	string::iterator ch, ch0;
	Token_Data var;

	len = 0;
	range = false;
	var.name = "";
	var.code = 0;
	var.type = 0;

	for (;;) {
	
		//---- remove leading spaces ----

		line.Trim_Left (" \t");
		if (line.empty ()) break;

		ch = ch0 = line.begin ();
	
		//---- determine the token type ----

		if ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z')) {
				
			do {
				if (++ch == line.end ()) break;
			} while ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z') ||
				(*ch >= '0' && *ch <= '9') || *ch == '_');

			len = (int) (ch - ch0);
			token = line.substr (0, len);
			line.erase (0, len);
			
			if (token.Equals ("ENDDEF")) {
				declare_flag = false;
				return (false);
			}

			//---- look for the variable in the list ----

			if (Variable (token) != 0) {
				exe->Error (String ("User Variable \"%s\" was Redefined") % token);
				return (false);
			}

			//---- process a variable range ----

			i = j = 0;

			if (range) {
				len = (int) var.name.size ();
				ch = token.begin ();

				for (; i < len && ch != token.end (); i++, ch++) {
					if ((*ch >= 'A' && *ch <= 'Z') || 
						(*ch >= 'a' && *ch <= 'z') || *ch == '_') {
						if (*ch != var.name [i]) goto syntax_error;
					} else if (var.name [i] >= '0' && var.name [i] <= '9') {
						sval = token.substr (i);
						j = sval.Integer ();
						token.erase (0, i);
						sval = var.name.substr (i);
						i = sval.Integer ();
						break;
					} else {
						goto syntax_error;
					}
				}
				if (i >= j) goto syntax_error;
				i++;
			}

			//---- create new variables ----

			for (; i <= j; i++) {
				if (range) {
					var.name ("%s%d") % token % i;
				} else {
					var.name = token;
				}
				switch (declare_type) {
					case IVAR:
						lnum = 0;
						var.type = LVARIABLE;
						var.code = (int) lvariable.size ();	//---- 0 based ----
						lvariable.push_back (lnum);
						break;
					case RVAR:
						fnum = 0.0;
						var.type = FVARIABLE;
						var.code = (int) fvariable.size ();	//---- 0 based ----
						fvariable.push_back (fnum);
						break;
					case SVAR:
						sval.clear ();
						var.type = SVARIABLE;
						var.code = (int) svariable.size ();	//---- 0 based ----
						svariable.push_back (sval);
						break;
				}
				variables.push_back (var);
			}
			range = false;
			
		} else if (*ch == ',') {

			line.erase (0, ch - ch0 + 1);
			var.name.clear ();

		} else if (*ch == '.' && *(ch+1) == '.') {

			if (var.name.empty ()) goto syntax_error;

			range = true;
			
			while (ch != line.end () && *ch == '.') ch++;

			line.erase (0, ch - ch0);

		} else {

			exe->Error ("Declaring User Variables");
			return (false);
		}
	}
	return (true);

syntax_error:
	exe->Error (String ("Illegal Declaration Range at %s Variable %s") % Data_Type (declare_type) % token);
	return (false);
}
