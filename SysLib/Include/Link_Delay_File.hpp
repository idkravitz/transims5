//******************************************************** 
//	Link_Delay_File.hpp - Link Delay Input/Output
//********************************************************

#ifndef LINK_DELAY_FILE_HPP
#define LINK_DELAY_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Link_Delay_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Delay_File : public Db_Header
{
public:
	Link_Delay_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);
	Link_Delay_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Type (void)                { return (Get_Integer (type)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	double Flow (void)                { return (Get_Double (flow)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	int    Out_Link (void)            { return (Get_Integer (out_link)); }
	double Out_Flow (void)            { return (Get_Double (out_flow)); }
	Dtime  Out_Time (void)            { return (Get_Time (out_time)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Flow (double value)        { Put_Field (flow, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Out_Link (int value)       { Put_Field (out_link, value); }
	void   Out_Flow (double value)    { Put_Field (out_flow, value); }
	void   Out_Time (Dtime value)     { Put_Field (out_time, value); }

	Units_Type Flow_Units (void)      { return (flow_units); }
	void Flow_Units (Units_Type type) { flow_units = type; }

	bool   Turn_Flag (void)           { return (turn_flag); }
	void   Turn_Flag (bool flag)      { turn_flag = flag; }
	
	void   Skip_Turns (bool flag);

	bool   Lane_Use_Flows (void)      { return (type_flag); }
	void   Lane_Use_Flows (bool flag) { type_flag = flag; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, type, start, end, flow, time;
	int out_link, out_flow, out_time;
	Units_Type flow_units;
	bool turn_flag, type_flag;
};

#endif
