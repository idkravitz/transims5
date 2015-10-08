//******************************************************** 
//	Phasing_File.hpp - Phasing Plan File Input/Output
//********************************************************

#ifndef PHASING_FILE_HPP
#define PHASING_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Phasing_File Class definition
//---------------------------------------------------------

class SYSLIB_API Phasing_File : public Db_Header
{
public:
	Phasing_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Phasing_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Signal (void)              { return (Get_Integer (signal)); }
	int    Phasing (void)             { return (Get_Integer (phasing)); }
	int    Phase (void)               { return (Get_Integer (phase)); }
	string Detectors (void)           { return (Get_String (detectors)); }
	int    Movements (void)           { return (Get_Integer (movements)); }
	int    Movement (void)            { return (Get_Integer (movement)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    To_Link (void)             { return (Get_Integer (to_link)); }
	int    Protection (void)          { return (Get_Integer (protect)); }

	void   Signal (int value)         { Put_Field (signal, value); }
	void   Phasing (int value)        { Put_Field (phasing, value); }
	void   Phase (int value)          { Put_Field (phase, value); }
	void   Detectors (char *value)    { Put_Field (detectors, value); }
	void   Detectors (string &value)  { Put_Field (detectors, value); }
	void   Movements (int value)      { Put_Field (movements, value); }
	void   Movement (int value)       { Put_Field (movement, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   To_Link (int value)        { Put_Field (to_link, value); }
	void   Protection (int value)     { Put_Field (protect, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int signal, phasing, phase, detectors, movements;
	int movement, link, dir, to_link, protect;
};

#endif
