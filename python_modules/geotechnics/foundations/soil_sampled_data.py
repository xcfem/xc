# -*- coding: utf-8 -*-
''' Class to store the values of the geotechnical parameters along the 
    shaft of a pile.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import os
import sys
import json
import math
import numpy as np
from scipy.constants import g
from scipy import interpolate
from geotechnics.foundations import soil_layers
from geotechnics.foundations import pytzqz_functions as pytzqzf
from matplotlib import pyplot, transforms
from misc_utils import log_messages as lmsg

class SoilSampledData(soil_layers.SoilLayers):
    ''' Class to store the values of the geotechnical parameters along the
        depth of a pile.
    '''
    def __init__(self, code, pile_data= None, puSwitch= 1, kSwitch= 1, waterTableZ= -math.inf, waterDensity= 1e3):
        ''' Constructor.

        :param code: code for the geotechnical parameters.
        :param pile_data: dictionary containing the geotechnical data of the
                          piles.
        :param puSwitch: select pult definition method for p-y curves
                         API --> 1
                         Brinch Hansen --> 2
        :param kSwitch: select variation in coefficent of subgrade reaction 
                        with depth for p-y curves
                        API linear variation (default)   --> 1
                        modified API parabolic variation --> 2
        :param waterTableZ: global Z coordinate of the water table.
        :param waterDensity: density of the water.
        '''
        super().__init__(waterTableZ= waterTableZ, groundLevel= None, waterDensity= waterDensity)
        self.code= code
        self.pileData= pile_data
        self.interpolationsDict= dict()
        self.puSwitch= puSwitch
        self.kSwitch= kSwitch
        
    def readFromFile(self, inputFileName):
        ''' Read the geotechnical data from the given file.

        :param inputFileName: name of the input file.
        '''
        with open(inputFileName, 'r') as f:
            self.pileData= json.load(f)

    def __str__(self):
        retval= ""        
        pile_parameters= self.pileData
        kh_data= self.pileData['kh_data']
        for key in kh_data:
            values= kh_data[key]
            retval+= str(pile_key)+', kh_data, '+str(key)+' values= '+str(values)+'\n'
        bearing_data= self.pileData['bearing_data']
        for key in bearing_data:
            values= bearing_data[key]
            retval+= str(pile_key)+', bearing_data, '+str(key)+' values= '+str(values)+'\n'
        return retval

    def _get_bearing_data(self):
        ''' Return the bearing data corresponding to the pile with the given
            code.
        '''
        return self.pileData['bearing_data']

    def _get_horizontal_stiffness_data(self):
        ''' Return the horizontal stiffness data corresponding to the pile with
            the given code.
        '''
        return self.pileData['kh_data']

    def _plot_property_along_pile_depth(self, propName, depths, values, unitSymbol, lengthUnitSymbol= 'm'):
        ''' Plot the values of a property along the pile depth.

        :param propName: property name.
        :param depths: depths along the pile depth.
        :param values: values of the property along the pile depth.
        :param unitSymbol: unit symbol of the property.
        :parma lengthUnitSymbol: lenght unit symbol.
        '''
        x= np.array(values)
        y= np.array([-d for d in depths])
        line = pyplot.plot(x, y)
        pyplot.title(self.code+' '+propName)
        pyplot.xlabel(propName+' ('+str(unitSymbol)+')')
        pyplot.ylabel('Depth ('+str(lengthUnitSymbol)+')')#, fontweight='bold')

        pyplot.show()

    def _get_bearing_property_along_pile_depth(self, propName):
        ''' Return the values of the property with the given name along the
            pile depth.

        :param propName: name of the property.
        '''
        depths= None
        values= None
        bearingProperties= self.getBearingProperties()
        if(propName in bearingProperties):
            bearingData= self._get_bearing_data()
            depths= bearingData['depthi'] # depths along the pile.
            values= bearingData[propName] # internal friction angles along the pile.
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warnMsg= '; property: '+str(propName)
            warnMsg+= ' is not available for pile: '+str(self.code)
            warnMsg+= '. Candidates are: '
            warnMsg+= str(bearingProperties)
            lmsg.warning(className+'.'+methodName+warnMsg)
            
        return depths, values

    def _get_staircase_property_interpolation(self, propName, depths, values):
        ''' Return a staircase shaped interpolation function for the given 
            property.

        :param propName: name of the property.
        :param depths: depths of the sampled values.
        :param values: sampled values.
        '''
        retval= None
        interpolationFunctionName= propName+'_interpolation'
        if(interpolationFunctionName in self.interpolationsDict):
            retval= self.interpolationsDict[interpolationFunctionName]
        else:
            bearingData= self._get_bearing_data()
            depths_size= len(depths)
            values_size= len(values)
            # Create a staircase shaped interpolation.
            if(depths_size==values_size):
                if(depths_size==1):
                    z= depths[0]
                    value= values[0]
                    retval= interpolate.interp1d([0.0, z], [value, value], fill_value='extrapolate')
                elif(depths_size==2):
                    z1= depths[0]
                    z2= depths[1]
                    value1= values[0]
                    value2= values[1]
                    retval= interpolate.interp1d([0.0, z1-1e-3,z1+1e-3,z2], [value1, value1,value2,value2], fill_value='extrapolate')
                elif(depths_size>=3):
                    zi= [0.0]
                    for depth in depths[:-1]:
                        zi.append(depth-1e-3)
                        zi.append(depth+1e-3)
                    zi.append(depths[-1])
                    vi= list()
                    for v in values:
                        vi.append(v)
                        vi.append(v)
                    retval= interpolate.interp1d(zi, vi, fill_value='extrapolate')
                else:
                    methodName= sys._getframe(0).f_code.co_name
                    errorMsg= '; too many values ('+str(depthi_size)+')'
                    lmsg.error(methodName+errorMsg)
            if(retval is not None):
                self.interpolationsDict[interpolationFunctionName]= retval
            else:
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= '; incompatible sizes: '+str(pierCode)+' '+str(depthi_size)+' '+str(khi_size)
                lmsg.error(methodName+errorMsg)
                exit(1)
        return retval
        
    def _get_linear_property_interpolation(self, propName, depths, values):
        ''' Return a linear interpolation function for the given property.

        :param propName: name of the property.
        :param depths: depths of the sampled values.
        :param values: sampled values.
        '''
        retval= None
        depths_size= len(depths)
        values_size= len(values)
        interpolationFunctionName= propName+'_interpolation'
        if(interpolationFunctionName in self.interpolationsDict):
            retval= self.interpolationsDict[interpolationFunctionName]
        else:
            if(depths_size==values_size):
                retval= interpolate.interp1d(depths, values, fill_value='extrapolate')
                self.interpolationsDict[interpolationFunctionName]= retval
            else:
                methodName= sys._getframe(0).f_code.co_name
                errorMsg= '; incompatible sizes: '+str(pierCode)+' '+str(depths_size)+' '+str(values_size)
                lmsg.error(methodName+errorMsg)
                exit(1)
        return retval
        
    def getPileCodes(self):
        ''' Return a list of the pile codes defined in the stored data.'''
        return list(self.pileData.keys())

    def getBearingProperties(self):
        ''' Return a list of the names of the bearing properties defined for
            the pile with the given code.
        '''
        bearingData= self._get_bearing_data()
        return list(bearingData.keys())

    def getInternalFrictionAngleAlongDepth(self):
        ''' Return the values of the internal friction angles along the pile
            shaft.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'phii')

    def getInternalFrictionAngleAtDepth(self, depth):
        ''' Return the value of the internal friction angle at the given depth.

        :param depth: desired depth.
        '''
        retval= None
        bearingProperties= self.getBearingProperties()
        if('phii' in bearingProperties):
            depths, values= self.getInternalFrictionAngleAlongDepth()
            interpolation= self._get_linear_property_interpolation(propName= 'phi', depths= depths, values= values)
            retval= float(interpolation(depth))
        return retval
    
    def plotInternalFrictionAngleAlongDepth(self, degrees=True):
        ''' Plot the values of the internal friction angles along the pile
            shaft.

        :param degrees: if True, transform the angles to degrees.
        '''
        depthi, phii= self.getInternalFrictionAngleAlongDepth()
        if(phii is not None):
            tmp= [math.degrees(phi) for phi in phii]
            self._plot_property_along_pile_depth(propName= '$\phi$', depths= depthi, values= tmp, unitSymbol= 'º')

    def getCohesionAlongDepth(self):
        ''' Return the values of the soil cohesion along the pile depth.
        '''
        return self._get_bearing_property_along_pile_depth(propName= 'ci')

    def getCohesionAtDepth(self, depth):
        ''' Return the value of the cohesion at the given depth.

        :param depth: desired depth.
        '''
        retval= None
        bearingProperties= self.getBearingProperties()
        if('ci' in bearingProperties):
            depths, values= self.getCohesionAlongDepth()
            interpolation= self._get_linear_property_interpolation(propName= 'ci', depths= depths, values= values)
            retval= float(interpolation(depth))
        return retval
    
    def plotCohesionAlongDepth(self, unitConvFactor= 1e-3, unitSymbol= 'kPa'):
        ''' Plot the values of the soil cohesion along the pile depth.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, ci= self.getCohesionAlongDepth()
        if(ci is not None):
            tmp= [c*unitConvFactor for c in ci]
            self._plot_property_along_pile_depth(propName= 'c', depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getUnitWeightAlongDepth(self):
        ''' Return the values of the soil unit weight along the pile depth.
        '''
        return self._get_bearing_property_along_pile_depth(propName= 'gammai')

    def getUnitWeightAtDepth(self, depth):
        ''' Return the value of the soil unit weight at the given depth.

        :param depth: desired depth.
        '''
        retval= None
        depths, values= self.getUnitWeightAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'gammai', depths= depths, values= values)
        retval= float(interpolation(depth))
        return retval
    
    def getUnitMassAtDepth(self, depth):
        ''' Return the value of the soil unit mass at the given depth.

        :param depth: desired depth.
        '''
        retval= self.getUnitWeightAtDepth(depth)
        retval/= g
        return retval

    def getVerticalStressAtDepth(self, depth, verticalStressOnSurface):
        ''' Return the value of the vertical stress at the given depth.

        :param depth: desired depth.
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        retval= verticalStressOnSurface
        depths, values= self.getUnitWeightAlongDepth()
        if(len(depths)>1):
            depth0= depths[0]
            v0= values[0]
            for depth1, v1 in zip(depths[1:], values[1:]):
                if(depth1<depth):
                    retval+= 0.5*(depth1-depth0)*(v0+v1)
                else:
                    v1= self.getUnitWeightAtDepth(depth)
                    retval+= 0.5*(depth-depth0)*(v0+v1)
                    break
                v0= v1
                depth0= depth1
        elif(len(depths)>0):
            retval= depth*values[0]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= '; not enough data to compute vertical pressure.'
            lmsg.error(className+'.'+methodName+errMsg)
            sys.exit(1)
        return retval
    
    def plotUnitWeightAlongDepth(self, unitConvFactor= 1e-3, unitSymbol= 'kN/m3'):
        ''' Plot the values of the soil unit weight along the pile depth.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, ci= self.getUnitWeightAlongDepth()
        if(ci is not None):
            tmp= [c*unitConvFactor for c in ci]
            self._plot_property_along_pile_depth(propName= '$\gamma$', depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getPileBearingCapacityAlongDepth(self, characteristic):
        ''' Plot the values of total bearing capacity of the pile 
            corresponding to different depths.

        :param characteristic: if true return the characteristic value, 
                               otherwise return the design one.
        '''
        if(characteristic):
            propName= 'rcki'
        else:
            propName= 'rcdi'
        return self._get_bearing_property_along_pile_depth(propName= propName)
            
    def getPileBearingCapacityAtDepth(self, depth, characteristic):
        ''' Return the value of the total bearing capacity of the pile at the
            given depth.

        :param depth: desired depth.
        :param characteristic: if true return the characteristic value, 
                               otherwise return the design one.
        '''
        retval= None
        bearingProperties= self.getBearingProperties()
        if(characteristic):
            propName= 'rcki'
        else:
            propName= 'rcdi'
        if(propName in bearingProperties):
            depths, values= self.getPileBearingCapacityAlongDepth(characteristic)
            interpolation= self._get_linear_property_interpolation(propName= propName, depths= depths, values= values)
            retval= float(interpolation(depth))
        return retval
    
    def plotPileBearingCapacityAlongDepth(self, characteristic, unitConvFactor= 1e-3, unitSymbol= 'kN'):
        ''' Plot the values of the soil unit weight along the pile depth.

        :param characteristic: if true return the characteristic value, 
                               otherwise return the design one.
        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, ri= self.getPileBearingCapacityAlongDepth(characteristic)
        if(characteristic):
            propName= '$R_{ck}$'
        else:
            propName= '$R_{cd}$'
        if(ri is not None):
            tmp= [r*unitConvFactor for r in ri]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getQuAlongDepth(self):
        ''' Return the uniaxial unconfined compressive strength of intact rock
            of the pile corresponding to different depths.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'qui')
            
    def getQuAtDepth(self, depth):
        ''' Return the value of the uniaxial unconfined compressive strength of 
            intact rock at the given depth.

        :param depth: desired depth.
        '''
        retval= None
        bearingProperties= self.getBearingProperties()
        if('qui' in bearingProperties):
            depths, values= self.getQuAlongDepth()
            interpolation= self._get_staircase_property_interpolation(propName= 'qui', depths= depths, values= values)
            retval= float(interpolation(depth))
        return retval
    
    def plotQuAlongDepth(self, unitConvFactor= 1e-3, unitSymbol= 'kN'):
        ''' Plot the uniaxial unconfined compressive strength of intact rock
            of the pile corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, qui= self.getQuAlongDepth()
        propName= '$q_{u}$'
        if(qui is not None):
            tmp= [qu*unitConvFactor for qu in qui]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getRockQualityDesignationAlongDepth(self):
        ''' Return the rock quality designation of the rock of the pile 
            corresponding to different depths.
        '''
        return self._get_bearing_property_along_pile_depth(propName= 'rqdi')
            
    def getRockQualityDesignationAtDepth(self, depth):
        ''' Return the value of the rock quality designation of the rock at the
            given depth.

        :param depth: desired depth.
        '''
        retval= None
        bearingProperties= self.getBearingProperties()
        if('rqdi' in bearingProperties):
            depths, values= self.getRockQualityDesignationAlongDepth()
            interpolation= self._get_staircase_property_interpolation(propName= 'rqdi', depths= depths, values= values)
            retval= float(interpolation(depth))
        return retval
    
    def plotRockQualityDesignationAlongDepth(self, unitConvFactor= 1.0, unitSymbol= ''):
        ''' Plot the rock quality designation of the rock of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, rqdi= self.getRockQualityDesignationAlongDepth()
        propName= 'RQD'
        if(rqdi is not None):
            tmp= [rqd*unitConvFactor for rqd in rqdi]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)

    def getKhAlongDepth(self):
        ''' Return the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        '''
        pileKhData= self.pileData['kh_data']
        depths= pileKhData['depthi']
        khi= pileKhData['khi']
        return depths, khi
            
    def getKhAtDepth(self, depth):
        ''' Return the value of the horizontal modulus of subgrade reaction 
            at the given depth.

        :param depth: desired depth.
        '''
        depths, values= self.getKhAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'kh', depths= depths, values= values)
        return float(interpolation(depth))
                                                                  
    def plotKhAlongDepth(self, unitConvFactor= 1e-6, unitSymbol= '$MN/m^3$'):
        ''' Plot the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, khi= self.getKhAlongDepth()
        propName= '$K_{h}$'
        if(khi is not None):
            tmp= [kh*unitConvFactor for kh in khi]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
                    
    def getDashpotCAlongDepth(self):
        ''' Return the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'dashpotci')
            
    def getDashpotCAtDepth(self, depth):
        ''' Return the value of the horizontal modulus of subgrade reaction 
            at the given depth.

        :param depth: desired depth.
        '''
        depths, values= self.getDashpotCAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'dashpotc', depths= depths, values= values)
        return float(interpolation(depth))
                                                                  
    def plotDashpotCAlongDepth(self, unitConvFactor= 1e-3, unitSymbol= '$kN.s/m$'):
        ''' Plot the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, dashpotCi= self.getDashpotCAlongDepth()
        propName= '$c$'
        if(dashpotCi is not None):
            tmp= [dashpotC*unitConvFactor for dashpotC in dashpotCi]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
        
    def getDragResistanceAlongDepth(self):
        ''' Return the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'drag_resistance_i')
            
    def getDragResistanceAtDepth(self, depth):
        ''' Return the value of the horizontal modulus of subgrade reaction 
            at the given depth.

        :param depth: desired depth.
        '''
        depths, values= self.getDragResistanceAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'drag_resistance', depths= depths, values= values)
        return float(interpolation(depth))
                                                                  
    def plotDragResistanceAlongDepth(self, unitConvFactor= 1e-3, unitSymbol= '$kN$'):
        ''' Plot the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, drag_resistance_i= self.getDragResistanceAlongDepth()
        propName= '$C_{d}$'
        if(drag_resistance_i is not None):
            tmp= [drag_resistance*unitConvFactor for drag_resistance in drag_resistance_i]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)

    def getShearModulusAlongDepth(self):
        ''' Return the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'gi')
            
    def getShearModulusAtDepth(self, depth):
        ''' Return the value of the horizontal modulus of subgrade reaction 
            at the given depth.

        :param depth: desired depth.
        '''
        depths, values= self.getShearModulusAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'shear_modulus', depths= depths, values= values)
        return float(interpolation(depth))
                                                                  
    def plotShearModulusAlongDepth(self, unitConvFactor= 1e-6, unitSymbol= '$MPa$'):
        ''' Plot the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, shear_modulus_i= self.getShearModulusAlongDepth()
        propName= '$G$'
        if(shear_modulus_i is not None):
            tmp= [shear_modulus*unitConvFactor for shear_modulus in shear_modulus_i]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getSuctionAlongDepth(self):
        ''' Return the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        '''
        return self._get_bearing_property_along_pile_depth(propName= 'suction_i')
            
    def getSuctionAtDepth(self, depth):
        ''' Return the value of the horizontal modulus of subgrade reaction 
            at the given depth.

        :param depth: desired depth.
        '''
        depths, values= self.getSuctionAlongDepth()
        interpolation= self._get_staircase_property_interpolation(propName= 'suction', depths= depths, values= values)
        return float(interpolation(depth))
                                                                  
    def plotSuctionAlongDepth(self, unitConvFactor= 1e-6, unitSymbol= '$MPa$'):
        ''' Plot the horizontal modulus of subgrade reaction of the pile 
            corresponding to different depths.

        :param unitConvFactor: unit conversion factor.
        :param unitSymbol: unit simbol.
        '''
        depthi, suction_i= self.getSuctionAlongDepth()
        propName= '$G$'
        if(suction_i is not None):
            tmp= [suction*unitConvFactor for suction in suction_i]
            self._plot_property_along_pile_depth(propName= propName, depths= depthi, values= tmp, unitSymbol= unitSymbol)
            
    def getPyParametersAtDepth(self, depth, pileDiameter, tributaryLength, gwtSwitch, verticalStressOnSurface= 0.0):
        '''  Compute the ultimate resistance (pult) according to API 
            recommendations (Reese and Van Impe, 2001 or API, 1987) for puSwitch= 1
            OR per the method of Brinch Hansen (1961) for puSwitch= 2 and the
            displacement at 50% lateral capacity (y50).

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param tributaryLength: tributary length.
        :param gwtSwitch: gwtSwitch= 1, k soil above the groundwater table,
                          gwtSwitch= 2, k soil below the groundwater table,
        :param verticalStressOnSurface: vertical stress on the soil surface.
        '''
        #soilRho= self.getUnitMassAtDepth(depth) # density.
        sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
        print('*** sigV= ', sigV)
        soilPhi= self.getInternalFrictionAngleAtDepth(depth) # internal friction angle.
        soilCu= self.getCohesionAtDepth(depth)
        print('*** soilPhi= ', math.degrees(soilPhi))
        print('*** depth= ', depth)
        print('*** gwtSwitch= ', gwtSwitch)
        pult, y50= pytzqzf.get_py_param(pyDepth= depth, phi= soilPhi, cu= soilCu, sigV= sigV, b= pileDiameter, tribLength= tributaryLength, puSwitch= self.puSwitch, kSwitch= self.kSwitch, gwtSwitch= gwtSwitch)
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
        sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
        soilPhi= self.getInternalFrictionAngleAtDepth(depth) # internal friction angle.
        tult, z50= pytzqzf.get_tz_param(phi= soilPhi, b= pileDiameter, sigV= sigV, tribLength= tributaryLength)
        return tult, z50
    
    def getQzParametersAtDepth(self, depth, pileDiameter, verticalStressOnSurface= 0.0):
        '''  Return the ultimate resistance, qult, and displacement at 50% 
            mobilization of qult, z50, for use in q-z curves for cohesionless
            soil.

        :param depth: depth of the poing of interest (m).
        :param pileDiameter: pile diameter (m).
        :param verticalStressOnSurface: additional stress on the soil surface.
        '''
        sigV= self.getVerticalStressAtDepth(depth, verticalStressOnSurface) # Vertical stress.
        soilPhi= self.getInternalFrictionAngleAtDepth(depth) # internal friction angle.
        soilG= self.getShearModulusAtDepth(depth) # soil shear modulus.
        qult, z50= pytzqzf.get_qz_param(phi= soilPhi, b= pileDiameter, sigV= sigV, G= soilG)
        return qult, z50

class SoilSampledDataSet(object):
    ''' Class to store the values of the geotechnical parameters for many piles.
    '''
    def __init__(self, pile_data_set= None):
        ''' Constructor.

        :param pile_data_set: dictionary containing the geotechnical data of the
                              piles.
        '''
        if(pile_data_set):
            self._create_piles_from_data_set(pile_data_set)

    def _create_piles_from_data_set(self, pile_data_set):
        ''' Create the SoilSampledData objects from the given data
            set.

        :param pile_data_set: dictionary containing the geotechnical data of the
                              piles.
        '''
        if(pile_data_set):
            self.pileDataSet= dict()
            for code in pile_data_set:
                pileData= pile_data_set[code]
                self.pileDataSet[code]= SoilSampledData(code= code, pile_data= pileData)
        
    def readFromFile(self, inputFileName):
        ''' Read the geotechnical data from the given file.

        :param inputFileName: name of the input file.
        '''
        with open(inputFileName, 'r') as f:
            pile_data_set= json.load(f)
            self._create_piles_from_data_set(pile_data_set)

    def __str__(self):
        retval= ""        
        for pile_key in self.pileDataSet:
            pile_parameters= self.pileDataSet[pile_key]
            retval+= str(pile_parameters)+'\n'
        return retval

    def getCodes(self):
        ''' Return a list of the pile codes defined in the stored data.'''
        return list(self.pileDataSet.keys())

    def getSoilSampledData(self, code):
        ''' Return the SoilSampledData object corresponding to the
            given code.

        :param code: code of the pile of interest.
        '''
        return self.pileDataSet[code]

                
