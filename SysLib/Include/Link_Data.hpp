//*********************************************************
//	Link_Data.hpp - network link data
//*********************************************************

#ifndef LINK_DATA_HPP
#define LINK_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Link_Data class definition
//---------------------------------------------------------

class SYSLIB_API Link_Data : public Notes_Data
{
public:
	Link_Data (void)             { Clear (); }

	int   Link (void)            { return (link); }
	int   Anode (void)           { return (anode); }
	int   Bnode (void)           { return (bnode); }
	int   Length (void)          { return (length); }
	int   Aoffset (void)         { return (aoffset); }
	int   Boffset (void)         { return (boffset); }
	int   Type (void)            { return (type); }
	int   Divided (void)         { return (divided); }
	int   Area_Type (void)       { return (area_type); }
	int   Use (void)             { return (use); }
	int   Grade (void)           { return (grade); }
	int   AB_Dir (void)          { return (ab_dir); }
	int   BA_Dir (void)          { return (ba_dir); }
	int   Shape (void)           { return (shape); }
	char *Name (void)            { return (name.Data ()); }
	
	void  Link (int value)       { link = value; }
	void  Anode (int value)      { anode = value; }
	void  Bnode (int value)      { bnode = value; }
	void  Length (int value)     { length = value; }
	void  Aoffset (int value)    { aoffset = (short) value; }
	void  Boffset (int value)    { boffset = (short) value; }
	void  Type (int value)       { type = (char) value; }
	void  Divided (int value)    { divided = (char) value; };
	void  Area_Type (int value)  { area_type = (short) value; }
	void  Use (int value)        { use = (short) value; }
	void  Grade (int value)      { grade = (short) value; }
	void  AB_Dir (int value)     { ab_dir = value; }
	void  BA_Dir (int value)     { ba_dir = value; }
	void  Shape (int value)      { shape = value; }

	void  Name (char *value)     { name.Data (value); }
	void  Name (string value)    { if (!value.empty ()) name.Data (value); else name.Clear (); }
	
	void  Length (double value)  { length = exe->Round (value); }
	void  Aoffset (double value) { aoffset = (short) exe->Round (value); }
	void  Boffset (double value) { boffset = (short) exe->Round (value); }
	void  Grade (double value)   { grade = (short) exe->Round (value); }

	int   Grade (bool ab_flag)   { return ((ab_flag) ? grade : -grade); }

	void Clear (void)
	{
		link = anode = bnode = length = 0; ab_dir = ba_dir = shape = -1; type = divided = 0; 
		aoffset = boffset = use = area_type = grade = 0; name.Size (0); Notes_Data::Clear (); 
	}

private:
	int    link;
	int    anode;
	int    bnode;
	int    length;
	short  aoffset;
	short  boffset;
	char   type;
	char   divided;
	short  area_type;
	short  use;
	short  grade;
	int    ab_dir;
	int    ba_dir;
	int    shape;
	Buffer name;
};

typedef vector <Link_Data>    Link_Array;
typedef Link_Array::iterator  Link_Itr;

#endif

