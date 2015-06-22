# -*- coding: utf-8 -*-

def VtkCargaIdsCells(nmbUGrid,nmbSet,nmbTipoEnt):
  etiqCeldas= VtkCreaStrArraySetData(nmbSet,nmbTipoEnt,"codigo")
  nmbUGrid.cell_data.set_strings(etiqCeldas)

# ****** Creamos las etiquetas para las celdas *******
def VtkDibujaIdsCells(nmbUGrid,nmbSet,nmbTipoEnt,renderer):
  ids= vtk.VtkIdFilter()
  ids.SetInput(nmbUGrid)
  ids.CellIdsOff()
  ids.PointIdsOff()
  VtkCargaIdsCells(nmbUGrid,nmbSet,nmbTipoEnt)
  # Dibuja las etiquetas de las líneas.
  cc= vtk.VtkCellCenters() # Centroides de las celdas.
  cc.SetInput(ids)
  visCells= vtk.VtkSelectVisiblePoints()
  visCells.SetInput(cc)
  visCells.SetRenderer(renderer)
  visCells.SelectionWindowOff()

  #Create the mapper to display the cell ids.  Specify the format to
  #   use for the labels.  Also create the associated actor.
  cellMapper= vtk.VtkLabeledShStrMapper()
  cellMapper.setInput(visCells)
  cellMapper.LabelTextProperty.setColor(0,0,0.9)
  cellLabels= vtk.VtkActor2D()
  cellLabels.setMapper(cellMapper)
  renderer.AddActor2D(cellLabels)

def VtkDefineActorCells(nmbUGrid,renderer,tipoRepr):
  # Creamos el actor para las superficies.
  gridMapper= vtk.VtkDataSetMapper()
  gridMapper.setInput(nmbUGrid)
  cellActor= vtk.VtkActor()
  cellActor.SetMapper(gridMapper)
  cellActor.property.SetColor(1,1,0)
  cellActor.property.SetRepresentation(tipoRepr)
  renderer.AddActor(cellActor)# Actor para las celdas
