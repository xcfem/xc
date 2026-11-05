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
    
    def getZ(self, depth):
        ''' Return the z coordinate corresponding to the given depth.'''
        return self.groundLevel-depth
    
    def getSoilsUntilZ(self, z):
        ''' Return the soil layers until the given depth is reached.

        :param z: z coordinate of the soil to reach.
        '''
        retval= list()
        if(self.soilProfile):
            for soilRow in self.soilProfile:
                layerZ= soilRow[0]
                soil= soilRow[1]
                if(layerZ<z):
                    retval.append((z, soil))
                    break
                else:
                    retval.append((layerZ, soil))
        return retval        

    def getSoilsUntilDepth(self, depth):
        ''' Return the soil layers until the given depth is reached.

        :param depth: depth of the desired soil.
        '''
        z= self.getZ(depth)
        return self.getSoilsUntilZ(z)

    def getSoilAtZ(self, z):
        ''' Return the soil corresponding to the given z coordinate.

        :param z: z coordinate of the desired soil.
        '''
        retval= None
        if(self.soilProfile):
            for soilRow in self.soilProfile:
                layerZ= soilRow[0]
                if(layerZ<z):
                    retval= soilRow[1]
                    break
        if(retval is None):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; no soil found at depth: '+str(depth)
            lmsg.error(className+'.'+methodName+errorMsg)
        return retval
    
    def getSoilAtDepth(self, depth):
        ''' Return the soil corresponding to the given depth.

        :param depth: depth of the desired soil.
        '''
        z= self.getZ(depth)
        return self.getSoilAtZ(z)

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
            currentDepth= self.getDepth(soilRow[0])
            currentSoil= soilRow[1]
            currentGamma= currentSoil.rho*g
            currentThickness= currentDepth-previousDepth
            retval+= currentThickness*currentGamma
            previousDepth= currentDepth
        if(currentDepth>waterTableDepth):
            # pore pressure.
            retval-= self.getPorePressureAtDepth(depth)
        return retval

    def getSuctionAtDepth(self, depth):
        ''' Return the suction at pile tip during tension/uplift.

        :param depth: depth of the poing of interest (m).
        '''
        soil= self.getSoilAtDepth(depth)
        return soil.suction
        
    def getPyParametersAtDepth(self, depth, pileDiameter, tributaryLength, verticalStressOnSurface= 0.0):
        '''  Compute the ultimate resistance (pult) according to API 
            recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
            OR per the method of Brinch Hansen (1961) for puSwitch= 2 and the
            displacement at 50% lateral capacity (y50).

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param tributaryLength: tributary length.
        :param verticalStressOnSurface: vertical stress on the soil surface.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            # soilRho= soil.rho # density.
            soilPhi= soil.phi # internal friction angle.
            soilCu= soil.getCohesion() # soil undrained cohesion.
            sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
            puSwitch= soil.puSwitch
            kSwitch= soil.kSwitch
            gwtSwitch= self.getGroundWaterSwitchAtDepth(depth)
            pult, y50= pytzqzf.get_py_param(pyDepth= depth, phi= soilPhi, cu= soilCu, sigV= sigV, b= pileDiameter, tribLength= tributaryLength, puSwitch= puSwitch, kSwitch= kSwitch, gwtSwitch= gwtSwitch)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; no soil found at depth: '+str(depth)
            lmsg.error(className+'.'+methodName+errorMsg)
            pult= None
            y50= None
        return pult, y50

    def getTzParametersAtDepth(self, depth, pileDiameter, tributaryLength, verticalStressOnSurface= 0.0, drilledShaft= True):
        ''' Return the ultimate resistance, tult, and displacement at 50% 
            mobilization of tult, z50, for use in t-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param tributaryLength: tributary length.
        :param verticalStressOnSurface: vertical stress on the soil surface.
        :param drilledShaft: true when the pile is drilled instead of driven.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            sigV_prime= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Effective vertical stress.
            porePressure= self.getPorePressureAtDepth(depth) # pore pressure.
            sigV= sigV_prime+porePressure # Total vertical pressure.
            soilPhi= soil.phi # internal friction angle.
            soilCu= soil.getCohesion() # soil undrained cohesion.
            tult, z50= pytzqzf.get_tz_param(phi= soilPhi, su= soilCu, b= pileDiameter, sigV= sigV, tribLength= tributaryLength, porePressure= porePressure, drilledShaft= drilledShaft)
        else:
            tult= None
            z50= None
        return tult, z50
    
    def getQzParametersAtDepth(self, depth, pileDiameter, verticalStressOnSurface= 0.0, drilledShaft= True):
        '''  Return the ultimate resistance, qult, and displacement at 50% 
            mobilization of qult, z50, for use in q-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        :param drilledShaft: true when the pile is drilled instead of driven.
        '''
        soil= self.getSoilAtDepth(depth)
        if(soil):
            sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
            soilPhi= soil.phi # internal friction angle.
            soilCu= soil.getCohesion() # soil undrained cohesion.
            soilG= soil.G # soil shear modulus.
            qult, z50= pytzqzf.get_qz_param(qzDepth= depth, phi= soilPhi, su= soilCu, b= pileDiameter, sigV= sigV, G= soilG, drilledShaft= drilledShaft)
        else:
            qult= None
            z50= None
        return qult, z50
        
