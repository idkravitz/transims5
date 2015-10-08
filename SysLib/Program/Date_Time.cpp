//********************************************************* 
//	Date_Time.cpp - date-time functions
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Data_Time
//---------------------------------------------------------

bool User_Program::Date_Time (void)
{
	struct tm dt, *dtim;
	Stack_Data *s2, *s3, *s4, *s5;
	String *str_ptr;

	if (s->type != INT_DATA) goto error;

	switch (cmd_ptr->token) {
		case DOW:
			dtim = local_time ((time_t *) &(s->lvalue));
			s->lvalue = dtim->tm_wday;
			break;
			
		case HOUR:
			dtim = local_time ((time_t *) &(s->lvalue));
			s->fvalue = dtim->tm_hour + (dtim->tm_min / 60.0) + (dtim->tm_sec / 3600.0);
			s->type = FLOAT_DATA;
			break;

		case MONTH:
			dtim = local_time ((time_t *) &(s->lvalue));
			s->lvalue = dtim->tm_mon + 1;
			break;

		case YEAR:
			dtim = local_time ((time_t *) &(s->lvalue));
			s->lvalue = dtim->tm_year + 1900;
			break;
			
		case DATE_STR:
			dtim = local_time ((time_t *) &(s->lvalue));
			str_ptr = &svalue [0];
			(*str_ptr)("%2d/%02d/%04d") % (dtim->tm_mon+1) % dtim->tm_mday % (dtim->tm_year+1900);
			s->type = STRING_DATA;
			s->str_ptr = str_ptr;
			break;
			
		case TIME_STR:
			dtim = local_time ((time_t *) &(s->lvalue));
			str_ptr = &svalue [0];
			(*str_ptr)("%2d:%02d:%02d") % dtim->tm_hour % dtim->tm_min % dtim->tm_sec;
			s->type = STRING_DATA;
			s->str_ptr = str_ptr;
			break;

		case DATE:

			if (sindex <= 2) goto error;

			s2 = &stack [sindex - 2];

			if (s->type != INT_DATA || s1->type != INT_DATA) goto error;

			if (s2->lvalue < 1900) {
				dt.tm_year = s2->lvalue;
			} else {
				dt.tm_year = s2->lvalue - 1900;
			}
			dt.tm_mon = s1->lvalue - 1;
			dt.tm_mday = s->lvalue;
			dt.tm_hour = 0;
			dt.tm_min = 0;
			dt.tm_sec = 0;
			dt.tm_isdst = -1;

			sindex -= 2;
			s = s2;
			s->lvalue = (int) mktime (&dt);
			break;
		
		case DATE_TIME:

			if (sindex <= 5) goto error;

			s2 = &stack [sindex - 2];
			s3 = &stack [sindex - 3];
			s4 = &stack [sindex - 4];
			s5 = &stack [sindex - 5];

			if (s5->type != INT_DATA || s4->type != INT_DATA ||
				s3->type != INT_DATA || s2->type != INT_DATA ||
				s1->type != INT_DATA) goto error;

			if (s5->lvalue < 1900) {
				dt.tm_year = s5->lvalue;
			} else {
				dt.tm_year = s5->lvalue - 1900;
			}
			dt.tm_mon = s4->lvalue - 1;
			dt.tm_mday = s3->lvalue;
			dt.tm_hour = s2->lvalue;
			dt.tm_min = s1->lvalue;
			dt.tm_sec = s->lvalue;
			dt.tm_isdst = -1;

			sindex -= 5;
			s = s5;
			s->lvalue = (int) mktime (&dt);
			break;

		default:
			exe->Error (String ("Date-Time Token %d was Unrecognized") % cmd_ptr->token);
			return (false);
	}
	return (true);

error:
	return (Syntax_Error ("Data-Time Function"));
}
