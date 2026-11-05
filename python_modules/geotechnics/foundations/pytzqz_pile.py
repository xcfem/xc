# -*- coding: utf-8 -*-
''' Base classes for pile foundations that use the PySimple1, TzSimple1 and 
QzSimple1 materials to represent the soil-structure interaction.

'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import math
import xc
from geotechnics.foundations import pile
from materials.sections import section_properties as sp
from materials import soil_structure_interaction as ssi
from model import predefined_spaces
from misc_utils import log_messages as lmsg

        
class PyTzQzPile(pile.Pile):
    '''Pile with soil structure interaction represented by PySimple1, TzSimple1
       and QzSimple1 materials.
    
    '''
    def __init__(self, E, crossSection, pileType, soilLayers, pileSet= None, rho= 2.5e3, drilledShaft= True):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param crossSection: cross-section of the pile.
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param soilLayers:  properties of the soil horizons.
        :param pileSet: set of nodes and elements defining a single pile.
        :param rho: material density.
        :param drilledShaft: true when the pile is drilled instead of driven.
        '''
        super().__init__(E= E, crossSection= crossSection, pileType= pileType, soilLayers= soilLayers, pileSet= pileSet, rho= rho, drilledShaft= drilledShaft)
     
    def getDepth(self, z):
        ''' Return the depth corresponding to the given z coordinate.'''
        retval= None
        if(self.soilLayers):
            retval= self.soilLayers.getDepth(z)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getPyParametersAtDepth(self, depth, tributaryLength):
        '''  Compute the ultimate resistance (pult) according to API 
            recommendations and the displacement at 50% lateral capacity (y50).

        :param depth: depth of the poing of interest (m).
        :param tributaryLength: tributary length.
        '''
        retval= None
        if(self.soilLayers):
            pileDiameter= self.getPileEquivalentDiameter()
            pult, y50= self.soilLayers.getPyParametersAtDepth(depth, pileDiameter, tributaryLength)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return pult, y50

    def getTzParametersAtDepth(self, depth, tributaryLength, verticalStressOnSurface= 0.0):
        '''  Return the ultimate resistance, tult, and displacement at 50% 
            mobilization of tult, z50, for use in t-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param tributaryLength: tributary length.
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        retval= None
        if(self.soilLayers):
            pileDiameter= self.getPileEquivalentDiameter()
            tult, z50= self.soilLayers.getTzParametersAtDepth(depth, pileDiameter, tributaryLength, verticalStressOnSurface, drilledShaft= self.drilledShaft)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return tult, z50
    
    def getQzParametersAtDepth(self, depth, verticalStressOnSurface= 0.0):
        '''  Return the ultimate resistance, qult, and displacement at 50% 
            mobilization of qult, z50, for use in q-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        retval= None
        if(self.soilLayers):
            pileDiameter= self.getPileEquivalentDiameter()
            qult, z50= self.soilLayers.getQzParametersAtDepth(depth, pileDiameter, verticalStressOnSurface, drilledShaft= self.drilledShaft)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return qult, z50
    
    def getSoilTypeAtDepth(self, depth):
        ''' Return the soil type corresponding to the given depth.

        -soilType= 1 Backbone of p-y curve approximates Matlock (1970) soft 
                   clay relation.

        -soilType= 2 Backbone of p-y curve approximates API (1993) 
                   sand relation.

        :param depth: depth of the desired soil.
        '''
        retval= None
        if(self.soilLayers):
            retval= self.soilLayers.getSoilTypeAtDepth(depth)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getViscousDampingTermAtDepth(self, depth):
        ''' Return the viscous damping term (dashpot).

        :param depth: depth of the point of interest.
        '''
        retval= 0.0
        if(self.soilLayers):
            retval= self.soilLayers.getViscousDampingTermAtDepth(depth)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getDragResistanceCoefficientAtDepth(self, depth):
        ''' Return the coefficient to compute the drag resistance within a 
            fully-mobilized gap as Cd*pult.

        :param depth: depth of the point of interest.
        '''
        retval= 0.0
        if(self.soilLayers):
            retval= self.soilLayers.getDragResistanceCoefficientAtDepth(depth)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getVerticalStressAtDepth(self, depth, verticalStressOnSurface= 0.0):
        ''' Return the vertical stress in the soil at the given depth.

        :param depth: depth of the poing of interest (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        retval= 0.0
        if(self.soilLayers):
            retval= self.soilLayers.getVerticalStressAtDepth(depth, verticalStressOnSurface)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval
    
    def getSuctionAtDepth(self, depth):
        ''' Return the suction at pile tip during tension/uplift.

        :param depth: depth of the poing of interest (m).
        '''
        retval= 0.0
        if(self.soilLayers):
            retval= self.soilLayers.getSuctionAtDepth(depth)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getPileTipNode2D(self):
        '''Return the node at the tip of the pile.'''
        retval= None
        nodeList= list(self.pileSet.nodes)
        if(nodeList):
            retval= nodeList[0]
            yMin= retval.getInitialPos3d.y
            for n in nodeList[1:]:
                nY= n.getInitialPos3d.y
                if(nY<yMin):
                    yMin= nY
                    retval= n
        return retval

    def getPileTipNode3D(self):
        '''Return the node at the tip of the pile.'''
        retval= None
        nodeList= list(self.pileSet.nodes)
        if(nodeList):
            retval= nodeList[0]
            zMin= retval.getInitialPos3d.z
            for n in nodeList[1:]:
                nZ= n.getInitialPos3d.z
                if(nZ<zMin):
                    zMin= nZ
                    retval= n
        return retval
    
    def generateSpringsPile2D(self, verticalStressOnSurface= 0.0):
        '''Generate the springs that simulate the soils along the pile in a 2D
           problem.

        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        self.springs= list()
        # Get FE model pre-processor.
        preprocessor= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(preprocessor)
        modelSpace.setElementDimension(2)
        # Compute tributary lengths for each node.
        self.computeTributaryLengths(initialGeometry= True) # Tributary lengths.
        for n in self.pileSet.nodes:
            nPos= n.getInitialPos3d
            nodeDepth= self.getDepth(nPos.y)
            tributaryLength= self.tributaryLengths[n.tag]
            # Create P-Y material.
            pult, y50= self.getPyParametersAtDepth(depth= nodeDepth, tributaryLength= tributaryLength)
            soilType= self.getSoilTypeAtDepth(depth= nodeDepth)
            dashpotC= self.getViscousDampingTermAtDepth(depth= nodeDepth)
            Cd= self.getDragResistanceCoefficientAtDepth(depth= nodeDepth)
            pyMatName= 'pyS_'+str(n.tag)
            pyMat= ssi.def_pysimple1_material(preprocessor,
                                              matName= pyMatName,
                                              soilType= soilType,
                                              pult= pult,
                                              Y50= y50,
                                              c= dashpotC,
                                              Cd= Cd)
            # Create T-Z material.
            tult, z50= self.getTzParametersAtDepth(depth= nodeDepth, tributaryLength= tributaryLength, verticalStressOnSurface= verticalStressOnSurface)
            tzMatName= 'tzS:'+str(n.tag)
            tzMat= ssi.def_tzsimple1_material(preprocessor,
                                              matName= tzMatName,
                                              tzType= soilType, # Mosher relation.
                                              tult= tult, # Ultimate skin friction capacity.
                                              z50= z50, # 50% capacity reached.
                                              c= dashpotC)
            # Create spring.
            fixNode= modelSpace.duplicateNode(n)
            modelSpace.fixNode('000', fixNode.tag)
            modelSpace.setDefaultMaterial(pyMat)
            pileShaftSpring= modelSpace.newElement("ZeroLength", [fixNode.tag, n.tag])
            pileShaftSpring.clearMaterials()
            pileShaftSpring.setMaterial(0, pyMat.name)
            pileShaftSpring.setMaterial(1, tzMat.name)
            self.springs.append(pileShaftSpring)
            # Remove materials pyMat and tZMat materials that are no longer
            # needed.
            modelSpace.removeMaterial(pyMat)
            modelSpace.removeMaterial(tzMat)

        # Create spring at the pile tip.
        tipNode= self.getPileTipNode2D()
        ### Compute tip node depth.
        tipNodePos= tipNode.getInitialPos3d
        tipNodeDepth= self.getDepth(tipNodePos.y)
        ### vertical effective stress at pile tip.
        qult, z50q= self.getQzParametersAtDepth(depth= tipNodeDepth, verticalStressOnSurface= verticalStressOnSurface)
        qzSoilType= self.getSoilTypeAtDepth(depth= nodeDepth)
        qzDashpotC= self.getViscousDampingTermAtDepth(depth= nodeDepth)
        qzSuction= self.getSuctionAtDepth(depth= nodeDepth)
        qzMatName= 'qzS:'+str(tipNode.tag)
        qzMat= ssi.def_qzsimple1_material(preprocessor,
                                  matName= qzMatName,
                                  qzType= qzSoilType, # soil type.
                                  qult= qult, # Ultimate tip capacity.
                                  z50= z50q, # 50% capacity reached.
                                  suction= qzSuction, # suction during tension/uplift.
                                  c= qzDashpotC) # viscous damping.
        
        pileTipFixNode= modelSpace.duplicateNode(tipNode)
        modelSpace.fixNode('000', pileTipFixNode.tag)
        modelSpace.setDefaultMaterial(qzMat)
        modelSpace.setElementDimension(2)
        pileTipSpring= modelSpace.newElement("ZeroLength", [pileTipFixNode.tag, tipNode.tag])
        # Vertical direction.
        pileTipSpring.setupVectors(xc.Vector([0,1,0]),xc.Vector([-1,0,0]))
        pileTipSpring.clearMaterials()
        pileTipSpring.setMaterial(0, qzMat.name)
        # Remove material that is no longer needed.
        modelSpace.removeMaterial(qzMat)
        return self.springs
    
    def generateSpringsPile3D(self, verticalStressOnSurface= 0.0):
        '''Generate the springs that simulate the soils along the pile in a 3D
           problem.

        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        self.springs= list()
        # Get FE model pre-processor.
        preprocessor= self.pileSet.getPreprocessor
        modelSpace= predefined_spaces.getModelSpace(preprocessor)
        modelSpace.setElementDimension(3)
        # Compute tributary lengths for each node.
        self.computeTributaryLengths(initialGeometry= True) # Tributary lengths.
        for n in self.pileSet.nodes:
            nPos= n.getInitialPos3d
            nodeDepth= self.getDepth(nPos.z)
            tributaryLength= self.tributaryLengths[n.tag]
            # Create P-Y material.
            pult, y50= self.getPyParametersAtDepth(depth= nodeDepth, tributaryLength= tributaryLength)
            soilType= self.getSoilTypeAtDepth(depth= nodeDepth)
            dashpotC= self.getViscousDampingTermAtDepth(depth= nodeDepth)
            Cd= self.getDragResistanceCoefficientAtDepth(depth= nodeDepth)
            pyMatName= 'pyS_'+str(n.tag)
            pyMat= ssi.def_pysimple1_material(preprocessor,
                                              matName= pyMatName,
                                              soilType= soilType,
                                              pult= pult,
                                              Y50= y50,
                                              c= dashpotC,
                                              Cd= Cd)
            # Create T-Z material.
            tult, z50= self.getTzParametersAtDepth(depth= nodeDepth, tributaryLength= tributaryLength, verticalStressOnSurface= verticalStressOnSurface)
            tzMatName= 'tzS:'+str(n.tag)
            tzMat= ssi.def_tzsimple1_material(preprocessor,
                                              matName= tzMatName,
                                              tzType= soilType, # Mosher relation.
                                              tult= tult, # Ultimate skin friction capacity.
                                              z50= z50, # 50% capacity reached.
                                              c= dashpotC)
            # Create spring.
            fixNode= modelSpace.duplicateNode(n)
            modelSpace.fixNode('000_000', fixNode.tag)
            modelSpace.setDefaultMaterial(pyMat)
            pileShaftSpring= modelSpace.newElement("ZeroLength", [fixNode.tag, n.tag])
            pileShaftSpring.clearMaterials()
            pileShaftSpring.setMaterial(0, pyMat.name)
            pileShaftSpring.setMaterial(1, pyMat.name)
            pileShaftSpring.setMaterial(2, tzMat.name)
            self.springs.append(pileShaftSpring)
            # Remove materials pyMat and tZMat materials that are no longer
            # needed.
            modelSpace.removeMaterial(pyMat)
            modelSpace.removeMaterial(tzMat)

        # Create spring at the pile tip.
        tipNode= self.getPileTipNode3D()
        ### Fix vertical rotation of tip node.
        modelSpace.fixNode('FFF_FF0', tipNode.tag)
        ### Compute tip node depth.
        tipNodePos= tipNode.getInitialPos3d
        tipNodeDepth= self.getDepth(tipNodePos.z)
        ### vertical effective stress at pile tip.
        qult, z50q= self.getQzParametersAtDepth(depth= tipNodeDepth, verticalStressOnSurface= verticalStressOnSurface)
        qzSoilType= self.getSoilTypeAtDepth(depth= nodeDepth)
        qzDashpotC= self.getViscousDampingTermAtDepth(depth= nodeDepth)
        qzSuction= self.getSuctionAtDepth(depth= nodeDepth)
        qzMatName= 'qzS:'+str(tipNode.tag)
        qzMat= ssi.def_qzsimple1_material(preprocessor,
                                  matName= qzMatName,
                                  qzType= qzSoilType, # soil type.
                                  qult= qult, # Ultimate tip capacity.
                                  z50= z50q, # 50% capacity reached.
                                  suction= qzSuction, # suction during tension/uplift.
                                  c= qzDashpotC) # viscous damping.
        
        pileTipFixNode= modelSpace.duplicateNode(tipNode)
        modelSpace.fixNode('000_000', pileTipFixNode.tag)
        modelSpace.setDefaultMaterial(qzMat)
        modelSpace.setElementDimension(3)
        pileTipSpring= modelSpace.newElement("ZeroLength", [pileTipFixNode.tag, tipNode.tag])
        # Vertical direction.
        pileTipSpring.setupVectors(xc.Vector([0,0,1]),xc.Vector([0,-1,0]))
        pileTipSpring.clearMaterials()
        pileTipSpring.setMaterial(0, qzMat.name)
        # Remove material that is no longer needed.
        modelSpace.removeMaterial(qzMat)
        return self.springs
        
    def generateSpringsPile(self):
        '''Generate the springs that simulate the soils along the pile

        '''
        preprocessor= self.pileSet.getPreprocessor
        numDOFs= preprocessor.getNodeHandler.numDOFs
        retval= None
        if(numDOFs==6): # 3D structural mechanics
            retval= self.generateSpringsPile3D()
        elif(numDOFs==3):
            retval= self.generateSpringsPile2D()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; not implemented for '+str(numDOFs) + ' degrees of freedom.')
        return retval
            
class PyTzQzCircularPile(PyTzQzPile):
    ''' Pile with circular cross-section.'''
    def __init__(self, E, pileType, diameter, soilLayers, pileSet):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param diameter: diameter of the pile cross-section.
        :param soilLayers:  properties of the soil horizons.
        :param pileSet: set of nodes and elements defining a single pile.
        '''
        cs= sp.CircularSection(name= None, Rext= diameter/2.0, Rint=0.0)

        super().__init__(E= E, crossSection= cs, pileType= pileType, soilLayers= soilLayers, pileSet= pileSet)

    def getDiameter(self):
        ''' Return the pile diameter.'''
        return self.crossSection.getDiameter()

    def getPileEquivalentDiameter(self):
        ''' Return the pile equivalent diameter.'''
        return self.getDiameter()
            
    
