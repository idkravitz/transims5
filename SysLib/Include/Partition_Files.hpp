//*********************************************************
//	Partition_Files.hpp - an array of partitioned files
//*********************************************************

#ifndef PARTITION_FILES_HPP
#define PARTITION_FILES_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Pointer_Array definition
//---------------------------------------------------------

template <typename Ptr_Type>
class Pointer_Array : public vector <Ptr_Type>
{
public:
	Pointer_Array (void) {}
	virtual ~Pointer_Array (void)
	{
		typename vector <Ptr_Type>::iterator itr;
		for (itr = vector <Ptr_Type>::begin (); itr != vector <Ptr_Type>::end (); itr++) {
			if (*itr != 0) delete *itr;
		}
		vector <Ptr_Type>::clear ();
	}
};

//---------------------------------------------------------
//	Partition_Files class definition
//---------------------------------------------------------

template <typename File_Type>
class Partition_Files : public Pointer_Array <File_Type *>
{
public:
	Partition_Files (void) {}

	void Initialize (File_Type *file, int num_parts, bool part_flag = true)
	{
		File_Type *file_ptr;

		file->Close ();

		for (int part = 0; part < num_parts; part++) {
			file_ptr = new File_Type (file->File_Access (), file->Dbase_Format ());
			file_ptr->Part_Flag (file->Part_Flag ());
			file_ptr->Pathname (file->Pathname ());
			file_ptr->First_Open (false);
			file_ptr->Sort_Type (file->Sort_Type ());
			file_ptr->Open ((part_flag) ? part : 0);
			this->push_back (file_ptr);
		}
	}
};

//---------------------------------------------------------
//	Partition_Data class definition
//---------------------------------------------------------

template <typename Type>
class Partition_Data : public Pointer_Array <Type *>
{
public:
	Partition_Data (void) {}

	void Initialize (int num_parts)
	{
		Type *ptr;

		for (int part = 0; part < num_parts; part++) {
			ptr = new Type ();
			this->push_back (ptr);
		}
	}
};
#endif
