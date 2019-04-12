import sys
sys.path.append("/usr/local/gmsh_release/lib")
import gmsh
import numpy as np
import matplotlib

gmsh.initialize(sys.argv)
gmsh.model.add("square")
gmsh.model.geo.addPoint(0, 0, 0, 0., 1)
gmsh.model.geo.addPoint(0.5, 0, 0, 0., 2)
gmsh.model.geo.addPoint(1, 0, 0, 0., 3)
gmsh.model.geo.addPoint(1, 1, 0, 0., 4)
gmsh.model.geo.addPoint(0, 1, 0, 0., 5)
gmsh.model.geo.addPoint(0.5, 0.2, 0, 0., 6)
gmsh.model.geo.addPoint(0.2, 0.5, 0, 0., 7)
gmsh.model.geo.addPoint(0.8, 0.5, 0, 0., 8)
gmsh.model.geo.addPoint(0.8, 0.5001, 0, 0., 9)

gmsh.model.geo.addLine(1, 2, 1)
gmsh.model.geo.addLine(2, 6, 2)
gmsh.model.geo.addLine(6, 2, 3)
gmsh.model.geo.addLine(2, 3, 4)
gmsh.model.geo.addLine(3, 4, 5)
gmsh.model.geo.addLine(4, 5, 6)
gmsh.model.geo.addLine(5, 1, 7)
gmsh.model.geo.addLine(7, 8, 8)
#gmsh.model.geo.addLine(8, 9, 9)
#gmsh.model.geo.addLine(9, 7, 10)
gmsh.model.addDiscreteEntity(2, 8, 8)
gmsh.model.geo.addCurveLoop([1, 2, 3, 4, 5, 6, 7], 1)
gmsh.model.geo.addCurveLoop([8, 9, 10], 2)
gmsh.model.geo.addPlaneSurface([1], 6)
gmsh.model.geo.synchronize()

#gmsh.model.mesh.setRecombine(2, 6)
gmsh.model.mesh.generate(2)

gmsh.fltk.run()
gmsh.write("square.msh")
gmsh.finalize()
