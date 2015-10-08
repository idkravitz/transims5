//******************************************************** 
//	Signal_File.hpp - Signal File Input/Output
//********************************************************

#ifndef SIGNAL_FILE_HPP
#define SIGNAL_FILE_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Signal_File Class definition
//---------------------------------------------------------

class SYSLIB_API Signal_File : public Db_Header
{
public:
	Signal_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Signal_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Signal (void)              { return (Get_Integer (signal)); }
	int    Group (void)               { return (Get_Integer (group)); }
	int    Times (void)               { return (Get_Integer (times)); }
	String Nodes (void)               { return (Get_String (nodes)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	int    Timing (void)              { return (Get_Integer (timing)); }
	int    Phasing (void)             { return (Get_Integer (phasing)); }

	void   Signal (int value)         { Put_Field (signal, value); }
	void   Group (int value)          { Put_Field (group, value); }
	void   Times (int value)          { Put_Field (times, value); }
	void   Nodes (char *value)        { Put_Field (nodes, value); }
	void   Nodes (string &value)      { Put_Field (nodes, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Timing (int value)         { Put_Field (timing, value); }
	void   Phasing (int value)        { Put_Field (phasing, value); }

	//---- version 4.0 compatibility ----

	int    Type (void)                { return (Get_Integer (type)); }
	int    Offset (void)              { return (Get_Integer (offset)); }

	void   Type (int value)           { Put_Field (type, value); }
	void   Offset (int value)         { Put_Field (offset, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int signal, group, times, nodes;
	int start, end, timing, phasing, type, offset;
	bool warrant_flag;
};

#endif
