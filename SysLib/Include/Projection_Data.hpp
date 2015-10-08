//*************************************************** 
//	Projection_Data.hpp - Coordinate Projection Data
//***************************************************

#ifndef PROJECTION_DATA_HPP
#define PROJECTION_DATA_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Projection_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Projection_Data : public Static_Service
{
public:
	Projection_Data (void);

	void Type (Projection_Type value)   { type = value; }
	void Units (Units_Type value)       { units = value; }
	void Code (int value)               { code = value; }
	void X_Offset (double value)        { x_offset = value; }
	void Y_Offset (double value)        { y_offset = value; }
	void X_Factor (double value)        { x_factor = value; }
	void Y_Factor (double value)        { y_factor = value; }

	Projection_Type  Type (void)        { return (type); }
	Units_Type       Units (void)       { return (units); }
	int              Code (void)        { return (code); }
	double           X_Offset (void)    { return (x_offset); }
	double           Y_Offset (void)    { return (y_offset); }
	double           X_Factor (void)    { return (x_factor); }
	double           Y_Factor (void)    { return (y_factor); }

	bool Projection_System (String key);
	bool Projection_Adjustment (String key);

private:
	Projection_Type type;
	Units_Type units;

	int code;
	double x_offset, y_offset, x_factor, y_factor;
};

#endif
