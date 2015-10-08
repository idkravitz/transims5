//*********************************************************
//	Equation.cpp - volume-delay equation array
//*********************************************************

#include "Equation.hpp"

#include "Execution_Service.hpp"
#include "TypeDefs.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Equation
//---------------------------------------------------------

bool Equation_Array::Add_Equation (int number, string &parameters)
{
	bool flag = true;
	Equation eq_rec;
	Equation_Stat eq_stat;

	if (!parameters.empty ()) {
		int num;
		Strings elements;

		num = String_Ptr (parameters)->Parse (elements);

		memset (&eq_rec, '\0', sizeof (eq_rec));

		if (num > 0) eq_rec.type = Equation_Code (elements [0]);
		if (num > 1) eq_rec.a = elements [1].Double ();
		if (num > 2) eq_rec.b = elements [2].Double ();
		if (num > 3) eq_rec.c = elements [3].Double ();
		if (num > 4) eq_rec.d = elements [4].Double ();

		if (eq_rec.type == BPR || eq_rec.type == BPR_PLUS) {
			if (eq_rec.a < 0.01 || eq_rec.b < 1.0 || eq_rec.c < 0.5) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("BPR Equation Parameters %.2lf, %.2lf, %.2lf are Out of Range") % eq_rec.a % eq_rec.b % eq_rec.c);
				}
				return (false);
			}
			if (eq_rec.type == BPR_PLUS) {
				if (eq_rec.d <= 0.0) eq_rec.d = 1.0;
				if (eq_rec.d > 7.0) {
					if (exe->Send_Messages ()) {
					exe->Error (String ("BPR Plus Equation Minimum Speed %.2lf is Out of Range") % eq_rec.d);
					}
					return (false);
				}
			}
		} else if (eq_rec.type == EXP) {
			if (eq_rec.a < 0.07 || eq_rec.a > 7.5 || eq_rec.b < 1.0 || eq_rec.b > 20.0 ||
				eq_rec.c < 30.0 || eq_rec.c > 750.0) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("Exponential Equation Parameters %.2lf, %.2lf, %.1lf are Out of Range") % eq_rec.a % eq_rec.b % eq_rec.c);
				}
			}
		} else if (eq_rec.type == CONICAL) {
			if (eq_rec.a <= 1.0 || eq_rec.a > 20.0) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("Conical Equation Parameter %.2lf is Out of Range (1..20)") % eq_rec.a);
				}
				return (false);
			}
			eq_rec.b = (2 * eq_rec.a - 1) / (2 * eq_rec.a - 2);
		} else if (eq_rec.type == AKCELIK) {
			if (eq_rec.a <= 0.05 || eq_rec.a > 2.5) {
				if (exe->Send_Messages ()) {
					exe->Error (String ("AKCELIK Equation Parameter *.2lf is Out of Range (0.05..2.5)") % eq_rec.a);
				}
				return (false);
			}
		}
	} else if (number == 1) {

		eq_rec.type = BPR;
		eq_rec.a = 0.15;
		eq_rec.b = 4.0;
		eq_rec.c = 0.75;
		eq_rec.d = 0.0;

	} else {
		Equation_Itr eq_itr;
		
		eq_itr = equations.find (number-1);
		if (eq_itr == equations.end ()) return (false);

		eq_rec = eq_itr->second;
		flag = false;
	}

	if (flag && exe->Send_Messages ()) {
		exe->Print (1, String ("Equation Parameters %d = %s, A=%0.2lf") % number % Equation_Code (eq_rec.type) % eq_rec.a);

		if (eq_rec.type != CONICAL && eq_rec.type != AKCELIK) {
			exe->Print (0, String (", B=%0.2lf, C=%.2lf") % eq_rec.b % eq_rec.c);
		}
		if (eq_rec.type == BPR_PLUS) {
			exe->Print (0, String (", D=%.2lf") % eq_rec.d);
		}
	}
	eq_stat = equations.insert (Equation_Data (number, eq_rec));
	return (eq_stat.second);
}

//---------------------------------------------------------
//	Apply_Equation
//---------------------------------------------------------

int Equation_Array::Apply_Equation (int number, int time0, double flow, int capacity, int length)
{
	if (flow <= 0) {
		return (time0);
	} else {
		int time;
		double cap, len;
		Equation *eq_ptr;
		Equation_Itr eq_itr;
		
		eq_itr = equations.find (number);
		if (eq_itr == equations.end ()) return (time0);

		eq_ptr = &(eq_itr->second);

		if (capacity < 1) capacity = 1;
		cap = capacity;

		if (eq_ptr->type == BPR || eq_ptr->type == BPR_PLUS) {
			cap *= eq_ptr->c;
			if (cap < 1.0) cap = 1.0;

			time = (int) (time0 * (1.0 + eq_ptr->a * pow ((flow / cap), eq_ptr->b)) + 0.5);

			if (eq_ptr->type == BPR_PLUS) {
				int max_time = (int) (length / eq_ptr->d + 0.5);
				if (time > max_time) time = max_time;
			}
		} else if (eq_ptr->type == EXP) {
			len = length / 1000.0;

			time = (int) (time0 + len * MIN (eq_ptr->a * exp (eq_ptr->b * (flow / cap)), eq_ptr->c) + 0.5);

		} else if (eq_ptr->type == CONICAL) {
			cap = 1 - (flow / cap);

			time = (int) (time0 * (2 - eq_ptr->b - eq_ptr->a * cap + 
				sqrt (eq_ptr->a * eq_ptr->a * cap * cap + eq_ptr->b * eq_ptr->b)) + 0.5);

		} else if (eq_ptr->type == AKCELIK && length > 0) {
			double vc, fac, spd;
			vc = flow / cap;
			fac = vc - 1;
			
			if (exe->Metric_Flag ()) {
				spd = ((double) time0 / length) / MPSTOMPH;
			} else {
				spd = ((double) time0 / length) * MPHTOFPS;
			}
			time = (int) (time0 + ((spd + 0.25 * 0.25 * (fac + sqrt ((fac * fac) + (8 * eq_ptr->a * vc)/cap)))) / spd + 0.5);

		} else {
			return (time0);
		}
		if (time < time0) {
			if (time < 0) {
				return (MAX_INTEGER);
			} else {
				return (time0);
			}
		}
		return (time);
	}
}
