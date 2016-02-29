# -*- coding: utf-8 -*-
# Representación de un modelo de elementos finitos.

import sys
import vtk

class RecordDefGrid(object):
  ''' Define las variables que se emplean para mostrar la malla. '''

  def __init__(self):
    self.setName= "nil"
    self.entToLabel= "nodos"
    self.cellType= "nil"
    self.uGrid= None
    self.dispScale= 0.0

class RecordDefDisplay(object):
  ''' Define las variables que se emplean para definir
     el dispositivo de salida. '''
  def __init__(self):
    self.renderer= None
    self.renWin= None
    self.windowWidth= 800
    self.windowHeight= 600
    self.viewName= "XYZPos"
    self.zoom= 1.0
    self.bgRComp= 0.65
    self.bgGComp= 0.65
    self.bgBComp= 0.65

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

  #Define una vista desde el eje X-
  def VistaXNeg(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(-100,0,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje X+
  def VistaXPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(100,0,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Z+
  def VistaZPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,1,0)
    cam.SetPosition(0,0,100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Z-
  def VistaZNeg(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,1,0)
    cam.SetPosition(0,0,-100)
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
    if(self.viewName=="ZPos"):
      self.VistaZPos()
    elif(self.viewName=="ZNeg"):
      self.VistaZNeg()
    elif(self.viewName=="YPos"):
      self.VistaYPos()
    elif(self.viewName=="YNeg"):
      self.VistaYNeg()
    elif(self.viewName=="XPos"):
      self.VistaXPos()
    elif(self.viewName=="XNeg"):
      self.VistaXNeg()
    elif(self.viewName=="XYZPos"):
      self.VistaXYZPos()
    else:
      sys.stderr.write("View name: '"+self.viewName+"' unknown.")

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
 
 
