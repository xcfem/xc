# -*- coding: utf-8 -*-
''' Utilities for the generation of strut-and-tie models.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import uuid
import geom
import xc
from materials import typical_materials
from model import model_inquiry
from misc_utils import log_messages as lmsg

dummy_spring_material= None
dummy_spring_material_name= 'strut-and-tie_dummy_spring_material'
dummy_spring_stiffness= 1e6

def create_dummy_spring_material(modelSpace):
    ''' Initializes the dummy spring material which will be used to add 
        rotational stiffnes to the desired nodes.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    '''
    global dummy_spring_material
    if(dummy_spring_material is None):
        preprocessor= modelSpace.preprocessor
        dummy_spring_material= typical_materials.defElasticMaterial(preprocessor, dummy_spring_material_name, dummy_spring_stiffness)
    return dummy_spring_material
    

def create_dummy_spring(modelSpace, node):
    ''' Put a spring to constrain the rotation of the given node. The nodes that
        are connected only to struts and ties can rotate freely, and that makes
        the model stiffness matrix singular. This function is used to avoid
        that.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    :param node: node whose rotations need to be constrained.
    '''
    create_dummy_spring_material(modelSpace)
    constrainedDOFs= model_inquiry.get_node_rotational_dofs(node)
    if(not constrainedDOFs):
        methodName= sys._getframe(0).f_code.co_name
        errorMessage= methodName+'; unknown rotational DOFs.'
        lmsg.error(errorMessage)
        zl= None
        newNode= None
    else:
        newNode= modelSpace.duplicateNode(node) # new node.
        # Create the spring.
        modelSpace.setDefaultMaterial(dummy_spring_material)
        modelSpace.setElementDimension(3)
        zl= modelSpace.newElement("ZeroLength",[newNode.tag, node.tag])
        zl.clearMaterials()
        for dof in constrainedDOFs:
            zl.setMaterial(dof, dummy_spring_material.name)
        # Boundary conditions: fix the newly created node.
        numDOFs= newNode.getNumberDOF
        newNodeTag= newNode.tag
        for dof in range(0,numDOFs):
            spc= modelSpace.newSPConstraint(newNodeTag, dof, 0.0)
            if(__debug__):
                if(not spc):
                    AssertionError('Can\'t create the constraint.')
    return zl, newNode

def define_dummy_springs(modelSpace, nodes):
    ''' Put a spring to constrain the rotation of the given nodes. The nodes
        that are connected only to struts and ties can rotate freely, and that
        makes the model stiffness matrix singular. This function is used to
        avoid that.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    :param node: node whose rotations need to be constrained.
    '''
    # Get the current default materials.
    previousDefaultMaterials= modelSpace.getDefaultMaterials()
    retval= list()
    for node in nodes:
        zl, newNode= create_dummy_spring(modelSpace, node)
        retval.append((zl, newNode))
    # Revert the default materials.
    modelSpace.setDefaultMaterials(previousDefaultMaterials)
    return retval

class StrutAndTieModel(object):
    ''' Base class for strut-and-tie models.

    '''
    def getStrutMaterial(self, modelSpace, concrete, linearElastic):
        ''' Get the material for the strut elements.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param concrete: concrete material.
        :param linearElastic: if True, assign a linear elastic material to
                              struts and ties (insteaa of an no-tension or
                              no-compression material). Used with debugging
                              purposes.
        '''
        self.concrete= concrete
        if(linearElastic):
            retval= self.concrete.defElasticMaterial(preprocessor= modelSpace.preprocessor)
        else:
            retval= self.concrete.defElasticNoTensMaterial(preprocessor= modelSpace.preprocessor, a= self.concrete_a)
        return retval

    def getTieMaterial(self, modelSpace, reinfSteel, linearElastic):
        ''' Get the material for the tie elements.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param reinfSteel: reinforcing steel material.
        :param linearElastic: if True, assign a linear elastic material to
                              struts and ties (insteaa of an no-tension or
                              no-compression material). Used with debugging
                              purposes.
        '''
        self.reinfSteel= reinfSteel
        if(linearElastic):
            retval= self.reinfSteel.defElasticMaterial(preprocessor= modelSpace.preprocessor)
        else:
            retval= self.reinfSteel.defElasticNoCompressionMaterial(preprocessor= modelSpace.preprocessor, a= self.steel_a)
        return retval
    
    def getStrutList(self):
        ''' Return a list of all the struts.'''
        retval= list()
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errorMessage= className+'.'+methodName+'; error: this method must be overloaded in derived classes.'
        lmsg.error(errorMessage)
        return retval

    def getTieList(self):
        ''' Return a list of all the ties.'''
        retval= list()
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        errorMessage= className+'.'+methodName+'; error: this method must be overloaded in derived classes.'
        lmsg.error(errorMessage)
        return retval

    def checkStressesSign(self):
        ''' Check the struts are compressed and the ties tensioned.'''
        retval= True
        
        # Check struts.
        struts= self.getStrutList()
        if(struts):
            fctm= self.concrete.fctk() # mean concrete characteristic tensile strength
            for s in struts:
                N= s.getN()
                stress= N/s.sectionArea
                if(stress>fctm):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    errorMessage= className+'.'+methodName+"; strut :"+str(s.tag)
                    errorMessage+= ' has a positive stress: '+str(stress/1e6)
                    errorMessage+= ' MPa. Something is wrong with this model.'
                    lmsg.error(errorMessage)
                    retval= False
        # Check ties.
        ties= self.getTieList()
        if(ties):
            steelCompressionLimit= -self.reinfSteel.fyd()/1e2
            for t in ties:
                N= t.getN()
                stress= N/t.sectionArea
                if(stress<steelCompressionLimit):
                    className= type(self).__name__
                    methodName= sys._getframe(0).f_code.co_name
                    errorMessage= className+'.'+methodName+"; tie :"+str(s.tag)
                    errorMessage+= ' has a negative stress: '+str(stress/1e6)
                    errorMessage+= ' MPa. Something is wrong with this model.'
                    lmsg.error(errorMessage)
                    retval= False
        return retval
