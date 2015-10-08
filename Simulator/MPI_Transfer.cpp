//*********************************************************
//	MPI_Transfer.cpp - transfer data between MPI machines
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	MPI_Transfer
//---------------------------------------------------------

bool Simulator::MPI_Transfer (bool status)
{
#ifdef MPI_EXE
	int i, rank, rec, traveler, part, vehicle, veh, num, lvalue [2];

	Sim_Traveler_Ptr sim_traveler_ptr = 0;
	Sim_Veh_Ptr sim_veh_ptr = 0;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Cap_Ptr sim_cap_ptr;

	Int2_Ptr rank_ptr;
	Integers *list;
	Int_Itr int_itr;
	Cell_Itr cell_itr;
	Int2_Set_Itr set_itr;
	I2_Ints_Map_Itr map_itr;

	//---- coodinate status and vehicle count ----

	if (Master ()) {

		//---- retrieve the status and vehicle count ----

		for (i=1; i < MPI_Size (); i++) {
			rank = Get_MPI_Array (lvalue, 2);

			if (lvalue [0] > 0) status = true;
			Add_Vehicles (lvalue [1]);
		}

		//---- send the combined status flag ----

		for (rank=1; rank < MPI_Size (); rank++) {
			lvalue [0] = (status) ? 1 : 0;

			Send_MPI_Array (lvalue, 1, rank);
		}

	} else {

		//---- send the status and vehicle count ----

		lvalue [0] = status;
		lvalue [1] = Num_Vehicles ();

		Send_MPI_Array (lvalue, 2);

		//---- update the status flag ----

		Get_MPI_Array (lvalue, 1, 0);

		status = (lvalue [0] > 0);
	}

	//---- transfer data records ----

	for (set_itr = mpi_exchange.begin (); set_itr != mpi_exchange.end (); set_itr++) {

		if (set_itr->first == MPI_Rank ()) {

			//---- process send commands ----

			rank = set_itr->second;
			rank_ptr = &mpi_range [rank];

			//---- count the part lists assigned to each machine ----

			mpi_buffer.Size (0);
			rec = 0;
			
			for (part = rank_ptr->first; part <= rank_ptr->second; part++) {
				rec += (int) mpi_parts [part].size ();
				list = &mpi_parts [part];
			
				//---- pack the traveler and vehicle records ----

				for (int_itr = list->begin (); int_itr != list->end (); int_itr++) {
					traveler = *int_itr;
					sim_traveler_ptr = sim_traveler_array [traveler];

					if (sim_traveler_ptr->Status () == 3) {
						sim_traveler_ptr->Status (0);
						traveler = -1;
					}
					vehicle = sim_traveler_ptr->plan_ptr->Vehicle ();
					if (vehicle >= 0) {
						sim_veh_ptr = sim_veh_array [vehicle];
						if (sim_veh_ptr == 0) vehicle = -1;
					}
					mpi_buffer.Add_Data (&traveler, sizeof (traveler));
					mpi_buffer.Add_Data (&part, sizeof (part));
					mpi_buffer.Add_Data (&vehicle, sizeof (vehicle));

					if (traveler >= 0) sim_traveler_ptr->Pack (mpi_buffer);

					if (vehicle >= 0) sim_veh_ptr->Pack (mpi_buffer);
				}
				list->clear ();
			}

			//---- transfer travel plans and vehicles ----

			Send_MPI_Array (&rec, 1, rank);

			if (rec > 0) Send_MPI_Buffer (rank);

		} else if (set_itr->second == MPI_Rank ()) {	

			//---- process receive commands ----

			rank = set_itr->first;

			Get_MPI_Array (&rec, 1, rank);

			if (rec == 0) continue;

			//---- retrieve the traveler and vehicle data ----

			Get_MPI_Buffer (rank);

			//---- unpack the traveler and vehicle records ----

			while (rec--) {
				mpi_buffer.Get_Data (&traveler, sizeof (traveler));
				mpi_buffer.Get_Data (&part, sizeof (part));
				mpi_buffer.Get_Data (&vehicle, sizeof (vehicle));

				part -= First_Partition ();

				//---- check the traveler array ----

				if (traveler >= 0) {
					num = traveler - (int) sim_traveler_array.size () + 1;

					if (num > 0) {
						sim_traveler_ptr = 0;
						sim_traveler_array.insert (sim_traveler_array.end (), num, sim_traveler_ptr);
					} else {
						sim_traveler_ptr = sim_traveler_array [traveler];
						if (sim_traveler_ptr != 0) delete sim_traveler_ptr;
					}

					//---- unpack the traveler data ----

					sim_traveler_ptr = new Sim_Traveler_Data ();
					sim_traveler_ptr->UnPack (mpi_buffer);

					sim_traveler_array [traveler] = sim_traveler_ptr;
					work_step.Put (traveler, part);
				}

				//---- unpack the vehicle data ----

				if (vehicle >= 0) {
					sim_veh_ptr = sim_veh_array [vehicle];
	
					if (sim_veh_ptr == 0) {
						sim_veh_array [vehicle] = sim_veh_ptr = new Sim_Veh_Data ();
					}
					sim_veh_ptr->UnPack (mpi_buffer);

					//---- load the vehicle onto the network ----

					for (cell_itr = sim_veh_ptr->begin (); cell_itr != sim_veh_ptr->end (); cell_itr++) {
						if (cell_itr->Index () >= 0 && cell_itr->Lane () >= 0) {
							sim_dir_ptr = &sim_dir_array [cell_itr->Index ()];

							if (sim_dir_ptr->Method () == MESOSCOPIC) {
								sim_dir_ptr->Cell (*cell_itr, vehicle);
							} else if (sim_dir_ptr->Method () == MACROSCOPIC) {
								if (cell_itr == sim_veh_ptr->begin ()) {
									sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

									if (sim_veh_ptr->Restricted ()) {
										sim_cap_ptr->Add_High ((int) sim_veh_ptr->size ());
									} else {
										sim_cap_ptr->Add_Low ((int) sim_veh_ptr->size ());
									}
									veh = sim_cap_ptr->Last_Veh ();
									sim_veh_ptr->Leader (veh);
									sim_veh_ptr->Follower (-1);

									if (veh >= 0) {
										Sim_Veh_Ptr lead_ptr = sim_veh_array [veh];
										lead_ptr->Follower (vehicle);
										sim_cap_ptr->Last_Veh (vehicle);
									} else {
										sim_cap_ptr->First_Veh (vehicle);
										sim_cap_ptr->Last_Veh (vehicle);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//---- transfer boundary link updates ----

	for (map_itr = mpi_boundary.begin (); map_itr != mpi_boundary.end (); map_itr++) {

		if (map_itr->first.first == MPI_Rank ()) {

			//---- process send commands ----

			rank = map_itr->first.second;
			list = &map_itr->second;

			mpi_buffer.Size (0);
			
			//---- pack the link records ----

			for (int_itr = list->begin (); int_itr != list->end (); int_itr++) {
				sim_dir_ptr = &sim_dir_array [*int_itr];

				sim_dir_ptr->Pack (mpi_buffer);
			}

			//---- transfer boundary links ----

			Send_MPI_Buffer (rank);

		} else if (map_itr->first.second == MPI_Rank ()) {	

			//---- process receive commands ----

			rank = map_itr->first.first;
			list = &map_itr->second;

			Get_MPI_Buffer (rank);

			//---- unpack the link records ----

			for (int_itr = list->begin (); int_itr != list->end (); int_itr++) {
				sim_dir_ptr = &sim_dir_array [*int_itr];

				sim_dir_ptr->UnPack (mpi_buffer);
			}
		}
	}

#endif
	return (status);
}
