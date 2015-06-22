def VtkLoadElementos(nmbSet):
  eSet= mdlr.getSets.getSet(nmbSet)
  elems= eSet.getElements
  print "nombre set: ",eSet.nombre
  print "num. elementos: ",eSet.getNumElementos
  for e in elems:
    ugrid.InsertNextCell(e.getVtkCellType,e.getVertices)
