//******************************************************** 
//	Link_File.hpp - Link File Input/Output
//********************************************************

#ifndef LINK_FILE_HPP
#define LINK_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Link_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_File : public Db_Header
{
public:
	Link_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	string Name (void)                { return (Get_String (name)); }
	int    Node_A (void)              { return (Get_Integer (node_a)); }
	int    Node_B (void)              { return (Get_Integer (node_b)); }
	double Length (void)              { return (Get_Double (length)); }
	double Setback_A (void)           { return (Get_Double (setback_a)); }
	double Setback_B (void)           { return (Get_Double (setback_b)); }
	int    Bearing_A (void)           { return (Get_Integer (bearing_a)); }
	int    Bearing_B (void)           { return (Get_Integer (bearing_b)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Divided (void)             { return (Get_Integer (divided)); }
	int    Area_Type (void)           { return (Get_Integer (area_type)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	double Grade (void)               { return (Get_Double (grade)); }
	int    Lanes_AB (void)            { return (Get_Integer (lanes_ab)); }
	double Speed_AB (void)            { return (Get_Double (speed_ab)); }
	double Fspd_AB (void)             { return (Get_Double (fspd_ab)); }
	int    Cap_AB (void)              { return (Get_Integer (cap_ab)); }
	int    Lanes_BA (void)            { return (Get_Integer (lanes_ba)); }
	double Speed_BA (void)            { return (Get_Double (speed_ba)); }
	double Fspd_BA (void)             { return (Get_Double (fspd_ba)); }
	int    Cap_BA (void)              { return (Get_Integer (cap_ba)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Name (char * value)        { Put_Field (name, value); }
	void   Name (string &value)       { Put_Field (name, value); }
	void   Node_A (int value)         { Put_Field (node_a, value); }
	void   Node_B (int value)         { Put_Field (node_b, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Setback_A (double value)   { Put_Field (setback_a, value); }
	void   Setback_B (double value)   { Put_Field (setback_b, value); }
	void   Bearing_A (int value)      { Put_Field (bearing_a, value); }
	void   Bearing_B (int value)      { Put_Field (bearing_b, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Divided (int value)        { Put_Field (divided, value); }
	void   Area_Type (int value)      { Put_Field (area_type, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Grade (double value)       { Put_Field (grade, value); }
	void   Lanes_AB (int value)       { Put_Field (lanes_ab, value); }
	void   Speed_AB (double value)    { Put_Field (speed_ab, value); }
	void   Fspd_AB (double value)     { Put_Field (fspd_ab, value); }
	void   Cap_AB (int value)         { Put_Field (cap_ab, value); }
	void   Lanes_BA (int value)       { Put_Field (lanes_ba, value); }
	void   Speed_BA (double value)    { Put_Field (speed_ba, value); }
	void   Fspd_BA (double value)     { Put_Field (fspd_ba, value); }
	void   Cap_BA (int value)         { Put_Field (cap_ba, value); }
	
	bool   Bearing_Flag (void)        { return (bearing_a >= 0 || bearing_b >= 0); }
	bool   Area_Type_Flag (void)      { return (area_type >= 0); }
	bool   Speed_Flag (void)          { return (speed_ab >= 0 || speed_ba >= 0); }
	bool   Fspd_Flag (void)           { return (fspd_ab >= 0 || fspd_ba >= 0); }

	//---- Version 4 fields ----
	
	int    Left_AB (void)             { return (Get_Integer (left_ab)); }
	int    Right_AB (void)            { return (Get_Integer (right_ab)); }
	int    Left_BA (void)             { return (Get_Integer (left_ba)); }
	int    Right_BA (void)            { return (Get_Integer (right_ba)); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, name, node_a, node_b, length, setback_a, setback_b, bearing_a, bearing_b;
	int type, divided, area_type, use, grade;
	int lanes_ab, speed_ab, fspd_ab, cap_ab;
	int lanes_ba, speed_ba, fspd_ba, cap_ba;
	int left_ab, right_ab, left_ba, right_ba;
};

#endif
