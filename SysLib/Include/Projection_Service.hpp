//*************************************************** 
//	Projection_Service.hpp - Coordinate Projection Service
//***************************************************

#ifndef PROJECTION_SERVICE_HPP
#define PROJECTION_SERVICE_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Static_Service.hpp"
#include "Projection_Data.hpp"

//---------------------------------------------------------
//	Projection_Service Class definition
//---------------------------------------------------------

class SYSLIB_API Projection_Service : public Static_Service
{
public:
	Projection_Service (void);

	void Add_Keys (void);

	bool Read_Control (void);

	void Input_Projection (Projection_Data value)   { input = value; }
	void Output_Projection (Projection_Data value)  { output = value; }

	Projection_Data Input_Projection (void)         { return (input); }
	Projection_Data Output_Projection (void)        { return (output); }

	bool Z_Flag (void)                              { return (z_flag); }
	bool M_Flag (void)                              { return (m_flag); }

private:
	enum projection_keys { INPUT_COORDINATE_SYSTEM = PROJECTION_OFFSET, 
		INPUT_COORDINATE_ADJUSTMENT, OUTPUT_COORDINATE_SYSTEM, OUTPUT_COORDINATE_ADJUSTMENT,
		OUTPUT_XYZ_SHAPES, OUTPUT_XYM_SHAPES };

	Projection_Data input, output;

	bool z_flag, m_flag;
};

#endif
