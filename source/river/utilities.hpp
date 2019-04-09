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
#include <chrono>
#include <ctime>

using namespace std;


namespace River{
    
    ///Prints to console program logo.
    void print_ascii_signature();

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
            Timing()
            {
                last_time_point = chrono::system_clock::now();
            }

            ///Return current date string.
            string CurrentDate()
            {   
                auto now = chrono::system_clock::now();
                auto t_time = chrono::system_clock::to_time_t(now);
                return ctime(&t_time);
            }

            ///Return date of object creation string.
            string CreationtDate()
            {   
                auto t_time = chrono::system_clock::to_time_t(last_time_point);
                return ctime(&t_time);
            }

            ///Returns elapsed time, and save it into vector.
            double Record()
            {
                records.push_back(
                    (chrono::system_clock::now() 
                    - last_time_point).count()
                );
                if(flag)
                {
                    //quickfix
                    records.erase(begin(records));
                    flag = false;
                }

                return records.back();
            }


            ///Return total time of simmulation.
            double Total()
            {
                double sum = 0;
                for(auto el: records)
                    sum += el;
                return sum;
            }

            ///Vector that holds all times between Record() calls.
            vector<double> records;
        
        private:

            //quickfix
            bool flag = true;

            ///Stores last time from callig Record function.
            chrono::system_clock::time_point last_time_point;
    };
}