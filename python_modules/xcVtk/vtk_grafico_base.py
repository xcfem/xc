# -*- coding: utf-8 -*-
# Representación de un modelo de elementos finitos.

import sys
import vtk

class RecordDefGrid(object):
  ''' Define las variables que se emplean para mostrar la malla. '''
  setName= "nil"
  entToLabel= "nodos"
  cellType= "nil"
  uGrid= None
  dispScale= 0.0

class RecordDefDisplay(object):
  ''' Define las variables que se emplean para definir
     el dispositivo de salida. '''
  renderer= None
  renWin= None
  windowWidth= 800
  windowHeight= 600
  nmbVista= "XYZPos"
  zoom= 1.0
  bgRComp= 0.65
  bgGComp= 0.65
  bgBComp= 0.65

  #Define una vista desde el eje Y-
  def VistaYNeg(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(0,-100,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Y+
  def VistaYPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(0,100,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Z+
  def VistaZPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(1,0,0)
    cam.SetPosition(0,0,100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde XYZ+
  def VistaXYZPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(-1,-1,1)
    cam.SetPosition(100,100,100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  def defineView(self):
    if(self.nmbVista=="ZPos"):
      self.VistaZPos()
    elif(self.nmbVista=="YPos"):
      self.VistaYPos()
    elif(self.nmbVista=="YNeg"):
      self.VistaYNeg()
    elif(self.nmbVista=="XYZPos"):
      self.VistaXYZPos()
    else:
      sys.stderr.write("View name: '"+self.nmbVista+"' unknown.")

  def setupWindow(self):
    self.renWin= vtk.vtkRenderWindow()
    self.renWin.AddRenderer(self.renderer)
    return self.renWin

  def MuestraVentana(self):
    self.setupWindow()
    iren= vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(self.renWin)
    iren.SetSize(self.windowWidth,self.windowHeight)
    iren.Initialize()
    iren.Start()
    return self.renWin


  def muestraEscena(self):
    # Muestra la escena en el dispositivo de salida.
    self.defineView()
    self.MuestraVentana()

  def plotScene(self,fName):
    self.defineView()
    self.setupWindow()
    self.renWin.SetSize(self.windowWidth,self.windowHeight)
    self.plot(fName)

  # Muestra la malla en el dispositivo de salida.
  def muestraMalla(self, preprocessor,recordGrid):
    self.defineEscenaMalla(preprocessor,recordGrid,None)
    self.muestraEscena()

  # Plot window contents
  def plot(self,fName):
    w2i = vtk.vtkWindowToImageFilter()
    writer = vtk.vtkJPEGWriter()
    w2i.SetInput(self.renWin)
    w2i.Update()
    writer.SetInputConnection(w2i.GetOutputPort())
    writer.SetFileName(fName)
    self.renWin.Render()
    w2i.Update()
    writer.Write()
    

