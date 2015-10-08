//*********************************************************
//	Simulator.hpp - Simulate Travel Plans
//*********************************************************

#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "APIDefs.hpp"
#include "Simulator_Base.hpp"

//---------------------------------------------------------
//	Simulator - execution class definition
//---------------------------------------------------------

class Simulator : public Simulator_Base
{
public:

	Simulator (void);

	virtual void Execute (void);

protected:
#ifdef DEBUG_KEYS
	enum Simulator_Keys { 
		DEBUG_TIME_RANGE = 1, DEBUG_VEHICLE_LIST, DEBUG_LINK_LIST,
	};
#endif
	virtual void Program_Control (void);

private:
	//enum Simulator_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	bool step_flag, count_flag;

	Dtime one_second, one_minute, one_hour, max_time;
	Dtime signal_update_time, timing_update_time, transit_update_time;

	int lane_change_levels, max_vehicles;

	Sim_Parameters  param;
	
	Plan_File *plan_file;

	void MPI_Setup (void);
	bool MPI_Distribute (bool status);
	bool MPI_Transfer (bool status);
	void MPI_Close (void);
};
#endif
