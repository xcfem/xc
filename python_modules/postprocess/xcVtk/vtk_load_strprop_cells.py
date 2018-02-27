# -*- coding: utf-8 -*-
def VtkLoadStrPropCells(setName, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray() # Define label container
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  elementsSet= eSet.getElements
  for e in elementSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Set label.

  ugrid.CellData().SetStrings(nmbContndr)
