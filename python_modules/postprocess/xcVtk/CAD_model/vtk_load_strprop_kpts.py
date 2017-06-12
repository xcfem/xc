# -*- coding: utf-8 -*-
def VtkLoadStrPropKPts(setName, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray()
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  pnts= eSet.getPuntos()
  for p in pnts:
    nmbContndr.insert_value(p.getIdx,deref(nmbProp)) # Asignamos etiqueta  
  ugrid.GetPointData().SetStrings(nmbContndr)
