# -*- coding: utf-8 -*-
''' Display block topology entities (k-points,lines,surfaces,bodies,...)
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import vtk
from postprocess.xcVtk.CAD_model import cad_mesh
from postprocess.xcVtk import vtk_graphic_base
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.fields import local_axes_vector_field as lavf

class DisplaySettingsBlockTopo(vtk_graphic_base.DisplaySettings):
    ''' Define graphic output.'''

    def defineLocalAxes(self, setToDisplay):
        ''' Define the local axes of the blocks.

        :param setToDisplay: set of elements to be displayed.
        '''
        lineAxesField= None
        surfAxesField= None
        numKPts= setToDisplay.getPoints.size
        if(numKPts>0):
            avgSize= setToDisplay.getEntities.getAverageSize()
            vectorScale= avgSize/4.0
            ## Draw lines local axes.
            lineAxesField= lavf.LinesLocalAxesVectorField(setToDisplay.name+'_lineLocalAxes', scaleFactor= vectorScale)
            lineAxesField.dumpVectors(setToDisplay)
            ## Draw quad surfaces local axes.
            surfAxesField= lavf.SurfacesLocalAxesVectorField(setToDisplay.name+'_surfLocalAxes', scaleFactor= vectorScale)
            surfAxesField.dumpVectors(setToDisplay)
        else:
            lmsg.warning("Error when drawing set: '"+setToDisplay.name+"' it has not key points so I can't get set geometry (use fillDownwards?)")
        return lineAxesField, surfAxesField
        
    def defineMeshScene(self):
        ''' Define mesh scene on ouput display.'''
        self.gridRecord.uGrid= vtk.vtkUnstructuredGrid()
        self.gridRecord.cellType= "lines"
        setToDraw= self.gridRecord.xcSet
        self.gridRecord.uGrid.name= setToDraw.name+'_grid'
        numKPts= setToDraw.getPoints.size
        if(numKPts>0):
            cad_mesh.vtk_load_mesh_data(self.gridRecord)
            self.renderer= vtk.vtkRenderer()
            self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
            cad_mesh.VtkDefineActorKPoint(self.gridRecord,self.renderer,0.02)
            cad_mesh.VtkDefineActorCells(self.gridRecord,self.renderer,"wireframe")
            self.renderer.ResetCamera()
        else:
            lmsg.warning("Error when drawing set: '"+setToDraw.name+"' it has not key points so I can't get set geometry (use fillDownwards?)")

        #Implement drawing of labels.
        # if(entToLabel=="cells"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsCells(self.gridRecord,setToDraw,cellTypeName,renderer)
        # elif(entToLabel=="points"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsKPts(self.gridRecord,setToDraw,renderer)


    def grafico_cad(self,setToDisplay,caption= ''):
        ''' Establish the set of entities to be displayed and add a caption 

        :param setToDisplay:   set to be represented
        :param caption: text to display in the graphic.
        '''
        self.setupGrid(setToDisplay)
        self.displayGrid(caption)

    def displayBlockEntities(self, setToDisplay, caption, fileName= None):
        ''' Display geometric entities (points, lines, surfaces and volumes)

        :param setToDisplay: set to be represented
        :param caption: text to display in the graphic.
        :param fileName: name of the graphic file to create (if None then -> screen window).
        '''
        self.setupGrid(setToDisplay)
        self.defineMeshScene()
        self.displayScene(caption,fileName)

    def displayLocalAxes(self, setToDisplay, caption, fileName= None):
        '''Display the element local axes.

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param caption:        text to display in the graphic 
        :param fileName: file name to store the image. If none -> window on screen.
        '''
        self.setupGrid(setToDisplay)
        # Draw local axes.
        lineAxesField, surfAxesField= self.defineLocalAxes(setToDisplay)
        self.defineMeshScene()
        if(lineAxesField):
            lineAxesField.addToDisplay(self) # Draw lines local axes.
        if(surfAxesField):
            surfAxesField.addToDisplay(self) # Draw quad surfaces local axes.
        self.displayScene(caption, fileName)

    def displayBlocks(self, setToDisplay, displayLocalAxes= True, caption= '', fileName= None):
        '''Display the element local axes.

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param displayLocalAxes: if true, show also local axes of entities.
        :param caption:        text to display in the graphic 
        :param fileName: file name to store the image. If none -> window on screen.
        '''
        if(displayLocalAxes):
            self.displayLocalAxes(setToDisplay, caption, fileName)
        else:
            self.displayBlockEntities(setToDisplay, caption, fileName)

