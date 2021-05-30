/*
 * test_phase_2.cpp
 *
 *  Created on: May 29, 2021
 *      Author: mad
 */

#include <chia/phase2.hpp>
#include <chia/DiskSort.hpp>

#include <iostream>
#include <fstream>

std::ifstream::pos_type get_file_size(const char* file_name)
{
	std::ifstream in(file_name, std::ifstream::ate | std::ifstream::binary);
	return in.tellg(); 
}

using namespace phase2;


int main(int argc, char** argv)
{
	const int num_threads = argc > 1 ? atoi(argv[1]) : 4;
	const int log_num_buckets = argc > 2 ? atoi(argv[2]) : 8;
	
	const auto total_begin = get_wall_time_micros();
	
	size_t max_table_size = 0;
	std::array<phase1::table_t, 7> input;
	for(size_t i = 0; i < input.size(); ++i)
	{
		const std::string file_name = "test.p1.table" + std::to_string(i + 1) + ".tmp";
		size_t size = 0;
		if(i == 0) {
			size = phase1::tmp_entry_1::disk_size;
		} else if(i < 6) {
			size = phase1::tmp_entry_x::disk_size;
		} else if(i == 6) {
			size = phase1::entry_7::disk_size;
		}
		input[i].file_name = file_name;
		input[i].num_entries = get_file_size(file_name.c_str()) / size;
		max_table_size = std::max(max_table_size, input[i].num_entries);
		
		std::cout << "[P2] Input table " << (i + 1) << ": "
				<< input[i].num_entries << " entries" << std::endl;
	}
	std::cout << "[P2] max_table_size = " << max_table_size << std::endl;
	
	bitfield curr_bitfield(max_table_size);
	bitfield next_bitfield(max_table_size);
	
	FILE* table_7 = fopen("test.p2.table7.tmp", "wb");
	if(!table_7) {
		return -1;
	}
	compute_table<entry_7, entry_7, DiskSort7>(
			7, num_threads, nullptr, table_7, input[5], input[6], &next_bitfield, nullptr);
	
	curr_bitfield.swap(next_bitfield);
	
	DiskSortT sort_6(32, log_num_buckets, num_threads, "test.p2.t6");
	compute_table<phase1::tmp_entry_x, entry_t, DiskSortT>(
			6, num_threads, &sort_6, nullptr, input[4], input[5], &next_bitfield, &curr_bitfield);
	
	curr_bitfield.swap(next_bitfield);
	
	DiskSortT sort_5(32, log_num_buckets, num_threads, "test.p2.t5");
	compute_table<phase1::tmp_entry_x, entry_t, DiskSortT>(
			5, num_threads, &sort_5, nullptr, input[3], input[4], &next_bitfield, &curr_bitfield);
	
	curr_bitfield.swap(next_bitfield);
	
	DiskSortT sort_4(32, log_num_buckets, num_threads, "test.p2.t4");
	compute_table<phase1::tmp_entry_x, entry_t, DiskSortT>(
			4, num_threads, &sort_4, nullptr, input[2], input[3], &next_bitfield, &curr_bitfield);
	
	curr_bitfield.swap(next_bitfield);
	
	DiskSortT sort_3(32, log_num_buckets, num_threads, "test.p2.t3");
	compute_table<phase1::tmp_entry_x, entry_t, DiskSortT>(
			3, num_threads, &sort_3, nullptr, input[1], input[2], &next_bitfield, &curr_bitfield);
	
	curr_bitfield.swap(next_bitfield);
	
	DiskSortT sort_2(32, log_num_buckets, num_threads, "test.p2.t2");
	compute_table<phase1::tmp_entry_x, entry_t, DiskSortT>(
			2, num_threads, &sort_2, nullptr, input[0], input[1], &next_bitfield, &curr_bitfield);
	
	fclose(table_7);
	
	std::cout << "Phase 2 took " << (get_wall_time_micros() - total_begin) / 1e6 << " sec" << std::endl;
}


