# -*- coding: utf-8 -*-
'''Generation of shallow elastic foundation with boundary conditions based on 
   springs.

The problem with the friction materials created in this model is that the 
friction force is independent of the normal reaction in the contact, and that 
does not correspond to the physical reality.
'''

from __future__ import division

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

import sys
import geom
from model import predefined_spaces
from materials import typical_materials
from model.sets import sets_mng
from geotechnics import ground_pressure
from postprocess.xcVtk.fields import fields
from postprocess.xcVtk.FE_model import vtk_FE_graphic
from misc_utils import log_messages as lmsg

class ShallowElasticFoundation(object):
    '''Region resting on springs (Winkler elastic foundation)

    :ivar wModulus: Winkler modulus of the foundation (springs in Z direction)
    :ivar hModulusFraction: fraction of the Winkler modulus to apply as 
                            stiffness in the contact plane (springs in X and Y 
                            directions)
    :ivar noTensionZ: if True springs in Z direction are made of 
                      no-tension material (defaults to None)
    '''
    def __init__(self, wModulus, hModulusFraction, noTensionZ=False):
        ''' Constructor.

        :param wModulus: Winkler modulus of the foundation (springs in Z 
                         direction).
        :param hModulusFraction: fraction of the Winkler modulus to apply as 
                                 stiffness in the contact plane (springs in X 
                                 and Y directions).
        :param noTensionZ: if True springs in Z direction are made of 
                           no-tension material (defaults to None)
        '''
        self.wModulus= wModulus
        self.hModulusFraction= hModulusFraction
        self.noTensionZ= noTensionZ
        self.xSpring= None
        self.ySpring= None
        self.zSpring= None

    def getSupportingNode(self, node):
        ''' Return the node that supports the given one (i.e. the node at the
            other end of the spring).

        :param node: node whose supporting one will be returned.
        '''
        retval= None
        if(len(self.springs)>0):
            nTag= node.tag
            if(nTag in self.springs):
                e= self.springs[nTag]
                retval= e.getNodes[0]
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= '; node: '+str(nTag)+' not found.'
                lmsg.error(className+'.'+methodName+errorMsg)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; springs not created yet.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval
        
    def createMaterials2D(self, preprocessor, name):
        ''' Create the spring materials for the zero-length elements in a 2D
        space. The materials are initialized here and the corresponding soil
        reaction modulus is assigned later to each node depending on the 
        tributary area of the node.

        :param preprocessor: pre-processor of the FE problem at hand.
        :param name: name to give to the spring materials. 
        '''
        if(self.xSpring is None and
           self.ySpring is None): # if not already created.
           
            xSpringMaterialName= name + '_xSpring'
            ySpringMaterialName= name + '_ySpring'
            self.xSpring= typical_materials.defElasticMaterial(preprocessor, xSpringMaterialName,0.1)
            if self.noTensionZ:
                self.ySpring= typical_materials.defElastNoTensMaterial(preprocessor, ySpringMaterialName,1)
            else:
                self.ySpring= typical_materials.defElasticMaterial(preprocessor,ySpringMaterialName,1)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; spring materials already created. Command ignored.'
            lmsg.warning(className+'.'+methodName+warningMsg)
            
    def createMaterials3D(self, preprocessor, name):
        ''' Create the spring materials for the zero-length elements in a 3D
        space. The materials are initialized here and the corresponding soil
        reaction modulus is assigned later to each node depending on its 
        tributary area.

        :param preprocessor: pre-processor of the FE problem at hand.
        :param name: name to give to the spring materials. 
        '''
        if(self.xSpring is None and
           self.ySpring is None and
           self.zSpring is None): # if not already created.
           
            xSpringMaterialName= name + '_xSpring'
            ySpringMaterialName= name + '_ySpring'
            zSpringMaterialName= name + '_zSpring'
            self.xSpring=typical_materials.defElasticMaterial(preprocessor, xSpringMaterialName, 0.1)
            self.ySpring=typical_materials.defElasticMaterial(preprocessor, ySpringMaterialName, 0.1)
            if self.noTensionZ:
                self.zSpring=typical_materials.defElastNoTensMaterial(preprocessor, zSpringMaterialName, 1)
            else:
                self.zSpring=typical_materials.defElasticMaterial(preprocessor, zSpringMaterialName, 1)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; spring materials already created. Command ignored.'
            lmsg.warning(className+'.'+methodName+warningMsg)
            
    def createMaterials(self, preprocessor, name):
        ''' Create the spring materials for the zero-length elements regardless
            of the space dimension. The materials are initialized here and the 
            corresponding soil reaction modulus is assigned later to each node 
            depending on its tributary area.

        :param preprocessor: pre-processor of the FE problem at hand.
        :param name: name to give to the spring materials. 
        '''
        dimSpace= preprocessor.getNodeHandler.dimSpace # space dimension.
        if(dimSpace==2):
            self.createMaterials2D(preprocessor, name)
        else:
            self.createMaterials3D(preprocessor, name)
            
    def generateSprings(self, xcSet):
        '''Creates the springs at the nodes of the given set.

        :param xcSet: set whose nodes will be constrained by the springs.
        '''
        self.foundationSet= xcSet #Set with elastic supported elements
        self.springs= dict() # spring elements.
        self.tributaryAreas= sets_mng.get_tributary_areas(xcSet= self.foundationSet, initialGeometry= False) # tributary areas.
        preprocessor= self.foundationSet.getPreprocessor
        # Get the dimensions of the space (usually 2 or 3).
        dimSpace= preprocessor.getNodeHandler.dimSpace # space dimension.
        if(dimSpace==2):
            modelSpace= predefined_spaces.getStructuralMechanics2DSpace(preprocessor)
        else:
            modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
        self.createMaterials(preprocessor, self.foundationSet.name)
        idElem= preprocessor.getElementHandler.defaultTag
        sNod= self.foundationSet.nodes
        for n in sNod:
            nodeTributaryArea= self.tributaryAreas[n.tag]
            # Tangent stiffness.
            tangentStiffness= self.hModulusFraction*self.wModulus*nodeTributaryArea
            normalStiffness= self.wModulus*nodeTributaryArea
            self.xSpring.setE(tangentStiffness) 
            if(dimSpace==2): # Two-dimensional problem.
                self.ySpring.setE(normalStiffness)
                nodeSprings= [self.xSpring.name,self.ySpring.name]
            else: # Three-dimensional problem.
                self.ySpring.setE(tangentStiffness)
                self.zSpring.setE(normalStiffness)
                nodeSprings= [self.xSpring.name, self.ySpring.name,self.zSpring.name]
            newNode, springElement= modelSpace.setBearing(n.tag, nodeSprings)
            if __debug__:
                if(not newNode):
                    AssertionError('Can\'t set bearing on node: '+str(n.tag))
                
            self.springs[n.tag]= springElement
            self.tributaryAreas[n.tag]= nodeTributaryArea
            idElem+= 1
            
    def getCentroid(self):
        '''Returns the geometric baricenter of the springs.'''
        dx= 0.0; dy= 0.0; dz= 0.0
        A= 0.0
        for nTag in self.springs:
            e= self.springs[nTag]
            n= e.getNodes[1]
            a= self.tributaryAreas[n.tag]
            pos= n.getInitialPos3d
            A+= a
            dx+= a*pos[0]; dy+= a*pos[1]; dz+= a*pos[2]
        dx/=A; dy/=A; dz/=A
        return geom.Pos3d(dx,dy,dz)
    
    def calcPressures(self):
        ''' Compute the foundation pressures over the soil. Calculates 
         pressures  and forces in the free nodes of the springs
         (those that belong to both the spring and the foundation)
         and stores these values as properties of those nodes:
         property 'soilPressure:' [xStress,yStress,zStress]
         property 'soilReaction:' [xForce,yForce,zForce]
        '''
        self.svdReac= ground_pressure.compute_ground_pressure_from_springs(soilSprings= self.springs, tributaryAreas= self.tributaryAreas, foundationCentroid= self.getCentroid())
        return self.svdReac
        
    def getFoundationSupportedNodes(self):
        ''' Return the nodes that belong to both the foundation ond to one of
            the springs that support the foundation.
        '''
        retval= list()
        for eTag in self.springs:
            e= self.springs[eTag]
            n= e.getNodes[1]
            retval.append(n)
        return retval
    
    def getFoundationSupportingNodes(self):
        ''' Return the nodes support the foundation (the constrained nodes).
        '''
        retval= list()
        for eTag in self.springs:
            e= self.springs[eTag]
            n= e.getNodes[0]
            retval.append(n)
        return retval

    def getReaction(self):
        ''' Return the 3D sliding vector system equivalent to the reactions on
            the supporting nodes.'''
        centroid= self.getCentroid()
        retval= geom.SlidingVectorsSystem3d(centroid)
        supportingNodes= self.getFoundationSupportingNodes()
        for n in supportingNodes:
            nodePos= n.getInitialPos3d
            nodeReaction= n.getReaction
            svec= geom.SlidingVector3d(nodePos, geom.Vector3d(nodeReaction[0], nodeReaction[1], nodeReaction[2]))
            retval+= svec
        return retval
        
    def getMaxMinAvgPressure(self, component):
        ''' Return the maximum foundation pressure component over the soil. 
         Calculates pressures  and forces in the free nodes of the springs
         (those that belongs to both the spring and the foundation)
         and stores these values as properties of those nodes:
         property 'soilPressure:' [xStress,yStress,zStress]
         property 'soilReaction:' [xForce,yForce,zForce]

        :param component: component of the pressure (0, 1, or 2) to be 
                          considered.
        '''
        foundationNodes= self.getFoundationSupportedNodes()
        maxPressure= 0.0
        maxPressureNode= None
        minPressure= 0.0
        minPressureNode= None
        
        if(foundationNodes):
            # Compute  pressures.
            self.calcPressures()
            n0= foundationNodes[0]
            pressure= n0.getProp('soilPressure')[component]
            maxPressure= pressure
            maxPressureNode= n0
            minPressure= pressure
            minPressureNode= n0
            avgPressure= pressure
            for n in foundationNodes[1:]:
                pressure= n.getProp('soilPressure')[component]
                if(pressure>maxPressure):
                    maxPressure= pressure
                    maxPressureNode= n
                if(pressure<minPressure):
                    minPressure= pressure
                    minPressureNode= n
                avgPressure+= pressure
            avgPressure/=len(foundationNodes)
        return maxPressure, maxPressureNode, minPressure, minPressureNode, avgPressure             

    def displayPressures(self, caption, fUnitConv, unitDescription,component= 2, rgMinMax=None,fileName=None):
        '''Display foundation pressures for a single load case.

        :param caption: caption for the displayed image.
        :param fUnitConv: unit conversion factor.
        :param unitDescription: unit description.
        :param component: component of the pressure to display.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
        :param fileName: file name (defaults to None -> screen display)
        '''
        reac= self.calcPressures()
        if __debug__:
            if(not reac):
                AssertionError('Error computing pressures.')

        field= fields.ExtrapolatedScalarField('soilPressure','getProp', self.foundationSet, component=2, fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        field.display(displaySettings, caption= caption, fileName=fileName, unitDescription= unitDescription)


    def displayMaxPressures(self, modelSpace, analysis, combs, caption, fUnitConv, unitDescription, rgMinMax=None, fileName=None):
        '''
        Calculate and display the maximum earth pressures (Z direction)
        obtained from the group of load combinations passed as paremeter.

        :param modelSpace: wrapper for the preprocessor of the FE problem at
                           hand.
        :param analysis: analysis type 
                         (e.g. predefined_solutions.simple_static_linear(FEprb))
        
        :param combs: load cases to analyze and compare to obtain the maximum 
                      pressures.
        :param caption: caption for the displayed image.
        :param fUnitConv: unit conversion factor to apply to the results.
        :param unitDescription: text to display as unit description.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax  in red (defaults to None).
        :param fileName: file name (defaults to None -> screen display)
        '''
        #Init max. pressures
        nodSet= self.foundationSet.getNodes
        for n in nodSet:
            n.setProp('maxSoilPressure',-1e10)
        # Calculate max. pressure
        comb_keys=[key for key in combs] 
        for k in comb_keys:
            modelSpace.removeAllLoadPatternsFromDomain()
            modelSpace.revertToStart()
            modelSpace.addNewLoadCaseToDomain(combs[k].name,combs[k].expr, reset= False) # Previous loads removed already, so no need to reset.
            result= analysis.analyze(1)
            if(result!=0):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; can\'t solve for load case: '+str(combs[k].name)+'.')
                exit(-1)
            reac= self.calcPressures()
            if __debug__:
                if(not reac):
                    AssertionError('Error computing pressures.')
            for n in nodSet:
                prs=n.getProp('soilPressure')[2]
                if prs > n.getProp('maxSoilPressure'):
                    n.setProp('maxSoilPressure',prs)
        # Display max. pressures
        field= fields.ExtrapolatedScalarField(name='maxSoilPressure',functionName='getProp',xcSet=self.foundationSet,component=None,fUnitConv=fUnitConv,rgMinMax=rgMinMax)
        displaySettings= vtk_FE_graphic.DisplaySettingsFE()
        field.display(displaySettings,caption= caption+' '+unitDescription,fileName=fileName)
        modelSpace.removeLoadCaseFromDomain(combs[k].name)
