# -*- coding: utf-8 -*-
''' Display nice images of the model. '''

from __future__ import print_function
import math


__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import sys
from vtk.vtkRenderingCore import (
    vtkRenderWindow,
    vtkRenderWindowInteractor,
    vtkRenderer,
    vtkWindowToImageFilter
)
from vtk.vtkCommonTransforms import (
    vtkTransform,
)
from vtk.vtkRenderingAnnotation import vtkAxesActor
from vtk.vtkIOImage import vtkJPEGWriter

import geom
import xc
import random as rd 
from postprocess.xcVtk import screen_annotation as sa
from misc_utils import log_messages as lmsg

def setup_render_window(windowName, actors, backgroundColor, xSize= 800, ySize= 600):
    ''' Setup render window, renderer, and interactor.

    :param windowName: window name.
    :param actors: actors that will be added to the renderer.
    :param backgroundColor: color for the window background.
    :param xSize: horizontal size in pixels.
    :param ySize: vertical size in pixels.
    '''
    renderer = vtkRenderer()
    renderWindow = vtkRenderWindow()
    renderWindow.SetWindowName(windowName)
    renderWindow.AddRenderer(renderer)
    renderWindow.SetSize(xSize, ySize)
    # Setup interactor.
    renderWindowInteractor = vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)
    for actor in actors:
        renderer.AddActor(actor)
    renderer.SetBackground(backgroundColor)

    renderWindow.Render()
    renderWindowInteractor.Start()
    


class RecordDefGrid(object):
    '''Provide the variables involved in the VTK grid representation

    :ivar xcSet:     set to be represented
    :ivar entToLabel:  entities to be labeled (defaults to "nodes")
    :ivar cellType:    specifies the type of data cells (defaults to "nil"). 
          Data cells are simple topological elements like points, lines, 
          polygons and tetrahedra of which visualization data sets are composed.
    :ivar uGrid: unstructure grid (defaults to None). An unstructure grid is a 
          concrete implementation of a vtk data set; represents any combination 
          of any cell types. This includes 0D (e.g. points), 1D (e.g., lines, 
          polylines), 2D (e.g., triangles, polygons), and 3D (e.g., hexahedron, 
          tetrahedron, polyhedron, etc.).

    '''
    def __init__(self):
        self.xcSet= None
        self.entToLabel= "nodes"
        self.cellTypes= list() # types of cells to be displayed.
        self.uGrid= None

    def getSetBND(self):
        ''' Returns the set boundary'''
        retval= self.xcSet.getBnd(0.0)
        numPoints= max(len(self.xcSet.getPoints), len(self.xcSet.getNodes))
        if(numPoints==0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= className+'.'+methodName+'; there are no points in the set: '
            warnMsg+= self.xcSet.name
            warnMsg+= '. Maybe you must call fillDownwards on the set to display.'
            lmsg.warning('Warning; '+warnMsg)
        return retval

    def getGridBND(self):
        ''' Returns the grid boundary'''
        retval= geom.BND3d()
        points= self.uGrid.GetPoints()
        if(points.GetNumberOfPoints()>0):
            bounds = [0]*6
            points.GetBounds(bounds)
            retval= geom.BND3d(geom.Pos3d(bounds[0],bounds[2],bounds[4]),geom.Pos3d(bounds[1],bounds[3],bounds[5]))
        else:
            warnMsg= 'there are no points in the grid: '
            warnMsg+= self.uGrid.name
            warnMsg+= '. Maybe you must call fillDownwards on the set to display.'
            lmsg.warning(warnMsg)
        return retval

    def getDiagonalLength(self):
        '''Return the length of the diagonal of the bounding box.'''
        return self.getSetBND().diagonal

    def setupSet(self, setToDisplay):
        ''' Prepares the set to be displayed.

        :param setToDisplay: set to be displayed.
        '''
        if(setToDisplay):
            self.xcSet= setToDisplay
            self.xcSet.fillDownwards()
            if self.xcSet.color.Norm()==0:
                self.xcSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
        else:
            lmsg.error('Set to display not defined; using total set.')
            self.xcSet= None
  
class CameraParameters(object):
    ''' Provides the parameters to define the camera.

    :ivar viewName:    name of the view that contains the renderer 
          (defaults to "XYZPos")
    :ivar viewUpVc: vector defined as [x,y,z] to orient the view. This vector
                    of the model is placed in vertical orientation in the
                    display
    :ivar posCVc: vector defined as [x,y,z] that points to the camera position.
    :ivar zoom: scale factor, a value greater than 1 is a zoom-in, a value 
                less than 1 is a zoom-out (defaults to 1.0).
    :ivar hCamFct: factor that applies to the height of the camera position 
                   in order to change perspective of isometric views 
                   (defaults to 1, usual values 0.1 to 10)
    :ivar focalPoint: focal of the camera in world coordinates.
    '''
    def __init__(self, viewNm= 'XYZPos', cameraPosition= [0,0,100], hCamF= 1.0, zoom= 1.0):
        ''' Constructor.

        :param viewNm: name of the view that contains the renderer 
                       (defaults to "XYZPos")
        :param hCamFct: factor that applies to the height of the camera 
                        position in order to change perspective of isometric 
                        views (defaults to 1, usual values 0.1 to 10)
        :param zoom:  scale factor, a value greater than 1 is a zoom-in, a 
                      value less than 1 is a zoom-out (defaults to 1.0).
        '''
        self.viewName= viewNm # name of the view.
        self.viewUpVc= [0,1,0] # view up direction for the camera
        self.posCVc= cameraPosition # position of the camera.
        self.zoom= zoom # zoom.
        self.hCamFct= hCamF
        self.focalPoint= [0, 0, 0] # focal point of the camera.
        self.defineViewParametersFromViewName()

    def defineViewParametersFromViewName(self):
        '''Sets the values of the view parameters
        from the following predefined viewNames:
        "ZPos","ZNeg","YPos","YNeg","XPos","XNeg","XYZPos"
        Zpos: View from positive Z axis (Z+)
        Zneg: View from negative Z axis (Z-)
        Ypos: View from positive Y axis (Y+)
        Yneg: View from negative Y axis (Y-)
        Xpos: View from positive X axis (X+)
        Xneg: View from negative X axis (X-)
        XYZPos or +X+Y+Z: View from point (1,1,1)
        +X+Y-Z: View from point (1,1,-1)
        +X-Y+Z: View from point (1,-1,1)
        +X-Y-Z: View from point (1,-1,-1)
        -X+Y+Z: View from point (-1,1,1)
        -X+Y-Z: View from point (-1,1,-1)
        -X-Y+Z: View from point (-1,-1,1)
        XYZNeg or -X-Y-Z: View from point (-1,-1,-1)
        '''
        if(self.viewName.lower()=="zpos"):
          self.viewUpVc= [0,1,0]
          self.posCVc= [0,0,100]
        elif(self.viewName.lower()=="zneg"):
          self.viewUpVc= [0,1,0]
          self.posCVc= [0,0,-100]
        elif(self.viewName.lower()=="ypos"):
          self.viewUpVc= [0,0,1]
          self.posCVc= [0,100,0]
        elif(self.viewName.lower()=="yneg"):
          self.viewUpVc= [0,0,1]
          self.posCVc= [0,-100,0]
        elif(self.viewName.lower()=="xpos"):
          self.viewUpVc= [0,0,1]
          self.posCVc= [100,0,0]
        elif(self.viewName.lower()=="xneg"):
          self.viewUpVc= [0,0,1]
          self.posCVc= [-100,0,0]
        elif(self.viewName.lower()=="xyzpos" or self.viewName.lower()=="+x+y+z"):
          self.viewUpVc= [-1,-1,1]
          self.posCVc= [100,100,self.hCamFct*100]
        elif(self.viewName.lower()=="+x+y-z"):
          self.viewUpVc= [1,1,1]
          self.posCVc= [100,100,-1*self.hCamFct*100]
        elif(self.viewName.lower()=="+x-y+z"):
          self.viewUpVc= [-1,1,1]
          self.posCVc= [100,-100,self.hCamFct*100]
        elif(self.viewName.lower()=="+x-y-z"):
          self.viewUpVc= [1,-1,1]
          self.posCVc= [100,-100,-1*self.hCamFct*100]
        elif(self.viewName.lower()=="-x+y+z"):
          self.viewUpVc= [1,-1,1]
          self.posCVc= [-100,100,self.hCamFct*100]
        elif(self.viewName.lower()=="-x+y-z"):
          self.viewUpVc= [-1,+1,1]
          self.posCVc= [-100,100,-1*self.hCamFct*100]
        elif(self.viewName.lower()=="-x-y+z"):
          self.viewUpVc= [1,1,1]
          self.posCVc= [-100,-100,self.hCamFct*100]
        elif(self.viewName.lower()=="xyzneg" or self.viewName.lower()=="-x-y-z"):
          self.viewUpVc= [-1,-1,1]
          self.posCVc= [-100,-100,-1*self.hCamFct*100]
        elif(self.viewName!="Custom"):
          sys.stderr.write("View name: '"+self.viewName+"' unknown.")

    def setFocalPoint(self, focalPoint):
        ''' Set the focal point for the camera.

        :param focalPoint: position of the focal point.
        '''
        self.focalPoint= focalPoint

    def setView(self, vtkCamera):
        '''Sets the camera parameters.
        
        :param vtkCamera: VTK camera object whose parameters will be changed
                          according to the information stored in this
                          object.
        '''
        if(self.viewName!="Custom"):
            self.defineViewParametersFromViewName()
        vtkCamera.SetViewUp(self.viewUpVc[0],self.viewUpVc[1],self.viewUpVc[2])
        cameraPosition= [self.focalPoint[0]+self.posCVc[0],self.focalPoint[1]+self.posCVc[1],self.focalPoint[2]+self.posCVc[2]]
        vtkCamera.SetPosition(cameraPosition)
        vtkCamera.SetFocalPoint(self.focalPoint[0],self.focalPoint[1],self.focalPoint[2])
        vtkCamera.SetParallelProjection(1)
        vtkCamera.Zoom(self.zoom)

defaultBackgroundColorRGB= (0.65, 0.65, 0.65)

class DisplaySettings(object):
    ''' Provides the variables to define the output device.

    :ivar renderer: specification of renderer. A renderer is an object that
          controls the rendering process for objects. Rendering is the 
          process of converting geometry, a specification for lights, and
          a camera view into an image. (defaults to None)
    :ivar renWin:  rendering window (defaults to None). A rendering window is a 
          window in a graphical user interface where renderers draw their images. 
    :ivar windowWidth: resolution expresed in pixels in the width direction of 
          the window (defaults to 800)
    :ivar windowHeight: resolution expresed in pixels in the height direction of 
          the window (defaults to 600)
    :ivar cameraParameters: parameters that define the camera position,
                            zoom, etc.
    :ivar bgRComp: background color red component (defaults to 0.65)
    :ivar bgGComp: background color green component (defaults to 0.65)
    :ivar bgBComp: background color blue component (defaults to 0.65)
    :ivar lineWidth: with of the lines in screen units.
    '''
    def __init__(self):
        '''Constructor.'''
        self.renderer= None
        self.renWin= None
        self.windowWidth= 800
        self.windowHeight= 600
        self.annotation= sa.ScreenAnnotation()
        self.bgRComp= defaultBackgroundColorRGB[0]
        self.bgGComp= defaultBackgroundColorRGB[1]
        self.bgBComp= defaultBackgroundColorRGB[2]
        self.cameraParameters= CameraParameters()
        self.lineWidth= None

    def setBackgroundColor(self, rgbComponents= defaultBackgroundColorRGB):
        ''' Sets the background color for the renderer.

        :param rgbComponents: (red, green, blue) components of the background
                              color.
        '''
        self.bgRComp= rgbComponents[0]
        self.bgGComp= rgbComponents[1]
        self.bgBComp= rgbComponents[2]

    def setLineWidth(self, lineWidth):
        ''' Set the width for the displayed lines.

        :param lineWidth: width of the lines in screen units.
        '''
        self.lineWidth= lineWidth

    def getLineWidth(self):
        ''' Return the value of the width for the displayed lines.'''
        return self.lineWidth
  
    def setView(self):
        '''Sets the view'''
        self.renderer.ResetCamera()
        cam= self.renderer.GetActiveCamera()
        self.cameraParameters.setView(cam)
        self.renderer.ResetCameraClippingRange()

    def setupAxes(self):
        '''Add an vtkAxesActor to the renderer.'''
        bnd= self.gridRecord.getGridBND()
        offsetVector= bnd.diagonal*0.1
        offset= offsetVector.getModulus()
        axesPosition= bnd.pMin-offsetVector
        transform = vtkTransform()
        transform.Translate(axesPosition.x, axesPosition.y, axesPosition.z)
        axes= vtkAxesActor()
        #  The axes are positioned with a user transform
        axes.SetUserTransform(transform)
        if math.isnan(float(offset)):
          offset=25
        length= offset
        axes.SetTotalLength(length,length,length)

        textSize= int(min(3*offset,25)) #Value out of range for int when displacements are extremely HUGE (>1E6)
        axes.GetXAxisCaptionActor2D().GetTextActor().SetTextScaleMode(False)
        axes.GetXAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(textSize)
        axes.GetYAxisCaptionActor2D().GetTextActor().SetTextScaleMode(False)
        axes.GetYAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(textSize)
        axes.GetZAxisCaptionActor2D().GetTextActor().SetTextScaleMode(False)
        axes.GetZAxisCaptionActor2D().GetTextActor().GetTextProperty().SetFontSize(textSize)

        # properties of the axes labels can be set as follows
        # this sets the x axis label to red
        # axes.GetXAxisCaptionActor2D().GetCaptionTextProperty().SetColor(1,0,0)

        # the actual text of the axis label can be changed:
        # axes.SetXAxisLabelText("test")
        self.renderer.AddActor(axes)

    def setupWindow(self, caption= 'XC window',  unitDescription= ''):
        '''sets the rendering window. A rendering window is a window in a
           graphical user interface where renderers draw their images.

        :param caption: caption of the image.
        :param unitDescription: description of the units.
        '''
        self.renWin= vtkRenderWindow()
        self.renWin.SetSize(self.windowWidth,self.windowHeight)
        self.renWin.AddRenderer(self.renderer)
        if(caption is not None):
            self.renWin.SetWindowName(caption)
        #Axes
        self.setupAxes()

        #Time stamp and window decorations.
        if(caption=='' or (caption is None)):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; window caption empty.')
        vtkCornerAnno= self.annotation.getVtkCornerAnnotation(caption= caption, unitDescription= unitDescription)
        self.renderer.AddActor(vtkCornerAnno)
        return self.renWin

    def setupWindowInteractor(self):
        '''sets the window interactor, which provides a platform-independent
        interaction mechanism for mouse/key/time events.
        '''
        iren= vtkRenderWindowInteractor()
        iren.SetRenderWindow(self.renWin)
        iren.SetSize(self.windowWidth,self.windowHeight)
        iren.Initialize()
        return iren

    def closeWindow(self, iren):
        ''' Close the current render window.'''
        iren.SetRenderWindow(None)
        # 20220922 Apparently "ShowWindowOff" method has been deprecated.
        if(hasattr(self.renWin,"ShowWindowOff")):
            self.renWin.ShowWindowOff()
        self.renWin.Finalize()
        iren.TerminateApp()
        
    def displayScene(self, caption= '',  unitDescription= '', fileName= None):
        ''' Displaying scene

        :param caption: caption to display with the scene.
        :param unitDescription: description of the units.
        :param fileName: name of the image file, in none -> screen window.
        '''
        self.setView()
        self.setupWindow(caption= caption, unitDescription= unitDescription)
        if(fileName):
            self.plot(fileName)
        else:
            iren= self.setupWindowInteractor() 
            iren.Start()
            self.closeWindow(iren)

    def setupGrid(self, xcSet):
        ''' Parameters:
           xcSet:     set to be represented
        '''
        self.gridRecord= RecordDefGrid()
        self.gridRecord.setupSet(xcSet)
        # Compute set boundary.
        bnd= self.gridRecord.getSetBND()
        # Compute focal point.
        center= bnd.getCenterOfMass()
        self.cameraParameters.setFocalPoint([center.x, center.y, center.z])
        return self.gridRecord

    def plot(self,fileName):
        '''Plots window contents

        :param fileName: name of the image file, in none -> screen window.
        '''
        self.renWin.SetOffScreenRendering(True) # Don't use screen.
        self.renWin.Render()

        w2if = vtkWindowToImageFilter()
        oldSB = self.renWin.GetSwapBuffers()
        # Tell render window to not swap buffers at end of render.
        self.renWin.SwapBuffersOff()
        # Let's grab from back buffer as that overcomes
        # issues with overlapping windows or other UI
        # components obfuscating the captured image on
        # certain windowing systems/platforms.
        w2if.ReadFrontBufferOff(); #Read from the back buffer.
        w2if.SetInput(self.renWin)
        if(hasattr(w2if,'SetScale')):
            w2if.SetScale(1) # image quality
        else:
            w2if.SetMagnification(1) # image quality (apparently deprecated in recent versions of VTL -LCPT 20/05/2020-)
        w2if.Update()
        # restore swapping state
        self.renWin.SetSwapBuffers(oldSB)
        writer= vtkJPEGWriter()
        writer.SetInputConnection(w2if.GetOutputPort())
        writer.SetFileName(fileName)
        writer.Write()
 
 
