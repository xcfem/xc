# -*- coding: utf-8 -*-

'''Graphic representation of fields. Base class'''

import vtk
from xcVtk import vtk_lut_field

class FieldBase(vtk_lut_field.LUTField):
  '''Scalar field defined at nodes.'''
  def __init__(self,name,fUnitConv):
    super(FieldBase,self).__init__(fUnitConv)
    self.name= name
