def VtkLoadNodes(setName, nmbContndr):
  nmbContndr= vtk.vtkPoints() # Define the point set to be drawn
  nSet= preprocessor.getSets.getSet(setName)
  nSet.numerate()
  print "nombre set: ",nSet.nombre
  print "num. nodes: ",nSet.getNumNodes
  nodes= nSet.nodes
  for n in nodes:
    nmbContndr.InsertPoint(n.getIdx,coord[0],coord[1],coord[2])
  ugrid.SetPoints(nmbContndr)
