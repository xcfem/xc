# -*- coding: utf-8 -*-
''' Hollow steel shapes as defined in EN 10210-2: 2019'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2026, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import structural_steel
from materials.sections.structural_shapes import en_10210_shapes_dictionaries
from materials.sections.structural_shapes import steel_shapes_utils as utils

def find_nearest_steel_shape_by_depth(namePattern:str, depth:float):
    ''' Return the steel shape whose name matches the pattern argument
        and whose depth is the nearest to the argument.

    :param namePattern: regular expression to match.
    :param depth: depth of the steel shape.
    '''
    en10210Dict= en_10210_shapes_dictionaries.en_10210_shapes
    return utils.find_nearest_steel_shape_by_depth(shapeDict= en10210Dict, namePattern= namePattern, depth= depth)

def get_steel_shape_depth_range(namePattern:str, minDepth:float, maxDepth: float):
    ''' Return the steel shapes whose name matches the pattern argument
        and whose depth is belongs to the range (minDepth, maxDepth)

    :param namePattern: regular expression to match.
    :param minDepth: minimum depth of the steel shape.
    :param maxDepth: maximum depth of the steel shape.
    '''
    # Find the dictionary to search in.
    en10210Dict= en10210_shapes_dictionaries.en10210_shapes
    return utils.get_steel_shape_depth_range(shapeDict= en10210Dict, namePattern= namePattern, minDepth= minDepth, maxDepth= maxDepth)

RHS= en_10210_shapes_dictionaries.RHS

class RHSShape(structural_steel.QHShape):
    def __init__(self, steel, name):
        super(RHSShape,self).__init__(steel,name,RHS)

    def t(self):
        '''Return the wall thickness.'''
        return self.get('e')

    def tw(self):
        ''' Return the web thickness (used in Eurocode 3
            shearBucklingVerificationNeeded).'''
        return self.get('e')
    
SHS= en_10210_shapes_dictionaries.SHS
    
class SHSShape(structural_steel.QHShape):
    ''' Square hollow structural section.
    '''
    def __init__(self,steel,name):
        super(SHSShape,self).__init__(steel,name,SHS)
        
    def t(self):
        '''Return the wall thickness'''
        return self.get('e')    

    def tw(self):
        ''' Return the web thickness (used in Eurocode 3
            shearBucklingVerificationNeeded).'''
        return self.get('e')

CHS= en_10210_shapes_dictionaries.CHS

class CHSShape(structural_steel.CHShape):
    ''' Circular hollow section.'''
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel type.
        :param name: name of the steel shape in the shapes dictionary.
        '''
        super(CHSShape,self).__init__(steel,name,CHS)
            
    def getOutsideDiameter(self):
        ''' Returns the external diameter of the section. '''
        return self.get('D')
    
    def t(self):
        '''Return HSS nominal wall thickess'''
        return self.get('t')
    
    def getSlendernessRatio(self):
        ''' Return the slenderness ratio.'''
        return self.getOutsideDiameter()/self.t()
    
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
    
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        return 1.0/2.0 #Shear distortion constant. See E. Oñate book page 122.
    
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        return self.alphaY()
