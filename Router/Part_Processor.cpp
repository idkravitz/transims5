//*********************************************************
//	Part_Processor.cpp - partition processing thread
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Part_Processor constructor
//---------------------------------------------------------

Router::Part_Processor::Part_Processor (void)
{
	exe = 0;
	num_processors = num_path_builders = 0;

#ifdef BOOST_THREADS
	trip_queue = 0;
	part_thread = 0;
#endif
}

//---------------------------------------------------------
//	Part_Processor destructor
//---------------------------------------------------------

Router::Part_Processor::~Part_Processor (void)
{
#ifdef BOOST_THREADS
	if (trip_queue != 0) {
		for (int i=0; i < num_processors; i++) {
			if (trip_queue [i] != 0) delete trip_queue [i];
		}
		delete trip_queue;
	}
	if (part_thread != 0) {
		for (int i=0; i < num_processors; i++) {
			if (part_thread [i] != 0) delete part_thread [i];
		}
		delete part_thread;
	}
#endif
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Router::Part_Processor::Initialize (Router *_exe)
{
	exe = _exe;
	if (exe == 0) return (false);

	num_path_builders = exe->Num_Threads ();

#ifdef BOOST_THREADS

	//---- allocate threads ----

	if (exe->Num_Threads () > 1) {
		if (exe->Num_Partitions () < 2) {
			num_processors = 1;
			if (exe->Num_Threads () > 2) {
				if (exe->Num_Threads () > 4) {
					num_path_builders = exe->Num_Threads () - 1;
				} else {
					num_path_builders = exe->Num_Threads ();
				}
			} else {
				num_path_builders = 2;
			}
		} else if (exe->Num_Threads () >= exe->Num_Partitions ()) {
			num_processors = exe->Num_Partitions ();
			num_path_builders = exe->Num_Threads () / num_processors;
			if (num_path_builders < 2) {
				if ((num_processors % 2) == 0) {
					num_processors /= 2;
					num_path_builders = 2;
				} else {
					num_path_builders = 1;
				}
			}
		} else if (exe->trip_set_flag) {
			num_processors = exe->Num_Threads ();
			num_path_builders = 1;
		} else if (exe->Num_Threads () > 4) {
			num_processors = exe->Num_Threads () / 3;
			if (num_processors < 2) num_processors = 2;
			num_path_builders = exe->Num_Threads () / num_processors;
			if (num_path_builders < 2) num_path_builders = 2;
		} else {
			num_processors = 1;
			num_path_builders = exe->Num_Threads ();
		}
		exe->Write (2, "Number of File Partition Processors = ") << num_processors;
		exe->Write (1, "Number of Path Builders per Process = ") << num_path_builders;
		exe->Write (1);
	} else {
		num_processors = 1;
		num_path_builders = 1;
	}

	//---- create processing processors ----

	if (num_processors > 1) {
		int i, j;
		part_thread = new Part_Thread * [num_processors];

		for (i=0; i < num_processors; i++) {
			part_thread [i] = new Part_Thread (i, this);
		}
		if (exe->trip_gap_map_flag) {
			exe->trip_gap_map_array.Initialize (exe->Num_Partitions ());
		}
		if (exe->trip_flag && !exe->trip_set_flag) {
			for (i=j=0; i < exe->Num_Partitions (); i++, j++) {
				if (j == num_processors) j = 0;
				partition_map.push_back (j);
			}
			trip_queue = new Trip_Queue * [num_processors];

			for (i=0; i < num_processors; i++) {
				trip_queue [i] = new Trip_Queue ();
			}
			return (false);
		}
		return (true);
	} 
#endif
	plan_processor.Initialize (exe, num_path_builders);
	return (false);
}

//---------------------------------------------------------
//	Part_Processor -- Read_Trips
//---------------------------------------------------------

void Router::Part_Processor::Read_Trips (void)
{
	int p;

#ifdef BOOST_THREADS
	if (num_processors > 1) {
		if (exe->thread_flag) {
			partition_queue.Reset ();

			for (p=0; p < exe->num_file_sets; p++) {
				partition_queue.Put (p);
			}
			partition_queue.End_of_Queue ();

			for (p=0; p < num_processors; p++) {
				threads.create_thread (boost::ref (*(part_thread [p])));
			}
			threads.join_all ();
		} else {
			for (p=0; p < num_processors; p++) {
				threads.create_thread (boost::ref (*(part_thread [p])));
			}
			exe->Read_Trips (0);

			for (p=0; p < num_processors; p++) {
				trip_queue [p]->End_Queue ();
				trip_queue [p]->Exit_Queue ();
			}
			threads.join_all ();
		}
		if (exe->Flow_Updates () || exe->Time_Updates ()) {
			for (p=0; p < num_processors; p++) {
				part_thread [p]->plan_processor.Save_Flows ();
			}
		}
		return;
	}

#endif
	plan_processor.Start_Processing ();

	if (exe->trip_set_flag) {
		for (p=0; p < exe->num_file_sets; p++) {
			exe->Read_Trips (p, &plan_processor);
		}
	} else {
		exe->Read_Trips (0, &plan_processor);
	}
	plan_processor.Stop_Processing ();

	if (exe->Flow_Updates () || exe->Time_Updates ()) {
		plan_processor.Save_Flows ();
	}
}

//---------------------------------------------------------
//	Part_Processor -- Copy_Plans
//---------------------------------------------------------

void Router::Part_Processor::Copy_Plans (void)
{
	int p;

#ifdef BOOST_THREADS
	if (num_processors > 1) {
		for (p=0; p < exe->num_file_sets; p++) {
			partition_queue.Put (p);
		}
		partition_queue.End_of_Queue ();

		for (p=0; p < num_processors; p++) {
			threads.create_thread (boost::ref (*(part_thread [p])));
		}
		threads.join_all ();

		if (exe->Flow_Updates () || exe->Time_Updates ()) {
			for (p=0; p < num_processors; p++) {
				part_thread [p]->plan_processor.Save_Flows ();
			}
		}
		return;
	}
#endif
	plan_processor.Start_Processing ();

	if (exe->plan_set_flag) {
		for (p=0; p < exe->num_file_sets; p++) {
			exe->Copy_Plans (p, &plan_processor);
		}
	} else {
		for (p=0; ; p++) {
			if (!exe->plan_file->Open (p)) break;
			if (exe->new_plan_flag) exe->new_plan_file->Open (p);
			exe->Copy_Plans (p, &plan_processor);
		}
	}
	plan_processor.Stop_Processing ();

	if ((exe->Flow_Updates () || exe->Time_Updates ()) && exe->System_File_Flag (NEW_LINK_DELAY)) {
		plan_processor.Save_Flows ();
	}
}

//---------------------------------------------------------
//	Part_Processor -- Plan_Build
//---------------------------------------------------------

void Router::Part_Processor::Plan_Build (int partition, Plan_Ptr_Array *plan_ptr_array)
{
#ifdef BOOST_THREADS
	partition = partition_map [partition];
	trip_queue [partition]->Put (plan_ptr_array);
#else
	delete plan_ptr_array;
	partition = 0;
#endif
}

#ifdef BOOST_THREADS
//---------------------------------------------------------
//	Part_Thread constructor
//---------------------------------------------------------

Router::Part_Processor::Part_Thread::Part_Thread (int num, Part_Processor *_ptr)
{
	ptr = _ptr;
	number = num;

	if (ptr) plan_processor.Initialize (ptr->exe, ptr->num_path_builders);
}

//---------------------------------------------------------
//	Part_Thread operator
//---------------------------------------------------------

void Router::Part_Processor::Part_Thread::operator()()
{
	int part;
	plan_processor.Start_Processing ();

	if (ptr->exe->thread_flag) {
		while (ptr->partition_queue.Get (part)) {
			if (ptr->exe->trip_flag) {
				ptr->exe->Read_Trips (part, &plan_processor);
			} else {
				ptr->exe->Copy_Plans (part, &plan_processor);
			}		
		}
	} else {
		Plan_Ptr_Array *plan_ptr_array;
		Trip_Queue *queue = ptr->trip_queue [number];
		queue->Reset ();

		while (queue->Get (plan_ptr_array)) {
			plan_processor.Plan_Build (plan_ptr_array);
		}
	}
	plan_processor.Stop_Processing ();
}
#endif
