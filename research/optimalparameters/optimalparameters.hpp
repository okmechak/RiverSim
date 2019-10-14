/*! \page experiments Testing Program

    \tableofcontents

    \section intro Introduction

    As it appearred, writing a simulation program is only first half of solution. 
    Next is testing, parameter adjustment, running simulations, processing of results, making conclussions etc.

    This section is dedicated to __parameter adjustment__ and __testing__.
    
    \section parlookscript Parameter Lookup Script.
    
    RiverSim program can accomplish two main things: 

    + Generate river network based on intial conditions(see \ref io.hpp).
    + Backward river evoltuion used in one of methods to calculate river parameters.

    These options are controled by program option --simulation-type(or -t, see ./riversim -h for details).

    + Also it has third mode - used for testing and development purposes.

    RiverSim program has a lot of parameters and a lot of them impact accuracy of results, but also time of
    calculations. To figure out "optimal" parameters is hard.

    For this purpose was created shell script: `parameter_lookup.sh`.
    It itterates over some most important parameters for accuracy in the third "development" mode of program.
    Also it runs in each program in parallel, this feature drastically improves perfomance. Also `parameter_lookup.sh`
    can be used as example for writing other scripts.

    [source code of parameter_lookup.sh](parameter_lookup.sh)
    
    \note on some systems default location of bash is different from `#!/bin/bash`

    \subsection freefemmathematicariversim Comparison with Mathematica and FreeFem
    
    \subsubsection freefemtest FreeFem++ Test Program

    [FreeFem source code](test.edp)

    __Illustratrion steps of how freefem++ test program works__: 

    <table style="width:100%">
        <tr>
            <th> Boundaries setup </th>
            <th> Mesh generation </th>
            <th> Adaptive mesh </th>
            <th> Solution </th>
            <th> Integration around tips </th>   
        </tr>
        <tr>
            <th> \imageSize{FreeFemBoundary.jpg,     height:100%;width:100%;, } </th>
            <th> \imageSize{FreeFemMesh.jpg,         height:100%;width:100%;, } </th>
            <th> \imageSize{FreeFemAdaptiveMesh.jpg, height:100%;width:100%;, } </th>
            <th> \imageSize{FreeFemSolution.jpg,     height:100%;width:100%;, } </th>
            <th> \imageSize{FreeFemTipMesh.jpg,      height:100%;width:100%;, } </th>
        </tr>
    </table>

    __Test values:__

    + series params
        + a1 = __0.1018660081624733__
        + a2 = __0.04546179329449326__
        + a3 = __0.1308697639326795__
    + integral over whole region:
        __0.03420196874531437__
    + integration over tip circle
        __0.0004613243016595069__

    \subsubsection mathematicatest Mathematica Test Program

    This script implements same solution to FreeFem++ but in high level language Wolfram Mathematica.

    [Mathemamtica program source code](RectangularRegionTest.nb)

    <CENTER>
    <table style="width:70%">
        <tr>
            <th> Boundaries setup </th>
            <th> Refined mesh </th>
            <th> Solution </th>
        </tr>
        <tr>
            <th> \imageSize{MathematicaBoundary.jpg,     height:100%;width:100%;, } </th>
            <th> \imageSize{MathematicaMesh.jpg,     height:100%;width:100%;, } </th>
            <th> \imageSize{MathematicaSolution.jpg,     height:100%;width:100%;, } </th>
        </tr>
    </table>
    </CENTER>

    __Test values:__

    + series params
        + a1 = 
        + a2 = 
        + a3 = 
    + integral over whole region:
        
    + integration over tip circle:
        
    + maximal value:

    \todo evaluate series params and target values in matheamtica
        
    \paragraph mathmeshreftest Mesh Refinment Function research

    Solving FEM is a lot about finding of golden ration, some optimal parameter values.
    On one side we have precission, and on the other - time. One of the biggest impact, on solution, 
    is done by mesh. Program uses FEM which needs meshing of region, and there are regions where solution 
    changes slowly(centrum), and very fast - around tips. So the problem is to find some mesh configuration, which
    will fit solution with less number of elements.

    FreeFem solution already uses function which constrains element size, in such way that mesh is smallest at tip,
    and gets coarser far away from tip. Currently FreeFem and riversim uses next heuristic function, to constrain size 
    of mesh element:
    \f[ 
        \frac{\beta}{\alpha+exp(-\frac{r}{r_0})}    
    \f] 
    where, \f$\beta, \alpha, r_0\f$ - parameters, and \f$r\f$ - distance to tip point.
    But how about all other possible types of function, like these:
    \f[ 
        const \\
        \alpha r \\
        \alpha r^n \\
        E^r
    \f]
    Does they give better solution with less resources? Lets find out.

    \imageSize{MeshConstraintFunctions.jpg,       height:50%;width:50%;, Mesh constraint test functions}
    \imageSize{MeshGenerationTime.jpg,            height:50%;width:50%;, Mesh generation time(s) as function of mesh elements number}
    \imageSize{SolutionTime.jpg,                  height:50%;width:50%;, PDE soltuion time as function of mesh elements number}
    
    Next plots show value of integral over whole rectangular region \f$\oint solution(x,y) dx dy\f$:

    <CENTER>
    <table style="width:70%">
        <tr>
            <th> All test functions </th>
            <th> Without the worst one </th>
        </tr>
        <tr>
            <th> \imageSize{WholeRegionConvergence.jpg,        height:100%;width:100%;, Integral value as function of mesh elements number} </th>
            <th> \imageSize{WholeRegionConvergenceDropped.jpg, height:100%;width:100%;, Integral value as function of mesh elements number} </th>
        </tr>
    </table>
    </CENTER>

    From these plots can be seen that \f$\alpha\sqrt{r}\f$ converge fastest. But if we will consider another target value instead
    of whole region integral, like integral over circular region aroud tip(which is very important to series parameters calculation)
    \f$\oint_{|(x,y)-\vec{R_{tip}}|<r_0}^{ }solution(x,y)dxdy\f$, then convergence plot will look like this:

    \imageSize{TipIntegralConvergenceDropped.jpg,      height:50%;width:50%;, Mesh generation time(s)}

    Now best mesh constraint area function is \f$\alpha r^{3/2}\f$, which is worst in previous case of whole region integral.
    
    Results are contradictive and trivial, becouse in case of whole region integral \f$\alpha \sqrt{r}\f$ gives small mesh elements
    on whole region. In case of tip integral \f$\alpha r^{3/2}\f$ - gives small elements near tip.

    \todo Create new target value, which will better judge mesh constraint funcitons. 

    \subsubsection mathematicanumparams Wolfram Mathematica Numerical Method Parameters

    \paragraph mathnumpar NItegrate, NDSolve

    Wolfram Mathematica provides a lot of builtin numerical functions. Some functions encapsulates a lot of functionality inside,
    which are controlled by parameters. For example __NItegrate__ and __NDSolve__, which are used for solving PDE and 
    evaluation of series parameters, dependce on few very important parameters: __AccuracyGoal__ and __PrecisionGoal__.
    These parameters have big inpact on precision but also on evalution time. The question is straightforward - 
    what are the optimal values for them? 

    \note Also there are __MaxRecursion__ and __WorkingPrecision__ parameters, 
    but testing these parameters didn't show significant impact on precision.

    \imageSize{PrecisionAndAccuracyIntegration.jpg, height:50%;width:50%;, Plot of result convergence and time of evalution as function of AccuracyGoal and PrecisionGoal}
    
    As can be seen from plots values AccuracyGoal ~= 5 and PrecisionGoal ~= 6 gives good precision in small evaluation time.
*/