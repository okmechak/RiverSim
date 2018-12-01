import sys
sys.path.append("/home/oleg/Programs/gmsh/lib")
import gmsh

gmsh.initialize(sys.argv)
gmsh.option.setNumber("General.Terminal", 1)
gmsh.option.setNumber("Mesh.MshFileVersion", 2.2)
gmsh.option.setNumber("Mesh.RecombineAll", 1)

gmsh.model.add("test")

# add discrete surface with tag 1
gmsh.model.geo.addPoint(0, 0, 0, 0.1)
gmsh.model.geo.addPoint(0.5, 0, 0, 0.001)
gmsh.model.geo.addPoint(0.5, 0.003, 0, 0.0001)
gmsh.model.geo.addPoint(0.5, 0.03, 0, 0.0001)
gmsh.model.geo.addPoint(0.50001, 0.003, 0, 0.0005)
gmsh.model.geo.addPoint(0.50001, 0, 0, 0.001)
gmsh.model.geo.addPoint(1, 0, 0, 0.1)
gmsh.model.geo.addPoint(1, 1, 0, 0.1)
gmsh.model.geo.addPoint(0, 1, 0, 0.1)



gmsh.model.geo.addLine(1 , 2)
gmsh.model.geo.addLine(2 , 3)
gmsh.model.geo.addLine(3 , 4)
gmsh.model.geo.addLine(4 , 5)
gmsh.model.geo.addLine(5 , 6)
gmsh.model.geo.addLine(6 , 7)
gmsh.model.geo.addLine(7 , 8)
gmsh.model.geo.addLine(8 , 9)
gmsh.model.geo.addLine(9 , 1)
gmsh.model.geo.addCurveLoop([1,2,3,4,5,6,7,8,9], 1)
gmsh.model.geo.addPlaneSurface([1])
gmsh.model.geo.synchronize()
gmsh.model.mesh.generate(2)
# export the mesh ; use explore.py to read and examine the mesh
gmsh.fltk.run()
gmsh.write("test.msh")

gmsh.finalize()

