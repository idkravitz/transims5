//********************************************************* 
//	Get_Token.cpp - reads a token from the command string
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Get_Token
//---------------------------------------------------------

bool User_Program::Get_Token (String &line)
{
	int i, j, len, lnum, type;
	Int_Itr litr, litr0;
	double fnum;
	Dbl_Itr fitr, fitr0;
	String token, token2;
	Str_Itr sitr, sitr0;
	string::iterator ch, ch0;
	size_t pos;
	File_Data *file = 0;

	static int type_map [] = {
		INTEGER, INTEGER, FLOAT, FLOAT, STRING, STRING, INTEGER, INTEGER, INTEGER, FLOAT
	};

	//---- remove leading spaces ----

	line.Trim_Left (" \t");
	if (line.empty ()) return (false);

	ch = ch0 = line.begin ();
	
	//---- determine the token type ----
	
	if ((*ch >= '0' && *ch <= '9') || 
		(*ch == '.' && *(ch+1) != '.') || *ch == ':') {
	
		//---- process a number ----
		
		do {
			if (++ch == line.end ()) break;
		} while ((*ch >= '0' && *ch <= '9') || 
			(*ch == '.' && *(ch+1) != '.') || *ch == ':');
			
		len = (int) (ch - ch0);
		token = line.substr (0, len);
		line.erase (0, len);
	
		//---- check for decimal point ----
		
		if (token.find ('.') == token.npos) {
			cmd.type = LVALUE;
			cmd.token = 0;
		
			//---- integer number ----

			pos = token.find (':');

			if (pos != token.npos) {
				lnum = (token.Integer () << 16);
				token.erase (0, pos); 
				lnum += token.Integer ();
			} else {
				lnum = token.Integer ();
			}

			//---- search for the number in the list ----

			for (litr = litr0 = lvalue.begin (); litr != lvalue.end (); litr++) {
				if (*litr == lnum) {
					cmd.value = (int) (litr - litr0);
					break;
				}
			}
			if (litr == lvalue.end ()) {
				cmd.value = (int) lvalue.size ();
				lvalue.push_back (lnum);
			}
			
		} else {
		
			//---- floating point number ----

			cmd.type = FVALUE;
			cmd.token = 0;

			fnum = token.Double ();
			
			for (fitr = fitr0 = fvalue.begin (); fitr != fvalue.end (); fitr++) {
				if (*fitr == fnum) {
					cmd.value = (int) (fitr - fitr0);
					break;
				}
			}
			if (fitr == fvalue.end ()) {
				cmd.value = (int) fvalue.size ();
				fvalue.push_back (fnum);
			}
		}
		return (true);
		
	} else if (*ch == '"') {
	
		//---- process a string ----
		
		ch++;
		token.clear ();
		
		while (ch != line.end () && *ch != '"') {
			token += *ch++;
		}
		if (ch == line.end ()) {
			return (Syntax_Error ("String Value"));
		}
		ch++;
		line.erase (0, ch - ch0);

		cmd.type = SVALUE;
		cmd.token = 0;

		for (sitr = sitr0 = svalue.begin (); sitr != svalue.end (); sitr++) {
			if (token.Equals (*sitr)) {
				cmd.value = (int) (sitr - sitr0);
				break;
			}
		}
		if (sitr == svalue.end ()) {
			cmd.value = (int) svalue.size ();
			svalue.push_back (token);
		}
		return (true);
	
	} else if ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z')) {
	
		//---- process a variable or command ---- 

		do {
			if (++ch == line.end ()) break;
		} while ((*ch >= 'A' && *ch <= 'Z') || 
			(*ch >= 'a' && *ch <= 'z') ||
			(*ch >= '0' && *ch <= '9') || 
			(*ch == '.' && *(ch+1) != '.') || *ch == '_');
			
		len = (int) (ch - ch0);
		token = line.substr (0, len);
		line.erase (0, len);

		//---- check for a special variable name ----

		token_ptr = Special (token);

		if (token_ptr != 0) {
			if (token_ptr->type == DECLARE) {
				declare_flag = true;
				declare_type = token_ptr->code;
				return (Initial_Declare (line));
			}
			if (token_ptr->type == TABLE) {
				cmd.type = token_ptr->type;
				cmd.token = (token_ptr->code % 10);
				cmd.value = token_ptr->code / 10;

				if (table [cmd.token] [cmd.value].size () == 0) {
					table_flag = true;
					table_type = cmd.token;
					Initial_Table (line);
					return (false);
				}
			}
			return (true);
		}
		
		//---- search for a keyword command ----

		token_ptr = Keyword (token);

		if (token_ptr != 0) {
			cmd.type = token_ptr->type;
			cmd.token = token_ptr->code;

			if (cmd.type == IN_OUT && cmd.token == FWRITE) {
				ascii_write = true;
			}
			return (true);
		}

		//---- check for a file label ----

		pos = token.find ('.');
		
		if (pos != token.npos) {
			token2 = token.substr (pos+1);
			token.erase (pos);

			for (i=0; i < num_files; i++) {
				file = &file_data [i];
				if (token.Equals (file->file->File_ID ())) break;
			}
			if (i >= num_files) {
				String temp = token + "1";

				for (i=0; i < num_files; i++) {
					file = &file_data [i];
					if (temp.Equals (file->file->File_ID ())) break;
				}
				if (i >= num_files) {
					exe->Error (String ("File Name \"%s\" was Not Found") % token);
					return (false);
				}
			}
			j = file->file->Field_Number (token2);
			type = DB_INTEGER;

			if (j < 0 && !token2.Equals ("NEST")) {

				//---- check for a field number ----

				for (ch = token2.begin (); ch != token2.end (); ch++) {
					if (*ch < '0' || *ch > '9') {
						exe->Error (String ("File \"%s\" Field \"%s\" was Not Found") % token % token2);
						return (false);
					}
				}
				j = token2.Integer () - 1;
				if (j < 0 || j >= file->file->Num_Fields ()) {
					exe->Error (String ("File \"%s\" Field Number \"%s\" was Out of Range") % token % token2);
					return (false);
				}
			}
			if (j >= 0) {
				Db_Field *fld = file->file->Field (j);
				type = fld->Type ();

				if (type == DB_STRING) {
					int code = j * num_files + i;

					Int_Map_Itr map_itr = svalue_index.find (code);

					if (map_itr == svalue_index.end ()) {
						svalue_index.insert (Int_Map_Data (code, (int) svalue.size ()));

						token.assign ((int) fld->Size (), ' ');

						svalue.push_back (token);
					}
				}
			}
			cmd.type = type_map [type];
			cmd.token = i;
			cmd.value = j;
			return (true);
		} 

		//---- check for a variable name ----

		token_ptr = Variable (token);

		if (token_ptr != 0) {
			cmd.type = token_ptr->type;
			cmd.token = 1;
			cmd.value = token_ptr->code;
			return (true);
		}

		//---- check for a file label ----

		for (i=0; i < num_files; i++) {
			file = &file_data [i];

			if (token.Equals (file->file->File_ID ())) {
				cmd.type = DATA;
				cmd.token = i;

				if (ascii_write) {
					file->file_stat |= 2;
					ascii_write = false;
						
					if (file->read_only) {
						exe->Error (String ("File \"%s\" is Read Only") % file->file->File_ID ());
						return (false);
					}
				}
				return (true);
			}
		}

		//---- check for a special variable name ----
		
		if (token.Equals ("RECORD")) {
			cmd.type = RECORD;
			cmd.token = 0;
			return (true);
		} 

	} else if (*ch == '(' || *ch == ')' || *ch == ',' || 
		(*ch == '.' && *(ch+1) == '.')) {
	
		//---- process a parenthesis and comma ----
	
		cmd.type = LIMIT;
		if (*ch == '.') {
			cmd.token = RANGE_DOT;
			ch++;
		} else if (*ch == ',') {
			cmd.token = COMMA;
		} else {
			cmd.token = (*ch == '(') ? OPEN_PAR : CLOSE_PAR;
		}
		ch++;
		line.erase (0, ch - ch0);
			
		return (true);
		
	} else {
	
		//---- process a symbolic command ----

		do {
			if (++ch == line.end ()) break;
		} while (!((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z') ||
			(*ch >= '0' && *ch <= '9') || (*ch == '.' && *(ch+1) != '.') || 
			*ch == ' ' || *ch == '(' || *ch == ')'));

		len = (int) (ch - ch0);
		token = line.substr (0, len);
		line.erase (0, len);

		token_ptr = Symbol (token);

		if (token_ptr != 0) {
			cmd.type = token_ptr->type;
			cmd.token = token_ptr->code;
			return (true);
		}	
	}
	exe->Error (String ("Token \"%s\" was Not Recognized") % token);
	return (false);
}
