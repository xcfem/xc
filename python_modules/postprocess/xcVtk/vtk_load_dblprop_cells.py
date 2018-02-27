# -*- coding: utf-8 -*-
def VtkLoadDblProp(setName, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkDoubleArray() # Define label container
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  edgeSet= eSet.getLines
  for e in edgeSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Set label

  ugrid.CellData().SetScalars(nmbContndr)

