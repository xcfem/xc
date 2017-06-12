# -*- coding: utf-8 -*-

def VtkDibujaIdsElementos(ids):
  # Dibuja las etiquetas de los elementos.
  cc= vtk.vtkCellCenters()
  vtk.SetInput(ids) #  Centroides de las celdas. 

  visCells= vtk.vtkSelectVisiblePoints()
  visCells.SetInput(cc)
  visCells.SetRenderer("renderer")
  visCells.SelectionWindowOff()

  #Create the mapper to display the cell ids.  Specify the format to
  # use for the labels.  Also create the associated actor.

  cellMapper= vtk.vtkLabeledShStrMapper
  cellMapper.SetInput(visCells)
  cellMapper.LabelTextProperty().SetColor(0,0,0.9)

  cellLabels= vtk.vtkActor2D()
  cellLabels.SetMapper(cellMapper)
