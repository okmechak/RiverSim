/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

/*! \file utilities.hpp
    \brief Console printouts, version, title and timing of program execution.
*/

#pragma once

#include "version.hpp"
///\cond
#include <chrono>
#include <time.h>
#include <numeric>
#include <iterator>
#include <vector>
///\endcond

using namespace std;


namespace River{

    ///Program title.
    string ProgramTitle();
    
    ///Prints to console program logo.
    void print_ascii_signature();

    ///Returns version string defined in \ref version.hpp.
    string version_string();

    ///Prints to console program Version.
    void print_version();

    


    /*! \brief Timing is used for measuring time of each evaluation cycle and whole program simulation time.
        \details
        At creation of object it records current time into string.
        Then \ref River::Timing::Record() can be used for erecording each cycle time.
    */
    class Timing
    {
        public:
            
            ///Default Constructor, saves time of creation.
            Timing():
                tik(clock()),
                creation_date(chrono::high_resolution_clock::to_time_t(chrono::high_resolution_clock::now()))
            {
                tik = clock();
            }

            ///Return current date string.
            string CurrentDate() const
            {   
                auto now = chrono::high_resolution_clock::now();
                auto t_time = chrono::high_resolution_clock::to_time_t(now);
                return ctime(&t_time);
            }

            ///Return date of object creation string.
            string CreationtDate() const
            {   
                return ctime(&creation_date);
            }

            ///Returns elapsed time from last \ref Record() call and saves it into vector.
            double Record()
            {
                auto tok = clock();
                records.push_back((double)(tok - tik)/CLOCKS_PER_SEC);
                tik = tok;
                return records.back();
            }


            ///Returns total time of simmulation.
            double Total() const
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
            clock_t tik;
            ///Used for time record.
            clock_t tok;
            ///Creation date of object
            time_t creation_date;
    };
}