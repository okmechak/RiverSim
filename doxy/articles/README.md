# RiverSim
Simulation of river growth using model based on Laplace equation[1].
Mathematicaly, in this program we solve PDE equation using Finite Element Method(FEM). And as result program produces VTK file, which contains solution and its details, which furthere can be viewed in [__ParaView__](https://www.paraview.org/)  

Program is written in C++ Language.  And contains few built-in  and external dependencies.  

#### Built-in dependencies:
  * [__Triangle__](http://www.cs.cmu.edu/afs/cs/Web/People/quake/triangle.html) - robust and state of the art mesh generation tool.  
  * [__Tethex__](https://github.com/martemyev/tethex/wiki) - tool used for convertation from triangles to quadrangels.
  * [__cxxopts__](https://github.com/jarro2783/cxxopts) - c++ header library used to process command line options. Has same syntaxys as Boost program options.

#### External dependencies:
* [__Gmsh__](http://gmsh.info/) - also mesh generation tool, with priority on GUI. And mostly used, in this case, for mesh visualization.
* [__Deal.II__](https://www.dealii.org/) - A C++ software library supporting the creation of finite element codes and an open community of users and developers.
* [__Boost__](https://www.boost.org/doc/libs/1_66_0/libs/test/doc/html/index.html) - used for unit testing.


## 1. System setup
As we discussed above product contains few external  dependencies: __Deal.II__,  __Gmsh__ and __Boost__.
To build system on computer

#### Deal.II setup:

* Using ubuntu package system:
    > sudo apt install libdeal.ii-dev  
* Download prebuild files from official site.
* Build from repository(Read INSTALL instruction for details, static lib options shoud be turned ON).

#### Gmsh setup:
* Download SDK files from official site.
* Build from repository(Read INSTALL instruction for details, static lib and share lib options shoud be turned ON).

#### Gmsh setup:
* Using Ubuntu package system is the most starightforward in this case:
    > sudo apt install libboost-all

## 2. Compiling and running Tests
When you have configured and setuped all dependencies. Now you can build RiverSim source code using __cmake__.  
It is very easy and fast(in comparsion to Deal.II compiling process), you just have to setup two variables in cmake: 
* DEALII_DIR - path too Deal.II files, and
* GMSH_DIR - path to Gmsh SDK files.

  > cmake -DDEALII_DIR:STRING=/path/to/deal/ii -DGMSH_DIR:STRING=/path/to/gmsh /path/to/riversim/project/folder

Makefile will be generated, which next is used for compiling of program:

  > make

#### Running Tests
After compiling, simply type a command:
  > make test


## 3. How to use RiverSim program

As any Linux program, now just type in terminal from folder source(in your build folder) command:
  > ./riversim

More details about parameters you can find in __help__ of program:
  > ./riversim -h
or
  > ./riversim --help

add more details about workflow with program(TODO)..


## 4. Notes

Gmsh and Triangle are interchangable, but both will be included into project.
Gmsh even is based on functionality of Tetgen, which is further development of __Triangle__.  

But Gmsh is preferrable, cos it inludes some postprocess functionality and quadric mehs elemts, which are mandatory for Deal.II. 


## 5. References
[1]: Add links on scientific work(TODO)

## 6. Links

* [Github repo](https://github.com/okmechak/RiverSim).
* [Website of project](https://okmechak.github.io/RiverSim/).