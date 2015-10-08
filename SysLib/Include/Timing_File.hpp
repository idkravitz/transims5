//******************************************************** 
//	Timing_File.hpp - Timing Plan File Input/Output
//********************************************************

#ifndef TIMING_FILE_HPP
#define TIMING_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Timing_File Class definition
//---------------------------------------------------------

class SYSLIB_API Timing_File : public Db_Header
{
public:
	Timing_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Timing_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Signal (void)                 { return (Get_Integer (signal)); }
	int    Timing (void)                 { return (Get_Integer (timing)); }
	int    Type (void)                   { return (Get_Integer (type)); }
	int    Cycle (void)                  { return (Get_Integer (cycle)); }
	int    Offset (void)                 { return (Get_Integer (offset)); }
	int    Phases (void)                 { return (Get_Integer (phases)); }
	int    Phase (void)                  { return (Get_Integer (phase)); }
	int    Barrier (void)                { return (Get_Integer (barrier)); }
	int    Ring (void)                   { return (Get_Integer (ring)); }
	int    Position (void)               { return (Get_Integer (position)); }
	int    Min_Green (void)              { return (Get_Integer (minimum)); }
	int    Max_Green (void)              { return (Get_Integer (maximum)); }
	int    Extension (void)              { return (Get_Integer (extend)); }
	int    Yellow (void)                 { return (Get_Integer (yellow)); }
	int    All_Red (void)                { return (Get_Integer (red)); }

	void   Signal (int value)            { Put_Field (signal, value); }
	void   Timing (int value)            { Put_Field (timing, value); }
	void   Type (int value)              { Put_Field (type, value); }
	void   Cycle (int value)             { Put_Field (cycle, value); }
	void   Offset (int value)            { Put_Field (offset, value); }
	void   Phases (int value)            { Put_Field (phases, value); }
	void   Phase (int value)             { Put_Field (phase, value); }
	void   Barrier (int value)           { Put_Field (barrier, value); }
	void   Ring (int value)              { Put_Field (ring, value); }
	void   Position (int value)          { Put_Field (position, value); }
	void   Min_Green (int value)         { Put_Field (minimum, value); }
	void   Max_Green (int value)         { Put_Field (maximum, value); }
	void   Extension (int value)         { Put_Field (extend, value); }
	void   Yellow (int value)            { Put_Field (yellow, value); }
	void   All_Red (int value)           { Put_Field (red, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int signal, timing, type, cycle, offset, phases;
	int phase, barrier, ring, position, minimum, maximum, extend, yellow, red;
};

#endif
