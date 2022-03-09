# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import structural_steel
from materials.sections.structural_shapes import bs_en_10219_shapes_dictionaries

# EN 10219 cold formed square hollow tubes.

CFSHS= bs_en_10219_shapes_dictionaries.shapes['CFSHS']
for item in CFSHS:
    shape= CFSHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    e= shape['e']
    shape['alpha']= 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['Avy']= 2*0.7*h*e
    shape['Avz']= 2*0.7*b*e
    shape['Wyel']= shape['Wzel']
    shape['Wypl']= shape['Wzpl']
    
class CFSHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(CFSHSShape,self).__init__(steel,name,CFSHS)

    def t(self):
        '''Return the wall thickness.'''
        return self.get('e')
    
#  EN 10219 cold formed rectangular hollow tubes.

CFRHS= bs_en_10219_shapes_dictionaries.shapes['RHS']
for item in CFRHS:
    shape= CFRHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    e= shape['e']
    shape['alpha']= 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 2*0.7*h*e
    shape['AreaQz']= 2*0.7*b*e
    
class RHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(RHSShape,self).__init__(steel,name,CFRHS)

    def t(self):
        '''Return the wall thickness'''
        return self.get('e')

# EN 10219 cold formed circular hollow tubes.

CFCHS= bs_en_10219_shapes_dictionaries.shapes['CFCHS']
for item in CFCHS:
    shape= CFCHS[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    shape['alpha']= 1.0/2.0
    shape['G']= E/(2*(1+nu))
    shape['AreaQy']= 0.5*A
    shape['AreaQz']= 0.5*A

class CHSShape(structural_steel.SteelShape):
    def __init__(self,steel,name):
        super(CHSShape,self).__init__(steel,name,CFCHS)
    def getRho(self):
        ''' Returns mass per unit length. '''
        return self.get('P')
    def alphaY(self):
        '''Return shear shape factor with respect to local y-axis'''
        return 1.0/2.0 #Shear distortion constant. See E. Oñate book page 122.
    def alphaZ(self):
        '''Return shear shape factor with respect to local z-axis'''
        return self.alphaY()
