//*********************************************************
//	Sim_Read_Plans.hpp - Read Simulation Plans Class
//*********************************************************

#ifndef SIM_READ_PLANS_HPP
#define SIM_READ_PLANS_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Plan_Data.hpp"
#include "Plan_File.hpp"
#include "Partition_Files.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Read_Plans Class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Read_Plans : public Static_Service
{
public:
	Sim_Read_Plans (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool First_Plan (void);
	bool Read_Plans (void);
	void Process_Plan (Plan_Data *plan_ptr);
	void Reposition_Plan (Plan_Data *plan_ptr);

private:
	int num_files, first_num;
	bool stat, first;

	Integers next;
	Partition_Files <Plan_File> file_set;
	Partition_Data <Plan_Data> plan_set;
	Partition_Data <Time_Index> time_set;

	Simulator_Service *exe;
};
#endif
