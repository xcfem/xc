# -*- coding: utf-8 -*-
def VtkLoadKPts(nmbSet, nmbContndr):
  nmbContndr= vtk.vtkPoints()
  eSet= mdlr.getSets.getSet(nmbSet)
  eSet.numerate()
  print("nombre set: ",eSet.nombre,"\n")
  print("num. puntos: ",eSet.getNumPuntos,"\n")
  points= eSet.getPoints
  for p in points:
    nmbContndr.insert_point(p.getIdx,pos.x,pos.y,pos.z)
  ugrid.SetPoints(nmbContndr)

