//*********************************************************
//	Db_Matrix.hpp - General Matrix DLL Interface
//*********************************************************

#ifndef DB_MATRIX_HPP
#define DB_MATRIX_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Partition_Files.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Db_Matrix Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Matrix : public Db_Header
{
public:
	Db_Matrix (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Db_Matrix (void)              { Close (); }

	virtual bool Db_Open (string filename) { return (filename.empty ()); }
	virtual bool Is_Open (void)            { return (false); }
	virtual bool Close (void);
	
	virtual bool Read_Record (int org = 0);
	virtual bool Write_Record (int org = 0);

	bool Set_Fields (int des = 0);
	bool Get_Fields (int des = 0);

	virtual bool Read (int org, int table, double *data);
	virtual bool Write (int org, int table, double *data);

	void Zones (int value)                 { zones = value; }
	int  Zones (void)                      { return (zones); }

	void Tables (int value)                { tables = value; }
	int  Tables (void)                     { return (tables); }

	bool Allocate_Data (void);
	double ** Data (void)                  { return (data); }

	void Zero_Data (void);
	
	int Num_Records (void)                 { return (zones * tables); }
	int Estimate_Records (void)            { return (Num_Records ()); }

protected:
	bool Table_Names (bool stat, Strings &labels, Integers &decimals);

private:
	virtual bool Load_DLL (void)           { return (true); }

	virtual bool Read_Header (bool stat)   { return (stat); }

	double **data;
	int zones, tables;
};

typedef Db_Matrix *                 Db_Mat_Ptr;
typedef Pointer_Array <Db_Mat_Ptr>  Db_Matrices;
typedef Db_Matrices::iterator       Db_Mat_Itr;
#endif
