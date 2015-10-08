//*********************************************************
//	User_Program.cpp - Processes User Programs
//*********************************************************

#include "User_Program.hpp"

//----------------------------------------------------------
//	User_Program constructor - No Files
//----------------------------------------------------------

User_Program::User_Program (void) : Static_Service ()
{
	Db_Base_Array files;

	Initialize (files);
}

//----------------------------------------------------------
//	User_Program constructor - Db_Base array
//----------------------------------------------------------

User_Program::User_Program (Db_Base_Array &files) : Static_Service ()
{
	Initialize (files);
}

//-----------------------------------------------------------
//	Initialize
//-----------------------------------------------------------

bool User_Program::Initialize (Db_Base_Array &files, int random_seed)
{
	table_flag = declare_flag = ascii_write = open_par = false;
	level = 1;
	record = line_num = assign_set = if_level = stack_index = 0;
	last_value = nest = loop = first_read = -1;
	String temp;
	Stack_Data stk;
	Db_Base_Itr itr;

	svalue.push_back (temp);
	stack.push_back (stk);

	memset (token_level, '\0', sizeof (token_level));

	Random_Seed (random_seed);

	//---- file data ----

	File_Data file_rec;
	num_files = (int) files.size ();

	for (itr = files.begin (); itr != files.end (); itr++) {
		file_rec.file = *itr;
		file_rec.file_type = file_rec.file->Record_Format ();
		file_rec.read_only = (file_rec.file->File_Access () == READ);
		file_rec.file_stat = 0;
		file_rec.out_flag = 0;

		file_data.push_back (file_rec);
	}
	return (true);
}

//-----------------------------------------------------------
//	Read_Only
//-----------------------------------------------------------

void User_Program::Read_Only (int num, bool flag)
{
	if (num >= 0 && num < (int) file_data.size ()) {
		File_Data *file_ptr = &file_data [num];
		file_ptr->read_only = flag;
	}
}

//----------------------------------------------------------
//	Clear
//----------------------------------------------------------

void User_Program::Clear (void)
{
	int i, j;

	//---- compile data ----

	stack.clear ();
	command.clear ();
	lvalue.clear ();
	fvalue.clear ();
	svalue.clear ();
	file_data.clear ();

	//---- table data ----

	for (i=0; i < 3; i++) {
		for (j=0; j < MAX_TABLE; j++) {
			table [i] [j].clear ();
		}
	}
}

//----------------------------------------------------------
//	Replicate
//----------------------------------------------------------

void User_Program::Replicate (User_Program *ptr)
{
	int i, j;

	stack.assign (ptr->stack.begin (), ptr->stack.end ());
	command.assign (ptr->command.begin (), ptr->command.end ());
	lvalue.assign (ptr->lvalue.begin (), ptr->lvalue.end ());
	fvalue.assign (ptr->fvalue.begin (), ptr->fvalue.end ());
	svalue.assign (ptr->svalue.begin (), ptr->svalue.end ());

	lvariable.assign (ptr->lvariable.begin (), ptr->lvariable.end ());
	fvariable.assign (ptr->fvariable.begin (), ptr->fvariable.end ());
	svariable.assign (ptr->svariable.begin (), ptr->svariable.end ());

	svalue_index.insert (ptr->svalue_index.begin (), ptr->svalue_index.end ());

	//---- table data ----

	for (i=0; i < 3; i++) {
		for (j=0; j < MAX_TABLE; j++) {
			table [i] [j].assign (ptr->table [i] [j].begin (), ptr->table [i] [j].end ());
		}
	}
}
