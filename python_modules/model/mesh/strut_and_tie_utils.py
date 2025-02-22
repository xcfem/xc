# -*- coding: utf-8 -*-
''' Utilities for the generation of strut-and-tie models.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import uuid
import geom
import xc
from materials import typical_materials
from materials.sections.fiber_section import def_simple_RC_section

dummy_spring_material= None
dummy_spring_material_name= 'strut-and-tie_dummy_spring_material'
dummy_spring_stiffness= 1e6

def create_dummy_spring(modelSpace, node):
    ''' Put a spring to constrain the rotation of the given node. The nodes that
        are connected only to struts and ties can rotate freely, and that makes
        the model stiffness matrix singular. This function is used to avoid
        that.

    :param modelSpace: PredefinedSpace object used to create the FE model.
    :param node: node whose rotations need to be constrained.
    '''
    global dummy_spring_material
    if(dummy_spring_material is None):
        preprocessor= modelSpace.preprocessor
        dummy_spring_material= typical_materials.defElasticMaterial(preprocessor, "dummy_spring_stiffness", dummy_spring_stiffness)
    nDOFs= node.getNumberDOF
    dim= node.dim
    if((nDOFs==3) and (dim==2)): # 2D structural.
        dimElem= 3
        constrainedDOFs= [2]
    elif((nDOFS==6) and (dim==3)): # 3D structural.
        dimElem= 6
        constrainedDOFs= [3, 4, 5]
    else:
        methodName= sys._getframe(0).f_code.co_name
        errorMessage= methodName+'; unknown rotational DOFs.'
        lmsg.error(errMsg)
    newNode= modelSpace.duplicateNode(node) # new node.
    # Create the spring.
    modelSpace.setDefaultMaterial(dummy_spring_material)
    modelSpace.setElementDimension(dimElem)
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

    
# 2 pile cap geometry.
#    
#                
#                 |  pier
#                 |
#       n1   n2   |n3 n4   n5
#         +----+--+--+----+
#         | \ /.\  / .\  /|
#         |  / .  \  . \/ |  d
#         | / \./   \. / \|
#         +----+-----+----+
#       n6|   n7     n8   |n9
#         |               | 
#         |               | 
#         |               | 
#    left | pile    right | pile
#    
class Pilecap2Piles(object):
    ''' Creates a strut-and-tie model for a pile cap for two piles.

    :ivar pierBottomNode: bottom node of the pier.
    :ivar leftPileTopNode: top node of the left pile.
    :ivar rightPileTopNode: top node of the right pile.
    :ivar pierEffectiveWidth: effective width of the pier.
    '''
    def __init__(self, pierBottomNode, leftPileTopNode, rightPileTopNode, pierEffectiveWidth):
        ''' Constructor.

        :param pierBottomNode: bottom node of the pier.
        :param leftPileTopNode: top node of the left pile.
        :param rightPileTopNode: top node of the right pile.
        :param pierEffectiveWidth: effective width of the pier.
        '''
        self.pierBottomNode= pierBottomNode
        self.leftPileTopNode= leftPileTopNode
        self.rightPileTopNode= rightPileTopNode
        self.pierEffectiveWidth= pierEffectiveWidth

    def createStrutAndTieModel(self, modelSpace, strutArea, concrete, topDownTiesArea, topChordTiesArea, bottomChordTiesArea, reinfSteel, xcPierSectionMaterial, linearElastic= False):
        ''' Creates an strut-and-tie model and attach it to the nodes of the 
            pier and the piles.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param strutAre: area of the struts.
        :param concrete: concrete material.
        :param topDownTiesArea: area of the ties.
        :param reinfSteel: reinforcing steel material.
        :param xcPierSectionMaterial: XC section material for the pier.
        '''
        pierBottomNodePos= self.pierBottomNode.getInitialPos3d
        leftPileTopNodePos= self.leftPileTopNode.getInitialPos3d
        rightPileTopNodePos= self.rightPileTopNode.getInitialPos3d
        bottomLine= geom.Segment3d(leftPileTopNodePos, rightPileTopNodePos)
        plane= geom.Plane3d(leftPileTopNodePos, rightPileTopNodePos, pierBottomNodePos)
        iVector= (rightPileTopNodePos-leftPileTopNodePos).normalized()
        kVector= plane.getNormal()
        jVector= kVector.cross(iVector).normalized()
        localReference= geom.Ref2d3d(geom.Pos3d(0,0,0), iVector, jVector)

        # Compute top chord node positions.
        v= leftPileTopNodePos.dist(rightPileTopNodePos)/2.0
        a= self.pierEffectiveWidth/2.0
        d= bottomLine.dist(pierBottomNodePos) # effective depth.
        x_i= [-v, -a, a, v]
        globalTopChordPositions= list()
        globalBottomChordPositions= list()
        for x in x_i:
            topPos= pierBottomNodePos+x*iVector
            globalTopChordPositions.append(topPos)
        for topPos in globalTopChordPositions[1:-1]: # The corners are the pile top nodes. 
            bottomPos= topPos-d*jVector
            globalBottomChordPositions.append(bottomPos)
        dim= self.pierBottomNode.dim
        self.topChordNodes= list()
        self.bottomChordNodes= list()
        if(dim==2):
            for pos in globalTopChordPositions:
                nodePos= localReference.getLocalPosition(pos)
                newNode= modelSpace.newNodeXY(nodePos.x, nodePos.y)
                self.topChordNodes.append(newNode)
            for pos in globalBottomChordPositions:
                nodePos= localReference.getLocalPosition(pos)
                newNode= modelSpace.newNodeXY(nodePos.x, nodePos.y)
                self.bottomChordNodes.append(newNode)
        elif(dim==3):
            for pos in globalTopChordPositions:
                newNode= modelSpace.newNodeXY(pos.x, pos.y, pos.z)
                self.topChordNodes.append(newNode)
            for pos in globalBottomChordPositions:
                newNode= modelSpace.newNodeXY(pos.x, pos.y, pos.z)
                self.bottomChordNodes.append(newNode)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMessage= className+'.'+methodName+"; not implemented for dimension: "+str(dim)
            lmsg.error(errMsg)
            
        # Complete the lists of nodes with the existing ones.
        self.topChordNodes.insert(2,self.pierBottomNode)
        self.bottomChordNodes.insert(0, self.leftPileTopNode)
        self.bottomChordNodes.append(self.rightPileTopNode)
        # Define elements.
        ## Define struts.
        ### Define material.
        if(linearElastic):
            concreteNoTension= concrete.defElasticMaterial(preprocessor= modelSpace.preprocessor)
        else:
            concreteNoTension= concrete.defElasticNoTensMaterial(preprocessor= modelSpace.preprocessor, a= .001)
        modelSpace.setElementDimension(dim)
        modelSpace.setDefaultMaterial(concreteNoTension)
        self.pilecapStruts= list() # Struts in the pilecap.
        pilecapStrutIndexes= [(0,1), (1,0), (3, 3), (4,2)]
        for (iTop, iBottom) in pilecapStrutIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newStrut= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newStrut.sectionArea= strutArea
            self.pilecapStruts.append(newStrut)
            
        self.pierStruts= list() # Pier struts (dummy struts to make the forces
                                # reach the bottom of the pilecap).
        pierStrutIndexes= [(1, 2), (3,1)]
        for (iTop, iBottom) in pierStrutIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newStrut= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)

        ## Define ties.
        ### Define material.
        if(linearElastic):
            steelNoCompression= reinfSteel.defElasticMaterial(preprocessor= modelSpace.preprocessor)
        else:
            steelNoCompression= reinfSteel.defElasticNoCompressionMaterial(preprocessor= modelSpace.preprocessor, a= .001)
        modelSpace.setDefaultMaterial(steelNoCompression)
        modelSpace.setElementDimension(dim)
        ### Define top-down ties.
        self.pilecapTopDownTies= list() # Top-down ties in the pilecap.
        topDownPilecapTieIndexes= [(0,0), (4,3)]
        for (iTop, iBottom) in topDownPilecapTieIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newTie= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newTie.sectionArea= topDownTiesArea
            self.pilecapTopDownTies.append(newTie)
        
        topDownPierTieIndexes= [(1,1), (3, 2)]
        self.pierTopDownTies= list() # Top-down ties in the pier(dummy ties
                                     # to make the forces reach the bottom
                                     # of the pilecap).
        for (iTop, iBottom) in topDownPierTieIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newTie= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newTie.sectionArea= topDownTiesArea
            self.pierTopDownTies.append(newTie)

        ### Define top chord ties.
        self.pilecapTopChordTies= list() # Top-down ties in the pilecap.
        topChordPilecapTieIndexes= [(0,1), (1, 3), (3,4)]
        for (i, j) in topChordPilecapTieIndexes:
            iNode= self.topChordNodes[i]
            jNode= self.topChordNodes[j]
            newTie= modelSpace.newElement("Truss", [iNode.tag, jNode.tag])
            newTie.sectionArea= topChordTiesArea
            self.pilecapTopChordTies.append(newTie)

        ### Define bottom chord ties.
        self.pilecapBottomChordTies= list() # Bottom chord ties in the pilecap.
        bottomChordPilecapTieIndexes= [(0,1), (1, 2), (2,3)]
        for (i, j) in bottomChordPilecapTieIndexes:
            iNode= self.bottomChordNodes[i]
            jNode= self.bottomChordNodes[j]
            newTie= modelSpace.newElement("Truss", [iNode.tag, jNode.tag])
            newTie.sectionArea= bottomChordTiesArea
            self.pilecapBottomChordTies.append(newTie)

        ### Define dummy springs (to fix rotational DOFs only).
        nodesToConstrain= [self.topChordNodes[0], self.topChordNodes[-1], self.bottomChordNodes[1], self.bottomChordNodes[2]]
        springsAndNodes= define_dummy_springs(modelSpace, nodesToConstrain)
        ### Connect the pier with the pilecap using the pier section
        ### as "rigid beam".
        modelSpace.setDefaultMaterial(xcPierSectionMaterial)
        n1Tag= self.topChordNodes[1].tag
        n2Tag= self.topChordNodes[2].tag
        n3Tag= self.topChordNodes[3].tag
        crdTransfName= str(uuid.uuid4().hex)
        if(dim==2):
            crdTransf= modelSpace.newLinearCrdTransf(crdTransfName)
            modelSpace.setDefaultCoordTransf(crdTransf)
            modelSpace.newElement('ElasticBeam2d', [n1Tag, n2Tag])
            modelSpace.newElement('ElasticBeam2d', [n3Tag, n2Tag])
        elif(dim==3):
            xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
            crdTransf= modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
            modelSpace.setDefaultCoordTransf(crdTransf)
            modelSpace.newElement('ElasticBeam3d', [n1Tag, n2Tag])
            modelSpace.newElement('ElasticBeam3d', [n3Tag, n2Tag])
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMessage= className+'.'+methodName+"; not implemented for dimension: "+str(dim)
            lmsg.error(errMsg)
            
    def createDummyElasticModel(self, modelSpace, concrete):
        ''' Creates a simple linear elastic model that transfer the loads from
            the pier to the piles. This model can be used when calculating 
            other parts of the model and the internal forces in the pilecap
            are irrelevant for the purpose of the performed analysis.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        '''
        
        pierBottomNodePos= self.pierBottomNode.getInitialPos3d
        leftPileTopNodePos= self.leftPileTopNode.getInitialPos3d
        rightPileTopNodePos= self.rightPileTopNode.getInitialPos3d
        bottomLine= geom.Segment3d(leftPileTopNodePos, rightPileTopNodePos)
        d= bottomLine.dist(pierBottomNodePos) # effective depth.
        
        dim= self.pierBottomNode.dim
        crdTransfName= str(uuid.uuid4().hex)
        n0Tag= self.pierBottomNode.tag
        n1Tag= self.leftPileTopNode.tag
        n2Tag= self.rightPileTopNode.tag
        # Define material.
        dummyRCSectionName= str(uuid.uuid4().hex)
        dummyRCSection= def_simple_RC_section.RCRectangularSection(name= dummyRCSectionName, sectionDescr= 'dummy RC section', concrType= concrete, reinfSteelType= None, width= d, depth= d)
        if(dim==2):
            crdTransf= modelSpace.newLinearCrdTransf(crdTransfName)
            modelSpace.setDefaultCoordTransf(crdTransf)
            xcDummySectionMaterial= dummyRCSection.defElasticShearSection2d(preprocessor= modelSpace.preprocessor)
            modelSpace.setDefaultMaterial(xcDummySectionMaterial)
            modelSpace.newElement('ElasticBeam2d', [n0Tag, n1Tag])
            modelSpace.newElement('ElasticBeam2d', [n0Tag, n2Tag])
            modelSpace.newElement('ElasticBeam2d', [n1Tag, n2Tag])
        elif(dim==3):
            plane= geom.Plane3d(leftPileTopNodePos, rightPileTopNodePos, pierBottomNodePos)
            kVector= plane.getNormal()
            xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
            crdTransf= modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
            modelSpace.setDefaultCoordTransf(crdTransf)
            xcDummySectionMaterial= dummyRCSection.defElasticShearSection3d(preprocessor= modelSpace.preprocessor)
            modelSpace.setDefaultMaterial(xcDummySectionMaterial)
            
            modelSpace.newElement('ElasticBeam3d', [n0Tag, n1Tag])
            modelSpace.newElement('ElasticBeam3d', [n0Tag, n2Tag])
            modelSpace.newElement('ElasticBeam3d', [n1Tag, n2Tag])
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMessage= className+'.'+methodName+"; not implemented for dimension: "+str(dim)
            lmsg.error(errMsg)
            
    def getBottomChordTies(self):
        ''' Return a list containing the elements that model the bottom chord
            tie.'''
        return self.pilecapBottomChordTies
            
        
        
        
        
        
        
        
