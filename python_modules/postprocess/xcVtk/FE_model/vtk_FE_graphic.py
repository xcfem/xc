# -*- coding: utf-8 -*-
''' Graphics of the finite element model.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
from misc_utils import log_messages as lmsg
import xc_base
from misc.vtk_utils import utils_vtk
from postprocess.xcVtk import vtk_graphic_base
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.fields import local_axes_vector_field as lavf
from postprocess.xcVtk.CAD_model import create_array_set_data
import random as rd 
import xc
from vtk.vtkCommonCore import (
    vtkIdList,
    vtkPoints
    )
from vtk.vtkCommonDataModel import (
    vtkUnstructuredGrid
  )
from vtk.vtkFiltersCore import (
    vtkGlyph3D,
    vtkIdFilter
)
from postprocess.xcVtk import vtk_version
if(vtk_version.vtk_major>=9):
    from vtk.vtkFiltersCore import vtkCellCenters
else:
    from vtk.vtkFiltersGeneral import vtkCellCenters
from vtk.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkPolyDataMapper,
    vtkDataSetMapper,
    vtkRenderer,
    vtkSelectVisiblePoints
)
from vtk.vtkCommonDataModel import (
    VTK_VERTEX,
    vtkCellTypes
)
from vtk.vtkFiltersSources import vtkSphereSource
from vtk.vtkRenderingLabel import vtkLabeledDataMapper

class DisplaySettingsFE(vtk_graphic_base.DisplaySettings):
    ''' Define the parameters to configure the output for
        displaying the finite element mesh.

    :ivar nodes: 3D representation of points in VTK.
    :ivar gridMapper: VTK unstructured grid used to represent the FE mesh.
    :ivar field: scalar field to be represented.
    '''
    def __init__(self):
        super(DisplaySettingsFE,self).__init__()
        self.nodes= None
        self.gridMapper= None
        self.field= None
        
    def _vtk_define_elements_actor(self, reprType, color=xc.Vector([rd.random(),rd.random(),rd.random()])):
        ''' Define the actor to display elements

        :param reprType: type of representation ("points", "wireframe" or
               "surface")
        :param color: RGB color to represent the elements (defaults to random
                      color)
        '''
        if(self.field):
            self.field.setupOnGrid(self.gridRecord.uGrid)
        self.gridMapper= vtkDataSetMapper()
        self.gridMapper.SetInputData(self.gridRecord.uGrid)
        if(self.field):
            self.field.setupOnMapper(self.gridMapper)
        elemActor= vtkActor()
        elemActor.SetMapper(self.gridMapper)
        elemActor.GetProperty().SetColor(color[0],color[1],color[2])

        if(reprType=="points"):
            elemActor.GetProperty().SetRepresentationToPoints()
        elif(reprType=="wireframe"):
            elemActor.GetProperty().SetRepresentationToWireFrame()
            if(self.lineWidth is not None):
                elemActor.GetProperty().SetLineWidth(self.lineWidth)
        elif(reprType=="surface"):
            elemActor.GetProperty().SetRepresentationToSurface()
            if(self.lineWidth is not None):
                elemActor.GetProperty().SetLineWidth(self.lineWidth)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+"; Representation type: '"+ reprType+ "' unknown.")
        self.renderer.AddActor(elemActor)
        if(self.field):
            self.field.creaColorScaleBar()
            self.renderer.AddActor2D(self.field.scalarBar)

    def _vtk_define_nodes_actor(self, radius):
        '''Define the actor to display nodes.

        :param radius: radius of the sphere used as symbol to represent nodes.
        '''
        sphereSource= vtkSphereSource()
        sphereSource.SetRadius(radius)
        sphereSource.SetThetaResolution(5)
        sphereSource.SetPhiResolution(5)

        markNodes= vtkGlyph3D()
        markNodes.SetInputData(self.gridRecord.uGrid)
        markNodes.SetSourceData(sphereSource.GetOutput())
        markNodes.ScalingOff()
        markNodes.OrientOff()

        mappNodes= vtkPolyDataMapper()
        mappNodes.SetInputData(markNodes.GetOutput())
        visNodes= vtkActor()
        visNodes.SetMapper(mappNodes)
        visNodes.GetProperty().SetColor(rd.random(),rd.random(),rd.random())
        self.renderer.AddActor(visNodes)

    def _vtk_define_field(self, nodeSet):
        ''' Define the field to be represented on the FE mesh.

        :param nodeSet: set of nodes to represent the field on.
        '''
        if(self.field):
            arr= self.field.fillArray(nodeSet)
            if(__debug__):
                if(not arr):
                    AssertionError('Can\'t create the array.')
            self.field.createLookUpTable()

    def _vtk_load_nodes(self, nodeSet, defFScale, eigenMode):
        ''' Load nodes in VTK.

        :param nodeSet: set of nodes to load into VTK.
        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. In case of modal analysis, the displayed 
                  position of each node equals to the initial position plus
                  its eigenVector multiplied by this factor.
                  (Defaults to 0.0, i.e. display of initial/undeformed shape)
        :param eigenMode: eigenvibration mode if we want to display the 
                          deformed shape associated with it when a modal 
                          analysis has been carried out. 
                          Defaults to None: no modal analysis.
        '''
        if eigenMode is None:
            for n in nodeSet:
                pos= n.getCurrentPos3d(defFScale)
                self.nodes.InsertPoint(n.getIdx,pos.x,pos.y,pos.z)
        else:
            for n in nodeSet:
                pos= n.getEigenPos3d(defFScale,eigenMode)
                self.nodes.InsertPoint(n.getIdx,pos.x,pos.y,pos.z)
                
    def _vtk_load_elements(self, elemSet):
        ''' Load elements into VTK.

        :param elemSet: finite element set to load into VTK.
        '''
        for e in elemSet:
            vertices= xc_base.vector_int_to_py_list(e.getIdxNodes)
            vtx= vtkIdList()
            for vIndex in vertices:
                vtx.InsertNextId(vIndex)
            #if(e.getVtkCellType!= VTK_VERTEX): LCPT commented out 20250120
            self.gridRecord.uGrid.InsertNextCell(e.getVtkCellType,vtx)
        
    def _vtk_load_constraints(self, constraintSet):
        ''' Load constraints into VTK.

        :param constraintSet: finite element set to load into VTK.
        '''
        for c in constraintSet:
            if(hasattr(c,'getIdxNodes')):
                vertices= xc_base.vector_int_to_py_list(c.getIdxNodes)
                vtx= vtkIdList()
                for vIndex in vertices:
                    vtx.InsertNextId(vIndex)
                if(c.getVtkCellType!= VTK_VERTEX):
                    self.gridRecord.uGrid.InsertNextCell(c.getVtkCellType,vtx)

    def _vtk_load_elem_mesh(self, defFScale=0.0, eigenMode=None):
        '''Load the element mesh

        :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. In case of modal analysis, the displayed 
                  position of each node equals to the initial position plus
                  its eigenVector multiplied by this factor.
                  (Defaults to 0.0, i.e. display of initial/undeformed shape)
        :param eigenMode: eigenvibration mode if we want to display the 
                          deformed shape associated with it when a modal 
                          analysis has been carried out. 
                          Defaults to None: no modal analysis.
        '''
        # Define grid
        self.nodes= vtkPoints()
        self.gridRecord.uGrid= vtkUnstructuredGrid()
        self.gridRecord.uGrid.SetPoints(self.nodes)
        eSet= self.gridRecord.xcSet
        eSet.numerate()
        self.gridRecord.uGrid.name= eSet.name+'_grid'
        # Scalar values.
        nodeSet= eSet.nodes
        numNodes= len(nodeSet)
        if(numNodes>0):
            self._vtk_define_field(nodeSet= nodeSet)
            # Load nodes into VTK.
            self._vtk_load_nodes(nodeSet= nodeSet, defFScale= defFScale, eigenMode= eigenMode)
            # Load elements into VTK.
            self._vtk_load_elements(elemSet= eSet.elements)
            # Load constraints into VTK.
            self._vtk_load_constraints(constraintSet= eSet.getConstraints)
            return True
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; error when drawing set: '"+eSet.name+"', it has no nodes so I can't get set geometry (use fillDownwards?)")
            return False
        
    def setField(self, field):
        ''' Assigns the field to be represented.

        :param field: scalar field to be represented
        '''
        if(field):
            if(self.field is None):
                self.field= field
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(className+'.'+methodName+"; field already assigned. Command ignored.")

    def defineMeshScene(self, defFScale= 0.0, eigenMode= None, color= xc.Vector([rd.random(),rd.random(),rd.random()])):
        '''Define the scene for the mesh

        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param eigenMode: eigenvibration mode if we want to display the 
                          deformed shape associated with it when a modal 
                          analysis has been carried out. 
                          Defaults to None: no modal analysis.
        :param color: RGB color to represent the elements (defaults to random
                      color)
        '''
        retval= self._vtk_load_elem_mesh(defFScale, eigenMode)
        self.renderer= vtkRenderer()
        self.renderer.SetBackground(self.bgRComp, self.bgGComp, self.bgBComp)
        self._vtk_define_nodes_actor(0.002)
        self._vtk_define_elements_actor("surface", color)
        self.renderer.ResetCamera()
        return retval

        #Implement labels.
        # if(self.gridRecord.entToLabel=="elementos"):
        #   VtkDisplayIdsElements(self.renderer)
        # elif(self.gridRecord.entToLabel=="nodes"):
        #   vtk_define_mesh_nodes.VtkDisplayIdsNodes(self.renderer)
        # else:
        #   lmsg.error("Entity: "+ self.gridRecord.entToLabel+ " unknown.")

    def FEmeshGraphic(self,setToDisplay,caption= '',cameraParameters= vtk_graphic_base.CameraParameters('XYZPos'), defFScale=0.0):
        ''' Graphic of the FE mesh

        :param setToDisplay:   XC set of elements to be displayed
        :param caption: text to write in the graphic
        :param cameraParameters: camera parameters (position, orientation,...).
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(className+'.'+methodName+'; FEmeshGraphic DEPRECATED use displayFEMesh.')
        self.cameraParameters= cameraParameters
        self.displayFEMesh(setToDisplay,caption,defFScale)

    def displayFEMesh(self, setToDisplay, caption= '', fileName= None, defFScale=0.0):
        ''' Graphic of the FE mesh

        :param setToDisplay: XC set of elements to be displayed.
        :param caption: text title to write in the graphic.
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        self.setupGrid(setToDisplay)
        self.defineMeshScene()
        self.displayScene(caption= caption, unitDescription= '', fileName= fileName)
        
    def displayLocalAxes(self, setToDisplay, caption= 'local axis', vectorScale=1.0, fileName= None, defFScale= 0.0):
        '''Display the element local axes.

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param caption:        text to display in the graphic 
        :param vectorScale:    factor to apply to the vectors length in the representation
        :param fileName: file name to store the image. If none -> window on screen.
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        '''
        self.setupGrid(setToDisplay)
        elementAvgSize= setToDisplay.elements.getAverageSize(False)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus()
        tol= 1e-3*LrefModSize
        if(elementAvgSize>tol):
            vScale= vectorScale*min(elementAvgSize, .15*LrefModSize)
        else: # only zero-length elements.
            vScale= vectorScale*.15*LrefModSize
        if(vScale==0.0): # single point model.
            vScale= 1.0
        vField= lavf.LocalAxesVectorField(setToDisplay.name+'_localAxes',vScale)
        vField.dumpVectors(setToDisplay)
        self.defineMeshScene() 
        vField.addToDisplay(self)
        self.displayScene(caption= caption, unitDescription= '', fileName= fileName)

    def displayStrongWeakAxis(self, setToDisplay, caption= 'strong [red] and weak [blue] axes', vectorScale=1.0, fileName= None):
        '''vector field display of the loads applied to the chosen set of elements in the load case passed as parameter

        :param setToDisplay:   set of elements to be displayed (defaults to total set)
        :param caption:        text to display in the graphic 
        :param vectorScale:    factor to apply to the vectors length in the representation
        :param fileName: file name to store the image. If none -> window on screen.
        '''
        self.setupGrid(setToDisplay)
        vField= lavf.StrongWeakAxisVectorField(setToDisplay.name+'_strongWeakAxis',vectorScale)
        vField.dumpVectors(setToDisplay)
        self.defineMeshScene() 
        vField.addToDisplay(self)
        self.displayScene(caption= caption, unitDescription= '', fileName= fileName)

    def _define_mesh_actors_for_set(self, elemSet, defFScale, nodeSize):
        ''' Define the mesh corresponding to the given set.

        :param elemSet: set of elements that form the mesh to display.
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        :param nodeSize: size of the spheres that represent nodes.
        '''
        self.setupGrid(elemSet)
        if elemSet.color.Norm()==0:
            elemSet.color=xc.Vector([rd.random(),rd.random(),rd.random()])
        self._vtk_load_elem_mesh(defFScale= defFScale, eigenMode=None)
        self._vtk_define_nodes_actor(nodeSize)
        self._vtk_define_elements_actor("surface", elemSet.color)

    def _define_field_actors(self, xcSets, defFScale, nodeSize, scaleConstr):
        ''' Define the fields actors in the given sets.

        :param xcSets: set or list of sets to be displayed
        :param defFScale: factor to apply to current displacement of nodes 
                   so that the display position of each node equals to
                   the initial position plus its displacement multiplied
                   by this factor. (Defaults to 0.0, i.e. display of 
                   initial/undeformed shape)
        :param nodeSize: size of the spheres that represent nodes.
        :param scaleConstr: scale of SPConstraints symbols (defaults to 0.2)
        '''
        if(type(xcSets)==list):
            for s in xcSets:
                self._define_mesh_actors_for_set(s, defFScale, nodeSize)
                self.displayConstraints(s, scaleConstr, defFScale)
        else:
            self._define_mesh_actors_for_set(xcSets, defFScale, nodeSize)
            self.displayConstraints(xcSets, scaleConstr, defFScale)

    def displayMesh(self, xcSets, field: fields.ScalarField = None, diagrams= None, caption= '', unitDescription= '', fileName= None, defFScale=0.0, nodeSize=0.01, scaleConstr= 0.2):
        '''Display the finite element mesh 

        :param xcSets: set or list of sets to be displayed
        :param field: scalar field to show (optional)
        :param diagrams: diagrams to show (optional)
        :param caption: text to display in the graphic.
        :param unitDescription: description of the units.
        :param fileName: name of the graphic file to create (if None -> screen window).
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        :param nodeSize: size of the spheres that represent nodes (defaults to
                    0.01)
        :param scaleConstr: scale of SPConstraints symbols (defaults to 0.2)
        '''
        if(field):
            self.setField(field)
        self.renderer= vtkRenderer()
        self.renderer.SetBackground(self.bgRComp,self.bgGComp,self.bgBComp)
        self._define_field_actors(xcSets= xcSets, defFScale= defFScale, nodeSize= nodeSize, scaleConstr= scaleConstr)
        self.renderer.ResetCamera()
        if(diagrams):
            for d in diagrams:
                self.appendDiagram(d)
        self.displayScene(caption= caption, unitDescription= unitDescription, fileName= fileName)

    def displayLoadOnNode(self, nod, color, force, moment, fScale, defFScale= 0.0):
        '''Display loads on one node

         :param nod: node instance
         :param color: color
         :param force: force (displayed as a single arrow)
         :param moment: moment (displayed as a torque arrow)
         :param fScale: scaling factor (forces and moments)
         :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
         '''
        #actorName= baseName+"%04d".format(nod.tag) # Node tag.
        pos= nod.getCurrentPos3d(defFScale)
        absForce= force.Norm()
        actors= list()
        if(absForce>1e-6):
            actors.extend(utils_vtk.draw_vtk_symbol(symbType= 'arrow', renderer= self.renderer, RGBcolor= color, vPos= pos, vDir= force, scale= fScale*absForce))
        absMoment= moment.Norm()
        if(absMoment>1e-6):
            actors.extend(utils_vtk.draw_vtk_symbol(symbType= 'doubleArrow', renderer= self.renderer, RGBcolor= color, vPos= pos, vDir= moment, scale= fScale*absMoment))
        for actor in actors:
            self.renderer.AddActor(actor)

    def displayNodalLoads(self, preprocessor, loadPattern, color, fScale):
        '''Display the all nodal loads defined in a load pattern

        :param preprocessor: preprocessor for the FE problem.
        :param loadPattern: load pattern
        :param color: color of the symbols (arrows)
        :param fScale: factor to apply to current displacement of nodes 
               so that the display position of each node equals to
               the initial position plus its displacement multiplied
               by this factor. (Defaults to 0.0, i.e. display of 
               initial/undeformed shape)
        '''
        loadPattern.addToDomain()
        #loadPatternName= loadPattern.getProp("dispName")
        lIter= loadPattern.loads.getNodalLoadIter
        load= lIter.next()
        while not(load is None):
            #actorName= "flecha"+loadPatternName+'{:04d}'.format(load.tag) # Tag force.
            nodeTag= load.getNodeTag
            node= preprocessor.getNodeHandler.getNode(nodeTag)
            force= load.getForce
            moment= load.getMoment
            self.displayLoadOnNode(node, color, force, moment,fScale)    
            load= lIter.next()
        loadPattern.removeFromDomain()

    def displayElementPunctualLoad(self, preprocessor, loadedElements, renderer, color, force, fScale):
        '''Display punctual loads on elements

         :param preprocessor: pre-processor of the finite element problem.
         :param loadedElements: punctual load to display (as a single arrow).
         :param renderer: vtk renderer.
         :param color: color
         :param force: force to displaay (as a single arrow)
         :param fScale: scaling factor (applied to displayed arrows)
        '''
        xForce= loadedElements.getElems()
        eleTags= loadedElements.elementTags
        for tag in eleTags:
            ele= preprocessor.getElementHandler.getElement(tag)
            pos= ele.point(xForce)
            utils_vtk.draw_vtk_symbol(symbType= 'arrow', RGBcolor= self.renderer, color= color, vPos= pos, vDir= force, scale= fScale)

    def displayElementUniformLoad(self, preprocessor, unifLoad, color, force, fScale):
        eleTags= unifLoad.elementTags
        for tag in eleTags:
            # ele= preprocessor.getElementHandler.getElement(tag)
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; displayElementUniformLoad not implemented.')

    def displayElementalLoads(self, preprocessor, loadPattern, color, fScale):
        # loadPattern.addToDomain()
        # eleLoadIter= loadPattern.loads.getElementalLoadIter
        # eleLoad= eleLoadIter.next()
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; displayElementalLoads not implemented.')
        # while not(eleLoad is None):
        #   force= eleLoad.getGlobalForces()
        #   category= eleLoad.category
        #   if(category=="uniform"):
        #     self.displayElementUniformLoad(preprocessor, eleLoad, color, force,fScale)
        #   else:
        #     self.displayElementPunctualLoad(preprocessor, eleLoad, color, force,fScale)
        # loadPattern.removeFromDomain()

    def displayLoads(self, preprocessor, loadPattern):
        clrVectores= loadPattern.getProp("color")
        fScaleVectores= loadPattern.getProp("scale")
        self.displayElementalLoads(preprocessor, loadPattern, clrVectores, fScaleVectores)
        self.displayNodalLoads(preprocessor, loadPattern, clrVectores,fScaleVectores)

    def appendDiagram(self, diagram, orientScbar=1, titleScbar=None):
        ''' Add the diagrams to the actors to display.

        :param diagram: diagram to append.
        :param orientScbar: orientation of the scalar bar (defaults to 1-horiz)
        :param titleScbar: title for the scalar bar (defaults to None)
        '''
        diagram.addDiagramToScene(self, orientScbar, titleScbar)

    def getSingleFreedomConstraintsData(self, preprocessor, nodsInSet):
        ''' Return the data to define the symbols of the single-fredom
            boundary conditions.

        :param preprocessor: preprocessor for the FE problem.
        :param nodsInSet: set of node tags whose constrains will
                          be displayed.
        '''
        # direction vectors for each DOF
        vx,vy,vz= [1,0,0],[0,1,0],[0,0,1]
        DOFdirVct= (vx,vy,vz,vx,vy,vz)
        pointGraphicData= list()
        spIter= preprocessor.getDomain.getConstraints.getSPs
        sp= spIter.next()
        while sp:
            nod= sp.getNode
            if nod.tag in nodsInSet:
                # Extract data.
                vPos= nod.getInitialPos3d
                dof= sp.getDOFNumber
                vDir= DOFdirVct[dof]
                if dof < 3: # This is not true in 2D problems.
                    symbType= 'cone'
                    color= [0,0,1]
                else:
                    symbType= 'shaftkey'
                    color= [0,1,0]
                spGraphicData={'vPos':vPos,'vDir':vDir,'symbType':symbType,'color':color}
                pointGraphicData.append(spGraphicData)
            sp= spIter.next()
        return pointGraphicData
        
    def getSkewConstraintsData(self, preprocessor, nodsInSet, defFScale):
        ''' Return the data to define the symbols of the skew
            boundary conditions.

        :param preprocessor: preprocessor for the FE problem.
        :param nodsInSet: set of node tags whose constrains will
                          be displayed.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        '''
        # direction vectors for each DOF
        pointGraphicData= list()
        mpIter= preprocessor.getDomain.getConstraints.getMPs
        mp= mpIter.next()
        while mp:
            tipo= mp.type()
            if('ymmetry' in tipo): # is a skew constraint.
                nod= mp.getNode
                if nod.tag in nodsInSet:
                    # Extract data.
                    vPos= nod.getCurrentPos3d(defFScale)
                    normal= mp.normal
                    if(len(normal)==2):
                        normal= xc.Vector([normal[0], normal[1], 0.0])
                    color= [normal[0], normal[1], normal[2]]
                    symbType= 'plane'
                    mpGraphicData={'vPos':vPos,'vDir':normal,'symbType':symbType,'color':color}
                    pointGraphicData.append(mpGraphicData)
            mp= mpIter.next()
        return pointGraphicData
    
    def displayConstraints(self, setToDisplay, scale, defFScale=0.0):
        ''' Display single point constraints.

        :param setToDisplay: set to be displayed
        :param scale: scale for SPConstraints symbols.
        :param defFScale: factor to apply to current displacement of nodes 
                    so that the display position of each node equals to
                    the initial position plus its displacement multiplied
                    by this factor. (Defaults to 0.0, i.e. display of 
                    initial/undeformed shape)
        '''
        prep= setToDisplay.getPreprocessor
        nodsInSet= set(setToDisplay.nodes.getTags())
        # Get constraint data.
        pointGraphicData= self.getSingleFreedomConstraintsData(preprocessor= prep, nodsInSet= nodsInSet)
        pointGraphicData.extend(self.getSkewConstraintsData(preprocessor= prep, nodsInSet= nodsInSet, defFScale= defFScale))
        # Create VTK actors.
        elementAvgSize= setToDisplay.elements.getAverageSize(False)
        LrefModSize= setToDisplay.getBnd(defFScale).diagonal.getModulus()
        cScale= scale*min(elementAvgSize, .15*LrefModSize)
        constraintSymbols= utils_vtk.create_actors_for_vtk_symbols(pointData= pointGraphicData, scale= cScale)
        # Add actors to renderer.
        for actor in constraintSymbols:
            self.renderer.AddActor(actor)
                    
def VtkLoadIdsNodes(recordGrid):
    '''Load node labels. Not yet implemented.'''
    nodeLabels=  create_array_set_data.VtkCreaStrArraySetData(recordGrid.setName,"nodes","etiqNod","tag")()
    if(__debug__):
        if(not nodeLabels):
            AssertionError('Can\'t create the labels.')
    recordGrid.GetPointData().SetStrings('etiqNod')

def VtkDisplayIdsNodes(recordGrid, renderer):
    '''Display node labels (not implemented yet)'''
    ids= vtkIdFilter()
    ids.SetInput(recordGrid.uGrid)
    ids.CellIdsOff()
    ids.PointIdsOff()

    VtkLoadIdsNodes(recordGrid)

    visPts= vtkSelectVisiblePoints()
    visPts.SetInput("ids")
    visPts.SetRenderer(renderer)
    visPts.SelectionWindowOff()

    #Create the mapper to display the point ids.  Specify the format to
    #   use for the labels.  Also create the associated actor.
    ldm= vtkLabeledDataMapper()
    ldm.SetInput("visPts")
    ldm.LabelTextProperty().SetColor(0.1,0.1,0.1)
    nodeLabels= vtkActor2D().SetMapper(ldm)
    renderer.AddActor2D(nodeLabels)

def VtkDisplayIdsElements(ids):
    '''Display element labels. Not implemented yet.'''
    cc= vtkCellCenters()
    cc.SetInput(ids) # Cell centroids. 

    visCells= vtkSelectVisiblePoints()
    visCells.SetInput(cc)
    visCells.SetRenderer("renderer")
    visCells.SelectionWindowOff()

    #Create the mapper to display the cell ids.  Specify the format to
    # use for the labels.  Also create the associated actor.

    cellMapper= vtkLabeledDataMapper
    cellMapper.SetInput(visCells)
    cellMapper.LabelTextProperty().SetColor(0,0,0.9)

    cellLabels= vtkActor2D()
    cellLabels.SetMapper(cellMapper)
