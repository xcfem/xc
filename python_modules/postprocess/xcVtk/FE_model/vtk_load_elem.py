def VtkLoadElementos(setName):
  eSet= preprocessor.getSets.getSet(setName)
  elems= eSet.getElements
  print "nombre set: ",eSet.nombre
  print "num. elementos: ",eSet.getNumElementos
  for e in elems:
    ugrid.InsertNextCell(e.getVtkCellType,e.getVertices)
