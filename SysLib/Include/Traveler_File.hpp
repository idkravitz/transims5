//******************************************************** 
//	Traveler_File.hpp - Traveler File Input/Output
//********************************************************

#ifndef TRAVELER_FILE_HPP
#define TRAVELER_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Traveler_File Class definition
//---------------------------------------------------------

class SYSLIB_API Traveler_File : public Db_Header
{
public:
	Traveler_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Traveler_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Household (void)           { return (Get_Integer (hhold)); }
	int    Person (void)              { return (Get_Integer (person)); }
	int    Tour (void)                { return (Get_Integer (tour)); }
	int    Trip (void)                { return (Get_Integer (trip)); }
	int    Mode (void)                { return (Get_Integer (mode)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	double Distance (void)            { return (Get_Double (distance)); }
	double Speed (void)               { return (Get_Double (speed)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Lane (void)                { return (Get_Integer (lane)); }
	double Offset (void)              { return (Get_Double (offset)); }
	int    Route (void)               { return (Get_Integer (route)); }

	void   Household (int value)      { Put_Field (hhold, value); }
	void   Person (int value)         { Put_Field (person, value); }
	void   Tour (int value)           { Put_Field (tour, value); }
	void   Trip (int value)           { Put_Field (trip, value); }
	void   Mode (int value)           { Put_Field (mode, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Distance (double value)    { Put_Field (distance, value); }
	void   Speed (double value)       { Put_Field (speed, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Lane (int value)           { Put_Field (lane, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Route (int value)          { Put_Field (route, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, person, tour, trip, mode, time, distance, speed, link, dir, lane, offset, route;
};

#endif
