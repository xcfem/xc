# -*- coding: utf-8 -*-

''' Display nice images of the model. '''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"


import sys
import vtk
import xc_base
from miscUtils import LogMessages as lmsg


class RecordDefGrid(object):
  '''Provides the variables involved in the grid representation
  Attributes:
    setName:     name of the set to be represented
    entToLabel:  entities to be labeled (defaults to "nodos")
    cellType:    specifies the type of data cells (defaults to "nil"). Data cells are simple 
                 topological elements like points, lines, polygons and tetrahedra of which 
                 visualization data sets are composed.
    uGrid:       unstructure grid (defaults to None). An unstructure grid is a concrete 
                 implementation of a vtk data set; represents any combination of any cell
                 types. This includes 0D (e.g. points), 1D (e.g., lines, polylines), 
                 2D (e.g., triangles, polygons), and 3D (e.g., hexahedron, tetrahedron, polyhedron, etc.).
    dispScale:   (defaults to 0.0)

  '''
  def __init__(self):
    self.setName= "nil"
    self.entToLabel= "nodos"
    self.cellType= "nil"
    self.uGrid= None
    self.dispScale= 0.0

class RecordDefDisplay(object):
  ''' Provides de variables to define the output device.
  Attributes:
    renderer:    specification of renderer. A renderer is an object that
                 controls the rendering process for objects. Rendering is the 
                 process of converting geometry, a specification for lights, and
                 a camera view into an image. (defaults to None)
    renWin:      rendering window (defaults to None). A rendering window is a window in a
                 graphical user interface where renderers draw their images. 
    windowWidth: resolution expresed in pixels in the width direction of the window 
                 (defaults to 800)
    windowHeight: resolution expresed in pixels in the height direction of the window 
                 (defaults to 600)
    viewName:    name of the view that contains the renderer (defaults to "XYZPos")
    zoom:        (defaults to 1.0)
    bgRComp:     red component (defaults to 0.65)
    bgGComp:     green component (defaults to 0.65)
    bgBComp:     blue component (defaults to 0.65)
  '''
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

  def ViewYNeg(self):
    '''View from negative Y axis (Y-)'''
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(0,-100,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  def ViewYPos(self):
    '''View from positive Y axis (Y+)'''
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(0,100,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje X-
  def ViewXNeg(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(-100,0,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje X+
  def ViewXPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,0,1)
    cam.SetPosition(100,0,0)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Z+
  def ViewZPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,1,0)
    cam.SetPosition(0,0,100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde el eje Z-
  def ViewZNeg(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(0,1,0)
    cam.SetPosition(0,0,-100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  #Define una vista desde XYZ+
  def ViewXYZPos(self):
    self.renderer.ResetCamera()
    cam= self.renderer.GetActiveCamera()
    cam.SetViewUp(-1,-1,1)
    cam.SetPosition(100,100,100)
    cam.SetParallelProjection(1)
    cam.Zoom(self.zoom)
    self.renderer.ResetCameraClippingRange()

  def defineView(self):
    if(self.viewName=="ZPos"):
      self.ViewZPos()
    elif(self.viewName=="ZNeg"):
      self.ViewZNeg()
    elif(self.viewName=="YPos"):
      self.ViewYPos()
    elif(self.viewName=="YNeg"):
      self.ViewYNeg()
    elif(self.viewName=="XPos"):
      self.ViewXPos()
    elif(self.viewName=="XNeg"):
      self.ViewXNeg()
    elif(self.viewName=="XYZPos"):
      self.ViewXYZPos()
    else:
      sys.stderr.write("View name: '"+self.viewName+"' unknown.")

  def setupWindow(self):
    self.renWin= vtk.vtkRenderWindow()
    self.renWin.SetSize(self.windowWidth,self.windowHeight)
    self.renWin.AddRenderer(self.renderer)
    return self.renWin

  def setupWindowInteractor(self):
    iren= vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(self.renWin)
    iren.SetSize(self.windowWidth,self.windowHeight)
    iren.Initialize()
    return iren

  def displayScene(self,fName= None):
    self.defineView()
    self.setupWindow()
    if(fName):
      self.plot(fName)
    else:
      iren= self.setupWindowInteractor()
      iren.Start()

  def muestraEscena(self):
    lmsg.warning('muestraEscena is deprecated. Use displayScene')
    self.displayScene(None)
    

  # Muestra la malla en el dispositivo de salida.
  def muestraMalla(self, preprocessor,recordGrid):
    self.defineEscenaMalla(preprocessor,recordGrid,None)
    self.displayScene()

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
 
 
