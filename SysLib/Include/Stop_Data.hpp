//*********************************************************
//	Stop_Data.hpp - network transit stop data
//*********************************************************

#ifndef STOP_DATA_HPP
#define STOP_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Link_Dir_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Stop_Data class definition
//---------------------------------------------------------

class SYSLIB_API Stop_Data : public Notes_Data
{
public:
	Stop_Data (void)            { Clear (); }

	int   Stop (void)           { return (stop); }
	int   Link_Dir (void)       { return (link_dir.Link_Dir ()); }
	int   Link (void)           { return (link_dir.Link ()); }
	int   Dir (void)            { return (link_dir.Dir ()); } 
	int   Offset (void)         { return (offset); }
	int   Use (void)            { return (use); }
	int   Type (void)           { return (type); }
	int   Space (void)          { return (space); }
	char *Name (void)           { return (name.Data ()); }

	void  Stop (int value)      { stop = value; }
	void  Link_Dir (int value)  { link_dir.Link_Dir (value); }
	void  Link (int value)      { link_dir.Link (value); }
	void  Dir (int value)       { link_dir.Dir (value); }
	void  Offset (int value)    { offset = value; }
	void  Use (int value)       { use = (unsigned short) value; }
	void  Type (int value)      { type = (unsigned char) value; }
	void  Space (int value)     { space = (unsigned char) value; }

	void  Name (char *value)    { name.Data (value); }
	void  Name (string value)   { if (!value.empty ()) name.Data (value); else name.Clear (); }
	
	void  Offset (double value) { offset = exe->Round (value); }

	void  Clear (void)
	{
		stop = offset = 0; Link_Dir (0); use = 0; type = space = 0; name.Size (0);
		Notes_Data::Clear ();
	}
private:
	int            stop;
	Link_Dir_Data  link_dir;
	int            offset; 
	unsigned short use;
	unsigned char  type;
	unsigned char  space;
	Buffer         name;
};

typedef vector <Stop_Data>    Stop_Array;
typedef Stop_Array::iterator  Stop_Itr;

#endif
