/*! \page mathematicascripts Wolfram Mathematica Scripts
    \brief Some usefuls scripts, functions for ease work with
    riversim program.
    
    \tableofcontents

    \section intromats List of Mathematica functions to work with RiverSIM

    \subsection iomath I/O

    Interactive dialog to open RiverSim output files in format of json

    ```Mathematica
    OpenJSONDialog[] := 
        SystemDialogInput[
            "FileOpen", {"simdata.json", {"RiverSim files" -> {"*.json"}}}
        ]
    ```

    Model configuration parameters. Can be used as input to riversim:

    \todo Fix this implementation, cos now program uses default value for parameter
        if this parameter isn't specified in input

    ```Mathematica
    RadiusRiversim = 0.01
    Options[ModelJSONStructure] = {
       "dx" -> 0.2,
       "width" -> 1,
       "height" -> 1,
       "riverBoundaryId" -> 4,
       "boundaryIds" -> {1, 2, 3, 4},
       "boundaryCondition" -> 0,
       "fieldValue" -> 1,
        ....
    ```

    RiverSim program of cource can be run from bash.

    \todo describe fucntions from ExperimentRunner.nb

*/