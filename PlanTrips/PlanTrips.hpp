//*********************************************************
//	PlanTrips.hpp - update trip start and end times
//*********************************************************

#ifndef PLANTRIPS_HPP
#define PLANTRIPS_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"

//---------------------------------------------------------
//	PlanTrips - execution class definition
//---------------------------------------------------------

class PlanTrips : public Data_Service, public Select_Service
{
public:
	PlanTrips (void);
	virtual ~PlanTrips (void);

	virtual void Execute (void);

	virtual void Initialize_Trips (Trip_File &file);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual int Put_Trip_Data (Trip_File &file, Trip_Data &data);

protected:
	enum PlanTrips_Keys { 
		//ZONE_RELOCATION_FILE = 1, 
	};

	virtual void Program_Control (void);

private:

	bool select_flag;

	//---- methods ----

	void Read_Plan (void);
};

#endif
