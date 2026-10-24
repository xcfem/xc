# -*- coding: utf-8 -*-
'''Generation of shallow elastic foundation with boundary conditions based on 
   springs.'''

from __future__ import division

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

import sys
import math
from misc_utils import log_messages as lmsg
from model import predefined_spaces
from model.sets import sets_mng
from model.boundary_cond import shallow_elastic_foundation as sef
from materials import typical_materials
from materials import soil_structure_interaction as ssi

class ShallowInelasticFoundation(sef.ShallowElasticFoundation):
    '''Region resting on springs (Winkler elastic no-tension foundation)


    :ivar frictionMu: Concrete-soil friction coefficient.
    '''
    def __init__(self, wModulus:float, frictionYieldStress:float, hModulusFraction:float= 0.1):
        ''' Constructor.

        :param wModulus: Winkler modulus of the foundation (springs in Z 
                         direction) [F/L^3].
        :param frictionYieldStress: friction yield stress (F/L^2).
        :param hModulusFraction: fraction of the Winkler modulus to apply as 
                                 stiffness in the contact plane (springs in X 
                                 and Y directions).
        '''
        super().__init__(wModulus, hModulusFraction= hModulusFraction, noTensionZ= True)
        self.frictionYieldStress= frictionYieldStress


    def getFrictionMaterial(self, preprocessor, matName):
        ''' Return the material that represents the friction between the
            foundation and the soil that supports it.The material is 
            initialized here and the corresponding soil reaction modulus 
            and the yield force are assigned later to each node depending 
            on its tributary area.

        :param preprocessor: pre-processor of the finite element problem.
        :param matName: name for the new material (if None: let the preprocessor
                        assign the name).
        '''
        return ssi.def_shallow_foundation_friction_material(preprocessor= preprocessor, matName= matName, initialStiffness= 0.1, yieldForce= self.frictionYieldStress)

    def getNormalReactionMaterial(self, preprocessor, matName):
        ''' Return the material that represents the normal reaction 
            between the foundation and the soil that supports it.The material
            is initialized here and the corresponding soil reaction modulus 
            is assigned later to each node depending on its tributary area.

        :param preprocessor: pre-processor of the finite element problem.
        :param matName: name for the new material (if None: let the preprocessor
                        assign the name).
        '''
        if self.noTensionZ:
            return typical_materials.defElastNoTensMaterial(preprocessor, matName, 1)
        else:
            return typical_materials.defElasticMaterial(preprocessor, matName, 1)
        
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
            self.xSpring= self.getFrictionMaterial(preprocessor,matName= xSpringMaterialName)
            self.ySpring= self.getNormalReactionMaterial(preprocessor,ySpringMaterialName)
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
            self.xSpring= self.getFrictionMaterial(preprocessor, matName= xSpringMaterialName)
            ySpringMaterialName= name + '_ySpring'
            self.ySpring= self.getFrictionMaterial(preprocessor, matName= ySpringMaterialName)
            zSpringMaterialName= name + '_zSpring'
            self.zSpring= self.getNormalReactionMaterial(preprocessor,matName= zSpringMaterialName)
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
            # Tangent reaction material properties.
            tangentStiffness= self.hModulusFraction*self.wModulus*nodeTributaryArea
            frictionYieldForce= self.frictionYieldStress*nodeTributaryArea
            # Normal reaction material properties.
            normalStiffness= self.wModulus*nodeTributaryArea
            self.xSpring.setE(tangentStiffness)
            self.xSpring.set_fyn(-frictionYieldForce)
            self.xSpring.set_fyp(frictionYieldForce)
            # Recompute internal values.
            self.xSpring.revertToStart()
            if(dimSpace==2): # Two-dimensional problem.
                self.ySpring.setE(normalStiffness)
                nodeSprings= [self.xSpring.name,self.ySpring.name]
            else: # Three-dimensional problem.
                self.ySpring.setE(tangentStiffness)
                self.ySpring.set_fyn(-frictionYieldForce)
                self.ySpring.set_fyp(frictionYieldForce)
                # Recompute internal values.
                self.ySpring.revertToStart()
                self.zSpring.setE(normalStiffness)
                # Recompute internal values.
                self.zSpring.revertToStart()
                nodeSprings= [self.xSpring.name, self.ySpring.name,self.zSpring.name]
            newNode, springElement= modelSpace.setBearing(n.tag, nodeSprings)
            if __debug__:
                if(not newNode):
                    AssertionError('Can\'t set bearing on node: '+str(n.tag))
                
            self.springs[n.tag]= springElement
            self.tributaryAreas[n.tag]= nodeTributaryArea
            idElem+= 1
