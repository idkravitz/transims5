//*********************************************************
//	Initial_Table.cpp - Interpret a Table
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Initial_Table
//---------------------------------------------------------

bool User_Program::Initial_Table (String &line)
{
	int i, j, index, lnum, len;
	Int_Itr litr, litr0;
	double fnum;
	Dbl_Itr fitr, fitr0;
	String token;
	Str_Itr sitr, sitr0;
	string::iterator ch, ch0;

	i = cmd.token;
	j = cmd.value;

	for (;;) {

		//---- remove leading spaces ----

		line.Trim_Left (" \t");
		if (line.empty ()) return (false);

		ch = ch0 = line.begin ();
	
		//---- determine the token type ----
	
		if ((*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.') || 
			(*ch == '-' && (*(ch+1) >= '0' && *(ch+1) <= '9'))) {
	
			//---- process a number ----

			if (*ch == '-') ch++;
		
			do {
				if (++ch == line.end ()) break;
			} while ((*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.'));
			
			len = (int) (ch - ch0);
			token = line.substr (0, len);
			line.erase (0, len);

			//---- set the table size ----
		
			if (open_par) {
				lnum = token.Integer ();
					
				if (lnum < 1) {
					exe->Error (String ("%s Table %d Size %d is Out of Range") % Data_Type (i) % j % lnum);
					return (false);
				}
				if (table [i] [j].size () > 0) {
					exe->Write (1);
					exe->Warning (String ("%s Table %d is Redefined") % Data_Type (i) % j);
					table [i] [j].clear ();
				}
				table [i] [j].reserve (lnum);

			} else {

				if (table_type == STAB) {
					exe->Error ("Attempt to Assign a Number to a String Table");
					return (false);
				}

				//---- search for the value in the list ----

				if (table_type == ITAB) {
					lnum = token.Integer ();

					for (litr = litr0 = lvalue.begin (); litr != lvalue.end (); litr++) {
						if (*litr == lnum) break;
					}
					if (litr == lvalue.end ()) {
						index = (int) lvalue.size ();	//---- 0 based ----
						lvalue.push_back (lnum);
					} else {
						index = (int) (litr - litr0);	//---- 0 based ----
					}
				} else {
					fnum = token.Double ();

					for (fitr = fitr0 = fvalue.begin (); fitr != fvalue.end (); fitr++) {
						if (*fitr == fnum) break;
					}
					if (fitr == fvalue.end ()) {
						index = (int) fvalue.size ();	//---- 0 based ----
						fvalue.push_back (fnum);
					} else {
						index = (int) (fitr - fitr0);	//---- 0 based ----
					}
				}
				table [i] [j].push_back (index);
			}

		} else if (*ch == '"') {

			if (table_type != STAB) goto error;

			//---- process a string ----
			
			ch++;
			token.clear ();
			
			while (ch != line.end () && *ch != '"') {
				token += *ch++;
			}
			ch++;
			line.erase (0, ch - ch0);

			for (sitr = sitr0 = svalue.begin (); sitr != svalue.end (); sitr++) {
				if (token.Equals (*sitr)) break;
			}
			if (sitr == svalue.end ()) {
				index = (int) svalue.size ();
				svalue.push_back (token);
			} else {
				index = (int) (sitr - sitr0);	//---- 0 based ----
			}
			table [i] [j].push_back (index);

		} else if ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z')) {
				
			do {
				if (++ch == line.end ()) break;
			} while ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z') ||
				(*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.') || *ch == '_');
			
			len = (int) (ch - ch0);
			token = line.substr (0, len);
			line.erase (0, len);

			if (token.Equals ("ENDTAB")) {
				table_flag = false;
				open_par = false;
				return (true);
			} else {
				goto error;
			}

		} else if (*ch == '(' || *ch == ')' || *ch == ',' || *ch == '=') {

			if (*ch == '(') {
				open_par = true;
			} else if (*ch == ')') {
				open_par = false;
			} else if (*ch == ',' || *ch == '=') {
				if (open_par) {
					goto error;
				}
			}
			ch++;
			line.erase (0, ch - ch0);

		} else {
			goto error;
		}
	}
	return (true);

error:
	return (Syntax_Error ("Table Declaration"));
}
