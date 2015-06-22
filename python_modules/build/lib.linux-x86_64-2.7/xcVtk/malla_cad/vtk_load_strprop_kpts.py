# -*- coding: utf-8 -*-
def VtkLoadStrPropKPts(nmbSet, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray()
  eSet= mdlr.getSets.getSet(nmbSet)
  eSet.numerate()
  pnts= eSet.getPuntos()
  for p in pnts:
    nmbContndr.insert_value(p.getIdx,deref(nmbProp)) # Asignamos etiqueta  
  ugrid.GetPointData().SetStrings(nmbContndr)
