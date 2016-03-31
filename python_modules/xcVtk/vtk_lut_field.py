 # -*- coding: utf-8 -*-

''' Drawing of diagrams showing a law of internal 
    forces (or any other input) on linear elements
'''
__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AOO)"
__cppyright__= "Copyright 2015, LCPT AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import vtk

class LUTField(object):
  '''Provides de variables involved in the drawing of a
  diagram showing a law of internal forces (or any other input)
  on linear elements
  Attributes:
    fUnitConv:  units conversion factor
    lookUpTable:object that is used by mapper objects to map scalar values
                into rga (red-green-blue-alpha transparency) color specification
                or rga into scalar values. 
    scalarBar:  legend that indicates to the viewer the correspondence between 
                color value and data value.
    mapper:     object that specifies the interface between data and graphic 
                primitives.
    actor:      used to represent an entity in a rendering scene. The actor has position, 
                orientation, scaling, a reference to the defining geometry, rendering 
                properties and possibly a texture map.
  '''
  def __init__(self,fUnitConv):
    self.initializeMinMax()
    self.lookUpTable= None
    self.scalarBar= None
    self.fConvUnidades= fUnitConv
    self.mapper= None
    self.actor= None

  def updateMinMax(self,value):
    self.valMin= min(self.valMin,value)
    self.valMax= max(self.valMax,value)

  def initializeMinMax(self,value= 1.0e99):
    '''Initialize minimum and maximum values that hopefully
       will be replaced by the real ones.''' 
    self.valMin= value #Extremely BIG (yes BIG) positive value.
    self.valMax= -self.valMin #Extremely BIG negative value.

  def creaLookUpTable(self):
    ''' Creates a  lookUpTable, that is an object used by mapper objects 
    to map scalar values into rga (red-green-blue-alpha transparency) color 
    specification or rga into scalar values. 
    '''
    self.lookUpTable= vtk.vtkLookupTable()
    self.lookUpTable.SetNumberOfTableValues(1024)
    self.lookUpTable.SetHueRange(0.667,0)
    # self.lookUpTable.SetTableRange(valMin,valMax)
    self.lookUpTable.Build()


  def updateLookUpTable(self):
    '''Sets the minimum and maximum scalar values for scalar mapping.
    Scalar values less than minimum and greater than maximum range values 
    are respectively clamped to those minimum and maximum range values
    '''
    self.lookUpTable.SetTableRange(self.valMin,self.valMax)
    self.lookUpTable.Build()

  def updateActorDiagrama(self):
    '''Updates de actor
    '''
    self.mapper.SetScalarRange(self.valMin,self.valMax)

  def creaColorScaleBar(self):
    '''Creates the scalar bar that indicates to the viewer the correspondence
    between color values and data values
    '''

    self.scalarBar= vtk.vtkScalarBarActor()

    pos= self.scalarBar.GetPositionCoordinate()
    pos.SetCoordinateSystemToNormalizedViewport()
    pos.SetValue(0.1,0.1)        

    self.scalarBar.SetOrientationToHorizontal()
    self.scalarBar.SetWidth(0.8)
    self.scalarBar.SetHeight(0.125)
    self.scalarBar.SetLookupTable(self.lookUpTable)
    self.scalarBar.Modified()
    return self.scalarBar

