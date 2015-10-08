//*********************************************************
//	RandomSelect.hpp - randomly distributed to partitions
//*********************************************************

#ifndef RANDOMSELECT_HPP
#define RANDOMSELECT_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	RandomSelect - execution class definition
//---------------------------------------------------------

class RandomSelect : public Data_Service, public Select_Service
{
public:

	RandomSelect (void);
	virtual ~RandomSelect (void);

	virtual void Execute (void);

protected:
	enum RandomSelect_Keys { 
		NUMBER_OF_PARTITIONS = 1	
	};
	virtual void Program_Control (void);
	virtual bool Get_Household_Data (Household_File &file, Household_Data &data, int partition = 0);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual void Initialize_Trips (Trip_File &file);
private:
	bool trip_flag;
	int num_parts;
	Integers part_count;
};
#endif
