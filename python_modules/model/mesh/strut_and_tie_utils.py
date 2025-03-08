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
from materials.sections.fiber_section import def_simple_RC_section
from model import model_inquiry
from misc_utils import log_messages as lmsg

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
class PileCap2Piles(StrutAndTieModel):
    ''' Creates a strut-and-tie model for a pile cap for two piles.

    :ivar pierBottomNode: bottom node of the pier.
    :ivar leftPileTopNode: top node of the left pile.
    :ivar rightPileTopNode: top node of the right pile.
    :ivar pierEffectiveWidth: effective width of the pier.
    '''
    concrete_a= .0001 # parameter to define the tension behaviour of concrete.
    steel_a= .0001 # parameter to define the compression behaviour of steel.
    def __init__(self, pierBottomNode, leftPileTopNode, rightPileTopNode, pierEffectiveWidth):
        ''' Constructor.

        :param pierBottomNode: bottom node of the pier.
        :param leftPileTopNode: top node of the left pile.
        :param rightPileTopNode: top node of the right pile.
        :param pierEffectiveWidth: effective width of the pier.
        '''
        super().__init__()
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
        :param linearElastic: if True, assign a linear elastic material to
                              struts and ties (insteaa of an no-tension or
                              no-compression material). Used with debugging
                              purposes.
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
            lmsg.error(errorMessage)
            
        # Complete the lists of nodes with the existing ones.
        self.topChordNodes.insert(2,self.pierBottomNode)
        self.bottomChordNodes.insert(0, self.leftPileTopNode)
        self.bottomChordNodes.append(self.rightPileTopNode)
        # Define elements.
        ## Define struts.
        ### Define material.
        concreteNoTension= self.getStrutMaterial(modelSpace= modelSpace, concrete= concrete, linearElastic= linearElastic)
        modelSpace.setElementDimension(dim)
        modelSpace.setDefaultMaterial(concreteNoTension)
        self.pileCapStruts= list() # Struts in the pile cap.
        pileCapStrutIndexes= [(0,1), (1,0), (3, 3), (4,2)]
        for (iTop, iBottom) in pileCapStrutIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newStrut= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newStrut.sectionArea= strutArea
            self.pileCapStruts.append(newStrut)
            
        self.pierStruts= list() # Pier struts (dummy struts to make the forces
                                # reach the bottom of the pile cap).
        pierStrutIndexes= [(1, 2), (3,1)]
        for (iTop, iBottom) in pierStrutIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newStrut= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)

        ## Define ties.
        ### Define material.
        steelNoCompression= self.getTieMaterial(modelSpace= modelSpace, reinfSteel= reinfSteel, linearElastic= linearElastic)
        modelSpace.setDefaultMaterial(steelNoCompression)
        modelSpace.setElementDimension(dim)
        ### Define top-down ties.
        self.pileCapTopDownTies= list() # Top-down ties in the pile cap.
        topDownPileCapTieIndexes= [(0,0), (4,3)]
        for (iTop, iBottom) in topDownPileCapTieIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newTie= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newTie.sectionArea= topDownTiesArea
            self.pileCapTopDownTies.append(newTie)
        
        topDownPierTieIndexes= [(1,1), (3, 2)]
        self.pierTopDownTies= list() # Top-down ties in the pier(dummy ties
                                     # to make the forces reach the bottom
                                     # of the pile cap).
        for (iTop, iBottom) in topDownPierTieIndexes:
            topNode= self.topChordNodes[iTop]
            bottomNode= self.bottomChordNodes[iBottom]
            newTie= modelSpace.newElement("Truss", [topNode.tag, bottomNode.tag])
            newTie.sectionArea= topDownTiesArea
            self.pierTopDownTies.append(newTie)

        ### Define top chord ties.
        self.pileCapTopChordTies= list() # Top-down ties in the pile cap.
        topChordPileCapTieIndexes= [(0,1), (1, 3), (3,4)]
        for (i, j) in topChordPileCapTieIndexes:
            iNode= self.topChordNodes[i]
            jNode= self.topChordNodes[j]
            newTie= modelSpace.newElement("Truss", [iNode.tag, jNode.tag])
            newTie.sectionArea= topChordTiesArea
            self.pileCapTopChordTies.append(newTie)

        ### Define bottom chord ties.
        self.pileCapBottomChordTies= list() # Bottom chord ties in the pile cap.
        bottomChordPileCapTieIndexes= [(0,1), (1, 2), (2,3)]
        for (i, j) in bottomChordPileCapTieIndexes:
            iNode= self.bottomChordNodes[i]
            jNode= self.bottomChordNodes[j]
            newTie= modelSpace.newElement("Truss", [iNode.tag, jNode.tag])
            newTie.sectionArea= bottomChordTiesArea
            self.pileCapBottomChordTies.append(newTie)

        ### Define dummy springs (to fix rotational DOFs only).
        nodesToConstrain= [self.topChordNodes[0], self.topChordNodes[-1], self.bottomChordNodes[1], self.bottomChordNodes[2]]
        springsAndNodes= define_dummy_springs(modelSpace, nodesToConstrain)
        ### Connect the pier with the pile cap using the pier section
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
            lmsg.error(errorMessage)
            
    def createDummyElasticModel(self, modelSpace, concrete):
        ''' Creates a simple linear elastic model that transfer the loads from
            the pier to the piles. This model can be used when calculating 
            other parts of the model and the internal forces in the pile cap
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
            lmsg.error(errorMessage)
            
    def getBottomChordTies(self):
        ''' Return a list containing the elements that model the bottom chord
            tie.'''
        return self.pileCapBottomChordTies

    def getStrutList(self):
        ''' Return a list of all the struts.'''
        retval= list()
        if(hasattr(self,'pileCapStruts')): # if strut-and-tie model is defined
            retval= self.pileCapStruts+self.pierStruts
        return retval

    def getTieList(self):
        ''' Return a list of all the ties.'''
        retval= list()
        if(hasattr(self,'pileCapTopDownTies')): # if strut-and-tie model is defined
            retval= self.pileCapTopDownTies+self.pierTopDownTies+self.pileCapTopChordTies+self.pileCapBottomChordTies
        return retval

class PileCap3Piles(StrutAndTieModel):
    ''' Creates a strut-and-tie model for a pile cap for three piles.

    :ivar pierBottomNode: bottom node of the pier.
    :ivar pileTopNodeA: top node of the first pile.
    :ivar pileTopNodeB: top node of the second pile.
    :ivar pileTopNodeC: top node of the third pile.
    :ivar pierEffectiveDiameter: effective diameter of the pier.
    '''
    concrete_a= .0001 # parameter to define the tension behaviour of concrete.
    steel_a= .00001 # parameter to define the compression behaviour of steel.
    
    def __init__(self, pierBottomNode, pileTopNodeA, pileTopNodeB, pileTopNodeC, pierEffectiveDiameter):
        ''' Constructor.

        :param pierBottomNode: bottom node of the pier.
        :param pileTopNodeA: top node of the first pile.
        :param pileTopNodeB: top node of the second pile.
        :param pileTopNodeC: top node of the third pile.
        :param pierEffectiveDiameter: effective diameter of the pier.
        '''
        super().__init__()
        self.pierBottomNode= pierBottomNode
        self.pileTopNodeA= pileTopNodeA
        self.pileTopNodeB= pileTopNodeB
        self.pileTopNodeC= pileTopNodeC
        self.pierEffectiveDiameter= pierEffectiveDiameter
        
    def createStrutAndTieModel(self, modelSpace, strutArea, concrete, pileTiesArea, radialTiesArea, bottomChordsTiesArea, topChordsTiesArea, shearTiesArea, reinfSteel, xcPierSectionMaterial, linearElastic= False):
        ''' Creates an strut-and-tie model and attach it to the nodes of the 
            pier and the piles.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param strutAre: area of the struts.
        :param concrete: concrete material.
        :param pileTiesArea: vertical ties over each pile.
        :param radialTiesArea= area of the radial ties joining the pier and 
                               the piles.
        :param bottomChordTiesArea= area of the ties joining the heads of the
                                    piles. 
        :param topChordTiesArea= area of the ties joining the tips of the 
                                 vertical ties on each pile.
        :param shearTiesArea: area of the vertical ties between piles.
        :param reinfSteel: reinforcing steel material.
        :param xcPierSectionMaterial: XC section material for the pier.
        :param linearElastic: if True, assign a linear elastic material to
                              struts and ties (insteaa of an no-tension or
                              no-compression material). Used with debugging
                              purposes.
        '''
        pierBottomNodePos= self.pierBottomNode.getInitialPos3d
        pileBottomNodePosA= self.pileTopNodeA.getInitialPos3d
        pileBottomNodePosB= self.pileTopNodeB.getInitialPos3d
        pileBottomNodePosC= self.pileTopNodeC.getInitialPos3d
        bottomTriangle= geom.Triangle3d(pileBottomNodePosA, pileBottomNodePosB, pileBottomNodePosC)
        bottomCentroid= bottomTriangle.getCenterOfMass()
        auxLineA= geom.Segment3d(bottomCentroid, pileBottomNodePosA)
        auxLineB= geom.Segment3d(bottomCentroid, pileBottomNodePosB)
        auxLineC= geom.Segment3d(bottomCentroid, pileBottomNodePosC)
        bottomPlane= bottomTriangle.getPlane()
        kVector= bottomPlane.getNormal().normalized()
        # Make sure kVectors points to the bottom of the pier
        pierBottomSide= bottomPlane.getSide(pierBottomNodePos)
        if(pierBottomSide<0):
            kVector*=-1
        d= bottomPlane.dist(pierBottomNodePos) # effective depth.
        # Compute the position of the pile top nodes.
        pileTopNodePosA= pileBottomNodePosA+d*kVector
        pileTopNodePosB= pileBottomNodePosB+d*kVector
        pileTopNodePosC= pileBottomNodePosC+d*kVector
        # Compute the position of the lateral intermediate nodes.
        midBottomNodePosAB= geom.Segment3d(pileBottomNodePosA, pileBottomNodePosB).getMidPoint()
        midTopNodePosAB= geom.Segment3d(pileTopNodePosA, pileTopNodePosB).getMidPoint()
        midBottomNodePosBC= geom.Segment3d(pileBottomNodePosB, pileBottomNodePosC).getMidPoint()
        midTopNodePosBC= geom.Segment3d(pileTopNodePosB, pileTopNodePosC).getMidPoint()
        midBottomNodePosCA= geom.Segment3d(pileBottomNodePosC, pileBottomNodePosA).getMidPoint()
        midTopNodePosCA= geom.Segment3d(pileTopNodePosC, pileTopNodePosA).getMidPoint()
        # Compute the positions of the pier nodes.
        radius= self.pierEffectiveDiameter/2.0
        pierBottomNodePosA= auxLineA.getPoint(radius)
        pierBottomNodePosB= auxLineB.getPoint(radius)
        pierBottomNodePosC= auxLineC.getPoint(radius)
        pierTopNodePosA= pierBottomNodePosA+d*kVector
        pierTopNodePosB= pierBottomNodePosB+d*kVector
        pierTopNodePosC= pierBottomNodePosC+d*kVector

        # Create the pile cap nodes.
        ## Nodes over the piles.
        pileBottomNodeA= self.pileTopNodeA # already exists.
        pileBottomNodeB= self.pileTopNodeB # already exists.
        pileBottomNodeC= self.pileTopNodeC # already exists.
        pileTopNodeA= modelSpace.newNodePos3d(pileTopNodePosA)
        pileTopNodeB= modelSpace.newNodePos3d(pileTopNodePosB)
        pileTopNodeC= modelSpace.newNodePos3d(pileTopNodePosC)
        ## Intermediate nodes.
        midBottomNodeAB= modelSpace.newNodePos3d(midBottomNodePosAB)
        midTopNodeAB= modelSpace.newNodePos3d(midTopNodePosAB)
        midBottomNodeBC= modelSpace.newNodePos3d(midBottomNodePosBC)
        midTopNodeBC= modelSpace.newNodePos3d(midTopNodePosBC)
        midBottomNodeCA= modelSpace.newNodePos3d(midBottomNodePosCA)
        midTopNodeCA= modelSpace.newNodePos3d(midTopNodePosCA)
        ## Nodes under the pier.
        pierBottomNodeA= modelSpace.newNodePos3d(pierBottomNodePosA)
        pierBottomNodeB= modelSpace.newNodePos3d(pierBottomNodePosB)
        pierBottomNodeC= modelSpace.newNodePos3d(pierBottomNodePosC)
        pierTopNodeA= modelSpace.newNodePos3d(pierTopNodePosA)
        pierTopNodeB= modelSpace.newNodePos3d(pierTopNodePosB)
        pierTopNodeC= modelSpace.newNodePos3d(pierTopNodePosC)
        # Define elements.
        ## Define struts.
        ### Define material.
        concreteNoTension= self.getStrutMaterial(modelSpace= modelSpace, concrete= concrete, linearElastic= linearElastic)
        modelSpace.setElementDimension(3)
        modelSpace.setDefaultMaterial(concreteNoTension)
        
        ### Radial struts.
        radialStrutsNodes= [(pierTopNodeA, pileBottomNodeA), (pierBottomNodeA, pileTopNodeA), (pierTopNodeB, pileBottomNodeB), (pierBottomNodeB, pileTopNodeB), (pierTopNodeC, pileBottomNodeC), (pierBottomNodeC, pileTopNodeC)]
        self.radialStruts= list()
        for (n1, n2) in radialStrutsNodes:
            newStrut= modelSpace.newElement("Truss", [n1.tag, n2.tag])
            newStrut.sectionArea= strutArea
            self.radialStruts.append(newStrut)
            
        ### Define struts from the pile to the center of the pile cap sides
        ### (needed to stabilize the top and bottom chords)
        stabilizingStrutsNodes= [(pierTopNodeA, pierBottomNodeA, midBottomNodeAB, midTopNodeAB), (pierTopNodeB, pierBottomNodeB, midBottomNodeAB, midTopNodeAB), (pierTopNodeB, pierBottomNodeB, midBottomNodeBC, midTopNodeBC), (pierTopNodeC, pierBottomNodeC, midBottomNodeBC, midTopNodeBC), (pierTopNodeC, pierBottomNodeC, midBottomNodeCA, midTopNodeCA), (pierTopNodeA, pierBottomNodeA, midBottomNodeCA, midTopNodeCA)]
        self.stabilizingStruts= list()
        for (nTop1, nBottom1, nBottom2, nTop2) in stabilizingStrutsNodes:
            newStrut= modelSpace.newElement("Truss", [nTop1.tag, nBottom2.tag])
            newStrut.sectionArea= 0.25*strutArea
            self.stabilizingStruts.append(newStrut)
            newStrut= modelSpace.newElement("Truss", [nBottom1.tag, nTop2.tag])
            newStrut.sectionArea= 0.25*strutArea
            self.stabilizingStruts.append(newStrut)
        
        ### Struts on the perimeter.
        pilesContourBottomNodes= [pileBottomNodeA, pileBottomNodeB, pileBottomNodeC] 
        pilesContourTopNodes= [pileTopNodeA, pileTopNodeB, pileTopNodeC]
        midContourBottomNodes= [midBottomNodeAB, midBottomNodeBC, midBottomNodeCA] 
        midContourTopNodes= [midTopNodeAB, midTopNodeBC, midTopNodeCA]
        contourBottomNodes= list()
        for pileBottomNode, midBottomNode in zip(pilesContourBottomNodes, midContourBottomNodes):
            contourBottomNodes.append(pileBottomNode)
            contourBottomNodes.append(midBottomNode)
        contourBottomNodes.append(contourBottomNodes[0]) # close "circuit".
        contourTopNodes= list()
        for pileTopNode, midTopNode in zip(pilesContourTopNodes, midContourTopNodes):
            contourTopNodes.append(pileTopNode)
            contourTopNodes.append(midTopNode)
        contourTopNodes.append(contourTopNodes[0]) # close "circuit".
        
        self.perimeterStruts= list()
        n0Bottom= contourBottomNodes[0]
        n0Top= contourTopNodes[0]
        for (n1Bottom, n1Top) in zip(contourBottomNodes[1:], contourTopNodes[1:]):
            newStrut= modelSpace.newElement("Truss", [n0Bottom.tag, n1Top.tag])
            newStrut.sectionArea= strutArea
            self.perimeterStruts.append(newStrut)
            newStrut= modelSpace.newElement("Truss", [n0Top.tag, n1Bottom.tag])
            newStrut.sectionArea= strutArea
            self.perimeterStruts.append(newStrut)
            n0Bottom= n1Bottom
            n0Top= n1Top

        ### Pier struts.
        pierContourBottomNodes= [pierBottomNodeA, pierBottomNodeB, pierBottomNodeC, pierBottomNodeA] 
        pierContourTopNodes= [pierTopNodeA, pierTopNodeB, pierTopNodeC, pierTopNodeA] 
        self.pierStruts= list()
        n0Bottom= pierContourBottomNodes[0]
        n0Top= pierContourTopNodes[0]
        for (n1Bottom, n1Top) in zip(pierContourBottomNodes[1:], pierContourTopNodes[1:]):
            newStrut= modelSpace.newElement("Truss", [n0Bottom.tag, n1Top.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)
            newStrut= modelSpace.newElement("Truss", [n0Top.tag, n1Bottom.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)
            n0Bottom= n1Bottom
            n0Top= n1Top
        
        ## Define ties.
        ### Define material.
        steelNoCompression= self.getTieMaterial(modelSpace= modelSpace, reinfSteel= reinfSteel, linearElastic= linearElastic)
        modelSpace.setDefaultMaterial(steelNoCompression)
        modelSpace.setElementDimension(3)

        ### Define radial ties.
        radialTiesNodes= [(pierTopNodeA, pileTopNodeA), (pierBottomNodeA, pileBottomNodeA), (pierTopNodeB, pileTopNodeB), (pierBottomNodeB, pileBottomNodeB), (pierTopNodeC, pileTopNodeC), (pierBottomNodeC, pileBottomNodeC)]
        self.radialTies= list()
        for (n1, n2) in radialTiesNodes:
            newTie= modelSpace.newElement("Truss", [n1.tag, n2.tag])
            newTie.sectionArea= radialTiesArea
            self.radialTies.append(newTie)

        ### Define ties from the pile to the center of the pile cap sides
        ### (needed to stabilize the top and bottom chords)
        stabilizingTiesNodes= [(pierTopNodeA, pierTopNodeB, midTopNodeAB), (pierBottomNodeA, pierBottomNodeB, midBottomNodeAB), (pierTopNodeB, pierTopNodeC, midTopNodeBC), (pierBottomNodeB, pierBottomNodeC, midBottomNodeBC), (pierTopNodeC, pierTopNodeA, midTopNodeCA), (pierBottomNodeC, pierBottomNodeA, midBottomNodeCA)]
        self.stabilizingTies= list()
        for (n1, n2, n3) in stabilizingTiesNodes:
            newTie= modelSpace.newElement("Truss", [n1.tag, n3.tag])
            newTie.sectionArea= radialTiesArea
            self.stabilizingTies.append(newTie)
            newTie= modelSpace.newElement("Truss", [n2.tag, n3.tag])
            newTie.sectionArea= radialTiesArea
            self.stabilizingTies.append(newTie)

        ### Define ties on the perimeter.
        #### Top chord.
        self.topChordTies= list()
        n0Top= contourTopNodes[0]
        for n1Top in contourTopNodes[1:]:
            newTie= modelSpace.newElement("Truss", [n0Top.tag, n1Top.tag])
            newTie.sectionArea= topChordsTiesArea
            self.topChordTies.append(newTie)
            n0Top= n1Top
        self.bottomChordTies= list()
        #### Bottom chord.
        n0Bottom= contourBottomNodes[0]
        for n1Bottom in contourBottomNodes[1:]:
            newTie= modelSpace.newElement("Truss", [n0Bottom.tag, n1Bottom.tag])
            newTie.sectionArea= bottomChordsTiesArea
            self.bottomChordTies.append(newTie)
            n0Bottom= n1Bottom
        #### Vertical ties over each pile.
        self.pileTies= list()
        for (nBottom, nTop) in zip(pilesContourBottomNodes, pilesContourTopNodes):
            newTie= modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= pileTiesArea
            self.pileTies.append(newTie)
        #### Shear ties between piles.
        self.shearTies= list()
        for (nBottom, nTop) in zip(midContourBottomNodes, midContourTopNodes):
            newTie= modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= shearTiesArea
            self.shearTies.append(newTie)

        ### Define pier ties.
        #### Top chord.
        self.pierTopChordTies= list()
        n0Top= pierContourTopNodes[0]
        for n1Top in pierContourTopNodes[1:]:
            newTie= modelSpace.newElement("Truss", [n0Top.tag, n1Top.tag])
            newTie.sectionArea= topChordsTiesArea
            self.pierTopChordTies.append(newTie)
            n0Top= n1Top
        #### Bottom chord.
        self.pierBottomChordTies= list()
        n0Bottom= pierContourBottomNodes[0]
        for n1Bottom in pierContourBottomNodes[1:]:
            newTie= modelSpace.newElement("Truss", [n0Bottom.tag, n1Bottom.tag])
            newTie.sectionArea= bottomChordsTiesArea
            self.pierBottomChordTies.append(newTie)
            n0Bottom= n1Bottom

        #### Top to bottom bars (they can take tensions and compressions).
        # Top-down ties in the pier(dummy bars to make the forces reach the
        # bottom of the pile cap).
        pierTopDownBarsMaterial= self.reinfSteel.defElasticMaterial(preprocessor= modelSpace.preprocessor)
        modelSpace.setDefaultMaterial(pierTopDownBarsMaterial)
        pierTopDownBarsArea= pileTiesArea # XXX 
        self.pierTopDownBars= list()
        for (nBottom, nTop) in zip(pierContourBottomNodes[:-1], pierContourTopNodes[:-1]):
            newTie= modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= pierTopDownBarsArea
            self.pierTopDownBars.append(newStrut)
        
        ### Define dummy springs (to fix rotational DOFs only).
        nodesToConstrain= contourTopNodes+midContourBottomNodes+pierContourBottomNodes
        springsAndNodes= define_dummy_springs(modelSpace, nodesToConstrain)
        
        ### Connect the pier with the pile cap using the pier section
        ### as "rigid beam".
        modelSpace.setDefaultMaterial(xcPierSectionMaterial)
        n0Tag= self.pierBottomNode.tag
        n1Tag= pierContourTopNodes[1].tag
        n2Tag= pierContourTopNodes[2].tag
        n3Tag= pierContourTopNodes[3].tag
        crdTransfName= str(uuid.uuid4().hex)
        xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
        crdTransf= modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
        modelSpace.setDefaultCoordTransf(crdTransf)
        modelSpace.newElement('ElasticBeam3d', [n0Tag, n1Tag])
        modelSpace.newElement('ElasticBeam3d', [n0Tag, n2Tag])
        modelSpace.newElement('ElasticBeam3d', [n0Tag, n3Tag])
        
    def createDummyElasticModel(self, modelSpace, concrete, releaseExtremities= False):
        ''' Creates a simple linear elastic model that transfer the loads from
            the pier to the piles. This model can be used when calculating 
            other parts of the model and the internal forces in the pile cap
            are irrelevant for the purpose of the performed analysis.

        :param modelSpace: PredefinedSpace object used to create the FE model.
        :param releaseExtremities: if true, release the bending moments at
                                   the nodes connecting with the piles.
        '''
        pierBottomNodePos= self.pierBottomNode.getInitialPos3d
        pileBottomNodePosA= self.pileTopNodeA.getInitialPos3d
        pileBottomNodePosB= self.pileTopNodeB.getInitialPos3d
        pileBottomNodePosC= self.pileTopNodeC.getInitialPos3d
        bottomTriangle= geom.Triangle3d(pileBottomNodePosA, pileBottomNodePosB, pileBottomNodePosC)
        bottomCentroid= bottomTriangle.getCenterOfMass()
        auxLineA= geom.Segment3d(bottomCentroid, pileBottomNodePosA)
        auxLineB= geom.Segment3d(bottomCentroid, pileBottomNodePosB)
        auxLineC= geom.Segment3d(bottomCentroid, pileBottomNodePosC)
        bottomPlane= bottomTriangle.getPlane()
        kVector= bottomPlane.getNormal().normalized()
        d= bottomPlane.dist(pierBottomNodePos) # effective depth.
        
        n0Tag= self.pierBottomNode.tag
        nATag= self.pileTopNodeA.tag
        nBTag= self.pileTopNodeB.tag
        nCTag= self.pileTopNodeC.tag
        # Define material.
        dummyRCSectionName= str(uuid.uuid4().hex)
        dummyRCSection= def_simple_RC_section.RCRectangularSection(name= dummyRCSectionName, sectionDescr= 'dummy RC section', concrType= concrete, reinfSteelType= None, width= d, depth= d)
        xcDummySectionMaterial= dummyRCSection.defElasticShearSection3d(preprocessor= modelSpace.preprocessor)

        # Compute date for the coordinate transformations.
        auxPoint= pierBottomNodePos-d*kVector # Point in the vertical of the
                                              # pile.
        planeA= geom.Plane3d(pierBottomNodePos, auxPoint, pileBottomNodePosA)
        kVectorA= planeA.getNormal()
        planeB= geom.Plane3d(pierBottomNodePos, auxPoint, pileBottomNodePosB)
        kVectorB= planeB.getNormal()
        planeC= geom.Plane3d(pierBottomNodePos, auxPoint, pileBottomNodePosC)
        kVectorC= planeC.getNormal()
        # Set element material.
        modelSpace.setDefaultMaterial(xcDummySectionMaterial)
        for nTag, kVector in zip([nATag, nBTag, nCTag], [kVectorA, kVectorB, kVectorC]):
            # Set coordinate transformation.
            xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
            crdTransfName= str(uuid.uuid4().hex)
            crdTransf= modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
            modelSpace.setDefaultCoordTransf(crdTransf)
            # Create dummy element.
            newElement= modelSpace.newElement('ElasticBeam3d', [n0Tag, nTag])
            if(releaseExtremities):
                # Release code; 0: no release, 1: node I, 2: node J, 2: both nodes.
                # newElement.releaseY= 2
                newElement.releaseZ= 2
        
    def getStrutList(self):
        ''' Return a list of all the struts.'''
        retval= list()
        if(hasattr(self,'radialStruts')): # if strut-and-tie model is defined
            retval= self.radialStruts+self.stabilizingStruts+self.perimeterStruts+self.pierStruts
        return retval

    def getTieList(self):
        ''' Return a list of all the ties.'''
        retval= list()
        if(hasattr(self,'radialTies')): # if strut-and-tie model is defined
            retval= self.radialTies+self.stabilizingTies+self.topChordTies+self.bottomChordTies+self.pileTies+self.shearTies+self.pierTopChordTies+self.pierBottomChordTies
        return retval

        
        
