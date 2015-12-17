# -*- coding: utf-8 -*-
def VtkLoadKPts(setName, nmbContndr):
  nmbContndr= vtk.vtkPoints()
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  print("nombre set: ",eSet.nombre,"\n")
  print("num. puntos: ",eSet.getNumPuntos,"\n")
  points= eSet.getPoints
  for p in points:
    nmbContndr.insert_point(p.getIdx,pos.x,pos.y,pos.z)
  ugrid.SetPoints(nmbContndr)

