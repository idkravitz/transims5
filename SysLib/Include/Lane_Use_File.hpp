//******************************************************** 
//	Lane_Use_File.hpp - Lane Use File Input/Output
//********************************************************

#ifndef LANE_USE_FILE_HPP
#define LANE_USE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Lane_Use_File Class definition
//---------------------------------------------------------

class SYSLIB_API Lane_Use_File : public Db_Header
{
public:
	Lane_Use_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Lane_Use_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Min_Veh_Type (void)        { return (Get_Integer (min_type)); }
	int    Max_Veh_Type (void)        { return (Get_Integer (max_type)); }
	int    Min_Traveler (void)        { return (Get_Integer (min_trav)); }
	int    Max_Traveler (void)        { return (Get_Integer (max_trav)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	double Offset (void)              { return (Get_Double (offset)); }
	double Length (void)              { return (Get_Double (length)); }
	int    Toll (void)                { return (Get_Integer (toll)); }
	double Toll_Rate (void)           { return (Get_Double (rate)); }
	double Min_Delay (void)           { return (Get_Double (min_delay)); }
	double Max_Delay (void)           { return (Get_Double (max_delay)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Min_Veh_Type (int value)   { Put_Field (min_type, value); }
	void   Max_Veh_Type (int value)   { Put_Field (max_type, value); }
	void   Min_Traveler (int value)   { Put_Field (min_trav, value); }
	void   Max_Traveler (int value)   { Put_Field (max_trav, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Toll (int value)           { Put_Field (toll, value); }
	void   Toll_Rate (double value)   { Put_Field (rate, value); }
	void   Min_Delay (double value)   { Put_Field (min_delay, value); }
	void   Max_Delay (double value)   { Put_Field (max_delay, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, lanes, type, use, min_type, max_type, min_trav, max_trav, start, end, offset, length, toll, rate, min_delay, max_delay;
};

#endif
