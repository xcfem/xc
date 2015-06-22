# -*- coding: utf-8 -*-

from xcVtk import vtk_cargas_elementos
from xcVtk import vtk_cargas_nodos

def VtkCargas(mdlr, loadPattern, nmbRecordDisplay):
  lp= mdlr.getLoadLoader.getLoadPatterns.getLoad(loadPattern)
  clrVectores= lp.getProp("color")
  fEscalaVectores= lp.getProp("escala")
  VtkCargasElementos(loadPattern,deref(nmbRecordDisplay).nmbRenderer,clrVectores,fEscalaVectores)
  VtkCargasNodos(loadPattern,deref(nmbRecordDisplay).nmbRenderer,clrVectores,fEscalaVectores)

