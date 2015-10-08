//*********************************************************
//	Projection_Data.cpp - Coordinate Projection Data
//*********************************************************

#include "Projection_Data.hpp"

#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Projection_Data constructor
//---------------------------------------------------------

Projection_Data::Projection_Data (void) : Static_Service ()
{
	Type (NO_PROJECTION);
	Code (0);
	Units (NO_UNITS);
	X_Offset (0.0);
	Y_Offset (0.0);
	X_Factor (1.0);
	Y_Factor (1.0);
}

//---------------------------------------------------------
//	Projection_System 
//---------------------------------------------------------

bool Projection_Data::Projection_System (String key)
{
	type = LATLONG;
	code = 0;
	units = DEGREES;

	if (!key.empty ()) {
		String field;

		key.Split (field);

		type = Projection_Code (field);

		if (type == STATEPLANE) {
			units = FEET;
		} else if (type == UTM) {
			units = METERS;
		}
		if (key.Split (field)) {
			if (type == LATLONG) {
				units = Coordinate_Code (field);

				if (units != MILLION_DEG && units != DEGREES) {
					if (exe->Send_Messages ()) {
						exe->Error (String ("Lat-Long Projection with %s Units is Illogical") % field);
					}
					return (false);
				}
				return (true);
			} else {
				if (field [0] < '0' || field [0] > '9') {
					if (exe->Send_Messages ()) {
						exe->Error (String ("%s Projection Code was Not Provided") % Projection_Code (type));
					}
					return (false);
				}
				code = field.Integer ();
			}
		}
		if (!key.empty ()) {
			units = Coordinate_Code (key);
		}
	} else {
		type = NO_PROJECTION;
	}
	return (true);
}

//---------------------------------------------------------
//	Projection_Adjustment 
//---------------------------------------------------------

bool Projection_Data::Projection_Adjustment (String key)
{
	x_offset = 0.0;
	y_offset = 0.0;
	x_factor = 1.0;
	y_factor = 1.0;

	if (!key.empty ()) {
		String field;
		char ch;
		double dvalue;
	
		for (int i=0; i < 4; i++) {
			if (!key.Split (field)) break;

			ch = field [0];
			
			if ((ch < '0' || ch > '9') && ch != '.' && ch != '-' && ch != '+') return (false);

			dvalue = field.Double ();

			switch (i) {
				case 0:
					x_offset = dvalue;
					break;
				case 1:
					y_offset = dvalue;
					break;
				case 2:
					x_factor = dvalue;
					break;
				case 3:
					y_factor = dvalue;
					break;
				default:
					return (false);
			}
		}
	}
	return (true);
}
