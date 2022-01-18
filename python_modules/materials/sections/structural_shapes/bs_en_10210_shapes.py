# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import structural_steel
from materials.sections.structural_shapes import bs_en_10210_shapes_dictionaries

''' Eurocode 3 (in accordance with the UK National Annex) (BS EN 10210-2: 2006).'''

HFSHS= bs_en_10210_shapes_dictionaries.HFSHS
for item in HFSHS:
    shape= HFSHS[item]
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

class HFSHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(HFSHSShape,self).__init__(steel,name,HFSHS)

    def t(self):
        '''Return the wall thickness.'''
        return self.get('e')
