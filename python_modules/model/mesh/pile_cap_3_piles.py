# -*- coding: utf-8 -*-
''' Utilities for the generation of two pile pile caps.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import geom
import uuid
import xc
from materials.sections.fiber_section import def_simple_RC_section
from model.mesh import strut_and_tie_base
from misc_utils import log_messages as lmsg

class PileCap3Piles(strut_and_tie_base.StrutAndTieModel):
    ''' Creates a strut-and-tie model for a pile cap for three piles.

    :ivar pierBottomNode: bottom node of the pier.
    :ivar pileTopNodeA: top node of the first pile.
    :ivar pileTopNodeB: top node of the second pile.
    :ivar pileTopNodeC: top node of the third pile.
    :ivar pierEffectiveDiameter: effective diameter of the pier.
    '''    
    def __init__(self, modelSpace, pierBottomNode, pileTopNodeA, pileTopNodeB, pileTopNodeC, pierEffectiveDiameter):
        ''' Constructor.

        :param modelSpace: PredefinedSpace object that manages FE model.
        :param pierBottomNode: bottom node of the pier.
        :param pileTopNodeA: top node of the first pile.
        :param pileTopNodeB: top node of the second pile.
        :param pileTopNodeC: top node of the third pile.
        :param pierEffectiveDiameter: effective diameter of the pier.
        '''
        super().__init__(modelSpace)
        self.pierBottomNode= pierBottomNode
        self.pileTopNodeA= pileTopNodeA
        self.pileTopNodeB= pileTopNodeB
        self.pileTopNodeC= pileTopNodeC
        self.pierEffectiveDiameter= pierEffectiveDiameter
    
    def createStrutAndTieModel(self, strutArea, concrete, pileTiesArea, radialTiesArea, bottomChordsTiesArea, topChordsTiesArea, shearTiesArea, reinfSteel, xcPierSectionMaterial, linearElastic= False):
        ''' Creates an strut-and-tie model and attach it to the nodes of the 
            pier and the piles.

        :param strutArea: area of the struts.
        :param concrete: concrete material.
        :param pileTiesArea: vertical ties over each pile.
        :param radialTiesArea: area of the radial ties joining the pier and 
                               the piles.
        :param bottomChordTiesArea: area of the ties joining the heads of the
                                    piles. 
        :param topChordTiesArea: area of the ties joining the tips of the 
                                 vertical ties on each pile.
        :param shearTiesArea: area of the vertical ties between piles.
        :param reinfSteel: reinforcing steel material.
        :param xcPierSectionMaterial: XC section material for the pier.
        :param linearElastic: if True, assign a linear elastic material to
                              struts and ties (insteaa of an no-tension or
                              no-compression material). Used with debugging
                              purposes.
        '''
        # Pier bottom node at the top of the pile cap:
        pierTopNodePos= self.pierBottomNode.getInitialPos3d
        # Pile top nodes at the bottom of the pile cap.
        pileBottomNodePosA= self.pileTopNodeA.getInitialPos3d
        pileBottomNodePosB= self.pileTopNodeB.getInitialPos3d
        pileBottomNodePosC= self.pileTopNodeC.getInitialPos3d
        bottomTriangle= geom.Triangle3d(pileBottomNodePosA, pileBottomNodePosB, pileBottomNodePosC)
        # Compute effective depth
        bottomPlane= bottomTriangle.getPlane()
        kVector= bottomPlane.getNormal().normalized()
        ## Make sure kVectors points to the bottom of the pier
        pierBottomSide= bottomPlane.getSide(pierTopNodePos)
        if(pierBottomSide<0):
            kVector*=-1
        d= bottomPlane.dist(pierTopNodePos) # effective depth.
        # Compute auxiliary lines.
        pierBottomPos= pierTopNodePos-d*kVector
        auxLineA= geom.Segment3d(pierBottomPos, pileBottomNodePosA)
        auxLineB= geom.Segment3d(pierBottomPos, pileBottomNodePosB)
        auxLineC= geom.Segment3d(pierBottomPos, pileBottomNodePosC)
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
        pileBottomNodeA= self.pileTopNodeA # already exists (no need to create it).
        pileBottomNodeB= self.pileTopNodeB # already exists (no need to create it).
        pileBottomNodeC= self.pileTopNodeC # already exists (no need to create it).
        pileTopNodeA= self.modelSpace.newNodePos3d(pileTopNodePosA)
        pileTopNodeB= self.modelSpace.newNodePos3d(pileTopNodePosB)
        pileTopNodeC= self.modelSpace.newNodePos3d(pileTopNodePosC)
        ## Intermediate nodes.
        midBottomNodeAB= self.modelSpace.newNodePos3d(midBottomNodePosAB)
        midTopNodeAB= self.modelSpace.newNodePos3d(midTopNodePosAB)
        midBottomNodeBC= self.modelSpace.newNodePos3d(midBottomNodePosBC)
        midTopNodeBC= self.modelSpace.newNodePos3d(midTopNodePosBC)
        midBottomNodeCA= self.modelSpace.newNodePos3d(midBottomNodePosCA)
        midTopNodeCA= self.modelSpace.newNodePos3d(midTopNodePosCA)
        ## Nodes under the pier.
        pierBottomNodeA= self.modelSpace.newNodePos3d(pierBottomNodePosA)
        pierBottomNodeB= self.modelSpace.newNodePos3d(pierBottomNodePosB)
        pierBottomNodeC= self.modelSpace.newNodePos3d(pierBottomNodePosC)
        pierTopNodeA= self.modelSpace.newNodePos3d(pierTopNodePosA)
        pierTopNodeB= self.modelSpace.newNodePos3d(pierTopNodePosB)
        pierTopNodeC= self.modelSpace.newNodePos3d(pierTopNodePosC)
        # Define elements.
        ## Define struts.
        ### Define material.
        concreteNoTension= self.defStrutMaterial(concrete= concrete, linearElastic= linearElastic)
        self.modelSpace.setElementDimension(3)
        self.modelSpace.setDefaultMaterial(concreteNoTension)
        
        ### Radial struts.
        radialStrutsNodes= [(pierTopNodeA, pileBottomNodeA), (pierBottomNodeA, pileTopNodeA), (pierTopNodeB, pileBottomNodeB), (pierBottomNodeB, pileTopNodeB), (pierTopNodeC, pileBottomNodeC), (pierBottomNodeC, pileTopNodeC)]
        self.radialStruts= list()
        for (n1, n2) in radialStrutsNodes:
            newStrut= self.modelSpace.newElement("Truss", [n1.tag, n2.tag])
            newStrut.sectionArea= strutArea
            self.radialStruts.append(newStrut)
            
        ### Define struts from the pile to the center of the pile cap sides
        ### (needed to stabilize the top and bottom chords)
        stabilizingStrutsNodes= [(pierTopNodeA, pierBottomNodeA, midBottomNodeAB, midTopNodeAB), (pierTopNodeB, pierBottomNodeB, midBottomNodeAB, midTopNodeAB), (pierTopNodeB, pierBottomNodeB, midBottomNodeBC, midTopNodeBC), (pierTopNodeC, pierBottomNodeC, midBottomNodeBC, midTopNodeBC), (pierTopNodeC, pierBottomNodeC, midBottomNodeCA, midTopNodeCA), (pierTopNodeA, pierBottomNodeA, midBottomNodeCA, midTopNodeCA)]
        self.stabilizingStruts= list()
        for (nTop1, nBottom1, nBottom2, nTop2) in stabilizingStrutsNodes:
            newStrut= self.modelSpace.newElement("Truss", [nTop1.tag, nBottom2.tag])
            newStrut.sectionArea= 0.25*strutArea
            self.stabilizingStruts.append(newStrut)
            newStrut= self.modelSpace.newElement("Truss", [nBottom1.tag, nTop2.tag])
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
            newStrut= self.modelSpace.newElement("Truss", [n0Bottom.tag, n1Top.tag])
            newStrut.sectionArea= strutArea
            self.perimeterStruts.append(newStrut)
            newStrut= self.modelSpace.newElement("Truss", [n0Top.tag, n1Bottom.tag])
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
            newStrut= self.modelSpace.newElement("Truss", [n0Bottom.tag, n1Top.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)
            newStrut= self.modelSpace.newElement("Truss", [n0Top.tag, n1Bottom.tag])
            newStrut.sectionArea= strutArea
            self.pierStruts.append(newStrut)
            n0Bottom= n1Bottom
            n0Top= n1Top
        
        ## Define ties.
        ### Define material.
        steelNoCompression= self.defTieMaterial(reinfSteel= reinfSteel, linearElastic= linearElastic)
        #### Material for elastic ties.
        if(not linearElastic):
            elasticTiesMaterial= self.reinfSteel.defElasticMaterial(preprocessor= self.modelSpace.preprocessor)
        else:
            elasticTiesMaterial= steelNoCompression
            
        self.modelSpace.setDefaultMaterial(steelNoCompression)
        self.modelSpace.setElementDimension(3)

        ### Define radial ties.
        radialTiesNodes= [(pierTopNodeA, pileTopNodeA), (pierBottomNodeA, pileBottomNodeA), (pierTopNodeB, pileTopNodeB), (pierBottomNodeB, pileBottomNodeB), (pierTopNodeC, pileTopNodeC), (pierBottomNodeC, pileBottomNodeC)]
        self.radialTies= list()
        for (n1, n2) in radialTiesNodes:
            newTie= self.modelSpace.newElement("Truss", [n1.tag, n2.tag])
            newTie.sectionArea= radialTiesArea
            self.radialTies.append(newTie)

        ### Define ties from the pile to the center of the pile cap sides
        ### (needed to stabilize the top and bottom chords)
        stabilizingTiesNodes= [(pierTopNodeA, pierTopNodeB, midTopNodeAB), (pierBottomNodeA, pierBottomNodeB, midBottomNodeAB), (pierTopNodeB, pierTopNodeC, midTopNodeBC), (pierBottomNodeB, pierBottomNodeC, midBottomNodeBC), (pierTopNodeC, pierTopNodeA, midTopNodeCA), (pierBottomNodeC, pierBottomNodeA, midBottomNodeCA)]
        self.stabilizingTies= list()
        for (n1, n2, n3) in stabilizingTiesNodes:
            newTie= self.modelSpace.newElement("Truss", [n1.tag, n3.tag])
            newTie.sectionArea= radialTiesArea
            self.stabilizingTies.append(newTie)
            newTie= self.modelSpace.newElement("Truss", [n2.tag, n3.tag])
            newTie.sectionArea= radialTiesArea
            self.stabilizingTies.append(newTie)

        ### Define ties on the perimeter.
        #### Top chord.
        self.topChordTies= list()
        n0Top= contourTopNodes[0]
        for n1Top in contourTopNodes[1:]:
            newTie= self.modelSpace.newElement("Truss", [n0Top.tag, n1Top.tag])
            newTie.sectionArea= topChordsTiesArea
            self.topChordTies.append(newTie)
            n0Top= n1Top
        self.bottomChordTies= list()
        #### Bottom chord.
        n0Bottom= contourBottomNodes[0]
        for n1Bottom in contourBottomNodes[1:]:
            newTie= self.modelSpace.newElement("Truss", [n0Bottom.tag, n1Bottom.tag])
            newTie.sectionArea= bottomChordsTiesArea
            self.bottomChordTies.append(newTie)
            n0Bottom= n1Bottom
        #### Vertical ties over each pile.
        # Make this particular ties elastic anyway.
        self.modelSpace.setDefaultMaterial(elasticTiesMaterial)
        self.pileTies= list()
        for (nBottom, nTop) in zip(pilesContourBottomNodes, pilesContourTopNodes):
            newTie= self.modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= pileTiesArea
            self.pileTies.append(newTie)
        self.modelSpace.setDefaultMaterial(steelNoCompression) # Restore material.
        
        #### Shear ties between piles.
        self.shearTies= list()
        for (nBottom, nTop) in zip(midContourBottomNodes, midContourTopNodes):
            newTie= self.modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= shearTiesArea
            self.shearTies.append(newTie)

        ### Define pier ties.
        #### Top chord.
        self.pierTopChordTies= list()
        n0Top= pierContourTopNodes[0]
        for n1Top in pierContourTopNodes[1:]:
            newTie= self.modelSpace.newElement("Truss", [n0Top.tag, n1Top.tag])
            newTie.sectionArea= topChordsTiesArea
            self.pierTopChordTies.append(newTie)
            n0Top= n1Top
        #### Bottom chord.
        self.pierBottomChordTies= list()
        n0Bottom= pierContourBottomNodes[0]
        for n1Bottom in pierContourBottomNodes[1:]:
            newTie= self.modelSpace.newElement("Truss", [n0Bottom.tag, n1Bottom.tag])
            newTie.sectionArea= bottomChordsTiesArea
            self.pierBottomChordTies.append(newTie)
            n0Bottom= n1Bottom

        #### Top to bottom bars (they can take tensions and compressions).
        # Top-down ties in the pier (dummy bars to make the forces reach the
        # bottom of the pile cap).
        pierTopDownBarsMaterial= elasticTiesMaterial
        self.modelSpace.setDefaultMaterial(pierTopDownBarsMaterial)
        pierTopDownBarsArea= pileTiesArea 
        self.pierTopDownBars= list()
        for (nBottom, nTop) in zip(pierContourBottomNodes[:-1], pierContourTopNodes[:-1]):
            newTie= self.modelSpace.newElement("Truss", [nBottom.tag, nTop.tag])
            newTie.sectionArea= pierTopDownBarsArea
            self.pierTopDownBars.append(newStrut)
        
        ### Define dummy springs (to fix rotational DOFs only).
        nodesToConstrain= contourTopNodes+midContourBottomNodes+pierContourBottomNodes
        springsAndNodes= strut_and_tie_base.define_dummy_springs(self.modelSpace, nodesToConstrain)

        ### Connect the pier with the pile cap using the pier section
        ### as "rigid beam".
        self.modelSpace.setDefaultMaterial(xcPierSectionMaterial)
        n0Tag= self.pierBottomNode.tag
        n1Tag= pierContourTopNodes[1].tag
        n2Tag= pierContourTopNodes[2].tag
        n3Tag= pierContourTopNodes[3].tag
        crdTransfName= str(uuid.uuid4().hex)
        xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
        crdTransf= self.modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
        self.modelSpace.setDefaultCoordTransf(crdTransf)
        self.modelSpace.newElement('ElasticBeam3d', [n0Tag, n1Tag])
        self.modelSpace.newElement('ElasticBeam3d', [n0Tag, n2Tag])
        self.modelSpace.newElement('ElasticBeam3d', [n0Tag, n3Tag])
        
    def createDummyElasticModel(self, concrete, releaseExtremities= False):
        ''' Creates a simple linear elastic model that transfer the loads from
            the pier to the piles. This model can be used when calculating 
            other parts of the model and the internal forces in the pile cap
            are irrelevant for the purpose of the performed analysis.

        :param concrete: concrete material.
        :param releaseExtremities: if true, release the bending moments at
                                   the nodes connecting with the piles.
        '''
        # Pier bottom node at the top of the pile cap:
        pierTopNodePos= self.pierBottomNode.getInitialPos3d
        # Pile top nodes at the bottom of the pile cap.
        pileBottomNodePosA= self.pileTopNodeA.getInitialPos3d
        pileBottomNodePosB= self.pileTopNodeB.getInitialPos3d
        pileBottomNodePosC= self.pileTopNodeC.getInitialPos3d
        bottomTriangle= geom.Triangle3d(pileBottomNodePosA, pileBottomNodePosB, pileBottomNodePosC)
        bottomPlane= bottomTriangle.getPlane()
        kVector= bottomPlane.getNormal().normalized()
        d= bottomPlane.dist(pierTopNodePos) # effective depth.
        
        n0Tag= self.pierBottomNode.tag
        nATag= self.pileTopNodeA.tag
        nBTag= self.pileTopNodeB.tag
        nCTag= self.pileTopNodeC.tag
        # Define material.
        dummyRCSectionName= str(uuid.uuid4().hex)
        dummyRCSection= def_simple_RC_section.RCRectangularSection(name= dummyRCSectionName, sectionDescr= 'dummy RC section', concrType= concrete, reinfSteelType= None, width= d, depth= d)
        xcDummySectionMaterial= dummyRCSection.defElasticShearSection3d(preprocessor= self.modelSpace.preprocessor)

        # Compute data for the coordinate transformations.
        auxPoint= pierTopNodePos-d*kVector # Point in the vertical of the
                                           # pile.
        planeA= geom.Plane3d(pierTopNodePos, auxPoint, pileBottomNodePosA)
        kVectorA= planeA.getNormal()
        planeB= geom.Plane3d(pierTopNodePos, auxPoint, pileBottomNodePosB)
        kVectorB= planeB.getNormal()
        planeC= geom.Plane3d(pierTopNodePos, auxPoint, pileBottomNodePosC)
        kVectorC= planeC.getNormal()
        # Set element material.
        self.modelSpace.setDefaultMaterial(xcDummySectionMaterial)
        for nTag, kVector in zip([nATag, nBTag, nCTag], [kVectorA, kVectorB, kVectorC]):
            # Set coordinate transformation.
            xzVector= xc.Vector([kVector.x, kVector.y, kVector.z])
            crdTransfName= str(uuid.uuid4().hex)
            crdTransf= self.modelSpace.newLinearCrdTransf(crdTransfName, xzVector= xzVector)
            self.modelSpace.setDefaultCoordTransf(crdTransf)
            # Create dummy element.
            newElement= self.modelSpace.newElement('ElasticBeam3d', [n0Tag, nTag])
            if(releaseExtremities):
                # Release code; 0: no release, 1: node I, 2: node J, 2: both nodes.
                # newElement.releaseY= 2
                newElement.releaseZ= 2
                
    def getRadialStrutList(self):
        ''' Return the struts that connect the pier and the piles.

        '''
        retval= list()
        if(hasattr(self,'radialStruts')): # if strut-and-tie model is defined
            retval= self.radialStruts
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMessage= className+'.'+methodName+"; no strut-and-tie model defined."
            lmsg.error(errorMessage)
        return retval
        
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
