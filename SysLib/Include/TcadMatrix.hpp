//*********************************************************
//	TcadMatrix.hpp - TransCAD Matrix DLL Interface
//*********************************************************

#ifndef TCADMATRIX_HPP
#define TCADMATRIX_HPP

#include "APIDefs.hpp"
#include "Db_Matrix.hpp"

#include "CaliperMTX.h"

extern int tc_status;

//---------------------------------------------------------
//	TransCAD_Matrix Class definition
//---------------------------------------------------------

class SYSLIB_API TransCAD_Matrix : public Db_Matrix
{
public:
	TransCAD_Matrix (Access_Type access = READ);

	virtual bool Db_Open (string filename);
	virtual bool Is_Open (void)        { return (fh != 0); }
	virtual bool Close (void);
	
	virtual bool Read_Record (int org = 0);
	virtual bool Write_Record (int org = 0);

	virtual bool Read (int org, int table, double *data);
	virtual bool Write (int org, int table, double *data);

private:
	virtual bool Load_DLL (void);
	
	virtual bool Read_Header (bool stat);

	MATRIX fh;
};
#endif
