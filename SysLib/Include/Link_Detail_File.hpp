//******************************************************** 
//	Link_Detail_File.hpp - Link Detail File Input/Output
//********************************************************

#ifndef LINK_DETAIL_FILE_HPP
#define LINK_DETAIL_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Link_Detail_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Detail_File : public Db_Header
{
public:
	Link_Detail_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Detail_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Control (void)             { return (Get_Integer (control)); }
	int    Group (void)               { return (Get_Integer (group)); }
	double LM_Length (void)           { return (Get_Double (lm_length)); }    
	int    Left_Merge (void)          { return (Get_Integer (left_merge)); }
	double LT_Length (void)           { return (Get_Double (lt_length)); }
	int    Left (void)                { return (Get_Integer (left)); }
	int    Left_Thru (void)           { return (Get_Integer (left_thru)); }
	int    Thru (void)                { return (Get_Integer (thru)); }
	int    Right_Thru (void)          { return (Get_Integer (right_thru)); }
	int    Right (void)               { return (Get_Integer (right)); }
	double RT_Length (void)           { return (Get_Double (rt_length)); }
	int    Right_Merge (void)         { return (Get_Integer (right_merge)); }
	double RM_Length (void)           { return (Get_Double (rm_length)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	string Period (void)              { return (Get_String (period)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Control (int value)        { Put_Field (control, value); }
	void   Group (int value)          { Put_Field (group, value); }
	void   LM_Length (double value)   { Put_Field (lm_length, value); }
	void   Left_Merge (int value)     { Put_Field (left_merge, value); }
	void   LT_Length (double value)   { Put_Field (lt_length, value); }
	void   Left (int value)           { Put_Field (left, value); }
	void   Left_Thru (int value)      { Put_Field (left_thru, value); }
	void   Thru (int value)           { Put_Field (thru, value); }
	void   Right_Thru (int value)     { Put_Field (right_thru, value); }
	void   Right (int value)          { Put_Field (right, value); }
	void   RT_Length (double value)   { Put_Field (rt_length, value); }
	void   Right_Merge (int value)    { Put_Field (right_merge, value); }
	void   RM_Length (double value)   { Put_Field (rm_length, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   Period (char * value)      { Put_Field (period, value); }
	void   Period (string &value)     { Put_Field (period, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, control, group, use, lanes, period;
	int left_merge, left, left_thru, thru, right_thru, right, right_merge;
	int lm_length, lt_length, rt_length, rm_length;
};

#endif
