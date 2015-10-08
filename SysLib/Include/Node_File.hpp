//******************************************************** 
//	Node_File.hpp - Node File Input/Output
//********************************************************

#ifndef NODE_FILE_HPP
#define NODE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Node_File Class definition
//---------------------------------------------------------

class SYSLIB_API Node_File : public Db_Header
{
public:
	Node_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Node_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Node (void)                { return (Get_Integer (node)); }
	double X (void)                   { return (Get_Double (x)); }
	double Y (void)                   { return (Get_Double (y)); }
	double Z (void)                   { return (Get_Double (z)); }
	int    Subarea (void)             { return (Get_Integer (subarea)); }
	int    Partition (void)           { return (Get_Integer (part)); }

	void   Node (int value)           { Put_Field (node, value); }
	void   X (double value)           { Put_Field (x, value); }
	void   Y (double value)           { Put_Field (y, value); }
	void   Z (double value)           { Put_Field (z, value); }
	void   Subarea (int value)        { Put_Field (subarea, value); }
	void   Partition (int value)      { Put_Field (part, value); }

	bool   Subarea_Flag (void)        { return (sub_flag); }
	void   Subarea_Flag (bool flag)   { sub_flag = flag; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	bool sub_flag;
	int node, x, y, z, subarea, part;
};

#endif
