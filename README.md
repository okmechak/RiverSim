# river_growth_simulation
Simulation of river growth using Laplace equation

## System setup
### Dependecies  

At firs install *Deal.ii* package by running command:
> sudo apt install libdeal.ii-dev  


Next you need *Gmsh SDK* package, from [official site](http://gmsh.info/#Download)
be sure that you choosed SDK link. And unpack it to /usr/local/gmsh location. Configuring *CMakeLists.txt* you can specify another location.  


And finaly *Tetgen*, [from github repo](https://github.com/libigl/tetgen).
Build it and install to /usr/local/tetgen. Copy by hand header(tetgen.h) and library(libtetgen.lib) file from build directory to 

### Note

Tetgen and Gmsh are interchangable, but both will be included into project.
Gmsh even is based on functionality of Tetgen, which is further development of Triangle.  

But Gmsh is preferrable, cos it inludes some postprocess functionality and quadric mehs ellemts, which are only supported by Deal.II.

## some errors 

make sure that you have installed *ligfortran3*(dependencie of Gmsh)





