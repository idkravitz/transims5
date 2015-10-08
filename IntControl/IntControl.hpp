//*********************************************************
//	IntControl.hpp - Intersection Control Utility
//*********************************************************

#ifndef INTCONTROL_HPP
#define INTCONTROL_HPP

#include "Data_Service.hpp"
#include "Best_List.hpp"
#include "Db_Array.hpp"
#include "Turn_Vol_File.hpp"
#include "List_Data.hpp"
#include "TypeDefs.hpp"

#define MAX_PHASE_NUM	20
#define MAX_DIR_NUM		5
#define MAX_DIR_TOT		20

//---------------------------------------------------------
//	IntControl - execution class definition
//---------------------------------------------------------

class IntControl : public Data_Service
{
public:
	IntControl (void);
	virtual ~IntControl (void);

	virtual void Execute (void);

protected:
	enum IntControl_Keys { 
		INPUT_SIGN_FILE = 1, INPUT_SIGNAL_FILE, DELETE_NODE_CONTROL_FILE, 
		PRINT_SIGN_WARNINGS, PRINT_MERGE_WARNINGS, SIGNAL_TYPE_CODE, NUMBER_OF_RINGS, 
		SIGNAL_TIME_BREAKS, SIGNAL_CYCLE_LENGTH, MINIMUM_PHASE_TIME, YELLOW_PHASE_TIME, 
		RED_CLEAR_PHASE_TIME, SIGNAL_DETECTOR_LENGTH, 
		POCKET_LANE_FACTOR, SHARED_LANE_FACTOR, TURN_MOVEMENT_FACTOR, PERMITTED_LEFT_FACTOR,
		GENERAL_GREEN_FACTOR, EXTENDED_GREEN_FACTOR, MAXIMUM_GREEN_FACTOR, SIGNAL_SPLIT_METHOD, 
		MINIMUM_LANE_CAPACITY, MAXIMUM_LANE_CAPACITY, TURN_VOLUME_FILE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum IntControl_Reports { TIMING_UPDATE = 1 };

	bool merge_flag, delay_flag, range_flag, signal_flag, sign_flag, delete_flag, update_flag;
	bool no_control_flag, warning_flag, input_sign_flag, input_signal_flag, time_flag, turn_flag;
	bool detector_flag, coord_flag, update_report, regen_flag, volume_flag;
	int num_new, num_update, nsign, ntiming, nsignal, nphasing, ndetector;

	Sign_File sign_file;
	Signal_File signal_file;
	Db_File delete_file;

	Int_Set update_signal;
	List_Array node_list;

	Turn_Vol_File turn_file;

	typedef struct {
		Integer_List signal_type;
		Integer_List num_rings;
		Time_Periods time_breaks;
		Integer_List cycle_len;
		Integer_List yellow;
		Integer_List red_clear;
		Integer_List min_phase;
		Integer_List method;
		Integer_List min_cap;
		Integer_List max_cap;
		Double_List  pocket_factor;
		Double_List  shared_factor;
		Double_List  turn_factor;
		Double_List  permit_factor;
		Double_List  general_factor;
		Double_List  extend_factor;
		Double_List  max_factor;
		int          detector;
	} Signal_Group;

	typedef map <int, Signal_Group>    Group_Map;
	typedef pair <int, Signal_Group>   Group_Map_Data;
	typedef Group_Map::iterator        Group_Map_Itr;
	typedef pair <Group_Map_Itr, bool> Group_Map_Stat;

	Group_Map group_map;

	void Node_Links (void);
	void Read_Deletes (void);
	void Read_Turn (void);
	void Read_Sign (void);
	void Read_Signal (void);
	void Create_Signals (void);
	void Update_Signals (void);
	void Update_Header (void);
};
#endif

