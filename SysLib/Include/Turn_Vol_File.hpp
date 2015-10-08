//******************************************************** 
//	Turn_Vol_File.hpp - Turn Volume File Input/Output
//********************************************************

#ifndef TURN_VOL_FILE_HPP
#define TURN_VOL_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Turn_Vol_File Class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Vol_File : public Db_Header
{
public:
	Turn_Vol_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Turn_Vol_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int   Node (void)               { return (Get_Integer (node)); }
	int   Link (void)               { return (Get_Integer (link)); }
	int   To_Link (void)            { return (Get_Integer (to_link)); }
	Dtime Start (void)              { return (Get_Time (start)); }
	Dtime End (void)                { return (Get_Time (end)); }
	int   Volume (void)				{ return (Get_Integer (volume)); }

	void   Node (int value)         { Put_Field (node, value); }	
	void   Link (int value)         { Put_Field (link, value); }
	void   To_Link (int value)      { Put_Field (to_link, value); }
	void   Start (Dtime value)      { Put_Field (start, value); }
	void   End (Dtime value)        { Put_Field (end, value); }
	void   Volume (int value)       { Put_Field (volume, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int node, link, to_link, start, end, volume;
};

#endif
