//********************************************************* 
//	Arcview_File.hpp - Arcview Shape File interface
//*********************************************************

#ifndef ARCVIEW_FILE_HPP
#define ARCVIEW_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Projection.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Arcview_File Class definition
//---------------------------------------------------------

class SYSLIB_API Arcview_File : public Db_Header, public Points, public Projection
{
public:
	Arcview_File (Access_Type access = READ);
	Arcview_File (string filename, Access_Type access = READ);
	~Arcview_File ()                        { Close (); }
	
	virtual bool Open (string filename = "");
	virtual bool Close (void);
	
	virtual bool Read_Record (int number = 0);
	virtual bool Write_Record (int number = 0);

	Integers parts;

	int * Get_Parts (void)                  { return (&(parts.front ())); }	
	XYZ_Point * Get_Points (void)           { return (&(front ())); }

	bool  Z_Flag (void)                     { return (z_flag); }
	void  Z_Flag (bool flag);

	bool  M_Flag (void)                     { return (m_flag); }
	void  M_Flag (bool flag)                { m_flag = flag; }

	int   Num_Parts (void)                  { return ((int) parts.size ()); }
	void  Num_Parts (int num)               { parts.resize (num); }

	int   Num_Points (void)                 { return ((int) size ()); }
	void  Num_Points (int num)              { resize (num); pts.resize (num); if (z_flag) zm.resize (num); }

	Shape_Code Shape_Type (void)            { return (shape_type); }
	void  Shape_Type (Shape_Code type)      { shape_type = type; }

	bool Shape_Filename (string path)       { return (shape_file.Filename (path)); }
	string& Shape_Filename (void)           { return (shape_file.Filename ()); }
	
	bool Set_Projection (Projection_Data input, Projection_Data output);

private:
	void  Initialize (void);
	void  Write_Projection (void);

	enum Arc_Shapes { POINT_TYPE = 1, ARC_TYPE = 3, POLYGON_TYPE = 5, MULTIPOINT_TYPE = 8 };

	//---- XY_Range typedef ----

	typedef  struct {
		double  xmin;
		double  ymin;
		double  xmax;
		double  ymax;
	} XY_Range;

	//---- Z_Range typedef ----

	typedef  struct {
		double  min;
		double  max;
	} Range;

	//---- Arc_Header typedef ----

#pragma pack (push, 4)
	typedef  struct {
		int       file_code;        //---- 9994 ----
		int       unused [5];
		int       file_size;        //---- words ----
		int       version;          //---- 1000 ----
		int       shape_type;       //---- 1, 3, 5, 8, 11, 13, 15, 18, 21, 23, 25, 28 ----
		XY_Range  box;
		Range     zbox;
		Range     mbox;
	} Arc_Header;
#pragma pack (pop)

	//---- Arc_Record typedef ----

	typedef  struct {
		int  rec_num;           //---- 1... ----
		int  rec_size;          //---- words ----
	} Arc_Record;

	//---- Arc_Multi_Point typedef ----

#pragma pack (push, 4)
	typedef  struct {          //---- shape_type = 8, 18, 28 ----
		XY_Range  box;
		int       num_pts;
	} Arc_Multi_Point;
#pragma pack (pop)

	//---- Arc_Shape typedef ----

	typedef  struct {          //---- shape_types = 3, 5, 13, 15, 23, 25 ----
		XY_Range  box;
		int       num_parts;
		int       num_pts;
	} Arc_Shape;

	Shape_Code shape_type;
	
	bool z_flag, m_flag;
	
	Doubles zm;
	XY_Points pts;

	double * Get_ZM (void)                  { return (&(zm.front ())); }
	XY_Point * Get_XY (void)                { return (&(pts.front ())); }

	XY_Range  extent;
	XY_Range  range;
	
	Range   _extent;
	Range   _range;

	Db_File  shape_file;
	Db_File  index_file;

	Arc_Header  shape_header;
	Arc_Header  index_header;

	void  Reorder_Bits (void *dat, int words);
};

#endif
