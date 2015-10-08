//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Link_Data_Control
//---------------------------------------------------------

void LinkSum::Set_Link_Data (Data_Group *group)
{
	switch (group->field) {
		case VC_DATA:
		case RATIO_DATA:
		case QUEUE_DATA:
		case DENSITY_DATA:
		case MAX_DENSITY_DATA:
		case CONG_TIME_DATA:
			group->file->Num_Decimals (2);
			break;
		case FLOW_DATA:
		case TTIME_DATA:
		case SPEED_DATA:
		case DELAY_DATA:
		case VMT_DATA:
		case VHT_DATA:
		case CONG_VMT_DATA:
		case CONG_VHT_DATA:
			group->file->Num_Decimals (1);
			break;
		default:
			group->file->Num_Decimals (0);
			break;
	}
	group->file->Data_Units (Performance_Units_Map ((Performance_Type) group->field));
	group->file->Copy_Periods (sum_periods);

	group->file->Create_Fields ();
	group->file->Write_Header ();
}
