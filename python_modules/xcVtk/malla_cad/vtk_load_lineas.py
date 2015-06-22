# -*- coding: utf-8 -*-
def VtkLoadLineas(nmbSet):
  eSet= mdlr.getSets.getSet(nmbSet)
  print "nombre set: ",eSet.nombre
  print "num. lineas: ",eSet.getNumLineas
  # Cargamos las celdas en vtk
  ugrid.insert_next_cell(getVtkCellType,getVertices)

