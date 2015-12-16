# -*- coding: utf-8 -*-

import vtk

def VtkCargaIdsElem(nmbUGrid, setName):
  etiqElementos= VtkCreaStrArraySetData(setName,"elementos","tag")
  nmbUgrid.CellData().SetStrings(etiqElementos)


def VtkDibujaIdsElementos(nmbUGrid, setName, renderer):
  # ****** Creamos las etiquetas para las celdas *******
  ids= vtk.vtkIdFilter()
  ids.SetInput(nmbUGrid)
  ids.CellIdsOff()
  ids.PointIdsOff()
 
  VtkCargaIdsElem(nmbUGrid,setName)()
    
  # Dibuja las etiquetas de las líneas.
  cc= vtk.vtkCellCenters()
  cc.SetInput(ids) #  Centroides de las celdas. 
        
  visElems= vtk.vtkSelectVisiblePoints()
  visElems.SetInput(cc)
  visElems.SetRenderer(renderer)
  visElems.SelectionWindowOff()

  ''' Create the mapper to display the element ids.  Specify the format to
     use for the labels.  Also create the associated actor. '''
  elemMapper= vtk.vtkLabeledShStrMapper()
  elemMapper.SetInput(visElems)
  elemMapper.LabelTextProperty.SetColor(0,0,0.9)

  elemLabels= vtk.vtkActor2D()
  elemLabels.SetMapper(elemMapper)
  renderer.AddActor2D(elemLabels)

