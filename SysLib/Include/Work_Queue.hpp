//*********************************************************
//	Work_Queue.hpp - work index queue class
//*********************************************************

#ifdef BOOST_THREADS
#ifndef WORK_QUEUE_HPP
#define WORK_QUEUE_HPP

#include "APIDefs.hpp"

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

//---------------------------------------------------------
//	Work_Queue class
//---------------------------------------------------------

class SYSLIB_API Work_Queue
{
public:
	Work_Queue (int max_records = 200);
	~Work_Queue (void)                    { Clear (); }

	void operator()();

	void Put_Work (int index);
	int  Get_Work (void);
	void Finished (void);

	void End_of_Work (void);
	void Complete_Work (void);

	void Exit_Queue (void);
	void Clear (void);
	bool Max_Records (int max_rec);

private:
	bool exit_flag, end_flag, finish_flag;
	int *queue, num_records, max_records, first, last, num_in, num_out;

	boost::condition_variable  work_full, work_empty, exit_wait, end_wait, data_wait;
	boost::mutex  work_mutex;
};
#endif
#endif
