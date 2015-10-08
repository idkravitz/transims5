//*********************************************************
//	Message_Service.cpp - general message services
//*********************************************************

#include "Message_Service.hpp"

//---------------------------------------------------------
//	Message_Service - constructor
//---------------------------------------------------------

Message_Service::Message_Service (void) : Static_Service ()
{
	status = DONE;
	Program ("Program");
	Version (0);
	CopyRight ("2012 by TRANSIMS Open-Source");
	time (&starttime);
	progress = 0;
	warnings = 0;
	max_warnings = 100000;
	warning_flag = true;
	pause = false;
	no_pause = false;
	detail = false;
	send_messages = true;
	mpi_rank = 0;
	mpi_size = 0;

#ifdef MPI_EXE	
	mpi_buffer.Max_Size (4096);
	mpi_text.Max_Size (4096);
#endif
}

//-----------------------------------------------------------
//	Show_Banner
//-----------------------------------------------------------

void Message_Service::Show_Banner ()
{
	if (!Quiet () && Master ()) {
		Banner ();
		cout << endl;
	}
}

//-----------------------------------------------------------
//	Banner
//-----------------------------------------------------------

void Message_Service::Banner (ofstream *file)
{
	time_t ltime;
	const char *bar = "********************************************";
	String version, copyright, timestamp;

	if (file == 0) file = (ofstream *) &cout;
	ltime = StartTime ();

	version ("%s - Version %d.%d.%d") % Program () % (VERSION_CODE / 10) % (VERSION_CODE % 10) % Version ();
	copyright ("Copyright %s") % CopyRight ();
	timestamp (c_time (&ltime));

	*file << "\n\t" << bar;
	*file << String ("\n\t|%42c|") % BLANK;
	*file << "\n\t|" << version.Center (42) << "|";
	*file << "\n\t|" << copyright.Center (42) << "|";
	*file << "\n\t|" << timestamp.Center (42) << "|";
	*file << String ("\n\t|%42c|") % BLANK;
	*file << "\n\t" << bar << endl << flush;
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Message_Service::Exit_Stat (Exit_Code stat) 
{
	if (stat != Program_Status ()) {
		Program_Status (stat);
	}
#ifdef MAKE_MPI
	MPI_Finalize();
#endif
	if (Master ()) {
		if (Pause ()) Pause_Process ();
		cout << endl;
	}
	int code = Return_Code ();

	delete this;

	exit (code);
}

//---------------------------------------------------------
//	Exit_Program
//---------------------------------------------------------

void Message_Service::Exit_Program (void) 
{
	exit (Return_Code ());
}

//---------------------------------------------------------
//	Return_Code
//---------------------------------------------------------
	
int Message_Service::Return_Code (void)
{ 
	if (Program_Status () == FATAL) {
		return (1);
	} else if (Program_Status () == WARNING) {
		return (2);
	} else {
		return (0);
	}
}

//---------------------------------------------------------
//	Show_Question
//---------------------------------------------------------

bool Message_Service::Show_Question (const char *text) 
{
	if (Detail ()) {
		Show_Error ("Show_Question in 'Detail' mode (-D)");
		return (false);
	}
	char answer [10];

	cout << "\n\t" << text << " (Y/N): " << flush;

	cin.getline (answer, sizeof (answer));

	return (answer [0] == 'y' || answer [0] == 'Y');
}

//---------------------------------------------------------
//	Show_Error
//---------------------------------------------------------

void Message_Service::Show_Error (const char *text) 
{
	if (Master ()) {
		if (Quiet ()) {
			Quiet (false);
			Show_Banner ();
		}
		if (Program_Status () == FATAL) {
			cout << "\n\t" << text << flush;
		} else {
			cout << "\n\n\tError: " << text << flush;

			Pause (true);

			Exit_Stat (FATAL);
		}
	} else {
		Exit_Stat (FATAL);
	}
}

//---------------------------------------------------------
//	Show_Warning
//---------------------------------------------------------

Message_Stream & Message_Service::Show_Warning (const char *text) 
{
	if (Master ()) {
		if (Quiet () && warnings == 0) {
			Quiet (false);
			Show_Banner ();
			Quiet (true);
		}
		warnings++;
		if (max_warnings > 0 && warnings > max_warnings) {
			if (warning_flag) {
				Show_Error (String ("Maximum Warning Messages % Exceeded") % max_warnings);
			}
			return (message.Active (false));
		}
		return (Show_Message ("Warning: ") << text);
	} else {
		return (message.Active (false));
	}
}


//---------------------------------------------------------
//	Set_Progress
//---------------------------------------------------------

void Message_Service::Set_Progress (void) 
{
	progress = 0;
	last = clock ();
}

//---------------------------------------------------------
//	Show_Dot
//---------------------------------------------------------

void Message_Service::Show_Dot (void) 
{
	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - last) >= 5 * CLOCKS_PER_SEC) {
			last = now;
			cout << "." << flush;
		}
	}
}

//---------------------------------------------------------
//	Show_Progress
//---------------------------------------------------------

void Message_Service::Show_Progress (int value) 
{
	if (value < 1) {
		value = ++progress;
	} else {
		progress = value;
	}
	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - last) >= CLOCKS_PER_SEC) {
			if (Detail ()) {
				cout << "\n\tRecord: " << value << flush; 
			} else {
				cout << String (" %-16d\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b") % value << flush;
			}
			last = now;
		}
	}
}

void Message_Service::Show_Progress (const char *text) 
{
	progress++;

	if (!Quiet ()) {
		clock_t now = clock ();

		if ((now - last) >= CLOCKS_PER_SEC) {
			if (Detail ()) {
				cout << "\n\tRecord: " << text << flush; 
			} else {
				cout << String (" %-16.16s\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b") % text << flush;
			}
			last = now;
		}
	}
}

//---------------------------------------------------------
//	End_Progress
//---------------------------------------------------------

void Message_Service::End_Progress (void) 
{
    if (!Quiet ()) {
		if (Detail ()) {
			cout << "\n\tRecord: " << progress << endl << flush; 
		} else {
			cout << " " << progress << endl;
		}
	}
}

void Message_Service::End_Progress (const char *text) 
{
    if (!Quiet ()) {
		if (Detail ()) {
			cout << "\n\tRecord: " << text << endl << flush; 
		} else {
			cout << String (" %-16.16s\n") % text;
		}
	}
}

//---------------------------------------------------------
//	Pause_Process
//---------------------------------------------------------

void Message_Service::Pause_Process (void) 
{
    if (!Quiet () && !No_Pause () && !Detail () && Master ()) {
		char pause [10];

		cout << "\n\n\tPress Enter to Continue" << flush;

		cin.getline (pause, sizeof (pause));
	}
}

//---------------------------------------------------------
//	Get_Control_File
//---------------------------------------------------------

String Message_Service::Get_Control_File () 
{
	String filename;
	char buffer [512];
	size_t index;

	cout << "\n\tEnter Control File Name: ";

	cin.getline (buffer, sizeof (buffer));

	filename = buffer;

	filename.Trim ();

	index = filename.find ('\n');
	if (index != filename.npos) filename.erase (index);

	return (filename);
}

#ifdef MPI_EXE	

//---------------------------------------------------------
//	Send_MPI_Message
//---------------------------------------------------------

void Message_Service::Send_MPI_Message (String message) 
{
	mpi_text.Message (message);

#ifdef BOOST_MPI
	world.send (0, 0, mpi_text.Data (), mpi_text.Size ());
#else
	MPI_Send (mpi_text.Data (), (int) mpi_text.Size (), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
#endif
}

//---------------------------------------------------------
//	Show_MPI_Message
//---------------------------------------------------------

int Message_Service::Show_MPI_Message (void) 
{
#ifdef BOOST_MPI
	mpi::status mpi_stat;
	mpi_stat = world.recv (mpi::any_source, 0, mpi_text.Data (), mpi_text.Max_Size ());
	if (mpi_stat.tag () == 0) Show_Message (1, mpi_text.Data ());
	return (mpi_stat.source ());
#else
	MPI_Status mpi_stat;
	MPI_Recv (mpi_text.Data (), (int) mpi_text.Max_Size (), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &mpi_stat);
	if (mpi_stat.MPI_TAG == 0) Show_Message (1, mpi_text.Data ());
	return (mpi_stat.MPI_SOURCE);
#endif
}

//---------------------------------------------------------
//	Send_MPI_Buffer
//---------------------------------------------------------

void Message_Service::Send_MPI_Buffer (Data_Buffer &data, int tag, int rank) 
{
	int num = (int) data.Size ();

#ifdef BOOST_MPI
	if (tag > 0) {
		boost::mutex::scoped_lock lock (mpi_mutex);
		world.send (rank, tag, &num, 1);
		if (num > 0) world.send (rank, tag, data.Data (), num);
	} else {
		world.send (rank, 0, &num, 1);
		if (num > 0) world.send (rank, 0, data.Data (), num);
	}
#else
	if (tag > 0) {
		boost::mutex::scoped_lock lock (mpi_mutex);
		MPI_Send (&num, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		if (num > 0) MPI_Send (data.Data (), num, MPI_BYTE, rank, tag, MPI_COMM_WORLD);
	} else {
		MPI_Send (&num, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
		if (num > 0) MPI_Send (data.Data (), num, MPI_BYTE, rank, 0, MPI_COMM_WORLD);
	}
#endif
}

//---------------------------------------------------------
//	Get_MPI_Buffer
//---------------------------------------------------------

int Message_Service::Get_MPI_Buffer (Data_Buffer &data, int tag, int rank) 
{
	int num = 0;
#ifdef BOOST_MPI
	mpi::status mpi_stat;
	if (rank < 0) rank = mpi::any_source;

	if (tag > 0) {
		boost::mutex::scoped_lock lock (mpi_mutex);
		mpi_stat = world.recv (rank, tag, &num, 1);
		rank = mpi_stat.source ();
		data.Size (num);
		if (num > 0) mpi_stat = world.recv (rank, tag, data.Data (), num);
	} else {
		mpi_stat = world.recv (rank, 0, &num, 1);
		rank = mpi_stat.source ();
		data.Size (num);
		if (num > 0) mpi_stat = world.recv (rank, 0, data.Data (), num)
	}
#else
	MPI_Status mpi_stat;
	if (rank < 0) rank = MPI_ANY_SOURCE;

	if (tag > 0) {
		boost::mutex::scoped_lock lock (mpi_mutex);
		MPI_Recv (&num, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, &mpi_stat);
		rank = mpi_stat.MPI_SOURCE;
		data.Size (num);
		if (num > 0) MPI_Recv (data.Data (), num, MPI_BYTE, rank, tag, MPI_COMM_WORLD, &mpi_stat);
	} else {
		MPI_Recv (&num, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &mpi_stat);
		rank = mpi_stat.MPI_SOURCE;
		data.Size (num);
		if (num > 0) MPI_Recv (data.Data (), num, MPI_BYTE, rank, 0, MPI_COMM_WORLD, &mpi_stat);
	}
#endif
	return (rank);
}

//---------------------------------------------------------
//	Send_MPI_Array
//---------------------------------------------------------

void Message_Service::Send_MPI_Array (int *data, int num, int rank) 
{
#ifdef BOOST_MPI
	world.send (rank, 0, data, num);
#else
	MPI_Send (data, num, MPI_INT, rank, 0, MPI_COMM_WORLD);
#endif
}

//---------------------------------------------------------
//	Get_MPI_Array
//---------------------------------------------------------

int Message_Service::Get_MPI_Array (int *data, int num, int rank) 
{
#ifdef BOOST_MPI
	mpi::status mpi_stat;
	if (rank < 0) rank = mpi::any_source;
	mpi_stat = world.recv (rank, 0, data, num);
	rank = mpi_stat.source ();
#else
	MPI_Status mpi_stat;
	if (rank < 0) rank = MPI_ANY_SOURCE;
	MPI_Recv (data, num, MPI_INT, rank, 0, MPI_COMM_WORLD, &mpi_stat);
	rank = mpi_stat.MPI_SOURCE;
#endif
	return (rank);
}
#endif
