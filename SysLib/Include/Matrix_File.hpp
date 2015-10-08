//********************************************************* 
//	Matrix_File.hpp - Matrix File Input/Output
//*********************************************************

#ifndef MATRIX_FILE_HPP
#define MATRIX_FILE_HPP

#include "Db_Header.hpp"
#include "Time_Periods.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Matrix_File Class definition
//---------------------------------------------------------

class SYSLIB_API Matrix_File : public Db_Header, public Time_Periods
{
public:
	Matrix_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);
	Matrix_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);

	int    Origin (void)                      { return (Get_Integer (origin)); }
	int    Destination (void)                 { return (Get_Integer (destination)); }
	int    Period (void)                      { return (Get_Integer (period)); }
	int    Data (void)                        { return (Get_Integer (data)); }
	double Factor (void)                      { return (Get_Double (data)); }

	int    Period (Dtime time)                { return (Time_Periods::Period (time)); }
	int    Org_Index (int origin);
	int    Des_Index (int destination); 
	int    Num_Org (void)                     { return (num_org); }
	int    Num_Des (void)                     { return (num_des); }

	void   Origin (int value)                 { Put_Field (origin, value); }
	void   Destination (int value)            { Put_Field (destination, value); }
	void   Period (int value)                 { Put_Field (period, value); }
	void   Data (int value)                   { Put_Field (data, value); }
	void   Factor (double value)              { Put_Field (data, value); }

	void   Num_Org (int value)                { num_org = value; }
	void   Num_Des (int value)                { num_des = value; }

	int    Add_Org (int origin);
	int    Add_Des (int destination); 

	Int_Map * Org_Map (void)                  { return (&org_map); }
	Int_Map * Des_Map (void)                  { return (&des_map); }

	virtual bool Create_Fields (void);

	Matrix_Type  Data_Type (void)             { return (matrix_type); }
	void  Data_Type (Matrix_Type value)       { matrix_type = value; }

	Units_Type OD_Units (void)                { return (od_units); }
	void  OD_Units (Units_Type type)          { od_units = type; }

	virtual bool Write_Header (string user_lines = "");

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);
	virtual void Setup (void);

private:
	int origin, destination, period, data, num_org, num_des;
	Matrix_Type matrix_type;
	Units_Type od_units;
	Int_Map org_map, des_map;
};

#endif
