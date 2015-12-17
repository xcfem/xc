# -*- coding: utf-8 -*-
def VtkLoadStrPropCells(setName, nmbProp, nmbContndr):
  nmbContndr= vtk.vtkShStrArray() # Definimos el contenedor para las etiquetas a dibujar
  eSet= preprocessor.getSets.getSet(setName)
  eSet.numerate()
  elementsSet= eSet.getElements
  for e in elementSet:
    nmbContndr.insert_value(e.getIdx,deref(nmbProp)) # Asignamos etiqueta

  ugrid.CellData().SetStrings(nmbContndr)
