//*********************************************************
//	Simulator_Base.hpp - simulator thread/slave manager
//*********************************************************

#ifndef SIMULATOR_BASE_HPP
#define SIMULATOR_BASE_HPP

#include "Simulator_Service.hpp"

#include "Sim_Method.hpp"
#include "Sim_Read_Plans.hpp"
#include "Problem_Output.hpp"
#include "Snapshot_Output.hpp"
#include "Link_Delay_Output.hpp"
#include "Performance_Output.hpp"
#include "Turn_Vol_Output.hpp"
#include "Ridership_Output.hpp"
#include "Occupancy_Output.hpp"
#include "Event_Output.hpp"
#include "Traveler_Output.hpp"
#include "Travel_Step.hpp"

#ifdef BOOST_THREADS
#include <boost/thread/thread.hpp>
#endif

//---------------------------------------------------------
//	Simulator_Base - execution class definition
//---------------------------------------------------------

class SYSLIB_API Simulator_Base : public Simulator_Service
{
	friend class Sim_Read_Plans;
	friend class Problem_Output;
	friend class Simulator_Output;
	friend class Snapshot_Output;
	friend class Link_Delay_Output;
	friend class Performance_Output;
	friend class Ridership_Output;
	friend class Turn_Vol_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;

public:
	Simulator_Base (void);
	virtual ~Simulator_Base (void);

	bool Start_Simulator (void);
	void Stop_Simulator (void);

	bool Start_Step (void);
	bool Step_IO (void);

	virtual void Output_Event (Event_Data &data)         { event_output.Output_Event (data); }
	virtual bool Check_Event (Event_Type type, int mode = DRIVE_MODE, int subarea = 0)
		{ return (event_output.In_Range (type, mode, subarea)); }

	virtual void Output_Traveler (Traveler_Data &data)   { traveler_output.Output_Traveler (data); }
 	virtual bool Check_Traveler (Traveler_Data &data)    { return (traveler_output.In_Range (data)); }
	virtual bool Output_Traveler_Flag (void)             { return (traveler_output.Output_Flag ()); }

	virtual void Output_Problem (Problem_Data &problem)  { problem_output.Output_Problem (problem); }
	virtual void Check_Output (Travel_Step &travel);
	virtual bool Active_Vehicles (void);

protected:
	virtual void Program_Control (void);
	virtual void Execute (void);

	Sim_Read_Plans sim_read_plans;

	//---- output services ----

	Problem_Output problem_output;
	Snapshot_Output snapshot_output;
	Link_Delay_Output link_delay_output;
	Performance_Output performance_output;
	Turn_Vol_Output turn_vol_output;
	Ridership_Output ridership_output;
	Occupancy_Output occupancy_output;
	Event_Output event_output;
	Traveler_Output traveler_output;

private:
	Sim_Method **simulator;

	//---------------------------------------------------------
	//	Step_Preps - partition preparation
	//---------------------------------------------------------

	class Step_Preps
	{
	public:
		Step_Preps (Simulator_Base *_exe) { exe = _exe; }
		void operator()();
	private:
		Simulator_Base *exe;
	};
	Step_Preps **step_preps;

	//---------------------------------------------------------
	//	Boundaries - partition boundaries
	//---------------------------------------------------------

	class Boundaries
	{
	public:
		Boundaries (Simulator_Base *_exe) { exe = _exe; }
		void operator()();
	private:
		Simulator_Base *exe;
	};
	Boundaries **boundaries;

	//---------------------------------------------------------
	//	Processors - partition processors
	//---------------------------------------------------------

	class Processors
	{
	public:
		Processors (Simulator_Base *_exe) { exe = _exe; }
		void operator()();
	private:
		Simulator_Base *exe;
	};
	Processors **processors;

#ifdef BOOST_THREADS
	boost::thread_group sim_threads;
	boost::thread_group io_threads;
#endif	//---- BOOST_THREADS ----
};
#endif
