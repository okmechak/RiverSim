/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/** 
 * @file utilities.hpp
 * 
 * Console printouts, version, title and program options processing.
 */

#pragma once

#include "cxxopts.hpp"
#include "version.hpp"
#include <chrono>
#include <time.h>
#include <numeric>
#include <iterator>

using namespace std;


namespace River{
    
    ///Prints to console program logo.
    void print_ascii_signature();

    ///Returns version string defined in @ref version.hpp.
    string version_string();

    ///Prints to console program Version.
    void print_version();

    ///Processing of program options.
    cxxopts::ParseResult process_program_options(int argc, char* argv[]);


    ///FIXME time isn't in seconds or is wrong
    ///Timing
    class Timing
    {
        public:
            
            ///Default Constructor, save time of creation.
            Timing():
                tik(clock()),
                creation_date(chrono::high_resolution_clock::to_time_t(chrono::high_resolution_clock::now()))
            {
                tik = clock();
            }

            ///Return current date string.
            string CurrentDate()
            {   
                auto now = chrono::high_resolution_clock::now();
                auto t_time = chrono::high_resolution_clock::to_time_t(now);
                return ctime(&t_time);
            }

            ///Return date of object creation string.
            string CreationtDate()
            {   
                
                return ctime(&creation_date);
            }

            ///Returns elapsed time, and save it into vector.
            double Record()
            {
                auto tok = clock();
                records.push_back((double)(tok - tik)/CLOCKS_PER_SEC);
                tik = tok;
                return records.back();
            }


            ///Return total time of simmulation.
            double Total()
            {
                double sum = 0;
                for(auto t: records)
                    sum += t;
                return sum;
            }

            ///Vector that holds all times between Record() calls.
            vector<double> records;
        
        private:

            ///Stores last time from callig Record function.
            clock_t tik, tok;
            time_t creation_date;
    };
}