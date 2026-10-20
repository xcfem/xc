# -*- coding: utf-8 -*-
'''Generation of boundary conditions based on springs.'''

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
from model.boundary_cond import shallow_elastic_foundation
from misc_utils import log_messages as lmsg

class SpringBC(object):
    '''Spring to be used as boundary condition

    :ivar name: name
    :ivar modelSpace: model space
    :ivar Kx: spring stiffness in X direction (defaults to 0 -> free)
    :ivar Ky: spring stiffness in Y direction (defaults to 0 -> free)
    :ivar Kz: spring stiffness in Z direction (defaults to 0 -> free)
    :ivar foundationSet: set of nodes and elements that belong to the
                         spring supported foundation.
    '''
    def __init__(self, name, modelSpace, Kx= 0, Ky= 0, Kz= 0):
        ''' Constructor.

        :param name: name
        :param modelSpace: model space
        :param Kx: spring stiffness in X direction (defaults to 0 -> free)
        :param Ky: spring stiffness in Y direction (defaults to 0 -> free)
        :param Kz: spring stiffness in Z direction (defaults to 0 -> free)
        '''
        self.name=name
        self.modelSpace= modelSpace
        self.Kx=Kx
        self.Ky=Ky
        self.Kz=Kz
        self.springMat=[None,None,None]  #material initialization (no springs)
    
    def createSpringMaterials(self):
        '''create the spring materials in X, Y, Z directions. If there is
        no spring in any of these directions, its corresponding material is
        equal to None.
        '''
        if(self.springMat[0] is None and
           self.springMat[1] is None and
           self.springMat[2] is None): # if not already created.
           
            xSpringMaterialName= self.name + '_xSpring'
            ySpringMaterialName= self.name + '_ySpring'
            zSpringMaterialName= self.name + '_zSpring'
            if self.Kx != 0:
                typical_materials.defElasticMaterial(self.modelSpace.preprocessor,xSpringMaterialName, self.Kx)
                self.springMat[0]= xSpringMaterialName
            if self.Ky != 0:
                typical_materials.defElasticMaterial(self.modelSpace.preprocessor,ySpringMaterialName, self.Ky)
                self.springMat[1]= ySpringMaterialName
            if self.Kz != 0:
                typical_materials.defElasticMaterial(self.modelSpace.preprocessor,zSpringMaterialName, self.Kz)
                self.springMat[2]= zSpringMaterialName
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; spring materials already created. Command ignored.'
            lmsg.warning(className+'.'+methodName+warningMsg)
            

    def applyOnPointsInSet(self, setEnt):
        ''' Create springs in all the points included in the given set.

        :param setEnt: set containing the points that whose nodes will be
                       constrained with a spring.
        '''
        lstnod= sets_mng.get_lstNod_on_points_fromSet(setEnt)
        return self.applyOnNodesLst(lstnod)
        
    def applyOnNodesInSet(self,setEnt):
        ''' Create springs in all the nodes included in the given set.

        :param setEnt: set containing the points that whose nodes will be
                       constrained with a spring.
        '''
        lstnod=sets_mng.setNod_to_lst(setEnt)
        self.applyOnNodesLst(lstnod)
        
    def applyOnNodesIn3Dpos(self, lst3DPos):
        ''' Create springs in the nearest nodes to the coordinates in the 
        list of 3D positions given as parameter.

        :param lst3DPos: list of positions to locate the nodes.
        '''
        lstnod=sets_mng.get_lstNod_from_lst3DPos(self.modelSpace.preprocessor,lst3DPos)
        self.applyOnNodesLst(lstnod)
        
    def applyOnNodesLst(self, nodeList):
        '''C reate spring boundary conditions in the nodes included in 
        the given list.

        :param nodeList: list of nodes.
        '''
        self.createSpringMaterials()
        for n in nodeList:
            fixedNode, elem= self.modelSpace.setBearing(n.tag,self.springMat)
            if(__debug__):
                if((not elem) or (not fixedNode)):
                    AssertionError('Can\'t set bearing on node: '+str(n.tag))
                

    def applyBConNode(self, node):
        '''Create spring boundary conditions in the given node.

        Return the spring element created.
        '''
        self.createSpringMaterials()
        unusedFixedNode, elem= self.modelSpace.setBearing(node.tag,self.springMat)
        return elem


ElasticFoundation= shallow_elastic_foundation.ShallowElasticFoundation
    
def spring_bound_cond_anydir(setNodes,orientation,Klist,name):
    '''Apply spring boundary conditions to a set of nodes.

    :param setNodes: set of nodes to apply boundary conditions
    :param orientation: (list) of two vectors [x,y] used to orient 
       the zero length element, where: 
       x are the vector components in global coordinates defining 
          local x-axis (optional)
       y: vector components in global coordinates defining a  vector
            that lies in the local x-y plane of the element(optional).
     If the optional orientation vector are not specified, the local
     element axes coincide with the global axes. Otherwise, the local
     z-axis is defined by the cross product between the vectors x 
     and yp specified in the command line.
    :param Klist: stiffness [KX,KY,KZ,KrotX,KrotY,KrotZ]
    :param name: string to name the spring materials
    '''
    prep=setNodes.getPreprocessor
    nodes=prep.getNodeHandler
    constr=prep.getBoundaryCondHandler
    matKX=typical_materials.defElasticMaterial(prep,name+'matKX',Klist[0])
    matKY=typical_materials.defElasticMaterial(prep,name+'matKY',Klist[1])
    matKZ=typical_materials.defElasticMaterial(prep,name+'matKZ',Klist[2])
    matKrotX=typical_materials.defElasticMaterial(prep,name+'matKrotX',Klist[3])
    matKrotY=typical_materials.defElasticMaterial(prep,name+'matKropY',Klist[4])
    matKrotZ=typical_materials.defElasticMaterial(prep,name+'matKrotZ',Klist[5])
    materials=[matKX.name,matKY.name,matKZ.name,matKrotX.name,matKrotY.name,matKrotZ.name]
    for n in setNodes.nodes:
        p=n.getInitialPos3d
        nn=nodes.newNodeXYZ(p.x,p.y,p.z)
        for i in range(6):
            constr.newSPConstraint(nn.tag,i,0.0)
        predefined_spaces.setBearingBetweenNodes(prep,n.tag,nn.tag,materials,orientation)
