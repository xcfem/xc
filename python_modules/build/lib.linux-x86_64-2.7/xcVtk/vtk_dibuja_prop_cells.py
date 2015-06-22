# -*- coding: utf-8 -*-
def VtkDibujaPropCells(nmbFilter):
  # Dibuja las etiquetas de las líneas.
  cc= vtk.vtkCellCenters()
  cc.SetInput(nmbFilter) #  Centroides de las celdas. 
        
  visCells= vtk.vtkSelectVisiblePoints()
  visCells.SetInput(cc)
  visCells.SetRenderer(renderer)
  visCells.SelectionWindowOff()

  #Create the mapper to display the cell ids.  Specify the format to
  # use for the labels.  Also create the associated actor.
  cellMapper= vtk.vtkLabeledShStrMapper()
  cellMapper.SetInput(visCells)
  ltp= cellMapper.LabelTextProperty()
  ltp.SetColor(0,0,0.9)
  ltp.BoldOff()
  ltp.ShadowOff() 
  ltp.ItalicOff()
  ltp.SetJutificationToCentered()
  ltp.SetFontSize(10)

  cellLabels= vtk.vtkActor2D()
  cellLabels.SetMapper(cellMapper)
        
  renderer.AddActor2D(cellLabels)

