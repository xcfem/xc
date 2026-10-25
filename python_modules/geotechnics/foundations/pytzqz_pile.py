# -*- coding: utf-8 -*-
''' Base classes for pile foundations that use the PySimple1, TzSimple1 and 
QzSimple1 materials to represent the soil-structure interaction.

'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy.constants import g
from geotechnics.foundations import pile
from geotechnics.foundations import pytzqz_functions as pytzqzf
from materials.sections import section_properties as sp

class SoilLayers(pile.SoilLayers):
    ''' Class to compute the values of the soil properties according 
        to the functions of pytzqzf.

    :ivar soilProfile: properties of the levels of soil, defined from 
                     top to bottom as a list: 
                     - [[zBottomSoil, soilModel], ...] where,
                     'zBottomSoil' is the global Z coordinate of the 
                     bottom level of the soil,
                     -'soilModel' is an FrictionalSoil or a
                                  FrictionalCohesiveSoil object that stores
                                  the relevant properies of the soil.
    '''
    def __init__(self, soilProfile, groundLevel, waterTableZ= None, waterDensity= 1e3):
        ''' Constructor.

        :param soilProfile:  properties of the levels of soil, defined from 
                             top to bottom as a list: 
                             - [[zBottomSoil, soilModel], ...] where,
                             'zBottomSoil' is the global Z coordinate of the 
                             bottom level of the soil,
                             -'soilModel' is an FrictionalSoil or a
                                  FrictionalCohesiveSoil object that stores
                                  the relevant properies of the soil.
        :param groundLevel: global Z coordinate of the ground level.
        :param waterTableZ: global Z coordinate of the water table.
        :param waterDensity: density of the water.
        '''
        super().__init__(groundLevel= groundLevel, waterTableZ= waterTableZ, waterDensity= waterDensity)
        self.soilProfile= soilProfile

    def getWaterTableDepth(self):
        ''' Return the depth of the water table.'''
        if(self.waterTableZ is not None):
            return self.waterTableZ-self.groundLevel
        else:
            return -math.inf
    
    def getSoilsUntilDepth(self, depth):
        ''' Return the soil layers until the given depth is reached.

        :param depth: depth of the desired soil.
        '''
        retval= list()
        if(self.soilProfile):
            for soilRow in self.soilProfile:
                layerDepth= soilRow[0]
                soil= soilRow[1]
                if(layerDepth<depth):
                    retval.append((depth, soil))
                    break
                else:
                    retval.append((layerDepth, soil))
        return retval        

    def getSoilAtDepth(self, depth):
        ''' Return the soil corresponding to the given depth.

        :param depth: depth of the desired soil.
        '''
        retval= None
        if(self.soilProfile):
            for soilRow in self.soilProfile:
                layerDepth= soilRow[0]
                if(layerDepth>depth):
                    retval= soilRow[1]
                    break
        return retval

    def getGroudWaterSwitch(self, depth):
        ''' Return the effect of ground water on subgrade reaction modulus for 
            p-y curves. Above gwt --> 1, Below gwt --> 2.

        :param depth: depth of the desired soil.
        '''
        waterTableDepth= self.getWaterTableDepth()
        if(depth<waterTableDepth):
            retval= 1
        else:
            retval= 2
        return retval

    def getWaterPressure(self, depth):
        ''' Return the pressure of the water (if any) at the given depth.

        :param depth: depth of the desired soil.
        '''
        retval= 0.0
        waterTableDepth= self.getWaterTableDepth()
        if(depth<waterTableDepth):
            waterThickness= depth-waterTableDepth
            retval= waterThickness*self.waterDensity*g
        return retval

    def getVerticalStress(self, depth, verticalStressOnSurface= 0.0):
        ''' Return the vertical stress in the soil at the given depth.

        :param depth: depth of the poing of interest (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        retval= verticalStressOnSurface
        soilRows= self.getSoilsUntilDepth(depth)
        waterTableDepth= self.getWaterTableDepth()
        previousDepth= 0.0
        for soilRow in soilRows:
            currentDepth= soilRow[0]
            currentSoil= soilRow[1]
            currentGamma= currentSoil.rho*g
            currentThickness= currentDepth-previousDepth
            retval+= currentThickness*currentGamma
            previousDepth= currentDepth
        if(currentDepth<waterTableDepth):
            # Buoyancy.
            waterThickness= depth-waterTableDepth
            retval-= waterThickness*self.waterDensity*g
        return retval

        
    def getPyParameters(self, depth, pileDiameter, tributaryLength):
        '''  Compute the ultimate resistance (pult) according to API 
            recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
            OR per the method of Brinch Hansen (1961) for puSwitch= 2 and the
            displacement at 50% lateral capacity (y50).

        :param depth: depth of the poing of interest (m).
        :param rho: soil unit mass (kg/m3).
        :param phi: soil internal friction angle (radians).
        :param b: pile diameter (m).
        :param puSwitch: if (puSwitch= 1) compute pult according to API 
                         recommendations (Reese and Van Impe, 2001 or API, 1987).
                         if (puSwitch= 2) compute pult according to the method 
                         of Brinch Hansen (1961).
        :param kSwitch: a linear variation of k with depth is defined for kSwitch= 1 
                        after API (1987) a parabolic variation of k with depth is 
                        defined for kSwitch= 2 after Boulanger et al. (2003).
        '''
        soil= self.getSoilAtDepth(depth)
        soilRho= soil.rho
        soilPhi= soil.phi
        puSwitch= soil.puSwitch
        kSwitch= soil.kSwitch
        gwtSwitch= self.GroudWaterSwitch(depth)
        pult, y50= pytzqzf.get_py_param(depth, soilRho, soilPhi, pileDiameter, tributaryLength, puSwitch, kSwitch, gwtSwitch)

class PyTzQzPile(pile.Pile):
    '''Pile with soil structure interaction represented by PySimple1, TzSimple1
       and QzSimple1 materials.
    
    '''
    def __init__(self, E, crossSection, pileType, soilLayers, pileSet= None, rho= 2.5e3):
        ''' Constructor.

        :param crossSection: cross-section of the pile.
        :param E: elastic modulus of pile material
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param soilLayers:  properties of the soil horizons.
        :param pileSet: set of nodes and elements defining a single pile.
        :param rho: material density.
        '''
        super().__init__(E= E, crossSection= crossSection, pileType= pileType, soilLayers= soilLayers, pileSet= pileSet, rho= rho)
     
    
    def generateSpringsPile2D(self, alphaKh_x, alphaKv_y, storeAsPileNodeProperties= True):
        '''Generate the springs that simulate the soils along the pile in a 2D
           problem.

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKv_y: coefficient to be applied to the vertical stiffness of
                          a single pile in Y direction
        :param storeAsPileNodeProperties: if true store the spring constants as
                                          properties of the pile nodes.
        '''
        print('XXX continue here.')
        self.springs= None
        return self.springs
    
    def generateSpringsPile3D(self):
        '''Generate the springs that simulate the soils along the pile in a 3D
           problem.

        '''
        print('XXX continue here.')
        self.springs= None
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
            
    def defElasticShearSection2d(self, preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including shear 
           deformations.

        :param preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)        
        return self.crossSection.defElasticShearSection2d(preprocessor= preprocessor, material= matData, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations.

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        matData= self.getElasticMaterialData(overrideRho= overrideRho)        
        return self.crossSection.defElasticShearSection3d(preprocessor= preprocessor, material= matData, overrideRho= overrideRho)
    
