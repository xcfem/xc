# -*- coding: utf-8 -*-

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

'''Field vector over points'''

import math
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import field_base as fb
from postprocess.xcVtk import vector_field_data as vfd

class VectorField(fb.FieldBase):
  '''Vector field defined at points.'''
  def __init__(self,name,fUnitConv,scaleFactor,showPushing= True,symType=vtk.vtkArrowSource()):
    super(VectorField,self).__init__(name,fUnitConv)
    self.data= vfd.VectorFieldData(name,3,scaleFactor)
    self.showPushing= showPushing #If true vector push else pulls from point
    self.symType=symType

  def setupMapper(self):
    self.data.setupGlyph(self.fUnitConv,self.symType)
    self.mapper = vtk.vtkPolyDataMapper()
    self.mapper.SetInputConnection(self.data.glyph.GetOutputPort())
    self.mapper.SetScalarModeToUsePointFieldData()
    self.mapper.SetColorModeToMapScalars()
    self.mapper.ScalarVisibilityOn()
    #self.mapper.SetScalarRange(self.glyph.GetOutputDataObject(0).GetPointData().GetArray(self.lenghtsName).GetRange())
    self.mapper.SetScalarRange(self.data.lengths.GetRange())
    self.mapper.SelectColorArray(self.data.lenghtsName)
    self.creaLookUpTable()
    self.mapper.SetLookupTable(self.lookUpTable)
    return self.mapper

  def setupActor(self):
    self.setupMapper()
    self.actor = vtk.vtkActor()
    self.actor.SetMapper(self.mapper)
    return self.actor

  def addToDisplay(self, recordDisplay, orientation= 'H'):
    # Adds the vector field to the display.
    nTuples= self.data.getNumberOfTuples()
    if(nTuples>0):
      self.setupActor()
      recordDisplay.renderer.AddActor(self.actor)
      self.creaColorScaleBar(orientation)
      # mapper2D= vtk.vtkPolyDataMapper2D()
      # self.scalarBar.SetMapper(mapper2D)
      recordDisplay.renderer.AddActor2D(self.scalarBar)
    else:
      lmsg.warning('vector field: no vectors to display. Command ignored.')
