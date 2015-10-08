//*********************************************************
//	Simulator_Base.cpp - simulator thread/slave manager
//*********************************************************

#include "Simulator_Base.hpp"
#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Simulator_Base constructor
//---------------------------------------------------------

Simulator_Base::Simulator_Base (void) : Simulator_Service ()
{
	Service_Level (SIMULATOR_BASE);

#ifdef BOOST_THREADS
	Enable_Threads (true);
#endif
	simulator = 0;
	step_preps = 0;
	boundaries = 0;
	processors = 0;
	sim_read_plans.Initialize (this);

	problem_output.Initialize (this);
	snapshot_output.Initialize (this);
	link_delay_output.Initialize (this);
	performance_output.Initialize (this);
	ridership_output.Initialize (this);
	turn_vol_output.Initialize (this);
	occupancy_output.Initialize (this);
	event_output.Initialize (this);
	traveler_output.Initialize (this);
}

//---------------------------------------------------------
//	Simulator_Base destructor
//---------------------------------------------------------

Simulator_Base::~Simulator_Base (void)
{
	if (simulator != 0) {
		for (int i=0; i < Num_Simulators (); i++) {
			delete simulator [i];
			simulator [i] = 0;
		}
		delete simulator;
		simulator = 0;
	}
	int num = (Num_Threads () > Num_Simulators ()) ? Num_Simulators () : Num_Threads ();

	if (step_preps != 0) {
		for (int i=0; i < num; i++) {
			delete step_preps [i];
			step_preps [i] = 0;
		}
		delete step_preps;
		step_preps = 0;
	}
	if (boundaries != 0) {
		for (int i=0; i < num; i++) {
			delete boundaries [i];
			boundaries [i] = 0;
		}
		delete boundaries;
		boundaries = 0;
	}
	if (processors != 0) {
		for (int i=0; i < num; i++) {
			delete processors [i];
			processors [i] = 0;
		}
		delete processors;
		processors = 0;
	}
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator_Base::Program_Control (void)
{
	Simulator_Service::Program_Control ();

	//---- problem file ----

	problem_output.Read_Control ();

	//---- snapshot keys ----

	snapshot_output.Read_Control ();

	//---- link delay keys ----

	link_delay_output.Read_Control ();

	//---- performance keys ----

	performance_output.Read_Control ();

	//---- turn volume keys ----

	turn_vol_output.Read_Control ();

	//---- ridership keys ----

	ridership_output.Read_Control ();

	//---- occupancy keys ----

	occupancy_output.Read_Control ();

	//---- event keys ----

	event_output.Read_Control ();

	//---- traveler keys ----

	traveler_output.Read_Control ();
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Simulator_Base::Execute (void)
{
	Simulator_Service::Execute ();
}

//---------------------------------------------------------
//	Start_Simulator
//---------------------------------------------------------

bool Simulator_Base::Start_Simulator (void)
{
	int num;
	Int2_Map_Itr part_itr;
	Int2_Map_Data part_rec;
	Int2_Array int2_array;

	//---- read the first travel plan ----

	if (Master ()) {
		if (!sim_read_plans.First_Plan ()) return (false);
	}

	//---- create simulators ----

	num = Last_Part () - First_Part () + 1;
	Num_Simulators (num);

	simulator = new Sim_Method * [num];

	if (Num_Parts () > 1) {
		for (part_itr = part_map.begin (); part_itr != part_map.end (); part_itr++) {
			if (part_itr->second >= First_Part () && part_itr->second <= Last_Part ()) {
				part_rec = *part_itr;
				num = part_itr->second - First_Part ();
				simulator [num] = new Sim_Method (*this, &part_rec);
			}
		}
	} else {
		*simulator = new Sim_Method (*this);
	}

	//---- create the partition lists ----

	work_step.Num_Workers (Num_Simulators ());
	transfers.assign (Num_Simulators (), int2_array);

	if (Num_Threads () == 1) {
		work_step.Num_Barriers (0);
		xfer_barrier.Num_Barriers (0);
		veh_barrier.Num_Barriers (0);
	} else {
		xfer_barrier.Num_Barriers (Num_Simulators ());
		veh_barrier.Num_Barriers (Num_Simulators ());
	}

	//---- create output threads ----

 #ifdef BOOST_THREADS
	if (Num_Threads () > 1) {
		int num_barrier = (int) snapshot_output.Output_Flag () + (int) link_delay_output.Output_Flag () + 
			(int) performance_output.Output_Flag () + (int) ridership_output.Output_Flag () + 
			(int) turn_vol_output.Output_Flag () + (int) occupancy_output.Output_Flag ();

		if (Master ()) {
			num_barrier++;
		}
#ifdef MPI_EXE
		if (problem_output.Output_Flag ()) num_barrier++;
		if (event_output.Output_Flag ()) num_barrier++;
		if (traveler_output.Output_Flag ()) num_barrier++;
#endif
		io_barrier.Num_Barriers (num_barrier);

		if (Master ()) {
			io_threads.create_thread (boost::ref (sim_read_plans));
		}
		if (snapshot_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (snapshot_output));
		}
		if (link_delay_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (link_delay_output));
		}
		if (performance_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (performance_output));
		}
		if (ridership_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (ridership_output));
		}
		if (turn_vol_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (turn_vol_output));
		}
		if (occupancy_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (occupancy_output));
		}

		//---- not barrier controlled unless MPI ----

		if (problem_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (problem_output));
		}
		if (event_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (event_output));
		}
		if (traveler_output.Output_Flag ()) {
			io_threads.create_thread (boost::ref (traveler_output));
		}
	}
#endif

	//---- processing threads ---

	num = (Num_Threads () > Num_Simulators ()) ? Num_Simulators () : Num_Threads ();

	step_preps = new Step_Preps * [num];
	boundaries = new Boundaries * [num];
	processors = new Processors * [num];

	for (int i=0; i < num; i++) {
		step_preps [i] = new Step_Preps (this);
		boundaries [i] = new Boundaries (this);
		processors [i] = new Processors (this);

#ifdef BOOST_THREADS
		if (Num_Threads () > 1) {
			sim_threads.create_thread (boost::ref (*step_preps [i]));
			sim_threads.create_thread (boost::ref (*boundaries [i]));
			sim_threads.create_thread (boost::ref (*processors [i]));
		}
#endif
	}
	return (true);
}

//---------------------------------------------------------
//	Stop_Simulator
//---------------------------------------------------------

void Simulator_Base::Stop_Simulator (void)
{
	Sim_Method *sim_ptr;

#ifdef BOOST_THREADS
	if (Num_Threads () > 1) {
		work_step.Exit ();
		xfer_barrier.Exit ();
		veh_barrier.Exit ();
		sim_threads.join_all ();

		if (problem_output.Output_Flag ()) {
			problem_output.End_Output ();
		}
		if (event_output.Output_Flag ()) {
			event_output.End_Output ();
		}
		if (traveler_output.Output_Flag ()) {
			traveler_output.End_Output ();
		}
		if (io_threads.size () > 0) {
			io_barrier.Exit ();
			io_threads.join_all ();
		}
	}
#endif
	for (int i=0; i < Num_Simulators (); i++) {
		sim_ptr = simulator [i];
		Add_Statistics (sim_ptr->Get_Statistics ());
	}
}

//---------------------------------------------------------
//	Start_Step
//---------------------------------------------------------

bool Simulator_Base::Start_Step (void)
{
	int i;
	bool status;
	Num_Vehicles (0);

	memset (veh_status, '\0', vehicle_array.size ());

	//---- update network, add/transfer travelers, process vehicles after partition boundaries ----

	if (Num_Threads () > 1) {
		status = work_step.Start ();
	} else {
		for (i=0, status = false; i < Num_Simulators (); i++) {
			if (simulator [i]->Step_Prep (work_step.Get (i))) status = true;
		}
	}
	if (status) {

		//---- process vehicles approaching partition boundaries ----

		if (Num_Threads () > 1) {
			xfer_barrier.Start ();
		} else {
			for (i=0; i < Num_Simulators (); i++) {
				simulator [i]->Boundary ();
			}
		}

		//---- process everyone else ----

		if (Num_Threads () > 1) {
			veh_barrier.Start ();
		} else {
			for (i=0; i < Num_Simulators (); i++) {
				simulator [i]->Process_Step ();
			}
		}

		int traveler, part, num;
		Int2s_Itr transfer_itr;
		Int2_Itr xfer_itr;

		for (num=0, transfer_itr = transfers.begin (); transfer_itr != transfers.end (); transfer_itr++, num++) {
			for (xfer_itr = transfer_itr->begin (); xfer_itr != transfer_itr->end (); xfer_itr++) {
				traveler = xfer_itr->first;
				part = xfer_itr->second;
				if (part >= First_Part () && part <= Last_Part ()) {
					work_step.Put (traveler, (part - First_Part ()));
#ifdef MPI_EXE
				} else {
					mpi_parts [part].push_back (xfer_itr->first);
#endif
				}
			}
		}
		for (i=0; i < Num_Simulators (); i++) {
			Add_Vehicles (simulator [i]->Num_Vehicles ());
		}
	}
	return (status);
}

//---------------------------------------------------------
//	Step_Preps operator
//---------------------------------------------------------

void Simulator_Base::Step_Preps::operator()()
{
	int part;
	Integers *new_trips;

	for (;;) {
		new_trips = exe->work_step.Next (part);
		if (new_trips == 0) break;

		exe->work_step.Result (exe->simulator [part]->Step_Prep (new_trips));
	}
}

//---------------------------------------------------------
//	Boundaries operator
//---------------------------------------------------------

void Simulator_Base::Boundaries::operator()()
{
	int part;

	while (exe->xfer_barrier.Go (part)) {
		exe->simulator [part]->Boundary ();
		exe->xfer_barrier.Result ();
	}
}

//---------------------------------------------------------
//	Processors operator
//---------------------------------------------------------

void Simulator_Base::Processors::operator()()
{
	int part;

	while (exe->veh_barrier.Go (part)) {
		exe->simulator [part]->Process_Step ();
		exe->veh_barrier.Result ();
	}
}

//---------------------------------------------------------
//	Step_IO
//---------------------------------------------------------

bool Simulator_Base::Step_IO (void)
{
	if (Num_Threads () > 1) {
		return (io_barrier.Start ());
	} else {
		snapshot_output.Output_Check ();
		link_delay_output.Output_Check ();
		performance_output.Output_Check ();
		ridership_output.Output_Check ();
		turn_vol_output.Output_Check ();
		occupancy_output.Output_Check ();
#ifdef MPI_EXE
		problem_output.MPI_Processing ();
		event_output.MPI_Processing ();
		traveler_output.MPI_Processing ();
#endif
		if (Master ()) {
			return (sim_read_plans.Read_Plans ());
		} else {
			return (true);
		}
	}
}

//---------------------------------------------------------
//	Check_Output
//---------------------------------------------------------

void Simulator_Base::Check_Output (Travel_Step &step)
{
	if (step.Vehicle () < 0) return;

	//---- link delay processing ----

	if (link_delay_output.Output_Flag ()) {
		link_delay_output.Summarize (step);
	}

	//----- performance processing -----

	if (performance_output.Output_Flag ()) {
		performance_output.Summarize (step);
	}

	//----- turn volume processing -----

	if (turn_vol_output.Output_Flag ()) {
		turn_vol_output.Summarize (step);
	}

	//----- occupancy processing -----

	if (occupancy_output.Output_Flag ()) {
		occupancy_output.Summarize (step);
	}
}

//---------------------------------------------------------
//	Active_Vehicles
//---------------------------------------------------------

bool Simulator_Base::Active_Vehicles (void)
{
	Int_List *list;
	Int_List_Itr list_itr;

	active_vehicles.clear ();

	for (int i=0; i < Num_Simulators (); i++) {
		list = simulator [i]->Active_Vehicles ();

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			active_vehicles.push_back (*list_itr);
		}
	}
	return (!active_vehicles.empty ());
}
