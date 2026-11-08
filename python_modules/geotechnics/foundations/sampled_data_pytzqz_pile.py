# -*- coding: utf-8 -*-
'''Definition of pile spring supports based on data sampled along the pile
   depth.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from geotechnics.foundations import pytzqz_pile
from materials.sections import section_properties as sp
from misc_utils import log_messages as lmsg

class PyTzQzPile(pytzqz_pile.PyTzQzPile):
    ''' PyTzQzPile based on data sampled along the pile depth.
    '''
    def __init__(self, E, crossSection, soilData, pileSet= None, rho= 2.5e3):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param crossSection: cross-section of the pile.
        :param soilData:  properties of the soil along depth.
        :param pileSet: set of nodes and elements defining a single pile.
        :param rho: material density.
        '''
        super().__init__(E= E, crossSection= crossSection, pileType= None, soilLayers= soilData, pileSet= pileSet, rho= rho)
        self.zMin= None
        self.zMax= None
  
    def _compute_z_min_and_max(self):
        ''' Compute the maximum and minimum z coordinates of the nodes along the
            pile.
        '''
        if(self.zMin is None):
            lstNodPile= self.getNodeZs() # node identifiers and depths.
            self.zMin= lstNodPile[-1][1]
            self.zMax= lstNodPile[0][1]
        
    def getDepth(self, z:float):
        ''' Return the depth corresponding to the given z coordinate.

        :param z: value of the z coordinate.
        '''
        if(self.zMin is None):
            self._compute_z_min_and_max()
        retval= self.zMax-z 
        return retval
    
    def getWaterTableDepth(self):
        ''' Return the depth of the water table.

        '''
        waterTableZ= self.soilLayers.waterTableZ
        if(waterTableZ is not None):
            return self.getDepth(waterTableZ)
        else:
            return math.inf
        
    def getGroundWaterSwitchAtDepth(self, depth):
        ''' Return the effect of ground water on subgrade reaction modulus for 
            p-y curves. Above gwt --> 1, Below gwt --> 2.

        :param depth: depth of the point of interest.
        '''
        waterTableDepth= self.getWaterTableDepth()
        if(depth<waterTableDepth):
            retval= 1
        else:
            retval= 2
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
            gwtSwitch= self.getGroundWaterSwitchAtDepth(depth)
            pult, y50= self.soilLayers.getPyParametersAtDepth(depth, pileDiameter, tributaryLength, gwtSwitch= gwtSwitch)
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errorMsg= '; soil layers not defined.'
            lmsg.error(className+'.'+methodName+errorMsg)
        return pult, y50
    
class PyTzQzCircularPile(PyTzQzPile):
    ''' Pile with circular cross-section.'''
    def __init__(self, E, diameter, soilData, pileSet):
        ''' Constructor.

        :param E: elastic modulus of pile material
        :param diameter: diameter of the pile cross-section.
        :param soilData:  properties of the soil along depth.
        :param pileSet: set of nodes and elements defining a single pile.
        '''
        cs= sp.CircularSection(name= None, Rext= diameter/2.0, Rint=0.0)

        super().__init__(E= E, crossSection= cs, soilData= soilData, pileSet= pileSet)

    def getDiameter(self):
        ''' Return the pile diameter.'''
        return self.crossSection.getDiameter()

    def getPileEquivalentDiameter(self):
        ''' Return the pile equivalent diameter.'''
        return self.getDiameter()
