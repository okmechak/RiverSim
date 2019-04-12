# This reimplements gmsh/demos/boolean/boolean.geo in Python.
import sys
sys.path.append("/usr/local/gmsh_release/lib")
import gmsh

model = gmsh.model
factory = model.occ

gmsh.initialize(sys.argv)

gmsh.option.setNumber("General.Terminal", 1)
gmsh.option.setNumber("Mesh.MshFileVersion", 2.2)
gmsh.option.setNumber("Mesh.RecombineAll", 1)

gmsh.model.add("square")
gmsh.model.occ.addRectangle(0, 0, 0, 1, 1, 100)
gmsh.model.occ.synchronize()
gmsh.model.mesh.setTransfiniteSurface(100)
gmsh.model.mesh.generate(2)
gmsh.plugin.run("NewView")
print "before get"
type, tags, data, time, numComp = gmsh.view.getModelData(0, 0)
print "after get"
gmsh.fltk.run()
gmsh.finalize()
