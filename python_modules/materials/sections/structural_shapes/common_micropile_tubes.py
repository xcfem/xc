# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import structural_steel
from materials.sections.structural_shapes import common_micropile_shapes_dictionaries

# Micropile circular hollow tubes.

MP=  common_micropile_shapes_dictionaries.shapes['MP']
for item in MP:
    shape= MP[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    shape['alpha']= 1.0/2.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 0.5*A
    shape['AreaQz']= 0.5*A

class MicropileTubeShape(structural_steel.SteelShape):
    ''' Cold formed circular hollow sections according to 
        BS EN 2019.'''
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel material.
        :param name: shape name (i.e. '"CFSHS350x350x12.5')
        '''
        super(MicropileTubeShape, self).__init__(steel,name,MP)
        
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
    
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        return 1.0/2.0 #Shear distortion constant. See E. Oñate book page 122.
    
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        return self.alphaY()
