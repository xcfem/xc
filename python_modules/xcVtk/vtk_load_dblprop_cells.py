# -*- coding: utf-8 -*-
def VtkLoadDblProp(nmbSet, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkDoubleArray() # Definimos el contenedor para las etiquetas a dibujar
  eSet= preprocessor.getSets.getSet(nmbSet)
  eSet.numerate()
  edgeSet= eSet.getLines
  for e in edgeSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Asignamos etiqueta

  ugrid.CellData().SetScalars(nmbContndr)

