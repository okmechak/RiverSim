# River Growth Simulation (RiverSim)

[![Faculty of Physics. University of Warsaw](https://www.fuw.edu.pl/tl_files/downloads/logo_18/FUW_znak-poziomy-EN.png)](https://www.fuw.edu.pl/)  

Simulation of river growth using model based on Laplace equation[1].
Mathematicaly, in this program we solve PDE equation using Finite Element Method(FEM). And as result program produces VTK file, which contains solution and its details, which furthere can be viewed in [__ParaView__](https://www.paraview.org/)  

Program is written in C++ Language.  And contains few built-in  and external dependencies.  

#### Built-in dependencies:
  * [__Triangle__](http://www.cs.cmu.edu/afs/cs/Web/People/quake/triangle.html) - robust and state of the art mesh generation tool.  
  * [__Tethex__](https://github.com/martemyev/tethex/wiki) - tool used for convertation from triangles to quadrangels.
  * [__cxxopts__](https://github.com/jarro2783/cxxopts) - c++ header library used to process command line options. Has same syntaxys as Boost program options.

#### External dependencies:
* [__Deal.II__](https://www.dealii.org/) - A C++ software library supporting the creation of finite element codes and an open community of users and developers.
* [__Boost__](https://www.boost.org/doc/libs/1_66_0/libs/test/doc/html/index.html) - used for unit testing.


## 1. System setup
##### This setup applies to Ubuntu, Mint and Windows Linux Subsyste, at least those are tested.
As we discussed above product contains few external  dependencies: __Deal.II__ and __Boost__.
To build system on computer

#### Build environmet
To build from source at every beginning you need tools like: _CMake_, _Make_, _GCC compiler_ etc.
easiest way to install them all is:
> sudo apt install build-esential

also _Cmake_ with curses gui(Terminal gui) is very handy:
> sudo app install cmake-curses-gui

and call command is same as cmake, but:
> ccmake <path-to-source> 

#### Deal.II setup:

* Using ubuntu package system:
    > sudo apt install libdeal.ii-dev  
* Download prebuild files from official site.
* Build from repository(Read INSTALL instruction for details, static lib options shoud be turned ON).

#### Boost:
> sudo apt install libbost-all-dev

### CentOS
Problem: Program uses c++ 17 standard features, but last prebuild gcc version for CentOS is 4.8 and it only supports c++ 11 standart. 
Solution is to build from source gcc compiler(more details by this [link](https://linuxhostsupport.com/blog/how-to-install-gcc-on-centos-7/)). Then compile Deal.II from source, there will be some problems which can be resolved by this [link](https://stackoverflow.com/questions/5216399/usr-lib-libstdc-so-6-version-glibcxx-3-4-15-not-found).

## 2. Compiling and running Tests
When you have configured and setuped all dependencies. Now you can build RiverSim source code using __cmake__.  
It is very easy and fast(in comparsion to Deal.II compiling process), you just have to setup two variables in cmake: 
* DEALII_DIR - path too Deal.II files, and

  > cmake -DDEALII_DIR:STRING=/path/to/deal/ii -DCMAKE_INSTALL_PREFIX=/installation/path  /path/to/riversim/project/folder

Makefile will be generated, which next is used for compiling of program:

  > make
  or for faster(multithread build) type
  > make -j5
  
##### Installation:
  > make install   
  
Project will be installed to path specified by _CMAKE_INSTALL_PREFIX_ variable.

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
  
Typical forward river simulation command:
> ./riversim -n 100 -o simdata

And after that, typical backward simulation commnad:
> ./riversim -n 130 -o revsim --simulation-type=1 simdata.json
  
add more details about workflow with program(TODO)..


## 4. Notes


## 5. References
[1]: [Problem odwrotny do ewolucji sieci rzecznych](https://www.fuw.edu.pl/~piotrek/theses/PMorawiecki.pdf).
[2]: Todo add other referneces.

## 6. Links

* [Github repo](https://github.com/okmechak/RiverSim).
* [Website of project](https://okmechak.github.io/RiverSim/).
