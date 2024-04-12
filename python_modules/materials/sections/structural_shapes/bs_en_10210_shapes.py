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

class HFSHSShape(structural_steel.QHShape):
    def __init__(self,steel,name):
        super(HFSHSShape,self).__init__(steel,name,HFSHS)

    def t(self):
        '''Return the wall thickness.'''
        return self.get('e')

    def tw(self):
        ''' Return the web thickness (used in Eurocode 3
            shearBucklingVerificationNeeded).'''
        return self.get('e')
