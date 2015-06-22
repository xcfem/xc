# -*- coding: utf-8 -*-
def VtkLoadStrPropCells(nmbSet, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray() # Definimos el contenedor para las etiquetas a dibujar
  eSet= mdlr.getSets.getSet(nmbSet)
  eSet.numerate()
  elementsSet= eSet.getElements
  for e in elementSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Asignamos etiqueta

  ugrid.CellData().SetStrings(nmbContndr)
