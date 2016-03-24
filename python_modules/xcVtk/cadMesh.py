# -*- coding: utf-8 -*-
import vtk
import xc_base
import xc
from miscUtils import LogMessages as lmsg
import creaArraySetData


def VtkDefineActorKPoint(recordGrid, renderer, radio):
  '''Returns a vtkActor to represent key-points in a rendering scene.
  It defines scale, rendering properties, textures, ...
  Attributes:
    recordGrid: 
  '''
  sphereSource= vtk.vtkSphereSource()
  sphereSource.SetRadius(radio)
  sphereSource.SetThetaResolution(5)
  sphereSource.SetPhiResolution(5)
  
  markKPts= vtk.vtkGlyph3D()
  markKPts.SetInput(recordGrid.uGrid) 
  markKPts.SetSource(sphereSource.GetOutput())
  markKPts.ScalingOff()
  markKPts.OrientOff()
    
  mappKPts= vtk.vtkPolyDataMapper()
  mappKPts.SetInput(markKPts.GetOutput())

  visKPts= vtk.vtkActor()
  visKPts.SetMapper(mappKPts)
  visKPts.GetProperty().SetColor(.7, .5, .5)
    
  renderer.AddActor(visKPts)

def VtkDefineActorCells(recordGrid, renderer, tipoRepr):
  # Creamos el actor para las superficies.
  uGridMapper= vtk.vtkDataSetMapper()
  uGridMapper.SetInput(recordGrid.uGrid)
  cellActor= vtk.vtkActor()
  cellActor.SetMapper(uGridMapper)
  cellActor.GetProperty().SetColor(1,1,0)
  if (tipoRepr=="points"):
    cellActor.GetProperty().SetRepresentationToPoints()
  elif(tipoRepr== "wireframe"):
    cellActor.GetProperty().SetRepresentationToWireframe()
  elif(tipoRepr== "surface"):
    cellActor.GetProperty().SetRepresentationToSurface()
  else:
    lmsg.error("error: "+tipoRepr+" no implementada.")
  renderer.AddActor(cellActor) # Actor para las celdas

def VtkCargaIdsKPts(uGrid, setToDraw):
  etiqKPt=  creaArraySetData.VtkCreaStrArraySetData(setToDraw,'pnts','nombre')
  uGrid.GetPointData().AddArray(etiqKPt)

def VtkCargaIdsCells(uGrid, setToDraw, nmbTipoEnt):
  etiqCells= creaArraySetData.VtkCreaStrArraySetData(setToDraw,nmbTipoEnt,'nombre')
  uGrid.GetCellData().AddArray(etiqCells)

# Dibuja las etiquetas de los puntos
def VtkDibujaIdsKPts(uGrid, setToDraw, renderer):
  numKPtsDI= setToDraw.getPoints.size
  if(numKPtsDI>0):
    ids= vtk.vtkIdFilter()
    ids.SetInput(uGrid)
    ids.CellIdsOff()
    ids.PointIdsOff()

    VtkCargaIdsKPts(uGrid,setToDraw)
            
    visPts= vtk.vtkSelectVisiblePoints()
    visPts.SetInputConnection(ids.GetOutputPort())
    visPts.SetRenderer(renderer)
    visPts.SelectionWindowOff()

    #Create the mapper to display the point ids.  Specify the format to
    #  use for the labels.  Also create the associated actor.
    ldm= vtk.vtkLabeledDataMapper()
    ldm.SetInputConnection(visPts.GetOutputPort())
    ldm.GetLabelTextProperty().SetColor(0.1,0.1,0.1)

    pointLabels= vtk.vtkActor2D()
    pointLabels.SetMapper(ldm)

    renderer.AddActor2D(pointLabels)
  else:
    print "El conjunto: '",setToDraw,"' no tiene KPts."

# ****** Creamos las etiquetas para las celdas *******
def VtkDibujaIdsCells(uGrid, setToDraw, nmbTipoEnt, renderer):
  ids= vtk.vtkIdFilter()
  ids.SetInput(uGrid)
  ids.CellIdsOff()
  ids.PointIdsOff()

  VtkCargaIdsCells(uGrid,setToDraw,nmbTipoEnt)
    
  # Dibuja las etiquetas de las líneas.
  cc= vtk.vtkCellCenters()
  cc.SetInputConnection(ids.GetOutputPort()) #  Centroides de las celdas. 
        
  visCells= vtk.vtkSelectVisiblePoints()
  visCells.SetInputConnection(cc.GetOutputPort())
  visCells.SetRenderer(renderer)
  visCells.SelectionWindowOff()

  #Create the mapper to display the cell ids.  Specify the format to
  # use for the labels.  Also create the associated actor.
  cellMapper= vtk.vtkLabeledDataMapper()
  cellMapper.SetInputConnection(visCells.GetOutputPort())
  cellMapper.GetLabelTextProperty().SetColor(0,0,0.9)

  cellLabels= vtk.vtkActor2D()
  cellLabels.SetMapper(cellMapper)
       
  renderer.AddActor2D(cellLabels)

def VtkCargaMalla(preprocessor,recordGrid):
  kpuntos= vtk.vtkPoints()
  # Definimos grid
  recordGrid.uGrid.SetPoints(kpuntos)
  setToDraw= preprocessor.getSets.getSet(recordGrid.setName)
  setToDraw.numerate()
  pnts= setToDraw.getPoints
  for p in pnts:
    kpuntos.InsertPoint(p.getIdx,p.getPos.x,p.getPos.y,p.getPos.z)
  cellSet= setToDraw.getLines # cells are lines as default.
  if(recordGrid.cellType=="faces"):
    cellSet= setToDraw.getSurfaces
  elif (recordGrid.cellType=="bodies"):
    cellSet= setToDraw.getBodies
  for c in cellSet:
    vertices= xc_base.vector_int_to_py_list(c.getIdxVertices)
    vtx= vtk.vtkIdList()
    for vIndex in vertices:
       vtx.InsertNextId(vIndex)
    recordGrid.uGrid.InsertNextCell(c.getVtkCellType,vtx)




