# -*- coding: utf-8 -*-
def VtkLoadDblProp(setName, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkDoubleArray() # Definimos el contenedor para las etiquetas a dibujar
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  edgeSet= eSet.getLines
  for e in edgeSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Asignamos etiqueta

  ugrid.CellData().SetScalars(nmbContndr)

