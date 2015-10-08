//*********************************************************
//	Equation.hpp - volume-delay equation array
//*********************************************************

#ifndef EQUATION_HPP
#define EQUATION_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"

#include <string>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Equation_Array Class definition
//---------------------------------------------------------

class SYSLIB_API Equation_Array : public Static_Service
{
public:
	Equation_Array (void) : Static_Service ()  { }

	int  Num_Equations (void)                  { return ((int) equations.size ()); }

	bool Add_Equation (int number, string &parameters);

	int  Apply_Equation (int number, int time0, double flow, int capacity, int length);

private:

	typedef struct {
		Equation_Type type;
		double a;
		double b;
		double c;
		double d;
	} Equation;

	typedef map <int, Equation>        Equations;
	typedef pair <int, Equation>       Equation_Data;
	typedef Equations::iterator        Equation_Itr;
	typedef pair <Equation_Itr, bool>  Equation_Stat;

	Equations equations;
};

#endif
