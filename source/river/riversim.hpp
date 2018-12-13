#pragma once

#include "utilities.hpp"
#include "geometry.hpp"
#include "mesh.hpp"
#include "solver.hpp"

namespace River
{
    namespace SimpleGeo{
    Geometry CustomRiverTree(double dl = 0.01, double eps = 1e-5);
    Geometry Box();
    }
    /* 
        TODO: 
        Implementation of some standard workflows 
        like:
            - forward river evolution
            - backward river evolution
            - statistics and postrocessing
            - evaluation of parameters
            - other
    
    */


}