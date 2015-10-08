//*********************************************************
//	Control_Service.cpp - array of control keys
//*********************************************************

#include "Control_Service.hpp"

#include "Db_File.hpp"
#include "String.hpp"

#define MAX_LEVELS		5

//---------------------------------------------------------
//	Control_Service - constructor
//---------------------------------------------------------

Control_Service::Control_Service (void) : Report_Service ()
{
	current_key = 0;
	current_units = default_units = NO_UNITS;
}

//-----------------------------------------------------------
//	Add_Control_Key - add a key to the group
//-----------------------------------------------------------

void Control_Service::Add_Control_Key (Control_Key key) 
{
	if (key.code == 0) return;

	for (Control_Key_Itr itr = keys.begin (); itr != keys.end (); itr++) {
		if (key.key.Equals (itr->key)) return;
	}
	keys.push_back (key);
}

//---------------------------------------------------------
//	Read_Control_File
//---------------------------------------------------------

bool Control_Service::Read_Control_File (string control_file) 
{
	Db_File file;
	bool message_flag = Send_Messages ();

	//---- open the control file ----

	Send_Messages (false);

	if (!file.Open (control_file)) return (false);

	Send_Messages (message_flag);

	//---- read the control file ----

	while (file.Read ()) {
		Process_Control_String ((char *) file.Record_String ());
	}
	file.Close ();

	return (true);
}

//---------------------------------------------------------
//	Write_Control_File
//---------------------------------------------------------

void Control_Service::Write_Control_File (void) 
{
	String key, value, range;
	bool found, first_file, first_data, first_select, first_flow, first_draw, first_router, first_sim;

	FILE *file;
	Control_Key_Itr key_itr;

	key = Control_File ();

	if (key.empty ()) {
		key = Program ();
		key += ".ctl";
	}
	file = f_open (key, "wt");
	if (file == 0) return;

	first_file = first_data = first_select = first_flow = first_draw = first_router = first_sim = true;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;

		//---- check section breaks ----

		if (key_itr->code == 1) {
			fprintf (file, "\n#---- %s Control Keys ----\n\n", Program ());
		} else if (first_file && key_itr->code >= SYSTEM_FILE_OFFSET && key_itr->code < DATA_SERVICE_OFFSET) {
			first_file = false;
			fprintf (file, "\n#---- System File Keys ----\n\n");
		} else if (first_data && key_itr->code >= DATA_SERVICE_OFFSET && key_itr->code < SELECT_SERVICE_OFFSET) {
			first_data = false;
			fprintf (file, "\n#---- Data Service Keys ----\n\n");
		} else if (first_select && key_itr->code >= SELECT_SERVICE_OFFSET && key_itr->code < FLOW_TIME_SERVICE_OFFSET) {
			first_select = false;
			fprintf (file, "\n#---- Select Service Keys ----\n\n");
		} else if (first_flow && key_itr->code >= FLOW_TIME_SERVICE_OFFSET && key_itr->code < DRAW_SERVICE_OFFSET) {
			first_flow = false;
			fprintf (file, "\n#---- Flow-Time Service Keys ----\n\n");
		} else if (first_draw && key_itr->code >= DRAW_SERVICE_OFFSET && key_itr->code < ROUTER_SERVICE_OFFSET) {
			first_draw = false;
			fprintf (file, "\n#---- Draw Service Keys ----\n\n");
		} else if (first_router && key_itr->code >= ROUTER_SERVICE_OFFSET && key_itr->code < SIM_SERVICE_OFFSET) {
			first_router = false;
			fprintf (file, "\n#---- Path Building Service Keys ----\n\n");
		} else if (first_sim && key_itr->code >= SIM_SERVICE_OFFSET && key_itr->code < PROJECTION_OFFSET) {
			first_sim = false;
			fprintf (file, "\n#---- Simulation Service Keys ----\n\n");
		} else if (key_itr->code == SNAPSHOT_OUTPUT_OFFSET) {
			fprintf (file, "\n#---- Snapshot Output Keys ----\n\n");
		} else if (key_itr->code == LINK_DELAY_OUTPUT_OFFSET) {
			fprintf (file, "\n#---- Link Delay Output Keys ----\n\n");
		} else if (key_itr->code == PERFORMANCE_OUTPUT_OFFSET) {
			fprintf (file, "\n#---- Performance Output Keys ----\n\n");
		} else if (key_itr->code == TURN_VOL_OUTPUT_OFFSET) {
			fprintf (file, "\n#---- Turn Volume Output Keys ----\n\n");
		} else if (key_itr->code == RIDERSHIP_OUTPUT_OFFSET) {
			fprintf (file, "\n#---- Ridership Output Keys ----\n\n");
		} else if (key_itr->code == PROJECTION_OFFSET) {
			fprintf (file, "\n#---- Coordinate Projection Keys ----\n\n");
		} else if (key_itr->code == SMOOTH_DATA_OFFSET) {
			fprintf (file, "\n#---- Data Smoothing Keys ----\n\n");
		}

		//---- write the key ----

		key = key_itr->key;
		range = key_itr->range;

		//---- check for values ----

		if (key_itr->levels > LEVEL0) {
			found = false;

			if (key_itr->levels == LEVEL1) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					value = Get_Control_String (key_itr->code, i);
					if (value.empty ()) continue;

					key = key_itr->key;
					key += String ("_%d") % i;
					found = true;

					Print_Key (file, key, value, range);
				}
			} else if (key_itr->levels == LEVEL2) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j < num2; j++) {
						value = Get_Control_String (key_itr->code, i, j);
						if (value.empty ()) continue;

						key = key_itr->key;
						key += String ("_%d_%d") % i % j;
						found = true;

						Print_Key (file, key, value, range);
					}
				}
			} else if (key_itr->levels == LEVEL3) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j < num2; j++) {
						int num3 = Highest_Control_Group (key_itr->code, i, j, 0);

						for (int k=1; k < num3; k++) {
							value = Get_Control_String (key_itr->code, i, j, k);
							if (value.empty ()) continue;

							key = key_itr->key;
							key += String ("_%d_%d_%d") % i % j % k;
							found = true;

							Print_Key (file, key, value, range);
						}
					}
				}
			} else if (key_itr->levels == LEVEL4) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j < num2; j++) {
						int num3 = Highest_Control_Group (key_itr->code, i, j, 0);
					
						for (int k=1; k < num3; k++) {
							int num4 = Highest_Control_Group (key_itr->code, i, j, k, 0);

							for (int l=1; l < num4; l++) {
								value = Get_Control_String (key_itr->code, i, j, k, l);
								if (value.empty ()) continue;

								key = key_itr->key;
								key += String ("_%d_%d_%d_%d") % i % j % k % l;
								found = true;

								Print_Key (file, key, value, range);
							}
						}
					}
				}
			}
			if (!found) {
				for (int i=1; i <= key_itr->levels; i++) {
					key += "_1";
				}
				value = key_itr->value;

				Print_Key (file, key, value, range);
			}
		} else {
			value = Get_Control_String (key_itr->code);
			if (value.empty ()) {
				if (key_itr->key.Equals ("TITLE")) {
					value = Program ();
					value += " Default Control Keys";
				} else {
					value = key_itr->value;
				}
			}
			Print_Key (file, key, value, range);
		}
	}
	fclose (file);
}

//---------------------------------------------------------
//	Print_Key
//---------------------------------------------------------

void Control_Service::Print_Key (FILE *file, String &key, String &value, String &range)
{
	if (range.empty ()) {
		fprintf (file, "%-40s%s\n", key.c_str (), value.c_str ());
	} else if (value.size () > 20) {
		if (range.Equals (FILE_RANGE)) {
			fprintf (file, "%-40s%s\n", key.c_str (), value.c_str ());
		} else {
			fprintf (file, "%-40s%s\t//---- %s\n", key.c_str (), value.c_str (), range.c_str ());
		}
	} else {
		fprintf (file, "%-40s%-24s//---- %s\n", key.c_str (), value.c_str (), range.c_str ());
	}
}

//---------------------------------------------------------
//	Process_Control_String
//---------------------------------------------------------

void Control_Service::Process_Control_String (String control) 
{
	int i, code;
	bool found;
	String key, nest;
	Control_Key_Itr key_itr;
	Key_Value value_data;
	Key_Value_Itr value_itr;
	Key_Value_Stat value_stat;

	//---- remove comments ----

	if (control.empty ()) return;

	control.Trim (" \t");

	//---- get the key and key value ----

	size_t index = control.find_first_of (" \t=");
	if (index == string::npos) return;

	key = control.substr (0, index);

	if (key [0] == '#') return;

	control.erase (0, index);
	control.Trim_Left (" \t=");

	if (control.Equals ("NULL")) control.clear ();

	//---- find the key ----

	found = false;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;

		//---- match a standard key ----

		if (key_itr->levels == 0) {
			if (key.Equals (key_itr->key)) {
				found = true;

				value_stat = values.insert (Key_Value_Data (key_itr->code, value_data));

				value_stat.first->second.value = control;
				break;
			}

		} else if (key.Starts_With (key_itr->key)) {

			//---- match a nested key ----

			nest = key.substr (key_itr->key.length ());

			if (nest.empty ()) {
				if (key_itr->levels > 1) break;
				nest = "_1";
			} else if (nest [1] < '0' || nest [1] > '9') continue;

			Strings levels;
			nest.Parse (levels, "_");

			if ((int) levels.size () != key_itr->levels + 1) break;

			found = true;

			value_stat = values.insert (Key_Value_Data (key_itr->code, value_data));

			Key_Values *values_ptr = &(value_stat.first->second.nest);

			for (i=1; i <= key_itr->levels; i++) {
				code = levels [i].Integer ();

				value_stat = values_ptr->insert (Key_Value_Data (code, value_data));

				if (i == key_itr->levels) {
					value_stat.first->second.value = control;
				} else {
					values_ptr = &(value_stat.first->second.nest);
				}
			}
		}
	}
	
	//---- add the key to the list of unrecognized keys ----

	if (!found && !control.empty ()) {
		Str_Itr itr;
		for (itr = extra_keys.begin (); itr != extra_keys.end (); itr++) {
			if (key.Equals (*itr)) break;
		}
		if (itr == extra_keys.end ()) {
			extra_keys.push_back (key);
		}
	}
}

//-----------------------------------------------------------
//	Get_Control_String - search for a specified record 
//-----------------------------------------------------------

String Control_Service::Get_Control_String (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return ("");

	current_units = NO_UNITS;
	codes [0] = current_key = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;
	nesting.clear ();

	values_ptr = &(values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (i) nesting += String (" #%d") % code;

		if (itr->second.nest.size () == 0) {
			if (i == 1 && code == 1 && Highest_Control_Group (current_key, 0) == 1) {
				nesting.clear ();
			}
			return (itr->second.value);
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
	return ("");
}

//-----------------------------------------------------------
//	Set_Control_Flag - search for a flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Set_Control_Flag (int code, int nest1, int nest2, int nest3, int nest4)
{
	String key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		Control_Key *key_ptr = Get_Control_Key (code);
		if (key_ptr == 0 || key_ptr->value.empty ()) return (false);
		key = key_ptr->value;
	}
	return (key.Bool ());
}

//-----------------------------------------------------------
//	Get_Control_Text - search for a string parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Control_Text (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	bool flag;
	String key, default_value;

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return ("");

		key = key_ptr->value;
		flag = false;
	} else {
		flag = true;
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << key;
	}
	return (key);
}

//-----------------------------------------------------------
//	Get_Control_Flag - search for a flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_Flag (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	bool flag, value_flag;
	
	String key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (false);

		key = key_ptr->value;
		value_flag = true;
	} else {
		value_flag = false;
	}
	flag = key.Bool ();

	//---- print the key message ----

	if (!value_flag) {
		key = key_ptr->key + nesting;
		Print (1, key.To_Title ()) << ((flag) ? " = TRUE" : " = FALSE");
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Control_Integer - search for a integer parameter 
//-----------------------------------------------------------
	
int Control_Service::Get_Control_Integer (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	int value;
	bool flag;
	String key, default_value;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}
	value = key.Integer ();

	if (units != NO_UNITS) {
		value = Internal_Units (value, &units);
	}
	current_units = units;

	//---- create the external key value ----

	if (units == NO_UNITS) {
		key ("%d") % value;
	} else {
		if (key_ptr->type == INT_KEY || key_ptr->type == LIST_KEY) {
			key ("%d") % (int) External_Units (value, units);
		} else if (key_ptr->type == FLOAT_KEY) {
			key ("%.1lf") % External_Units (value, units);
		} else {
			key ("%d") % value;
		}
		key += " ";
		key += Units_Code (units);
		key.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << key;

		return (Integer_Range_Check (value, key_ptr->range));
	}
	return (value);
}

//-----------------------------------------------------------
//	Get_Control_Double - search for a double parameter 
//-----------------------------------------------------------
	
double Control_Service::Get_Control_Double (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	double value;
	bool flag;
	String key, default_value;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0.0);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}
	value = key.Double ();

	if (units != NO_UNITS) {
		value = Internal_Units (value, &units);
	}
	current_units = units;

	//---- create the external key value ----

	if (units == NO_UNITS) {
		key ("%g") % value;
	} else {
		if (key_ptr->type == INT_KEY) {
			key ("%d") % (int) External_Units (value, units);
		} else if (key_ptr->type == FLOAT_KEY || key_ptr->type == LIST_KEY) {
			key ("%.2lf") % External_Units (value, units);
		} else {
			key ("%g") % value;
		}
		key += " ";
		key += Units_Code (units);
		key.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << key;

		return (Double_Range_Check (value, key_ptr->range));
	}
	return (value);
}

//-----------------------------------------------------------
//	Get_Control_Time - search for a time parameter 
//-----------------------------------------------------------
	
Dtime Control_Service::Get_Control_Time (int code, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	Dtime value;
	bool flag;
	String key, default_value;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (value);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}
	value.Time_String (key, units);

	//---- create the external key value ----

	if (units == NO_UNITS) {
		units = Time_Format ();
	}
	current_units = units;

	key = value.Time_String (units);

	if (units == SECONDS || units == MINUTES || units == HOURS) {
		key += " ";
		key += Units_Code (units);
		key.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << key;

		return (Time_Range_Check (value, key_ptr->range));
	}
	return (value);
}

//-----------------------------------------------------------
//	Get_Control_List - search for a integer list parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List (int code, Integer_List &list, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	int value;
	bool flag;
	String key, default_value, text, message, txt;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- initialize the list ----

	list.clear ();
	value = default_value.Integer ();

	if (default_units != NO_UNITS) {
		value = Internal_Units (value, default_units);
	}
	list.push_back (value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}

	//---- process each value ----

	while (key.Split (text)) {
		value = text.Integer ();

		current_units = units;

		if (current_units != NO_UNITS) {
			value = Internal_Units (value, &current_units);
		}
		list.push_back (value);

		if (list.size () > 2) {
			message += ", ";
		} else if (list.size () == 2) {
			list [0] = value;
		}
		if (current_units == NO_UNITS) {
			txt ("%d") % value;
		} else {
			if (key_ptr->type == INT_KEY || key_ptr->type == LIST_KEY) {
				txt ("%d") % (int) External_Units (value, current_units);
			} else if (key_ptr->type == FLOAT_KEY) {
				txt ("%.1lf") % External_Units (value, current_units);
			} else {
				txt ("%d") % value;
			}
		}
		message += txt;
	}

	//---- create the external key value ----

	if (current_units != NO_UNITS) {
		message += " ";
		message += Units_Code (current_units);
		message.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << message;

		return (List_Range_Check (list, key_ptr->range));
	}
	return (list.size () > 1);
}

//-----------------------------------------------------------
//	Get_Control_List - search for a double list parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List (int code, Double_List &list, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	double value;
	bool flag;
	String key, default_value, text, message, txt;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- initialize the list ----

	list.clear ();
	value = default_value.Double ();

	if (default_units != NO_UNITS) {
		value = Internal_Units (value, default_units);
	}
	list.push_back (value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}

	//---- process each value ----

	while (key.Split (text)) {
		value = text.Double ();

		current_units = units;

		if (current_units != NO_UNITS) {
			value = Internal_Units (value, &current_units);
		}
		list.push_back (value);

		if (list.size () > 2) {
			message += ", ";
		} else if (list.size () == 2) {
			list [0] = value;
		}
		if (current_units == NO_UNITS) {
			txt ("%g") % value;
		} else {
			if (key_ptr->type == INT_KEY) {
				txt ("%d") % (int) External_Units (value, current_units);
			} else if (key_ptr->type == FLOAT_KEY || key_ptr->type == LIST_KEY) {
				txt ("%.2lf") % External_Units (value, current_units);
			} else {
				txt ("%g") % value;
			}
		}
		message += txt;
	}

	//---- create the external key value ----

	if (current_units != NO_UNITS) {
		message += " ";
		message += Units_Code (current_units);
		message.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << message;

		return (List_Range_Check (list, key_ptr->range));
	}
	return (list.size () > 1);
}

//-----------------------------------------------------------
//	Get_Control_List - search for a Dtime list parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List (int code, Dtime_List &list, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	Dtime value;
	bool flag;
	String key, default_value, text, message, txt;
	Units_Type units;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);

	//---- initialize the list ----

	list.clear ();
	value.Time_String (default_value, default_units);

	list.push_back (value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
		units = default_units;
	} else {
		flag = true;
		units = Parse_Units (key);
		if (units == NO_UNITS) {
			units = default_units;
		}
	}

	//---- process each value ----

	while (key.Split (text)) {
		value.Time_String (text, units);

		list.push_back (value);

		if (list.size () > 2) {
			message += ", ";
		} else if (list.size () == 2) {
			list [0] = value;
		}
		message += value.Time_String (units);
	}

	//---- create the external key value ----

	if (units != NO_UNITS) {
		message += " ";
		message += Units_Code (units);
		message.To_Lower ();
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << message;

		return (List_Range_Check (list, key_ptr->range));
	}
	return (list.size () > 1);
}

//-----------------------------------------------------------
//	Get_Control_List - search for a bool list parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List (int code, Bool_List &list, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	bool value;
	bool flag;
	String key, default_value, text, message;

	default_value = key_ptr->value;
	default_units = Parse_Units (default_value);
	current_units = NO_UNITS;

	//---- initialize the list ----

	list.clear ();
	value = default_value.Bool ();
	list.push_back (value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
	} else {
		flag = true;
	}

	//---- process each value ----

	while (key.Split (text)) {
		value = text.Bool ();
		list.push_back (value);

		if (list.size () > 2) {
			message += ", ";
		} else if (list.size () == 2) {
			list [0] = value;
		}
		message += text;
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << message;
	}
	return (list.size () > 1);
}

//-----------------------------------------------------------
//	Get_Control_List - search for a string list parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List (int code, String_List &list, int nest1, int nest2, int nest3, int nest4)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	bool flag;
	String key, default_value, text, message;

	default_value = key_ptr->value;
	default_units = current_units = NO_UNITS;

	//---- initialize the list ----

	list.clear ();
	list.push_back (default_value);

	//---- convert to the internal key value ----

	key = Get_Control_String (code, nest1, nest2, nest3, nest4);

	if (key.empty ()) {
		if (key_ptr->value.empty ()) return (0);
		flag = false;
		key = default_value;
	} else {
		flag = true;
	}

	//---- process each value ----

	while (key.Split (text)) {
		list.push_back (text);

		if (list.size () > 2) {
			message += ", ";
		} else if (list.size () == 2) {
			list [0] = text;
		}
		message += text;
	}

	//---- print the key message ----

	if (flag) {
		default_value = key_ptr->key + nesting;
		Print (1, default_value.To_Title ()) << " = " << message;
	}
	return (list.size () > 1);
}

//-----------------------------------------------------------
//	Get_Control_List_Groups - search for integer list parameters 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List_Groups (int code, Integer_List &list, bool round_flag)
{
	bool flag = false;
	int num = Highest_Control_Group (code, 0);

	if (num > 0) {
		list.clear ();

		for (int i=1; i <= num; i++) {
			if (!Check_Control_Key (code, i)) continue;
			Integer_List temp;

			Get_Control_List (code, temp, i);

			if (list.Combine (temp, i, round_flag)) flag = true;
		}
		if (num > 1) Print (1);
	} else {
		Get_Control_List (code, list, 1);
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Control_List_Groups - search for double list parameters 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List_Groups (int code, Double_List &list, bool scale_flag)
{
	bool flag = false;
	int num = Highest_Control_Group (code, 0);

	if (num > 0) {
		list.clear ();

		for (int i=1; i <= num; i++) {
			if (!Check_Control_Key (code, i)) continue;
			Double_List temp;

			Get_Control_List (code, temp, i);

			if (list.Combine (temp, i, scale_flag)) flag = true;
		}
		if (num > 1) Print (1);
	} else {
		Get_Control_List (code, list, 1);
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Control_List_Groups - search for Dtime list parameters 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List_Groups (int code, Dtime_List &list, bool scale_flag)
{
	bool flag = false;
	int num = Highest_Control_Group (code, 0);

	if (num > 0) {
		list.clear ();

		for (int i=1; i <= num; i++) {
			if (!Check_Control_Key (code, i)) continue;
			Dtime_List temp;

			Get_Control_List (code, temp, i);

			if (list.Combine (temp, i, scale_flag)) flag = true;
		}
		if (num > 1) Print (1);
	} else {
		Get_Control_List (code, list, 1);
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Control_List_Groups - search for String list parameters 
//-----------------------------------------------------------
	
bool Control_Service::Get_Control_List_Groups (int code, String_List &list)
{
	bool flag = false;
	int num = Highest_Control_Group (code, 0);

	if (num > 0) {
		list.clear ();

		for (int i=1; i <= num; i++) {
			if (!Check_Control_Key (code, i)) continue;
			String_List temp;

			Get_Control_List (code, temp, i);

			if (list.Combine (temp, i)) flag = true;
		}
		if (num > 1) Print (1);
	} else {
		Get_Control_List (code, list, 1);
	}
	return (flag);
}

//-----------------------------------------------------------
//	Get_Default_Text - get the default text parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Default_Text (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	return (key_ptr->value);
}

//-----------------------------------------------------------
//	Get_Default_Flag - get the default flag parameter 
//-----------------------------------------------------------
	
bool Control_Service::Get_Default_Flag (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	return (key_ptr->value.Bool ());
}

//-----------------------------------------------------------
//	Get_Default_Integer - get the default integer parameter 
//-----------------------------------------------------------
	
int Control_Service::Get_Default_Integer (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	String key = key_ptr->value;
	Units_Type units = Parse_Units (key);

	if (units == NO_UNITS) {
		return (key.Integer ());
	} else {
		return (Internal_Units (key.Integer (), units));
	}
}

//-----------------------------------------------------------
//	Get_Default_Double - get the default double parameter 
//-----------------------------------------------------------
	
double Control_Service::Get_Default_Double (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	String key = key_ptr->value;
	Units_Type units = Parse_Units (key);

	if (units == NO_UNITS) {
		return (key.Double ());
	} else {
		return (Internal_Units (key.Double (), units));
	}
}

//-----------------------------------------------------------
//	Get_Range_String - get the range string parameter 
//-----------------------------------------------------------
	
String Control_Service::Get_Range_String (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return ("");

	return (key_ptr->range);
}

//-----------------------------------------------------------
//	Get_Default_Time - get the default time parameter 
//-----------------------------------------------------------
	
Dtime Control_Service::Get_Default_Time (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (0);

	String key = key_ptr->value;
	Units_Type units = Parse_Units (key);

	return (Dtime (key, units));
}

//-----------------------------------------------------------
//	Integer_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
int Control_Service::Integer_Range_Check (int value, String range)
{
	Parse_Units (range);
	if (range.empty ()) return (value);

	int low, high;
	double d1, d2;
	Strings ranges;
	Str_Itr itr;
	String number;

	//---- unpack the range string ----	

	range.Parse (ranges, COMMA_DELIMITERS);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			low = DTOI (Internal_Units (number.Double (), default_units));
			if (value >= low) return (value);
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			low = DTOI (Internal_Units (number.Double (), default_units));
			if (value > low) return (value);
		} else {
			if (!itr->Range (d1, d2)) continue;

			low = DTOI (Internal_Units (d1, default_units));
			high = DTOI (Internal_Units (d2, default_units));

			if (value >= low && value <= high) return (value);
		}
	}

	//---- write the error message ----

	Control_Key *key_ptr = Get_Control_Key (current_key);
	if (key_ptr == 0) return (value);

	int i;
	String message, key;
	
	key = key_ptr->key;
	key.To_Title ();
	key += " = ";

	//---- create the external key value ----

	if (current_units == NO_UNITS) {
		message ("%d") % value;
	} else {
		if (key_ptr->type == INT_KEY || key_ptr->type == LIST_KEY) {
			message ("%d") % (int) External_Units (value, current_units);
		} else if (key_ptr->type == FLOAT_KEY) {
			message ("%.2lf") % External_Units (value, current_units);
		} else {
			message ("%d") % value;
		}
		message += " ";
		message += Units_Code (current_units);
		message.To_Lower ();
	}
	message += " is Out of Range (";

	//---- external range values ----

	for (i=0, itr = ranges.begin (); itr != ranges.end (); itr++, i++) {
		if (i > 0) message += ", ";

		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			message += ">= ";
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			message += "> ";
		} else {
			number = *itr;
		}
		if (!number.Range (d1, d2)) continue;

		low = DTOI (Internal_Units (d1, default_units));
		high = DTOI (Internal_Units (d2, default_units));

		if (current_units == NO_UNITS) {
			if (low != high) message += String ("%d..") % low;
			message += String ("%d") % high;
		} else {
			if (key_ptr->type == INT_KEY || key_ptr->type == LIST_KEY) {
				if (low != high) message += String ("%d..") % (int) External_Units (low, current_units);
				message += String ("%d") % (int) External_Units (high, current_units);
			} else if (key_ptr->type == FLOAT_KEY) {
				if (low != high) message += String ("%g..") % External_Units (low, current_units);
				message += String ("%g") % External_Units (high, current_units);
			} else {
				if (low != high) message += String ("%d..") % low;
				message += String ("%d") % high;
			}
		}
	}
	Error (key + message + ")");

	return (value);
}

//-----------------------------------------------------------
//	Double_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
double Control_Service::Double_Range_Check (double value, String range)
{
	Parse_Units (range);
	if (range.empty ()) return (value);

	double low, high;
	Strings ranges;
	Str_Itr itr;
	String number;

	//---- unpack the range string ----	

	range.Parse (ranges, COMMA_DELIMITERS);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			low = Internal_Units (number.Double (), default_units);
			if (value >= low) return (value);
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			low = Internal_Units (number.Double (), default_units);
			if (value > low) return (value);
		} else {
			if (!itr->Range (low, high)) continue;

			low = Internal_Units (low, default_units);
			high = Internal_Units (high, default_units);

			if (value >= low && value <= high) return (value);
		}
	}

	//---- write the error message ----

	Control_Key *key_ptr = Get_Control_Key (current_key);
	if (key_ptr == 0) return (value);

	int i;
	String message, key;
	
	key = key_ptr->key;
	key.To_Title ();
	key += " = ";

	//---- create the external key value ----

	if (current_units == NO_UNITS) {
		message ("%g") % value;
	} else {
		if (key_ptr->type == INT_KEY) {
			message ("%d") % (int) External_Units (value, current_units);
		} else if (key_ptr->type == FLOAT_KEY) {
			message ("%g") % External_Units (value, current_units);
		} else {
			message ("%g") % value;
		}
		message += " ";
		message += Units_Code (current_units);
		message.To_Lower ();
	}
	message += " is Out of Range (";

	//---- external range values ----

	for (i=0, itr = ranges.begin (); itr != ranges.end (); itr++, i++) {
		if (i > 0) message += ", ";

		if (itr->Starts_With (">=")) {
			number = itr->substr (2);
			message += ">= ";
		} else if (itr->Starts_With (">")) {
			number = itr->substr (1);
			message += "> ";
		} else {
			number = *itr;
		}
		if (!number.Range (low, high)) continue;

		low = Internal_Units (low, default_units);
		high = Internal_Units (high, default_units);

		if (current_units == NO_UNITS) {
			if (low != high) message += String ("%g..") % low;
			message += String ("%g") % high;
		} else {
			if (key_ptr->type == INT_KEY || key_ptr->type == LIST_KEY) {
				if (low != high) message += String ("%d..") % (int) External_Units (low, current_units);
				message += String ("%d") % (int) External_Units (high, current_units);
			} else if (key_ptr->type == FLOAT_KEY) {
				if (low != high) message += String ("%.1lf..") % External_Units (low, current_units);
				message += String ("%.1lf") % External_Units (high, current_units);
			} else {
				if (low != high) message += String ("%g..") % low;
				message += String ("%g") % high;
			}
		}
	}
	Error (key + message + ")");

	return (value);
}

//-----------------------------------------------------------
//	Time_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
Dtime Control_Service::Time_Range_Check (Dtime value, String range)
{
	Parse_Units (range);
	if (range.empty ()) return (value);

	Dtime low, high;
	double d1, d2;
	Strings ranges;
	Str_Itr itr;

	//---- unpack the range string ----	

	range.Parse (ranges, COMMA_DELIMITERS);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (!itr->Range (d1, d2)) continue;

		if (default_units == NO_UNITS) {
			low.Hours (d1);
			high.Hours (d2);
		} else {
			low = Dtime (d1, default_units);
			high = Dtime (d2, default_units);
		}
		if (value >= low && value <= high) return (value);
	}

	//---- write the error message ----

	Control_Key *key_ptr = Get_Control_Key (current_key);
	if (key_ptr == 0) return (value);

	int i;
	String message, key;
	
	key = key_ptr->key;
	key.To_Title ();
	key += " = ";

	//---- create the external key value ----

	message = value.Time_String (current_units);

	if (current_units == SECONDS || current_units == MINUTES || current_units == HOURS) {
		message += " ";
		message += Units_Code (current_units);
		message.To_Lower ();
	}
	message += " is Out of Range (";

	//---- external range values ----

	for (i=0, itr = ranges.begin (); itr != ranges.end (); itr++, i++) {
		if (!itr->Range (d1, d2)) continue;

		if (default_units == NO_UNITS) {
			low.Hours (d1);
			high.Hours (d2);
		} else {
			low = Dtime (d1, default_units);
			high = Dtime (d2, default_units);
		}		
		if (i > 0) message += ", ";

		if (low != high) {
			message += low.Time_String (current_units);
			message += "..";
		}
		message += high.Time_String (current_units);
	}
	if (current_units == SECONDS || current_units == MINUTES || current_units == HOURS) {
		String temp = Units_Code (current_units);
		message += " ";
		message += temp.To_Lower ();
	}
	Error (key + message + ")");

	return (value);
}

//-----------------------------------------------------------
//	List_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
bool Control_Service::List_Range_Check (Integer_List &list, String range)
{
	Int_Itr itr;

	for (itr = list.begin (); itr != list.end (); itr++) {
		Integer_Range_Check (*itr, range);
	}
	return ((list.size () > 1));
}

//-----------------------------------------------------------
//	List_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
bool Control_Service::List_Range_Check (Double_List &list, String range)
{
	Dbl_Itr itr;

	for (itr = list.begin (); itr != list.end (); itr++) {
		Double_Range_Check (*itr, range);
	}
	return ((list.size () > 1));
}

//-----------------------------------------------------------
//	List_Range_Check - check/write control key range error
//-----------------------------------------------------------
	
bool Control_Service::List_Range_Check (Dtime_List &list, String range)
{
	Dtime_Itr itr;

	for (itr = list.begin (); itr != list.end (); itr++) {
		Time_Range_Check (*itr, range);
	}
	return ((list.size () > 1));
}

//-----------------------------------------------------------
//	Highest_Control_Group - the highest group number 
//-----------------------------------------------------------

int Control_Service::Highest_Control_Group (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, highest, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Value_Stat stat;
	Key_Values *values_ptr;

	if (code == 0) return (0);

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(values);
	highest = 0;

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) {
			if (values_ptr->size () > 0) {
				itr = values_ptr->end ();
				highest = (--itr)->first;
			}
			break;
		}
		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;
		
		values_ptr = &(itr->second.nest);
	}
	return (highest);
}

//-----------------------------------------------------------
//	Number_Control_Group - the number of records in a group 
//-----------------------------------------------------------

int Control_Service::Number_Control_Group (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, count, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return (0);

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(values);
	count = 0;

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) {
			count = 0;
			break;
		}
		values_ptr = &(itr->second.nest);
		count = (int) (values_ptr->size ());
	}
	return (count);
}

//-----------------------------------------------------------
//	Check_Control_Key - search for a key code 
//-----------------------------------------------------------
	
bool Control_Service::Check_Control_Key (int code, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;

	if (code == 0) return (false);

	current_units = NO_UNITS;
	codes [0] = current_key = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (itr->second.nest.size () == 0) {
			return (true);
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
	return (false);
}

//-----------------------------------------------------------
//	Control_Key_Error - write a control key error message 
//-----------------------------------------------------------
	
void Control_Service::Control_Key_Error (int code, string range, int nest1, int nest2, int nest3, int nest4)
{
	int i, codes [MAX_LEVELS];
	Key_Value_Itr itr;
	Key_Values *values_ptr;
	String message;

	if (code == 0) return;

	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return;

	message = key_ptr->key;
	message.To_Title ();

	codes [0] = code;
	codes [1] = nest1;
	codes [2] = nest2;
	codes [3] = nest3;
	codes [4] = nest4;

	values_ptr = &(values);

	for (i=0; i < MAX_LEVELS; i++) {
		code = codes [i];
		if (code == 0) break;

		itr = values_ptr->find (code);

		if (itr == values_ptr->end ()) break;

		if (i) {
			message += String ("_%d") % code;
		}
		if (itr->second.nest.size () == 0) {
			message += " = " + itr->second.value + " is Out of Range " + range;
			Error (message);
			return;
		} else {
			values_ptr = &(itr->second.nest);
		}
	}
}

//-----------------------------------------------------------
//	Mode_Range_Key - process a mode range control key 
//-----------------------------------------------------------
	
bool Control_Service::Mode_Range_Key (int code, bool *modes, int nest1, int nest2, int nest3, int nest4)
{
	int i;
	bool flag = false;

	if (Control_Key_Status (code)) {
		String key = Get_Control_Text (code, nest1, nest2, nest3, nest4);

		if (!key.empty () && !key.Equals ("ALL")) {
			memset (modes, '\0', MAX_MODE * sizeof (bool));
			int low, high;
			String item, low_text, high_text;

			while (!key.empty ()) {
				key.Split (item);
				item.Range (low_text, high_text);

				low = Mode_Code (low_text);
				high = Mode_Code (high_text);

				if (low < 1 || low >= MAX_MODE) {
					Error (String ("Specified Mode %s is Out of Range (1..%d)") % low_text % (MAX_MODE-1));
				}
				if (high < 1 || high >= MAX_MODE) {
					Error (String ("Specified Mode %s is Out of Range (1..%d)") % high_text % (MAX_MODE-1));
				}
				if (high < low) {
					Error (String ("Mode Range %s is Out of Order") % item);
				}
				flag = true;
				if (low == high) {
					modes [low] = true;

					if (low_text [0] >= '0' && low_text [0] <= '9') {
						Print (0, String (" (%s)") % Mode_Code ((Mode_Type) low));
					}
				} else {
					for (i=low; i <= high; i++) {
						modes [i] = true;
					}
					if (low_text [0] >= '0' && low_text [0] <= '9') {
						Print (0, String (" (%s..%s)") % Mode_Code ((Mode_Type) low) % Mode_Code ((Mode_Type) high));
					}
				}
			}
			for (i=1; i < MAX_MODE; i++) {
				if (!modes [i]) {
					flag = true;
					break;
				}
			}
		} else {
			for (i=0; i < MAX_MODE; i++) {
				modes [i] = true;
			}
		}	
	} else {
		for (i=0; i < MAX_MODE; i++) {
			modes [i] = true;
		}
	}
	return (flag);
}

//-----------------------------------------------------------
//	Event_Range_Key - process a event range control key 
//-----------------------------------------------------------
	
bool Control_Service::Event_Range_Key (int code, bool *types, int nest1, int nest2, int nest3, int nest4)
{
	int i;
	bool flag = false;

	if (Control_Key_Status (code)) {
		String key = Get_Control_Text (code, nest1, nest2, nest3, nest4);

		if (!key.empty () && !key.Equals ("ALL")) {
			memset (types, '\0', MAX_EVENT * sizeof (bool));
			int low, high;
			String item, low_text, high_text;

			while (!key.empty ()) {
				key.Split (item);
				item.Range (low_text, high_text);

				low = Event_Code (low_text);
				high = Event_Code (high_text);

				if (low < 0 || low >= MAX_EVENT) {
					Error (String ("Specified Event %s is Out of Range (1..%d)") % low_text % (MAX_EVENT-1));
				}
				if (high < 0 || high >= MAX_EVENT) {
					Error (String ("Specified Event %s is Out of Range (1..%d)") % high_text % (MAX_EVENT-1));
				}
				if (high < low) {
					Error (String ("Event Range %s is Out of Order") % item);
				}
				flag = true;
				if (low == high) {
					types [low] = true;

					if (low_text [0] >= '0' && low_text [0] <= '9') {
						Print (0, String (" (%s)") % Event_Code ((Event_Type) low));
					}
				} else {
					for (i=low; i <= high; i++) {
						types [i] = true;
					}
					if (low_text [0] >= '0' && low_text [0] <= '9') {
						Print (0, String (" (%s..%s)") % Event_Code ((Event_Type) low) % Event_Code ((Event_Type) high));
					}
				}
			}
			for (i=1; i < MAX_EVENT; i++) {
				if (!types [i]) {
					flag = true;
					break;
				}
			}
		} else {
			for (i=0; i < MAX_EVENT; i++) {
				types [i] = true;
			}
		}	
	} else {
		for (i=0; i < MAX_EVENT; i++) {
			types [i] = true;
		}
	}
	return (flag);
}

//-----------------------------------------------------------
//	Output_Control_Label - construct a key for printing 
//-----------------------------------------------------------
	
Print_Stream & Control_Service::Output_Control_Label (int code)
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (Print (0));

	//---- print the label message ----

	String label = key_ptr->key;
	return (Print (1, label.To_Title ()) << " = ");
}

//-----------------------------------------------------------
//	Current_Key - return the current key text 
//-----------------------------------------------------------

String Control_Service::Current_Key (void) 
{
	Control_Key_Itr key_itr;
	String key;

	if (current_key == 0) return (key);

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == current_key) {
			key = key_itr->key;

			for (int i=1; i <= key_itr->levels; i++) {
				key += "_#";
			}
			break;
		}
	}
	return (key);
}

//-----------------------------------------------------------
//	Current_Label - return the current label text 
//-----------------------------------------------------------

String Control_Service::Current_Label (void) 
{
	if (current_key == 0) return ("");

	Control_Key *key_ptr = Get_Control_Key (current_key);
	if (key_ptr == 0) return ("");

	String label = key_ptr->key + nesting;
	label.To_Title ();

	return (label);
}

//-----------------------------------------------------------
//	Show_Program_Keys - show the program control keys
//-----------------------------------------------------------

void Control_Service::Show_Program_Keys (void) 
{
	Control_Key_Itr key_itr;
	string key;

	cout << "\n\n\tControl File Keys:";

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		key = key_itr->key;

		for (int i=1; i <= key_itr->levels; i++) {
			key += "_#";
		}
		cout << String ("\n\t  %-32.32s") % key;

		if (key_itr->option == REQ_KEY) {
			cout << "Req.";
		} else {
			cout << "Opt.";
		}
		switch (key_itr->type) {
			case INT_KEY:
				cout << "Int.";
				break;
			case FLOAT_KEY:
				cout << "Dec.";
				break;
			case BOOL_KEY:
				cout << "Bool";
				break;
			case IN_KEY:
				cout << "File";
				break;
			case OUT_KEY:
				cout << "New ";
				break;
			case PATH_KEY:
				cout << "Path";
				break;
			case TIME_KEY:
				cout << "Time";
				break;
			case LIST_KEY:
				cout << "List";
				break;
			default:
				cout << "Text";
				break;
		}
		if (!key_itr->value.empty ()) {
			cout << " = " << key_itr->value;
		}
	}
	cout << flush;
}

//-----------------------------------------------------------
//	Show_Extra_Keys - show the extra control keys
//-----------------------------------------------------------

void Control_Service::Show_Extra_Keys (void) 
{
	if (extra_keys.size () == 0) return;

	Print (1);
	for (Str_Itr itr = extra_keys.begin (); itr != extra_keys.end (); itr++) {
		Warning ("Control Key \"") << *itr << "\" was Not Used";
	}
	Show_Message (1);
}

//-----------------------------------------------------------
//	Check_Keys - check for required keys
//-----------------------------------------------------------

bool Control_Service::Check_Keys (void) 
{
	Control_Key_Itr key_itr;
	Key_Value_Itr itr;
	String key;
	bool first = true;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == REQ_KEY) {
			itr = values.find (key_itr->code);

			if (itr != values.end ()) continue;

			key = key_itr->key;

			for (int i=1; i <= key_itr->levels; i++) {
				key += "_#";
			}
			if (first) {
				first = false;
				Write (2, "Missing Control Keys:");
			}
			Write (1, "\t") << key;
		}
	}
	if (!first) {
		Flush ();
	}
	return (first);
}

//-----------------------------------------------------------
//	Required_Control_Key
//-----------------------------------------------------------

bool Control_Service::Required_Control_Key (int code) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);

	return (key_ptr->option == REQ_KEY);
}

bool Control_Service::Required_Control_Key (int code, bool flag) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);
	key_ptr->option = (flag) ? REQ_KEY : OPT_KEY;
	return (flag);
}

//-----------------------------------------------------------
//	Ignore_Control_Key
//-----------------------------------------------------------

bool Control_Service::Ignore_Control_Key (int code) 
{
	Control_Key *key_ptr = Get_Control_Key (code);
	if (key_ptr == 0) return (false);
	key_ptr->option = IGNORE_KEY;
	return (true);
}

//-----------------------------------------------------------
//	Get_Control_Key - return the key data 
//-----------------------------------------------------------
	
Control_Key * Control_Service::Get_Control_Key (int code)
{
	Control_Key_Itr key_itr;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == code) {
			return (&(*key_itr));
		}
	}
	return (0);
}

//-----------------------------------------------------------
//	Control_Key_Status - true if a key is currently active
//-----------------------------------------------------------
	
bool Control_Service::Control_Key_Status (int code)
{
	Control_Key_Itr key_itr;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		if (key_itr->code == code) {
			return (true);
		}
	}
	return (false);
}

//-----------------------------------------------------------
//	XML_Controls - write the control keys to the xml file
//-----------------------------------------------------------

void Control_Service::XML_Controls (void) 
{
	Control_Key_Itr key_itr;
	string key, value;

	if (!XML_Flag ()) return;

	XML (1, "<CONTROL_KEYS>");

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;
		key = key_itr->key;

		for (int i=1; i <= key_itr->levels; i++) {
			key += "_#";
		}
		XML (1, String ("<KEY CODE=\"%d\" ") % key_itr->code);
		XML (0, String ("NAME=\"%s\" ") % key);

		key = (key_itr->option == REQ_KEY) ? "true" : "false";

		XML (0, String ("REQUIRED=\"%s\" ") % key);

		switch (key_itr->type) {
			case INT_KEY:
				key = "Integer";
				break;
			case FLOAT_KEY:
				key = "Decimal";
				break;
			case BOOL_KEY:
				key = "Bool";
				break;
			case IN_KEY:
				if (key_itr->code >= SYSTEM_FILE_OFFSET && key_itr->code < (SYSTEM_FILE_OFFSET + SELECTION)) {
					key = "Net";
				} else {
					key = "File";
				}
				break;
			case OUT_KEY:
				if (key_itr->code >= (SYSTEM_FILE_OFFSET + NEW_NODE) && key_itr->code < (SYSTEM_FILE_OFFSET + NEW_SELECTION)) {
					key = "NewNet";
				} else {
					key = "NewFile";
				}
				break;
			case PATH_KEY:
				key = "Path";
				break;
			case TIME_KEY:
				key = "Time";
				break;
			case LIST_KEY:
				key = "List";
				break;
			default:
				key = "Text";
				break;
		}
		XML (0, String ("TYPE=\"%s\" ") % key);

		if (!key_itr->value.empty ()) {
			XML (0, String ("DEFAULT=\"%s\" ") % key_itr->value);
		}
		if (!key_itr->range.empty ()) {
			XML (0, String ("RANGE=\"%s\" ") % key_itr->range);
		}
		if (key_itr->help != NO_HELP) {
			XML (0, String ("HELP=\"%d\" ") % key_itr->help);

			help_set.insert (key_itr->help);
		}

		//----- check for values ----

		if (key_itr->levels > 0) {
			bool flag = false;

			if (key_itr->levels == 1) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					value = Get_Control_String (key_itr->code, i);
					if (value.empty ()) continue;

					if (!flag) {
						XML (0, ">");
						XML (1, "<LEVEL_KEYS>");
						flag = true;
					}
					key = key_itr->key;
					key += String ("_%d") % i;
					XML (1, String ("<LEVEL NAME=\"%s\" ") % key);
					XML (0, String ("VALUE=\"%s\" />") % value);
				}
			} else if (key_itr->levels == 2) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j < num2; j++) {
						value = Get_Control_String (key_itr->code, i, j);
						if (value.empty ()) continue;

						if (!flag) {
							XML (0, ">");
							XML (1, "<LEVEL_KEYS>");
							flag = true;
						}
						key = key_itr->key;
						key += String ("_%d_%d") % i % j;
						XML (1, String ("<LEVEL NAME=\"%s\" ") % key);
						XML (0, String ("VALUE=\"%s\" />") % value);
					}
				}
			} else if (key_itr->levels == 3) {
				int num = Highest_Control_Group (key_itr->code, 0);

				for (int i=1; i <= num; i++) {
					int num2 = Highest_Control_Group (key_itr->code, i, 0);

					for (int j=1; j < num2; j++) {
						int num3 = Highest_Control_Group (key_itr->code, i, j, 0);

						for (int k=1; k < num3; k++) {
							value = Get_Control_String (key_itr->code, i, j, k);
							if (value.empty ()) continue;

							if (!flag) {
								XML (0, ">");
								XML (1, "<LEVEL_KEYS>");
								flag = true;
							}
							key = key_itr->key;
							key += String ("_%d_%d_%d") % i % j % k;
							XML (1, String ("<LEVEL NAME=\"%s\" ") % key);
							XML (0, String ("VALUE=\"%s\" />") % value);
						}
					}
				}
			}
			if (flag) {
				XML (1, "</LEVEL_KEYS>");
				XML (1, "</KEY>");
			} else {
				XML (0, "/>");
			}
		} else {
			value = Get_Control_String (key_itr->code);
			if (!value.empty ()) {
				XML (0, String ("VALUE=\"%s\" ") % value);
			}
			XML (0, "/>");
		}
	}
	XML (1, "</CONTROL_KEYS>");
}

//-----------------------------------------------------------
//	Document_Controls - write the program control keys
//-----------------------------------------------------------

void Control_Service::Document_Controls (FILE *file, bool flag) 
{
	Control_Key_Itr key_itr;
	String key, value, range;
	bool first_exe, first_file, first_data, first_select, first_flow, first_draw, first_router, first_sim;

	first_exe = first_file = first_data = first_select = first_flow = first_draw = first_router = first_sim = true;

	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		if (key_itr->option == IGNORE_KEY) continue;

		//---- check section breaks ----

		if (key_itr->code == 1) {
			fprintf (file, "\n\n%s Control Keys:", Program ());
		} else if (first_exe && key_itr->code >= EXECUTION_OFFSET && key_itr->code < SYSTEM_FILE_OFFSET) {
			first_exe = false;
			fprintf (file, "\n\nExecution Service Keys:");
		} else if (first_file && key_itr->code >= SYSTEM_FILE_OFFSET && key_itr->code < DATA_SERVICE_OFFSET) {
			first_file = false;
			fprintf (file, "\n\nSystem File Keys:");
		} else if (first_data && key_itr->code >= DATA_SERVICE_OFFSET && key_itr->code < SELECT_SERVICE_OFFSET) {
			first_data = false;
			fprintf (file, "\n\nData Service Keys:");
		} else if (first_select && key_itr->code >= SELECT_SERVICE_OFFSET && key_itr->code < FLOW_TIME_SERVICE_OFFSET) {
			first_select = false;
			fprintf (file, "\n\nSelect Service Keys:");
		} else if (first_flow && key_itr->code >= FLOW_TIME_SERVICE_OFFSET && key_itr->code < DRAW_SERVICE_OFFSET) {
			first_flow = false;
			fprintf (file, "\n\nFlow-Time Service Keys:");
		} else if (first_draw && key_itr->code >= DRAW_SERVICE_OFFSET && key_itr->code < ROUTER_SERVICE_OFFSET) {
			first_draw = false;
			fprintf (file, "\n\nDraw Service Keys:");
		} else if (first_router && key_itr->code >= ROUTER_SERVICE_OFFSET && key_itr->code < SIM_SERVICE_OFFSET) {
			first_router = false;
			fprintf (file, "\n\nPath Building Service Keys:");
		} else if (first_sim && key_itr->code >= SIM_SERVICE_OFFSET) {
			first_sim = false;
			fprintf (file, "\n\nSimulation Service Keys:");
		} else if (key_itr->code == SNAPSHOT_OUTPUT_OFFSET) {
			fprintf (file, "\n\nSnapshot Output Keys:");
		} else if (key_itr->code == LINK_DELAY_OUTPUT_OFFSET) {
			fprintf (file, "\n\nLink Delay Output Keys:");
		} else if (key_itr->code == PERFORMANCE_OUTPUT_OFFSET) {
			fprintf (file, "\n\nPerformance Output Keys:");
		} else if (key_itr->code == TURN_VOL_OUTPUT_OFFSET) {
			fprintf (file, "\n\nTurn Volume Output Keys:");
		} else if (key_itr->code == RIDERSHIP_OUTPUT_OFFSET) {
			fprintf (file, "\n\nRidership Output Keys:");
		} else if (key_itr->code == PROJECTION_OFFSET) {
			fprintf (file, "\n\nCoordinate Projection Keys:");
		} else if (key_itr->code == SMOOTH_DATA_OFFSET) {
			fprintf (file, "\n\nData Smoothing Keys:");
		}
		key = key_itr->key;

		for (int i=1; i <= key_itr->levels; i++) {
			key += "_#";
		}
		fprintf (file, "\n\t%s", key.c_str ());

		if (key_itr->option == REQ_KEY) {
			fprintf (file, ((flag) ? "     (required" : "\tRequired"));
		} else {
			fprintf (file, ((flag) ? "     (optional" : "\tOptional"));
		}
		switch (key_itr->type) {
			case INT_KEY:
				fprintf (file, ((flag) ? ", integer" : "\tInteger"));
				break;
			case FLOAT_KEY:
				fprintf (file, ((flag) ? ", decimal" : "\tDecimal"));
				break;
			case BOOL_KEY:
				fprintf (file, ((flag) ? ", flag" : "\tBoolean"));
				break;
			case IN_KEY:
				fprintf (file, ((flag) ? ", input file" : "\tInput File"));
				break;
			case OUT_KEY:
				fprintf (file, ((flag) ? ", output file" : "\tOutput File"));
				break;
			case PATH_KEY:
				fprintf (file, ((flag) ? ", directory" : "\tDirectory"));
				break;
			case TIME_KEY:
				fprintf (file, ((flag) ? ", time" : "\tTime"));
				break;
			case LIST_KEY:
				fprintf (file, ((flag) ? ", list" : "\tList"));
				break;
			default:
				fprintf (file, ((flag) ? ", text" : "\tText"));
				break;
		}
		fprintf (file, ((flag) ? ", %s" : "\t%s"), key_itr->value.c_str ());
		fprintf (file, ((flag) ? ", %s" : "\t%s"), key_itr->range.c_str ());
		if (flag) fprintf (file, ")");
	}
}
