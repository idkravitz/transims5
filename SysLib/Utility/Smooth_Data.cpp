//*********************************************************
//	Smooth_Data.cpp - data smoothing class
//*********************************************************

#include "Smooth_Data.hpp"

//---------------------------------------------------------
//	Smooth_Data constructor
//---------------------------------------------------------

Smooth_Data::Smooth_Data (int num_in, int iterations, bool loop_flag, int group_size, 
		  double forward, double backward, int num_sub, bool dup_flag)
{
	Setup (num_in, iterations, loop_flag, group_size, forward, backward, num_sub, dup_flag);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

bool Smooth_Data::Setup (int in, int iter, bool flag, int group, double front, double back, int sub, bool dup)
{
	niter = iter;
	loop_flag = flag;
	size = group;
	forward = front;
	backward = back;
	num_sub = sub;
	dup_flag = dup;
	interpolate = false;

	return (Num_Input (in));
}

//---------------------------------------------------------
//	Interpolate
//---------------------------------------------------------

bool Smooth_Data::Interpolate (int in, double inc, int iter, int group, double front, double back, bool dup)
{
	niter = iter;
	loop_flag = false;
	size = group;
	forward = front;
	backward = back;
	num_sub = 1;
	dup_flag = dup;
	interpolate = true;
	increment = inc;

	return (Num_Input (in));
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Smooth_Data::Clear (void)
{
	num_in = num_out = 0;
	input.clear ();
	output.clear ();
	current.clear ();
}

//---------------------------------------------------------
//	Num_Input
//---------------------------------------------------------

bool Smooth_Data::Num_Input (int num)
{
	int max_iter, max_size;
	double factor;
	bool stat = true;

	Clear ();

	//---- number of input values ----

	if (num < 3) {
		if (num != 0) {
			exe->Warning (String ("Number of Input Values %d is Out of Range (>= 3)") % num);
			stat = false;
		}
		num = 24;
	}
	input.assign (num, 0.0);

	num_in = num;

	//---- number of output values ----

	if (num_sub < 1) {
		exe->Warning (String ("Number of Subdivisions %d is Out of Range (>= 1)") % num_sub);
		num_sub = 1;
		stat = false;
	}
	if (interpolate) {
		if (increment < 0.1) {
			exe->Warning (String ("Smoothing Increment %.2lf is Out of Range (>= 0.1)") % increment);
			increment = 1.0;
		}
		num_out = (int) ((num_in - 1) * increment + 0.9) + 1;
	} else {
		num_out = num_in * num_sub;
	}

	max_iter = ((num_in + 2) / 3) * num_out / num_in;
	max_size = ((num_in + 18) / 20) * 2 + 1;

	output.assign (num_out, 0.0);
	current.assign (num_out, 0.0);

	//---- number of iterations ----

	if (niter < 1 || niter > max_iter) {
		exe->Warning (String ("Number of Iterations %d is Out of Range (1-%d)") % niter % max_iter);
		if (niter < 1) {
			niter = 1;
		} else {
			niter = max_iter;
		}
		stat = false;
	}

	//---- group size ----

	if (size < 3 || size > max_size) {
		exe->Warning (String ("Smooth Group Size %d is Out of Range (3..%d)") % size % max_size);
		if (size < 3) {
			size = 3;
		} else if (size > max_size) {
			size = max_size;
		}
		stat = false;
	}
	if ((size % 2) != 1) {
		exe->Warning (String ("Smooth Group Size %d must be an Odd Number") % size);
		size += 1;
		stat = false;
	}

	//---- percent forward ----

	factor = 100.0 - (50.0 / size);

	if (forward < 0.0 || forward > factor) {
		exe->Warning (String ("Forward Percentage %.1lf is Out of Range (0..%.1lf)") % forward % factor);
		if (forward < 0.0) {
			forward = 20.0;
		} else if (forward > factor) {
			forward = factor;
		}
		stat = false;
	}

	//---- read the percent distributed backward ----

	if (backward < 0.0 || backward > factor) {
		exe->Warning (String ("Backward Percentage %.1lf is Out of Range (0..%.1lf)") % backward % factor);
		if (backward < 0.0) {
			backward = 20.0;
		} else if (backward > factor) {
			backward = factor;
		}
		stat = false;
	}	
	weight = forward + backward;
	factor = 100.0 - (100.0 / size);

	if (weight < 5.0 || weight > factor) {
		exe->Warning (String ("Combined Percentage %.1lf is Out of Range (5..%.0lf)") % weight % factor);
		if (weight < 5.0) {
			forward += 2.5;
			backward += 2.5;
		} else if (weight > factor) {
			factor /= weight;
			forward *= factor;
			backward *= factor;
		}
		stat = false;
	}

	//---- adjust the factors ----

	weight = 0.0;
	max_size = size / 2;

	for (max_iter=1; max_iter <= max_size; max_iter++) {
		weight += max_iter;
	}
	weight = 1.0 / weight;

	return (stat);
}

//---------------------------------------------------------
//	Smooth
//---------------------------------------------------------

int Smooth_Data::Smooth (int num_rec)
{
	int i, j, n, i1, i2, num, nout;
	double share, forward_total, backward_total, delta, cum;

	nout = num_out;

	if (num_rec <= 0 || num_rec > num_in) {
		num_rec = num_in;
	} else if (!interpolate) {
		if (num_sub > 1) {
			nout = num_rec * num_sub;
		} else {
			nout = num_rec;
		}
	}

	//---- initialize the output data ----

	if (interpolate) {
		delta = 1.0 / increment;

		backward_total = 0;
		forward_total = input [0];

		output.assign (nout, 0.0);

		for (i=n=0, cum=0; n < nout; n++, cum += delta) {
			if (cum >= i) {
				backward_total = forward_total;

				if (++i > num_rec) break;
				if (i == num_rec) {
					forward_total = 0;
				} else {
					forward_total = input [i];
				}
			}
			output [n] = (cum - i + 1) * (forward_total - backward_total) + backward_total;
		}
	} else if (num_sub > 1) {
		for (i=n=0; i < num_rec; i++) {
			if (dup_flag) {
				share = input [i];
			} else {
				share = input [i] / num_sub;
			}
			for (j=0; j < num_sub; j++) {
				output [n++] = share;
			}
		}
	} else {
		output.assign (input.begin (), input.end ());
	}
	num = size / 2;

	//---- process each iteration ----

	for (n=0; n < niter; n++) {

		current.assign (output.begin (), output.end ());
		output.assign (nout, 0.0);

		for (i=0; i < nout; i++) {
			share = current [i];

			forward_total = share * forward / 100.0;
			backward_total = share * backward / 100.0;

			output [i] += share - forward_total - backward_total;

			i1 = i2 = i;
			forward_total *= weight;
			backward_total *= weight;

			for (j=num; j > 0; j--) {
				i1--;
				i2++;

				if (i1 < 0) {
					i1 = (loop_flag) ? nout - 1 : 0;
				}
				if (i2 >= nout) {
					i2 = (loop_flag) ? 0 : nout - 1;
				}
				output [i1] += backward_total * j;
				output [i2] += forward_total * j;
			}
		}
	}
	if (interpolate && dup_flag) {
		forward_total = backward_total = 0.0;

		for (i=0; i < num_rec; i++) {
			backward_total += input [i];
		}
		for (j=0; j < nout; j++) {
			forward_total += output [j];
		}
		if (forward_total > 0.0) {
			share = backward_total / forward_total;

			for (j=0; j < nout; j++) {
				output [j] *= share;
			}
		}
	}
	return (nout);
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Smooth_Data::Add_Keys (void)
{
	Control_Key smooth_data_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SMOOTH_GROUP_SIZE, "SMOOTH_GROUP_SIZE", LEVEL0, OPT_KEY, INT_KEY, "3", "0, >= 3", NO_HELP },
		{ PERCENT_MOVED_FORWARD, "PERCENT_MOVED_FORWARD", LEVEL0, OPT_KEY, FLOAT_KEY, "20", "> 0.0", NO_HELP },
		{ PERCENT_MOVED_BACKWARD, "PERCENT_MOVED_BACKWARD", LEVEL0, OPT_KEY, FLOAT_KEY, "20", "> 0.0", NO_HELP },	
		{ NUMBER_OF_ITERATIONS, "NUMBER_OF_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "3", "> 0", NO_HELP },
		{ CIRCULAR_GROUP_FLAG, "CIRCULAR_GROUP_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	if (exe != 0) {
		exe->Key_List (smooth_data_keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Smooth_Data::Read_Control (void)
{
	if (exe == 0) return (false);
	if (!exe->Check_Control_Key (SMOOTH_GROUP_SIZE)) return (false);

	//---- read the number smooth records ----

	size = exe->Get_Control_Integer (SMOOTH_GROUP_SIZE);
	if (size == 0) return (false);

	//---- read the percent distributed forward ----

	forward = exe->Get_Control_Double (PERCENT_MOVED_FORWARD);

	//---- read the percent distributed backwarde ----
	
	backward = exe->Get_Control_Double (PERCENT_MOVED_BACKWARD);

	//---- number of iterations ----

	niter = exe->Get_Control_Integer (NUMBER_OF_ITERATIONS);

	//---- read the circular smoothing flag ----
	
	loop_flag = exe->Get_Control_Flag (CIRCULAR_GROUP_FLAG);
	return (true);
}
