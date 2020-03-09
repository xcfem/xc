# -*- coding: utf-8 -*-
from __future__ import print_function

''' Geometric entities representation (k-points,lines,surfaces,bodies,...)
'''
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import vtk
from postprocess.xcVtk.CAD_model import cad_mesh
from postprocess.xcVtk import vtk_graphic_base
from miscUtils import log_messages as lmsg
from postprocess.xcVtk.fields import local_axes_vector_field as lavf

class RecordDefDisplayCAD(vtk_graphic_base.RecordDefDisplay):
    ''' Define graphic output.'''
    def defineMeshScene(self):
        ''' Define mesh scene on ouput display.'''
        self.gridRecord.uGrid= vtk.vtkUnstructuredGrid()
        self.gridRecord.cellType= "lines"
        setToDraw= self.gridRecord.xcSet
        self.gridRecord.uGrid.name= setToDraw.name+'_grid'
        numKPts= setToDraw.getPoints.size
        if(numKPts>0):
          cad_mesh.VtkCargaMalla(self.gridRecord)
          self.renderer= vtk.vtkRenderer()
          self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
          cad_mesh.VtkDefineActorKPoint(self.gridRecord,self.renderer,0.02)
          cad_mesh.VtkDefineActorCells(self.gridRecord,self.renderer,"wireframe")
          #Experimental yet (31/07/2018) LCPT
          vField=lavf.QuadSurfacesLocalAxesVectorField(setToDraw.name+'_localAxes',1)#vectorScale)
          vField.dumpVectors(setToDraw)
          vField.addToDisplay(self)
          #End of the experiment
          self.renderer.ResetCamera()
        else:
          lmsg.warning("Error when drawing set: '"+setToDraw.name+"' it has not key points so I can't get set geometry (use fillDownwards?)")

        #Implementar dibujo de etiquetas.
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

    def displayBlocks(self, setToDisplay, caption= '',fileName= None):
        ''' Display geometric entities (points, lines, surfaces and volumes)

        :param setToDisplay: set to be represented
        :param caption: text to display in the graphic.
        :param fileName: name of the graphic file to create (if None then -> screen window).
        '''
        self.setupGrid(setToDisplay)
        self.defineMeshScene()
        self.displayScene(caption,fileName)



