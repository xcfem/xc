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
from geotechnics.foundations import soil_layers
from scipy.constants import g
from geotechnics.foundations import pytzqz_functions as pytzqzf
from misc_utils import log_messages as lmsg

class SoilLayers(soil_layers.SoilLayers):
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

    def getDepth(self, z):
        ''' Return the depth corresponding to the given z coordinate.'''
        return self.groundLevel-z

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
                if(layerDepth<depth):
                    retval= soilRow[1]
                    break
        if(retval is None):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; no soil found at depth: '+str(depth)
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval

    def getSoilTypeAtDepth(self, depth):
        ''' Return the soil type corresponding to the given depth.

        -soilType= 1 Backbone of p-y curve approximates Matlock (1970) soft 
                   clay relation.

        -soilType= 2 Backbone of p-y curve approximates API (1993) 
                   sand relation.

        :param depth: depth of the desired soil.
        '''
        retval= None
        soil= self.getSoilAtDepth(depth)
        if(soil.getCohesion()>0.0):
            retval= 1
        else:
            retval= 2
        return retval        

    def getGroundWaterSwitchAtDepth(self, depth):
        ''' Return the effect of ground water on subgrade reaction modulus for 
            p-y curves. Above gwt --> 1, Below gwt --> 2.

        :param depth: depth of the point of interest.
        '''
        waterTableDepth= self.getWaterTableDepth()
        if(depth>waterTableDepth):
            retval= 1
        else:
            retval= 2
        return retval

    def getViscousDampingTermAtDepth(self, depth):
        ''' Return the viscous damping term (dashpot).

        :param depth: depth of the point of interest.
        '''
        soil= self.getSoilAtDepth(depth)
        return soil.dashpotC

    def getDragResistanceCoefficientAtDepth(self, depth):
        ''' Return the coefficient to compute the drag resistance within a 
            fully-mobilized gap as Cd*pult.

        :param depth: depth of the point of interest.
        '''
        soil= self.getSoilAtDepth(depth)
        return soil.Cd

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

    def getVerticalStressAtDepth(self, depth, verticalStressOnSurface= 0.0):
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
    
    def getSuctionAtDepth(self, depth):
        ''' Return the suction at pile tip during tension/uplift.

        :param depth: depth of the poing of interest (m).
        '''
        soil= self.getSoilAtDepth(depth)
        return soil.suction
        
    def getPyParametersAtDepth(self, depth, pileDiameter, tributaryLength):
        '''  Compute the ultimate resistance (pult) according to API 
            recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
            OR per the method of Brinch Hansen (1961) for puSwitch= 2 and the
            displacement at 50% lateral capacity (y50).

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param tributaryLength: tributary length.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            soilRho= soil.rho # density.
            soilPhi= soil.phi # internal friction angle.
            puSwitch= soil.puSwitch
            kSwitch= soil.kSwitch
            gwtSwitch= self.getGroundWaterSwitchAtDepth(depth)
            pult, y50= pytzqzf.get_py_param(pyDepth= depth, rho= soilRho, phi= soilPhi, b= pileDiameter, tribLength= tributaryLength, puSwitch= puSwitch, kSwitch= kSwitch, gwtSwitch= gwtSwitch)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; no soil found at depth: '+str(depth)
            lmsg.error(className+'.'+methodName+errorMsg)
            pult= None
            y50= None
        return pult, y50

    def getTzParametersAtDepth(self, depth, pileDiameter, tributaryLength, verticalStressOnSurface= 0.0):
        ''' Return the ultimate resistance, tult, and displacement at 50% 
            mobilization of tult, z50, for use in t-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param tributaryLength: tributary length.
        :param verticalStressOnSurface: vertical stress on the soil surface.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
            soilPhi= soil.phi # internal friction angle.
            tult, z50= pytzqzf.get_tz_param(phi= soilPhi, b= pileDiameter, sigV= sigV, tribLength= tributaryLength)
        else:
            tult= None
            z50= None
        return tult, z50
    
    def getQzParametersAtDepth(self, depth, pileDiameter, verticalStressOnSurface= 0.0):
        '''  Return the ultimate resistance, qult, and displacement at 50% 
            mobilization of qult, z50, for use in q-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
            soilPhi= soil.phi # internal friction angle.
            soilG= soil.G # soil shear modulus.
            qult, z50= pytzqzf.get_qz_param(phi= soilPhi, b= pileDiameter, sigV= sigV, G= soilG)
        else:
            qult= None
            z50= None
        return qult, z50
        
