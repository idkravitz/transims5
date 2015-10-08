//******************************************************** 
//	Connect_File.hpp - link connection file input/output
//********************************************************

#ifndef CONNECT_FILE_HPP
#define CONNECT_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Connect_File Class definition
//---------------------------------------------------------

class SYSLIB_API Connect_File : public Db_Header
{
public:
	Connect_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Connect_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    To_Link (void)             { return (Get_Integer (to_link)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	int    To_Lanes (void)            { return (Get_Integer (to_lanes)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Penalty (void)             { return (Get_Integer (penalty)); }
	double Speed (void)               { return (Get_Double (speed)); }
	int    Capacity (void)            { return (Get_Integer (capacity)); }
	
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   To_Link (int value)        { Put_Field (to_link, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   To_Lanes (int value)       { Put_Field (to_lanes, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Penalty (int value)        { Put_Field (penalty, value); }
	void   Speed (double value)       { Put_Field (speed, value); }
	void   Capacity (int value)       { Put_Field (capacity, value); }
	
	bool Type_Flag (void)             { return (type >= 0); }

	//---- 4.0 fields ----

	int    In_High (void)             { return (Get_Integer (in_high)); }
	int    Out_High (void)            { return (Get_Integer (out_high)); }

	void   In_High (int value)        { Put_Field (in_high, value); }
	void   Out_High (int value)       { Put_Field (out_high, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, to_link, lanes, to_lanes, type, penalty, speed, capacity, in_high, out_high;
};

#endif
