# -*- coding: utf-8 -*-
''' Graphics of the finite element model.
'''
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
import vtk
from miscUtils import LogMessages as lmsg
import xc_base
from vtkUtils import utilsVtk
from postprocess.xcVtk import vtk_grafico_base

class RecordDefDisplayEF(vtk_grafico_base.RecordDefDisplay):
    ''' Define the parameters to configure the output device.
    '''
    def __init__(self):
        super(RecordDefDisplayEF,self).__init__()
        self.nodos= None
        self.gridMapper= None
    def VtkDefineElementsActor(self, reprType,field):
        ''' Define the actor to display elements

        :param reprType: type of representation ("points", "wireframe" or
               "surface")
        :param field: field to be repreresented
        '''
        if(field):
          field.setupOnGrid(self.gridRecord.uGrid)
        self.gridMapper= vtk.vtkDataSetMapper()
        self.gridMapper.SetInput(self.gridRecord.uGrid)
        if(field):
          field.setupOnMapper(self.gridMapper)
        elemActor= vtk.vtkActor()
        elemActor.SetMapper(self.gridMapper)
        elemActor.GetProperty().SetColor(1,1,0)

        if(reprType=="points"):
          elemActor.GetProperty().SetRepresentationToPoints()
        elif(reprType=="wireframe"):
          elemActor.GetProperty().SetRepresentationToWireFrame()
        elif(reprType=="surface"):
          elemActor.GetProperty().SetRepresentationToSurface()
        else:
          lmsg.error("Representation type: '"+ reprType+ "' unknown.")
        self.renderer.AddActor(elemActor)
        if(field):
          field.creaColorScaleBar()
          self.renderer.AddActor2D(field.scalarBar)

    def VtkDefineNodesActor(self, radius):
        '''Define the actor to display nodes.

        :param radius: radius of the sphere used as symbol to represent nodes.
        '''
        sphereSource= vtk.vtkSphereSource()
        sphereSource.SetRadius(radius)
        sphereSource.SetThetaResolution(5)
        sphereSource.SetPhiResolution(5)

        markNodos= vtk.vtkGlyph3D()
        markNodos.SetInput(self.gridRecord.uGrid)
        markNodos.SetSource(sphereSource.GetOutput())
        markNodos.ScalingOff()
        markNodos.OrientOff()

        mappNodos= vtk.vtkPolyDataMapper()
        mappNodos.SetInput(markNodos.GetOutput())
        visNodos= vtk.vtkActor()
        visNodos.SetMapper(mappNodos)
        visNodos.GetProperty().SetColor(.7, .5, .5)
        self.renderer.AddActor(visNodos)

    def VtkLoadElemMesh(self,field,defFScale=0.0):
        '''Load the element mesh

        :param field: scalar field to be represented
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
        '''
        # Definie grid
        self.nodos= vtk.vtkPoints()
        self.gridRecord.uGrid= vtk.vtkUnstructuredGrid()
        self.gridRecord.uGrid.SetPoints(self.nodos)
        eSet= self.gridRecord.xcSet
        eSet.numerate()
        self.gridRecord.uGrid.name= eSet.name+'_grid'
        # Scalar values.
        nodeSet= eSet.getNodes
        if(field):
          arr= field.fillArray(nodeSet)
          field.creaLookUpTable()      
        # Load nodes in vtk
        setNodos= eSet.getNodes
        for n in setNodos:
          pos= n.getCurrentPos3d(defFScale)
          self.nodos.InsertPoint(n.getIdx,pos.x,pos.y,pos.z)
        # Load elements in vtk
        setElems= eSet.getElements
        for e in setElems:
          vertices= xc_base.vector_int_to_py_list(e.getIdxNodes)
          vtx= vtk.vtkIdList()
          for vIndex in vertices:
            vtx.InsertNextId(vIndex)
          if(e.getVtkCellType!= vtk.VTK_VERTEX):
            self.gridRecord.uGrid.InsertNextCell(e.getVtkCellType,vtx)
        #Load constraints (FAILS)
        setConstraints= eSet.getConstraints
        for c in setConstraints:
          vtx= vtk.vtkIdList()
          vtx.InsertNextId(c.getNodeIdx)
          if(c.getVtkCellType!= vtk.VTK_LINE):
            self.gridRecord.uGrid.InsertNextCell(c.getVtkCellType,vtx)

    def defineMeshScene(self, field,defFScale=0.0):
          '''Define the scene for the mesh

          :param field: scalar field to be represented
          :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
          '''
          self.VtkLoadElemMesh(field,defFScale)
          self.renderer= vtk.vtkRenderer()
          self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
          self.VtkDefineNodesActor(0.002)
          self.VtkDefineElementsActor("surface",field)
          self.renderer.ResetCamera()

          #Implement labels.
          # if(self.gridRecord.entToLabel=="elementos"):
          #   VtkDibujaIdsElementos(self.renderer)
          # elif(self.gridRecord.entToLabel=="nodos"):
          #   vtk_define_mesh_nodes.VtkDibujaIdsNodos(self.renderer)
          # else:
          #   print "Entity: ", self.gridRecord.entToLabel, " unknown."

    def FEmeshGraphic(self,xcSet,caption= '',viewNm='XYZPos',defFScale=0.0):
          ''' Graphic of the FE mesh

          :param xcSet:   XC set of elements to be displayed
          :param caption: text to write in the graphic
          :param viewNm:  name of the view to use for the representation
                 predefined view names: 'XYZPos','XNeg','XPos','YNeg','YPos',
                 'ZNeg','ZPos'  (defaults to 'XYZPos')
          :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
          '''
          self.viewName=viewNm
          self.setupGrid(xcSet)
          self.displayGrid(caption)

    def displayMesh(self, xcSet, field= None, diagrams= None, fName= None, caption= '',defFScale=0.0):
          '''Display the finite element mesh 

          :param xcSet: set to be displayed
          :param field: scalar field to show (optional)
          :param diagrams: diagrams to show (optional)
          :param fName: name of the graphic file to create (if None -> screen window).
          :param caption: text to display in the graphic.
          :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
          '''
          self.setupGrid(xcSet)
          self.defineMeshScene(field,defFScale)
          if(diagrams):
            for d in diagrams:
              self.appendDiagram(d)
          self.displayScene(caption,fName)

    def displayScalarField(self, preprocessor, xcSet, field, fName= None):
          lmsg.warning('displayScalarField DEPRECATED; use displayMesh.')
          self.displayMesh(xcSet, field, None, fName)

    def displayLoadOnNode(self, nod, color, force, moment, fScale,defFScale=0.0):
          '''Display loads on one node

          :param nod: node instance
          :param color: color
          :param force: force (displayed as a single arrow)
          :param moment: moment (displayed as a double arrow)
          :param fScale: scaling factor (forces and moments)
          :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
          '''
          #actorName= baseName+"%04d".format(nod.tag) # Tag nodo.

          pos= nod.getCurrentPos3d(defFScale)
          absForce= force.Norm()
          if(absForce>1e-6):
            utilsVtk.dibujaFlecha(self.renderer,color,pos,force,fScale*absForce)
          absMoment= moment.Norm()
          if(absMoment>1e-6):
            utilsVtk.dibujaFlechaDoble(self.renderer,color,pos,moment,fScale*absMoment)

    def displayNodalLoads(self, preprocessor, loadPattern, color, fScale):
        '''Display the all nodal loads defined in a load pattern

        :param preprocessor: preprocessor
        :param loadPattern: load pattern
        :param color: color of the symbols (arrows)
        :param fScale: factor to apply to current displacement of nodes 
               so that the display position of each node equals to
               the initial position plus its displacement multiplied
               by this factor. (Defaults to 0.0, i.e. display of 
               initial/undeformed shape)
        '''
        loadPattern.addToDomain()
        loadPatternName= loadPattern.getProp("dispName")
        lIter= loadPattern.loads.getNodalLoadIter
        load= lIter.next()
        while not(load is None):
            actorName= "flecha"+loadPatternName+"%04d".format(load.tag) # Tag force.
            nodeTag= load.getNodeTag
            node= preprocessor.getNodeLoader.getNode(nodeTag)
            force= load.getForce
            moment= load.getMoment
            self.displayLoadOnNode(node, color,force,moment,fScale)    
            load= lIter.next()
        loadPattern.removeFromDomain()


    def displayElementPunctualLoad(self, preprocessor, pLoad,loadPattern, renderer, color, force, fScale):
        '''Display punctual loads on elements
        '''
        xForce= pLoad.getElems()
        eleTags= pLoad.elementTags
        loadPatternName= loadPattern.getProp("dispName")
        actorName= "flechaP"+loadPatternName+"%04d".format(tag) # Tag force.
        for tag in eleTags:
          ele= preprocessor.getElementLoader.getElement(tag)
          actorName+= "%04d".format(tag) # Tag elemento.
          pos= ele.punto(xForce)
          utilsVtk.dibujaFlecha(self.renderer,color,pos,force,fScale)()

    def displayElementUniformLoad(self, preprocessor, unifLoad,loadPattern, color, force, fScale):
        loadPatternName= loadPattern.getProp("dispName")
        actorName= "flechaU"+loadPatternName+"%04d".format(unifLoad.tag)
        eleTags= unifLoad.elementTags
        for tag in eleTags:
          ele= preprocessor.getElementLoader.getElement(tag)
          actorName+= "%04d".format(tag) # Tag elemento.
          lmsg.error('displayElementUniformLoad not implemented.')
          # puntos= ele.getPoints(3,1,1,True)
          # i= 0
          # for capa in puntos:
          #   for pos in capa: 
          #     print pos
          #     utilsVtk.dibujaFlecha(self.renderer,color,pos,force,fScale*force.Norm())
          #     i+= 1

    def displayElementalLoads(self, preprocessor,loadPattern, color, fScale):
        loadPattern.addToDomain()
        eleLoadIter= loadPattern.loads.getElementalLoadIter
        eleLoad= eleLoadIter.next()
        lmsg.error('displayElementalLoads not implemented.')
        # while not(eleLoad is None):
        #   force= eleLoad.getGlobalForces()
        #   categoria= eleLoad.category
        #   if(categoria=="uniforme"):
        #     self.displayElementUniformLoad(preprocessor, eleLoad,loadPattern,color,force,fScale)
        #   else:
        #     self.displayElementPunctualLoad(preprocessor, eleLoad,loadPattern,color,force,fScale)
        # loadPattern.removeFromDomain()

    def displayLoads(self, preprocessor, loadPattern):
        clrVectores= loadPattern.getProp("color")
        fScaleVectores= loadPattern.getProp("scale")
        self.displayElementalLoads(preprocessor, loadPattern,clrVectores,fScaleVectores)
        self.displayNodalLoads(preprocessor, loadPattern,clrVectores,fScaleVectores)

    def appendDiagram(self,diagram):
        diagram.addDiagramToScene(self)
