# River Growth Simulation
### Simulation of river growth using Laplace equation

## System setup
### Dependecies  

At firs install __Deal.ii__ package by running command:
> sudo apt install libdeal.ii-dev  


Next you need __Gmsh SDK__ package, from [official site](http://gmsh.info/#Download)
be sure that you choosed SDK link. And unpack it to /usr/local/gmsh location. Configuring *CMakeLists.txt* you can specify another location. Also it is easy to build from [source](https://gitlab.onelab.info/gmsh/gmsh) with shared objects option turned on.


And finaly __Tetgen__, [from github repo](https://github.com/libigl/tetgen).
Build it and install to /usr/local/tetgen. Copy by hand header(tetgen.h) and library(libtetgen.so) file from build directory to. You have to reconfigure CMakeFile by replacing STATIC -> SHARED to generate libtetgen.so instead of libtetgen.a. 

### Notes

Tetgen and Gmsh are interchangable, but both will be included into project.
Gmsh even is based on functionality of Tetgen, which is further development of __Triangle__(which is also included in project).  

But Gmsh is preferrable, cos it inludes some postprocess functionality and quadric mehs ellemts, which are mandatory for Deal.II. 

Make sure that you have installed __libgfortran3__ (dependencie of Gmsh).





