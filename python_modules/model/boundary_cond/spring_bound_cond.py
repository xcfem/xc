# -*- coding: utf-8 -*-
from __future__ import division

import xc_base
import geom
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from model.sets import sets_mng 

'''Generation of boundary conditions based on springs 
'''

__author__= " Ana Ortega (AO_O) "
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

class SpringBC(object):
    '''Spring to be used as boundary condition

    :ivar name: name
    :ivar Kx: spring stiffness in X direction (defaults to 0 -> free)
    :ivar Ky: spring stiffness in Y direction (defaults to 0 -> free)
    :ivar Kz: spring stiffness in Z direction (defaults to 0 -> free)
    '''
    def __init__(self,name,Kx=0,Ky=0,Kz=0):
        self.name=name
        self.Kx=Kx
        self.Ky=Ky
        self.Kz=Kz
        self.springMat=[None,None,None]  #material initialization (no springs)
    
    def createSpringMaterials(self,modelSpace):
        '''create the spring materials in X, Y, Z directions. If there is
        no spring in any of these directions, its corresponding material is
        equal to None 
        '''
        if self.Kx != 0:
            typical_materials.defElasticMaterial(modelSpace.preprocessor,self.name + '_xSpring',self.Kx)
            self.springMat[0]=self.name + '_xSpring'
        if self.Ky != 0:
            typical_materials.defElasticMaterial(modelSpace.preprocessor,self.name + '_ySpring',self.Ky)
            self.springMat[1]=self.name + '_ySpring'
        if self.Kz != 0:
            typical_materials.defElasticMaterial(modelSpace.preprocessor,self.name + '_zSpring',self.Kz)
            self.springMat[2]=self.name + '_zSpring'


    def applyOnPointsInSet(self,setEnt):
        '''create springs in all the points included in the set of entities 
        given as parameter
        '''
        prep=setEnt.getPreprocessor
        lstnod=sets_mng.get_lstNod_on_points_fromSet(setEnt)
        return self.applyOnNodesLst(prep,lstnod)
        
    def applyOnNodesInSet(self,setEnt):
        '''create springs in all the nodes included in the set of entities 
        given as parameter
        '''
        prep=setEnt.getPreprocessor
        lstnod=sets_mng.setNod_to_lst(setEnt)
        self.applyOnNodesLst(prep,lstnod)
        
    def applyOnNodesIn3Dpos(self,modelSpace,lst3DPos):
        '''create springs in the nearest nodes to the coordinates in the 
        list of 3D positions given as parameter
        '''
        lstnod=sets_mng.get_lstNod_from_lst3DPos(modelSpace.preprocessor,lst3DPos)
        self.applyOnNodesLst(modelSpace,lstnod)
        
    def applyOnNodesLst(self,modelSpace,Nodelist):
        '''create spring boundary conditions in the nodes included in 
        the list of nodes passed as parameter.
        Include the elements created in a set called by the name of the 
        SpringBD + '_set'
        '''
        # setsMng=modelSpace.preprocessor.getSets
        # setName=self.name+'_set'
        # if setsMng.exists(setName):
        #     s=setsMng.getSet(setName)
        # else:
        #     s=setsMng.defSet(setName)
        self.createSpringMaterials(modelSpace)
        # idElem= modelSpace.preprocessor.getElementLoader.defaultTag
        for n in Nodelist:
            fixedNode, elem= modelSpace.setBearing(n.tag,self.springMat)
        #    s.getElements.append(elem)

