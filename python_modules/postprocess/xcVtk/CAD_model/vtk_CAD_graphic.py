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

import sys
from vtk.vtkCommonDataModel import vtkUnstructuredGrid
from vtk.vtkRenderingCore import vtkRenderer
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
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; error when drawing set: '"+setToDisplay.name+"', it has not key points so I can't get set geometry (use fillDownwards?)")
        return lineAxesField, surfAxesField
        
    def defineMeshScene(self, displayCellTypes):
        ''' Define mesh scene on ouput display.

        :param displayCellTypes: types of cell to be displayed.
        '''
        self.gridRecord.uGrid= vtkUnstructuredGrid()
        self.gridRecord.cellTypes= displayCellTypes
        setToDraw= self.gridRecord.xcSet
        self.gridRecord.uGrid.name= setToDraw.name+'_grid'
        numKPts= setToDraw.getPoints.size
        if(numKPts>0):
            cad_mesh.vtk_load_mesh_data(self.gridRecord)
            self.renderer= vtkRenderer()
            self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
            cad_mesh.vtk_define_kpoint_actor(self.gridRecord,self.renderer,0.02)
            cad_mesh.vtk_define_cells_actor(gridRecord= self.gridRecord, renderer= self.renderer, reprType= "surface")
            self.renderer.ResetCamera()
            return True
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; error when drawing set: '"+setToDraw.name+"', it has no key points so I can't get set geometry (use fillDownwards?)")
            return False

        #Implement drawing of labels.
        # if(entToLabel=="cells"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsCells(self.gridRecord,setToDraw,cellTypeName,renderer)
        # elif(entToLabel=="points"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsKPts(self.gridRecord,setToDraw,renderer)

    def displayBlockEntities(self, setToDisplay, caption, displayCellTypes, fileName= None):
        ''' Display geometric entities (points, lines, surfaces and volumes)

        :param setToDisplay: set to be represented
        :param caption: text to display in the graphic.
        :param displayCellTypes: types of cell to be displayed.
        :param fileName: name of the graphic file to create (if None then -> screen window).
        '''
        self.setupGrid(setToDisplay)
        meshSceneOk= self.defineMeshScene(displayCellTypes= displayCellTypes)
        if(meshSceneOk):
            self.displayScene(caption= caption, unitDescription= '', fileName= fileName)

    def displayLocalAxes(self, setToDisplay, caption, displayCellTypes, fileName= None):
        '''Display the element local axes.

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param caption:        text to display in the graphic 
        :param displayCellTypes: types of cell to be displayed.
        :param fileName: file name to store the image. If none -> window on screen.
        '''
        self.setupGrid(setToDisplay)
        # Draw local axes.
        lineAxesField, surfAxesField= self.defineLocalAxes(setToDisplay)
        meshSceneOk= self.defineMeshScene(displayCellTypes= displayCellTypes)
        if(meshSceneOk):
            if(lineAxesField):
                lineAxesField.addToDisplay(self) # Draw lines local axes.
            if(surfAxesField):
                surfAxesField.addToDisplay(self) # Draw quad surfaces local axes.
            self.displayScene(caption= caption, unitDescription= '', fileName= fileName)

    def displayBlocks(self, setToDisplay, displayLocalAxes= True, caption= '', fileName= None, displayCellTypes= ['lines', 'faces', 'bodies']):
        '''Display the element local axes.

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param displayLocalAxes: if true, show also local axes of entities.
        :param caption:        text to display in the graphic 
        :param fileName: file name to store the image. If none -> window on screen.
        :param displayCellTypes: types of cell to be displayed.
        '''
        if(displayLocalAxes):
            self.displayLocalAxes(setToDisplay= setToDisplay, caption= caption, displayCellTypes= displayCellTypes, fileName= fileName)
        else:
            self.displayBlockEntities(setToDisplay= setToDisplay, caption= caption, displayCellTypes= displayCellTypes, fileName= fileName)

