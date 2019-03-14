#pragma once

#include "utilities.hpp"
#include "geometry.hpp"
#include "geometrynew.hpp"
#include "mesh.hpp"
#include "solver.hpp"
#include "physmodel.hpp"
#include "border.hpp"

namespace River
{
    namespace SimpleGeo{
        Geometry Box(Model &model);
        Geometry SingleTip(Model &model, int size);
        Geometry CustomRiverTree(Model &model, int size);
    }

    int ForwardRiverEvolution(cxxopts::ParseResult &vm);
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