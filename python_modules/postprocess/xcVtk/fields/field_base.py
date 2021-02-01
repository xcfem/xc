# -*- coding: utf-8 -*-
'''Graphic representation of fields. Base class'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import vtk
from postprocess.xcVtk.fields import vtk_lut_field

class FieldBase(vtk_lut_field.LUTField):
    '''Base classs for fields.'''
    def __init__(self,name,fUnitConv):
        super(FieldBase,self).__init__(fUnitConv)
        self.name= name
