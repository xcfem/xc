# -*- coding: utf-8 -*-
def VtkLoadLineas(setName):
  eSet= preprocessor.getSets.getSet(setName)
  print "nombre set: ",eSet.nombre
  print "num. lineas: ",eSet.getNumLineas
  # Cargamos las celdas en vtk
  ugrid.insert_next_cell(getVtkCellType,getVertices)

