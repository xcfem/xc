# -*- coding: utf-8 -*-
''' Geometric entities representation (k-points,lines,surfaces,bodies,...)
'''
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import vtk
from postprocess.xcVtk import cad_mesh
from postprocess.xcVtk import vtk_grafico_base
from miscUtils import LogMessages as lmsg

class RecordDefDisplayCAD(vtk_grafico_base.RecordDefDisplay):
    ''' Define graphic output.'''
    def defineMeshScene(self, field):
        ''' Define mesh scene on ouput display.'''
        self.gridRecord.uGrid= vtk.vtkUnstructuredGrid()
        self.gridRecord.cellType= "lines"
        setToDraw= self.gridRecord.xcSet
        numKPts= setToDraw.getPoints.size
        if(numKPts>0):
          cad_mesh.VtkCargaMalla(self.gridRecord)
          self.renderer= vtk.vtkRenderer()
          self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
          cad_mesh.VtkDefineActorKPoint(self.gridRecord,self.renderer,0.02)
          cad_mesh.VtkDefineActorCells(self.gridRecord,self.renderer,"wireframe")
          self.renderer.ResetCamera()
        else:
          lmsg.warning("Error when drawing set: '"+setToDraw.name+"' it has not key points so I can't get set geometry (use fillDownwards?)")

        #Implementar dibujo de etiquetas.
        # if(entToLabel=="cells"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsCells(self.gridRecord,setToDraw,nmbTipoCeldas,renderer)
        # elif(entToLabel=="points"):
        #   postprocess.xcVtk.cad_mesh.VtkDibujaIdsKPts(self.gridRecord,setToDraw,renderer)


    def grafico_cad(self,xcSet,caption= ''):
        ''' Establish the set of entities to be displayed and add a caption 

        :param xcSet:   set to be represented
        :param caption: text to display in the graphic.
        '''
        self.setupGrid(xcSet)
        self.displayGrid(caption)

    def displayBlocks(self, xcSet, fName= None, caption= ''):
        ''' Display geometric entities (points, lines, surfaces and volumes)

        :param xcSet: set to be represented
        :param field: field to show (optional)
        :param diagrams: diagrams to show (optional)
        :param fName: name of the graphic file to create (if None then -> screen window).
        :param caption: text to display in the graphic.
        '''
        self.setupGrid(xcSet)
        self.defineMeshScene(None)
        self.displayScene(caption,fName)

    def plotCadModel(self, xcSet, field, fName, caption= ''):
        lmsg.warning('plotCadMOdel DEPRECATED; use displayBlocks.')
        self.displayBlocks(xcSet,fName,caption)


