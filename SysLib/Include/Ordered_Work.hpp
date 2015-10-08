//*********************************************************
//	Ordered_Work.hpp - ordered work queue template class
//*********************************************************

#ifdef BOOST_THREADS
#ifndef ORDERED_WORK_HPP
#define ORDERED_WORK_HPP

//---------------------------------------------------------
//	Ordered_Work -- template class definition
//---------------------------------------------------------
//	Ordered_Work <Work, Result> (int max_records = 200);
//
//	bool Put_Work (Work *data);
//  Work * Get_Work (int &number);
//
//	bool Put_Result (Result *data, int number);
//  Result * Get_Result (void);
//  void Result_Done (void);
//
//	void End_of_Work (void);
//  void Complete_Work (void);
//
//	bool Max_Records (int max_records);
//---------------------------------------------------------

#include "Ordered_Queue.hpp"

#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//---------------------------------------------------------
//	Ordered_Work template
//---------------------------------------------------------

template <typename Work, typename Result>
class Ordered_Work : private boost::noncopyable
{
public:
	//---------------------------------------------------------
	//	Ordered_Work constructor
	//---------------------------------------------------------

	Ordered_Work (int max_records = 200) 
	{
		Max_Records (max_records);
		Start_Work ();
	}

	void operator()()
	{
		boost::mutex::scoped_lock lock (exit_mutex);
		exit_wait.wait (lock);
	}

	//--------------------------------------------------------
	//	Start_Work
	//--------------------------------------------------------

	void Start_Work (void)
	{
		end_of_work = work_done = end_of_data = data_done = false;
		num_in = num_out = 0;
	}

	//---------------------------------------------------------
	//	Put_Work
	//---------------------------------------------------------

	bool Put_Work (Work *work) 
	{
		boost::mutex::scoped_lock lock (work_mutex);

		while (work_queue.Full ()) {
			work_full.wait (lock);
		}
		if (work_queue.Put (work)) {
			num_in++;
			work_empty.notify_one ();
		}
		return (true);
	}

	//---------------------------------------------------------
	//	Get_Work
	//---------------------------------------------------------

	Work * Get_Work (int &number) 
	{
		boost::mutex::scoped_lock lock (work_mutex);

		while (!end_of_work && work_queue.Empty ()) {
			work_empty.wait (lock);
		}
		Work *ptr = work_queue.Get (number);

		if (ptr != 0) {
			work_full.notify_one ();
		} else if (end_of_work) {
			work_empty.notify_all ();
		}
		return (ptr);
	}

	//---------------------------------------------------------
	//	Put_Result
	//---------------------------------------------------------

	bool Put_Result (Result *result, int number) 
	{
		for (;;) {
			boost::mutex::scoped_lock lock (result_mutex);

			while (result_queue.Full () && number >= result_queue.Max_Number ()) {
				result_full.wait (lock);
			}
			if (result_queue.Put (result, number)) {
				result_empty.notify_one ();
				return (true);
			}
		}
	}

	//---------------------------------------------------------
	//	Get_Result
	//---------------------------------------------------------

	Result * Get_Result (void) 
	{
		for (;;) {
			boost::mutex::scoped_lock lock (result_mutex);

			while (!work_done && !data_done && result_queue.Empty ()) {
				result_empty.wait (lock);
			}
			int number;

			Result *ptr = result_queue.Get (number);

			if (ptr != 0) {
				result_full.notify_one ();
				return (ptr);
			} else if (work_done) {
				boost::mutex::scoped_lock lock (exit_mutex);
				exit_wait.notify_one ();
				return (ptr);
			} else if (data_done) {
				boost::mutex::scoped_lock lock (data_mutex);
				data_wait.notify_one ();
			} else {
				result_full.notify_all ();
			}
		}
	}

	//--------------------------------------------------------
	//	Result_Done
	//--------------------------------------------------------

	void Result_Done (void)
	{
		boost::mutex::scoped_lock lock (result_mutex);
		num_out++;
		if (num_in == num_out && (end_of_work || end_of_data)) {
			work_done = end_of_work;
			data_done = end_of_data;
			result_empty.notify_one ();
		}
	}

	//---------------------------------------------------------
	//	End_of_Work
	//---------------------------------------------------------

	void End_of_Work (void)
	{ 
		boost::mutex::scoped_lock lock (work_mutex);
		end_of_work = true;
		work_empty.notify_one ();

		if (num_in == num_out) {
			work_done = true;
			result_empty.notify_one ();
		}
	}

	//--------------------------------------------------------
	//	Complete_Work
	//--------------------------------------------------------

	void Complete_Work (void)
	{
		if (num_in != num_out) {
			boost::mutex::scoped_lock lock (data_mutex);
			end_of_data = true;
			data_wait.wait (lock);
		}
		data_done = end_of_data = false;
	}

	//---------------------------------------------------------
	//	Max_Records 
	//---------------------------------------------------------

	bool Max_Records (int max_records) 
	{
		return (work_queue.Max_Records (max_records) && result_queue.Max_Records (max_records));
	}

	//---------------------------------------------------------
	//	Support Methods
	//---------------------------------------------------------

private:
	bool end_of_work, work_done, end_of_data, data_done;
	int num_in, num_out;

	Ordered_Queue <Work> work_queue;
	Ordered_Queue <Result> result_queue;

	boost::condition_variable  work_full, work_empty, result_full, result_empty;
	boost::condition_variable  exit_wait, data_wait;
	boost::mutex  work_mutex, result_mutex, exit_mutex, data_mutex;
};
#endif
#endif
