//******************************************************** 
//	Tour_File.hpp - Tour File Input/Output
//********************************************************

#ifndef TOUR_FILE_HPP
#define TOUR_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Tour_File Class definition
//---------------------------------------------------------

class  Tour_File : public Db_Header
{
public:
	Tour_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Tour_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	int  Household (void)               { return (Get_Integer (hhold)); }
	int  Person (void)                  { return (Get_Integer (person)); }
	int  Tour (void)                    { return (Get_Integer (tour)); }
	int  Purpose (void)                 { return (Get_Integer (purpose)); }
	int  Mode (void)                    { return (Get_Integer (mode)); }
	int  Origin (void)                  { return (Get_Integer (origin)); }
	int  Destination (void)             { return (Get_Integer (destination)); }
	int  Stop_Out (void)                { return (Get_Integer (stop_out)); }
	int  Stop_In (void)                 { return (Get_Integer (stop_in)); }
	int  Start (void)                   { return (Get_Integer (start)); }
	int  Return (void)                  { return (Get_Integer (end)); }
	int  Group (void)                   { return (Get_Integer (group)); }

	void Household (int value)          { Put_Field (hhold, value); }
	void Person (int value)             { Put_Field (person, value); }
	void Tour (int value)               { Put_Field (tour, value); }
	void Purpose (int value)            { Put_Field (purpose, value); }
	void Mode (int value)               { Put_Field (mode, value); }
	void Origin (int value)             { Put_Field (origin, value); }
	void Destination (int value)        { Put_Field (destination, value); }
	void Stop_Out (int value)           { Put_Field (stop_out, value); }
	void Stop_In (int value)            { Put_Field (stop_in, value); }
	void Start (int value)              { Put_Field (start, value); }
	void Return (int value)             { Put_Field (end, value); }
	void Group (int value)              { Put_Field (group, value); }

	virtual bool Create_Fields (void);
	
	int  HHold_Field (void)             { return (hhold); }
	int  Person_Field (void)            { return (person); }
	int  Tour_Field (void)              { return (tour); }
	int  Purpose_Field (void)           { return (purpose); }
	int  Mode_Field (void)              { return (mode); }
	int  Origin_Field (void)            { return (origin); }
	int  Dest_Field (void)              { return (destination); }
	int  Stop_Out_Field (void)          { return (stop_out); }
	int  Stop_In_Field (void)           { return (stop_in); }  
	int  Start_Field (void)             { return (start); }
	int  Return_Field (void)            { return (end); }
	int  Group_Field (void)             { return (group); }

	int  HHold_Field (string name)      { return ((hhold = Required_Field (name))); }
	int  Person_Field (string name)     { return ((person = Required_Field (name))); }
	int  Tour_Field (string name)       { return ((tour = Required_Field (name))); }
	int  Purpose_Field (string name)    { return ((purpose = Required_Field (name))); }
	int  Mode_Field (string name)       { return ((mode = Required_Field (name))); }
	int  Origin_Field (string name)     { return ((origin = Required_Field (name))); }
	int  Dest_Field (string name)       { return ((destination = Required_Field (name))); }
	int  Stop_Out_Field (string name)   { return ((stop_out = Required_Field (name))); }
	int  Stop_In_Field (string name)    { return ((stop_in = Required_Field (name))); }  
	int  Start_Field (string name)      { return ((start = Required_Field (name))); }
	int  Return_Field (string name)     { return ((end = Required_Field (name))); }
	int  Group_Field (string name)      { return ((group = Required_Field (name))); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, person, tour, purpose, mode, origin, destination, stop_out, stop_in, start, end, group;
};

#endif
