# -*- coding: utf-8 -*-
def VtkLoadStrPropLineas(nmbSet, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray()
  eSet= preprocessor.getSets.getSet(nmbSet)
  eSet.numerate()
  lineas= eSet.getLines
  for l in lineas:
    nmbContndr.insert_value(l.getIdx,deref(nmbProp)) # Asignamos etiqueta
  ugrid.CellData().SetString(nmbContndr)
