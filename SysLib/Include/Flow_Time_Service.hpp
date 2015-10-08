//*********************************************************
//	Flow_Time_Service.hpp - flow-time service keys and data
//*********************************************************

#ifndef FLOW_TIME_SERVICE_HPP
#define FLOW_TIME_SERVICE_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Equation.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Flow_Service - flow data class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Service
{
public:

	Flow_Time_Service (void);

	enum Flow_Time_Service_Keys { 
		 UPDATE_FLOW_RATES = FLOW_TIME_SERVICE_OFFSET, UPDATE_TURNING_MOVEMENTS, CLEAR_INPUT_FLOW_RATES, 
		 UPDATE_TRAVEL_TIMES, LINK_DELAY_UPDATE_RATE, LINK_DELAY_FLOW_FACTOR, 
		 EQUATION_PARAMETERS
	};

	void Flow_Time_Service_Keys (int *keys = 0);

	void Build_Flow_Time_Arrays (Flow_Time_Period_Array &link_array, Flow_Time_Period_Array &turn_array);
	void Build_Flow_Time_Arrays (void) { Build_Flow_Time_Arrays (dat->link_delay_array, dat->turn_delay_array); }
	
	void Build_Turn_Arrays (Flow_Time_Period_Array &turn_array);
	void Build_Turn_Arrays (void)      { Build_Turn_Arrays (dat->turn_delay_array); }

	bool Flow_Updates (void)         { return (flow_updates); }
	void Flow_Updates (bool flag)    { flow_updates = flag; }

	bool Turn_Updates (void)         { return (turn_updates); }
	void Turn_Updates (bool flag)    { turn_updates = flag; }

	bool Time_Updates (void)         { return (time_updates); }
	void Time_Updates (bool flag)    { time_updates = flag; }

protected:

	void Read_Flow_Time_Keys (void);

	void Update_Travel_Times (int mpi_size = 1);

	bool flow_updates, turn_updates, time_updates, first_update;
	int update_rate;
	double flow_factor;

	Equation_Array equation;

private:

};

#endif
