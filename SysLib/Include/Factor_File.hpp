//********************************************************* 
//	Factor_File.hpp - Factor File Input/Output
//*********************************************************

#ifndef FACTOR_FILE_HPP
#define FACTOR_FILE_HPP

#include "Matrix_File.hpp"

//---------------------------------------------------------
//	Factor_File Class definition
//---------------------------------------------------------

class SYSLIB_API Factor_File : public Matrix_File
{
public:
	Factor_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type type = DATA_TABLE, Units_Type od = ZONE_OD);
	Factor_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type type = DATA_TABLE, Units_Type od = ZONE_OD);

protected:
	virtual void Setup (void);
};

#endif
