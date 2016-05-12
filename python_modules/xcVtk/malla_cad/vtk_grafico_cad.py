# -*- coding: utf-8 -*-
# Geometric entities representation (k-points,lines,surfaces,bodies,...)

import vtk
from xcVtk import cadMesh
from xcVtk import views
from xcVtk import vtk_grafico_base

class RecordDefDisplayCAD(vtk_grafico_base.RecordDefDisplay):
  ''' Define la salida gráfica.'''
  def defineEscenaMalla(self, recordGrid, field):
    # Define la escena de la malla en el dispositivo de salida.
    recordGrid.uGrid= vtk.vtkUnstructuredGrid()
    recordGrid.cellType= "lines"
    setToDraw= recordGrid.xcSet
    numKPts= setToDraw.getPoints.size
    if(numKPts>0):
      cadMesh.VtkCargaMalla(recordGrid)
      self.renderer= vtk.vtkRenderer()
      self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
      cadMesh.VtkDefineActorKPoint(recordGrid,self.renderer,0.02)
      cadMesh.VtkDefineActorCells(recordGrid,self.renderer,"wireframe")
      self.renderer.ResetCamera()

    #Implementar dibujo de etiquetas.
    # if(entToLabel=="cells"):
    #   xcVtk.cadMesh.VtkDibujaIdsCells(uGridCad,setToDraw,nmbTipoCeldas,renderer)
    # elif(entToLabel=="points"):
    #   xcVtk.cadMesh.VtkDibujaIdsKPts(uGridCad,setToDraw,renderer)

  def grafico_cad(self,xcSet,caption= ''):
    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.xcSet= xcSet
    self.displayGrid(defGrid,caption)

  def plotCadModel(self, xcSet, field, fName, caption= ''):
    defGrid= vtk_grafico_base.RecordDefGrid()
    defGrid.xcSet= xcSet
    self.defineEscenaMalla(defGrid,field)
    self.displayScene(caption,fName)


