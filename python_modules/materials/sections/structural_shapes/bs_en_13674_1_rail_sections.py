# -*- coding: utf-8 -*-
''' EN 13674-1 Flat bottom rails >46kg/m.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import structural_steel
from materials.sections.structural_shapes import bs_en_13674_1_rail_sections_dictionaires

# Micropile circular hollow tubes.

RAIL=  bs_en_13674_1_rail_sections_dictionaires.shapes['RAIL']
for item in RAIL:
    shape= RAIL[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['foot_width']
    shape['b']= b
    h= shape['h']
    shape['alpha']= 1.0/2.0
    shape['G']= E/(2*(1+nu))

class RailShape(structural_steel.IShape):
    ''' Rail profile according to BS EN 13674-1.'''
    def __init__(self,steel,name):
        ''' Constructor.

        :param steel: steel object (e.g. S275JR)
        :param name: name identifying the section in the table
        :param table: module containing a dictionary with mechanical 
                      characteristics of a series of shapes.
        '''
        super().__init__(steel= steel, name= name, table= RAIL)
        
    
    def getWarpingConstant(self):
        ''' Return the value of the section warping constant.'''
        return self.get('Cw')
