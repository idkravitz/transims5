//*********************************************************
//	Control_Service.hpp - array of control keys
//*********************************************************

#ifndef CONTROL_SERVICE_HPP
#define CONTROL_SERVICE_HPP

#include "APIDefs.hpp"
#include "Report_Service.hpp"
#include "Best_List.hpp"
#include "TypeDefs.hpp"
#include "Dtime.hpp"

#include <stdio.h>

//---------------------------------------------------------
//	Control_Key -- key specification structure and array
//---------------------------------------------------------

struct Control_Key {
	int         code;
	String      key;
	Level_Type  levels;
	Option_Type option;
	Key_Type    type;
	String      value;
	String      range;
	Help_Type   help;
};

typedef vector <Control_Key>    Control_Keys;
typedef Control_Keys::iterator  Control_Key_Itr;

//---------------------------------------------------------
//	Key_Value -- key value structure and array
//---------------------------------------------------------

typedef struct key_value {
	String                value;
	map <int, key_value>  nest;
} Key_Value;

typedef map <int, Key_Value>        Key_Values;
typedef pair <int, Key_Value>       Key_Value_Data;
typedef Key_Values::iterator        Key_Value_Itr;
typedef pair <Key_Value_Itr, bool>  Key_Value_Stat;

//---------------------------------------------------------
//	Control_Service -- class definition
//---------------------------------------------------------

class SYSLIB_API Control_Service : public Report_Service
{
public:
	Control_Service (void);

	void   Add_Control_Key (Control_Key key);

	bool   Read_Control_File (string control_file);
	void   Write_Control_File (void);
	
	void   Process_Control_String (String control);

	String Get_Control_String (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Set_Control_Flag (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 

	String Get_Control_Text (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_Flag (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 
	int    Get_Control_Integer (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 
	double Get_Control_Double (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 
	Dtime  Get_Control_Time (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0); 

	bool   Get_Control_List (int code, Integer_List &list, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_List (int code, Double_List &list, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_List (int code, Dtime_List &list, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_List (int code, Bool_List &list, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Get_Control_List (int code, String_List &list, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	bool   Get_Control_List_Groups (int code, Integer_List &list, bool round_flag = false);
	bool   Get_Control_List_Groups (int code, Double_List &list, bool scale_flag = false);
	bool   Get_Control_List_Groups (int code, Dtime_List &list, bool round_flag = false);
	bool   Get_Control_List_Groups (int code, String_List &list);

	String Get_Default_Text (int code);
	bool   Get_Default_Flag (int code);
	int    Get_Default_Integer (int code);
	double Get_Default_Double (int code);
	Dtime  Get_Default_Time (int code);

	String Get_Range_String (int code);
	
	int    Integer_Range_Check (int value, String range);	
	double Double_Range_Check (double value, String range);
	Dtime  Time_Range_Check (Dtime value, String range);
	bool   List_Range_Check (Integer_List &list, String range);
	bool   List_Range_Check (Double_List &list, String range);
	bool   List_Range_Check (Dtime_List &list, String range);

	bool   Check_Control_Key (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	void   Control_Key_Error (int code, string range = "", int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	int    Highest_Control_Group (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	int    Number_Control_Group (int code, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	bool   Mode_Range_Key (int code, bool *modes, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);
	bool   Event_Range_Key (int code, bool *types, int nest1 = 0, int nest2 = 0, int nest3 = 0, int nest4 = 0);

	bool   Required_Control_Key (int code);
	bool   Required_Control_Key (int code, bool flag);
	bool   Ignore_Control_Key (int code);

	Print_Stream & Output_Control_Label (int code);
	void   Output_Control_Key (int code, string value)  { Output_Control_Label (code) << value; }

	String Current_Label (void);
	String Current_Key (void);
	Units_Type Current_Units (void)                     { return (current_units); }

	bool   Control_Key_Status (int code);
	void   Show_Program_Keys (void);
	void   Show_Extra_Keys (void);
	bool   Check_Keys (void);
	void   XML_Controls (void);
	void   Document_Controls (FILE *file, bool flag = false);

private:
	int current_key;
	Units_Type current_units, default_units;
	String nesting;

	Control_Key * Get_Control_Key (int code);

	Control_Keys  keys;
	Key_Values    values;
	Strings       extra_keys;

	void Print_Key (FILE *file, String &key, String &value, String &range);
};

#endif
