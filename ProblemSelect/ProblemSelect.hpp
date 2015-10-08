//*********************************************************
//	ProblemSelect.hpp - Select Households for Re-Routing
//*********************************************************

#ifndef PROBLEMSELECT_HPP
#define PROBLEMSELECT_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Data_Queue.hpp"

//---------------------------------------------------------
//	ProblemSelect - execution class definition
//---------------------------------------------------------

class ProblemSelect : public Data_Service, public Select_Service
{
public:
	ProblemSelect (void);
	virtual ~ProblemSelect (void);

	virtual void Execute (void);

protected:
	enum ProblemSelect_Keys { 
	};

	virtual void Program_Control (void);
	virtual bool Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition);
	virtual void Initialize_Problems (Problem_File &file);

private:
	int max_problem, num_problem;
};
#endif
