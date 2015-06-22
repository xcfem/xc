# -*- coding: utf-8 -*-

import vtk

def VtkCargaIdsNodos(recordGrid):
  VtkCreaStrArraySetData(recordGrid.nmbSet,"nodos","etiqNod","tag")()
  nmbUGrid.GetPointData().SetStrings(etiqNod)

def VtkDibujaIdsNodos(recordGrid, renderer):
  # Dibuja las etiquetas de los nodos
  ids= vtk.vtkIdFilter()
  ids.SetInput(recordGrid.uGrid)
  ids.CellIdsOff()
  ids.PointIdsOff()

  VtkCargaIdsNodos(recordGrid)
    
  visPts= vtk.vtkSelectVisiblePoints()
  visPts.SetInput("ids")
  visPts.SetRenderer(renderer)
  visPts.SelectionWindowOff()

  #Create the mapper to display the point ids.  Specify the format to
  #   use for the labels.  Also create the associated actor.
  ldm= vtk.vtkLabeledShStrMapper()
  ldm.SetInput("visPts")
  ldm.LabelTextProperty().SetColor(0.1,0.1,0.1)
  nodeLabels= vtk.vtkActor2D().SetMapper(ldm)
  renderer.AddActor2D(nodeLabels)

