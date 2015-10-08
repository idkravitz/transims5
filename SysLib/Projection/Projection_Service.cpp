//*********************************************************
//	Projection_Service.cpp - Coordinate Projection Service
//*********************************************************

#include "Projection_Service.hpp"

#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Projection_Service constructor
//---------------------------------------------------------

Projection_Service::Projection_Service (void) : Static_Service ()
{
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Projection_Service::Add_Keys (void)
{
	String system_range ("LATLONG, DEGREES/MILLION_DEGREES or STATEPLANE/UTM, code, FEET/METERS/MILES/KILOMETERS");
	String adjust_range ("X Offset, Y Offset, X Factor, Y Factor");

	Control_Key projection_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ INPUT_COORDINATE_SYSTEM, "INPUT_COORDINATE_SYSTEM", LEVEL0, OPT_KEY, LIST_KEY, "", system_range, NO_HELP },
		{ INPUT_COORDINATE_ADJUSTMENT, "INPUT_COORDINATE_ADJUSTMENT", LEVEL0, OPT_KEY, LIST_KEY, "", adjust_range, NO_HELP },
		{ OUTPUT_COORDINATE_SYSTEM, "OUTPUT_COORDINATE_SYSTEM", LEVEL0, OPT_KEY, LIST_KEY, "", system_range, NO_HELP },
		{ OUTPUT_COORDINATE_ADJUSTMENT, "OUTPUT_COORDINATE_ADJUSTMENT", LEVEL0, OPT_KEY, LIST_KEY, "", adjust_range, NO_HELP },
		{ OUTPUT_XYZ_SHAPES, "OUTPUT_XYZ_SHAPES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ OUTPUT_XYM_SHAPES, "OUTPUT_XYM_SHAPES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	if (exe != 0) {
		exe->Key_List (projection_keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Projection_Service::Read_Control (void)
{
	if (exe == 0) return (false);

	String key;
	bool first = true;
	bool defined = false;

	//---- input coordination system ----

	if (exe->Check_Control_Key (INPUT_COORDINATE_SYSTEM)) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		if (!input.Projection_System (exe->Get_Control_Text (INPUT_COORDINATE_SYSTEM))) {
			exe->Control_Key_Error (INPUT_COORDINATE_SYSTEM);
		}
		defined = true;
	}
	if (exe->Check_Control_Key (INPUT_COORDINATE_ADJUSTMENT)) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		if (!input.Projection_Adjustment (exe->Get_Control_Text (INPUT_COORDINATE_ADJUSTMENT))) {
			exe->Control_Key_Error (INPUT_COORDINATE_ADJUSTMENT);
		}
		defined = true;
	}

	//---- output coordination system ----

	if (exe->Check_Control_Key (OUTPUT_COORDINATE_SYSTEM)) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		if (!output.Projection_System (exe->Get_Control_Text (OUTPUT_COORDINATE_SYSTEM))) {
			exe->Control_Key_Error (OUTPUT_COORDINATE_SYSTEM);
		}
		defined = true;
	}
	if (exe->Check_Control_Key (OUTPUT_COORDINATE_ADJUSTMENT)) {
		if (first) {
			exe->Print (1);
			first = false;
		}
		if (!output.Projection_Adjustment (exe->Get_Control_Text (OUTPUT_COORDINATE_ADJUSTMENT))) {
			exe->Control_Key_Error (OUTPUT_COORDINATE_ADJUSTMENT);
		}
		defined = true;
	}

	//---- coordinate flags ----

	m_flag = exe->Get_Control_Flag (OUTPUT_XYM_SHAPES);
	z_flag = (m_flag || exe->Get_Control_Flag (OUTPUT_XYZ_SHAPES));

	return (defined);
}
