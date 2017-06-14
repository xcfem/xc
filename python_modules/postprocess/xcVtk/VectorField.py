# -*- coding: utf-8 -*-

'''Field vector over points'''

import math
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import field_base as fb
from postprocess.xcVtk import VectorFieldData as vfd

class VectorField(fb.FieldBase):
  '''Vector field defined at points.'''
  def __init__(self,name,fUnitConv,scaleFactor,showPushing= True):
    super(VectorField,self).__init__(name,fUnitConv)
    self.data= vfd.VectorFieldData(name,3,scaleFactor)
    self.showPushing= showPushing #If true vector push else pulls from point


  def setupMapper(self):
    self.data.setupGlyph(self.fUnitConv)
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

  def addToDisplay(self, recordDisplay):
    # Adds the vector field to the display.
    self.setupActor()
    recordDisplay.renderer.AddActor(self.actor)
    self.creaColorScaleBar()
    # mapper2D= vtk.vtkPolyDataMapper2D()
    # self.scalarBar.SetMapper(mapper2D)
    recordDisplay.renderer.AddActor2D(self.scalarBar)
