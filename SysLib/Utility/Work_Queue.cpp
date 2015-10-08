//*********************************************************
//	Work_Queue.cpp - work index queue class
//*********************************************************

#include "Work_Queue.hpp"

#ifdef BOOST_THREADS
//---------------------------------------------------------
//	Work_Queue constructor/destructor
//---------------------------------------------------------

Work_Queue::Work_Queue (int max_records) 
{
	queue = 0;
	exit_flag = end_flag = finish_flag = false;
	num_in = num_out = 0;
	Max_Records (max_records);
}

void Work_Queue::operator()()
{
	boost::mutex::scoped_lock lock (work_mutex);
	exit_wait.wait (lock);
}

//---------------------------------------------------------
//	Put_Work
//---------------------------------------------------------

void Work_Queue::Put_Work (int index) 
{
	boost::mutex::scoped_lock lock (work_mutex);

	while (num_records == max_records) {
		work_full.wait (lock);
	}
	num_in++;
	num_records++;
	queue [last++] = index;

	if (last >= max_records) last = 0;

	work_empty.notify_one ();
}

//---------------------------------------------------------
//	Get_Work
//---------------------------------------------------------

int Work_Queue::Get_Work (void) 
{
	while (1) {
		boost::mutex::scoped_lock lock (work_mutex);

		while (!exit_flag && !end_flag && num_records == 0) {
			work_empty.wait (lock);
		}
		if (num_records == 0) {
			if (exit_flag) {
				work_empty.notify_all ();
				return (-1);
			}
			if (end_flag) {
				end_flag = false;
				end_wait.notify_one ();
				continue;
			}
		}
		num_records--;

		int index = queue [first++];

		if (first >= max_records) first = 0;

		work_full.notify_one ();
		return (index);
	}
}

//--------------------------------------------------------
//	Finished
//--------------------------------------------------------

void Work_Queue::Finished (void)
{
	boost::mutex::scoped_lock lock (work_mutex);

	num_out++;
	if (finish_flag && num_in == num_out) {
		data_wait.notify_one ();
	}
}

//--------------------------------------------------------
//	End_of_Work
//--------------------------------------------------------

void Work_Queue::End_of_Work (void)
{
	boost::mutex::scoped_lock lock (work_mutex);

	if (num_records > 0) {
		end_flag = true;
		end_wait.wait (lock);
	}
	end_flag = false;
}

//--------------------------------------------------------
//	Complete_Work
//--------------------------------------------------------

void Work_Queue::Complete_Work (void)
{
	End_of_Work ();

	boost::mutex::scoped_lock lock (work_mutex);
	
	while (num_in > num_out) {
		finish_flag = true;
		data_wait.wait (lock);
	}
	num_in = num_out = 0;
	finish_flag = false;
}

//---------------------------------------------------------
//	Exit_Queue
//---------------------------------------------------------

void Work_Queue::Exit_Queue (void)
{ 
	boost::mutex::scoped_lock lock (work_mutex);
	exit_flag = true;
	work_empty.notify_all ();
}


//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Work_Queue::Clear (void)
{
	if (queue != 0) {
		delete [] queue;
		queue = 0;
	}
	max_records = num_records = 0;
	first = last = -1;
	num_in = num_out = 0;
}

//---------------------------------------------------------
//	Max_Records 
//---------------------------------------------------------

bool Work_Queue::Max_Records (int max_rec) 
{
	Clear ();
	if (max_rec > 0) {
		queue = new int [max_rec];
		if (queue == 0) return (false);
		max_records = max_rec;
	}
	return (true);
}
#endif
