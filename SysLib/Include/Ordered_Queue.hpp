//*********************************************************
//	Ordered_Queue.hpp - circular queue of ordered values
//*********************************************************

#ifndef ORDERED_QUEUE_HPP
#define ORDERED_QUEUE_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Ordered_Queue -- template class definition
//---------------------------------------------------------
//	Ordered_Queue <Type> (int max_records = 200);
//
//	bool Put (Type *data, int number = -1);
//
//  Type * Get (int &number);
//
//	bool Full (void);
//	bool Empty (void);
//
//	void Reset (void);
//
//	int  Num_Records (void);
//
//	int  Max_Records (void)
//	bool Max_Records (int max_records);
//---------------------------------------------------------

template <typename Type>
class SYSLIB_API Ordered_Queue
{
	typedef vector <Type *> Type_Array;

public:

	//---------------------------------------------------------
	//	Ordered_Queue constructor
	//---------------------------------------------------------

	Ordered_Queue (int _max_records = 200)
	{
		num_records = max_records = first_index = last_index = first_num = last_num = 0;

		Max_Records (_max_records);
	}

	//---------------------------------------------------------
	//	Put
	//---------------------------------------------------------

	bool Put (Type *data, int number = -1)
	{
		if (number < 0) number = last_num;

		int num = num_records + number - last_num;

		if (num >= max_records) return (false);

		int index = last_index;

		if (num < num_records) {
			index = last_index - num_records + num;
			if (index < 0) index += max_records;
			type_array [index] = data;
		} else {
			while (num > num_records) {
				type_array [last_index++] = 0;
				if (last_index >= max_records) last_index = 0;
				num_records++;
			}
			type_array [last_index++] = data;
			if (last_index >= max_records) last_index = 0;
			num_records++;
			last_num = number + 1;
		}
		return (true);
	}

	//---------------------------------------------------------
	//	Get
	//---------------------------------------------------------

	Type * Get (int &number)
	{
		if (num_records == 0) return (0);

		Type *ptr = type_array [first_index];

		if (ptr == 0) return (0);

		num_records--;
		first_index++;
		number = first_num++;

		if (first_index >= max_records) first_index = 0;

		return (ptr);
	}

	//---------------------------------------------------------
	//	Full
	//---------------------------------------------------------

	bool Full (void)
	{
		return (num_records == max_records);
	}

	//---------------------------------------------------------
	//	Empty
	//---------------------------------------------------------

	bool Empty (void)
	{
		return (num_records == 0);
	}

	//---------------------------------------------------------
	//	Reset
	//---------------------------------------------------------

	void Reset (void)
	{
		num_records = first_index = last_index = first_num = last_num = 0;
	}

	//---------------------------------------------------------
	//	Max_Number
	//---------------------------------------------------------

	int  Max_Number (void)
	{
		return (last_num);
	}

	//---------------------------------------------------------
	//	Num_Records
	//---------------------------------------------------------

	int  Num_Records (void)
	{
		return (num_records);
	}

	//---------------------------------------------------------
	//	Max_Records
	//---------------------------------------------------------

	int  Max_Records (void)
	{
		return (max_records);
	}
	bool Max_Records (int max_size)
	{
		type_array.resize (max_size);
		max_records = (int) type_array.size ();
		return (max_records > 0);
	}

private:

	//---- data ----

	int num_records, max_records, first_index, last_index, first_num, last_num;

	Type_Array type_array;
};

#endif
