 # -*- coding: utf-8 -*-

''' Representación de diagramas de esfuerzos (o de otras magnitudes)
   sobre elementos lineales. '''

import vtk

''' Define las variables que se emplean para definir
   un diagrama de esfuerzos. '''
class LUTField(object):
  lookUpTable= None
  scalarBar= None
  valMin= 1e99
  valMax= -1e99
  fConvUnidades= 1e-3 #Factor de conversión de unidades.
  mapper= None
  actor= None

  def __init__(self,fUnitConv):
    self.valMin= 1e99
    self.valMax= -self.valMin
    self.lookUpTable= None
    self.scalarBar= None
    self.fConvUnidades= fUnitConv
    self.mapper= None
    self.actor= None

  def updateMinMax(self,value):
    self.valMin= min(self.valMin,value)
    self.valMax= max(self.valMax,value)

  def creaLookUpTable(self):
    ''' Crea una tabla de búsqueda de colores.'''
    self.lookUpTable= vtk.vtkLookupTable()
    self.lookUpTable.SetNumberOfTableValues(1024)
    self.lookUpTable.SetHueRange(0.667,0)
    # self.lookUpTable.SetTableRange(valMin,valMax)
    self.lookUpTable.Build()


  def updateLookUpTable(self):
    # Actualiza la tabla de búsqueda de colores.
 
    self.lookUpTable.SetTableRange(self.valMin,self.valMax)
    self.lookUpTable.Build()

  def updateActorDiagrama(self):
    # Actualiza el actor para el diagrama.
    self.mapper.SetScalarRange(self.valMin,self.valMax)

  def creaEscalaColores(self):
    # Crea la escala de colores.

    self.scalarBar= vtk.vtkScalarBarActor()

    pos= self.scalarBar.GetPositionCoordinate()
    pos.SetCoordinateSystemToNormalizedViewport()
    pos.SetValue(0.1,0.1)        

    self.scalarBar.SetOrientationToHorizontal()
    self.scalarBar.SetWidth(0.8)
    self.scalarBar.SetHeight(0.125)
    self.scalarBar.SetLookupTable(self.lookUpTable)
    self.scalarBar.Modified()


