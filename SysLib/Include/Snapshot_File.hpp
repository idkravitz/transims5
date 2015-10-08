//******************************************************** 
//	Snapshot_File.hpp - Snapshot File Input/Output
//********************************************************

#ifndef SNAPSHOT_FILE_HPP
#define SNAPSHOT_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Dtime.hpp"

#pragma pack (push, 4)
typedef struct {
	unsigned int hhold      : 26;	//---- 1
	unsigned int type       :  6;
	unsigned int lane       :  4;	//---- 2
	unsigned int dir        :  1;	
	unsigned int passengers : 10;
	unsigned int offset     : 17;
	unsigned int speed      :  8;   //---- 3
	unsigned int link       : 24;
	unsigned int wait       : 11;	//---- 4
	unsigned int sign       :  1;
	unsigned int diff       : 12;
	unsigned int user       :  8;
} Compressed_Snapshot;
#pragma pack (pop)

//---------------------------------------------------------
//	Snapshot_File Class definition
//---------------------------------------------------------

class SYSLIB_API Snapshot_File : public Db_Header
{
public:
	Snapshot_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Snapshot_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	virtual bool Create_Fields (void);
	virtual bool Open (string filename = "");
	virtual bool Close (void);

	bool   Read_Index (Dtime &step, unsigned &record);
	bool   Read_Index (Dtime &step, off_t &offset);
	bool   Write_Index (Dtime step);
	bool   Write_Index (Dtime step, unsigned record_num);

	bool   Rewind_Index (void)        { return (index_file.Rewind ()); }

	int    Household (void)           { return ((compress) ? rec->hhold : Get_Integer (hhold)); }
	int    Vehicle (void)             { return (Get_Integer (vehicle)); }
	int    Cell (void)                { return (Get_Integer (cell)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	int    Link (void)                { return ((compress) ? rec->link : Get_Integer (link)); }
	int    Dir (void)                 { return ((compress) ? rec->dir : Get_Integer (dir)); }
	int    Lane (void)                { return ((compress) ? rec->lane : Get_Integer (lane)); }
	double Offset (void)              { return ((compress) ? rec->offset : Get_Double (offset)); }
	double Speed (void)               { return ((compress) ? rec->speed : Get_Double (speed)); }
	int    Passengers (void)          { return ((compress) ? rec->passengers : Get_Integer (pass)); }
	int    Type (void)                { return ((compress) ? rec->type : Get_Integer (type)); }
	int    Wait (void)                { return ((compress) ? rec->wait : Get_Integer (wait)); }
	int    Time_Diff (void)           { return ((compress) ? ((rec->sign) ? rec->diff : -(int) rec->diff) : Get_Integer (diff)); }
	int    User (void)                { return ((compress) ? rec->user : Get_Integer (user)); }
	double X (void)                   { return (Get_Double (x)); }
	double Y (void)                   { return (Get_Double (y)); }
	double Z (void)                   { return (Get_Double (z)); }
	double Bearing (void)             { return (Get_Double (bearing)); }

	void   Household (int value)      { if (compress) rec->hhold = value; else Put_Field (hhold, value); }
	void   Vehicle (int value)        { Put_Field (vehicle, value); }
	void   Cell (int value)           { Put_Field (cell, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Link (int value)           { if (compress) rec->link = value; else Put_Field (link, value); }
	void   Dir (int value)            { if (compress) rec->dir = value; else Put_Field (dir, value); }
	void   Lane (int value)           { if (compress) rec->lane = value; else Put_Field (lane, value); }
	void   Offset (double value)      { if (compress) rec->offset = DTOI (value); else Put_Field (offset, value); }
	void   Speed (double value)       { if (compress) rec->speed = DTOI (value); else Put_Field (speed, value); }
	void   Passengers (int value)     { if (compress) rec->passengers = value; else Put_Field (pass, value); }
	void   Type (int value)           { if (compress) rec->type = value; else Put_Field (type, value); }
	void   Wait (int value)           { if (compress) rec->wait = value; else Put_Field (wait, value); }
	void   Time_Diff (int value)      { if (compress) {if (value < 0) {rec->sign = 1; rec->diff = -value;} else {rec->sign = 0; rec->diff = value;}} else Put_Field (diff, value); }
	void   User (int value)           { if (compress) rec->user = value; else Put_Field (user, value); }
	void   X (double value)           { Put_Field (x, value); }
	void   Y (double value)           { Put_Field (y, value); }
	void   Z (double value)           { Put_Field (z, value); }
	void   Bearing (double value)     { Put_Field (bearing, value); }

	int  Link_Field (void)            { return (link); }
	int  Dir_Field (void)             { return (dir); }
	int  Lane_Field (void)            { return (lane); }
	int  Offset_Field (void)          { return (offset); }
	int  Type_Field (void)            { return (type); }
	int  Cell_Field (void)            { return (cell); }
	int  Wait_Field (void)            { return (wait); }
	int  Diff_Field (void)            { return (diff); }
	int  User_Field (void)            { return (user); }

	bool Location_Flag (void)         { return (location_flag); }
	void Location_Flag (bool flag)    { location_flag = flag; }
	
	bool Cell_Flag (void)             { return (cell_flag); }
	void Cell_Flag (bool flag)        { cell_flag = flag; }

	bool Compress_Flag (void)         { return (compress); }
	void Compress_Flag (bool flag)    { compress = flag; }

	bool Status_Flag (void)           { return (status_flag); }
	void Status_Flag (bool flag)      { status_flag = flag; }

	Units_Type Compress_Units (void)  { return (units); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, vehicle, time, link, dir, lane, offset, speed, pass, type;
	int wait, diff, user, cell, x, y, z, bearing, blob;
	bool location_flag, status_flag, cell_flag, compress;
	Units_Type units;

	typedef struct {
		Dtime    time;
		unsigned record;
	} Index_Record;

	Compressed_Snapshot *rec;
	Index_Record *index;

	Db_File  index_file;
};

#endif
