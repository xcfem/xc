# -*- coding: utf-8 -*-

'''Graphic representation of fields over the mesh.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import vtk
from miscUtils import LogMessages as lmsg
from xcVtk import FieldBase as fb
from postprocess import extrapolate_elem_attr
from postprocess import ControlVars as cv

class ScalarField(fb.FieldBase):
  '''Scalar field defined at nodes.'''
  def __init__(self,name, functionName, component=None, fUnitConv= 1.0):
    super(ScalarField,self).__init__(name,fUnitConv)
    self.attrName= functionName
    self.attrComponent= component
    self.arr= None

  def fillArray(self, nodeSet):
    '''Creates an vtkDoubleArray filled with the proper values.'''
    # Scalar values.
    self.arr= vtk.vtkDoubleArray()
    self.arr.SetName(self.name)
    self.arr.SetNumberOfTuples(len(nodeSet))
    self.arr.SetNumberOfComponents(1)
    for n in nodeSet:
      attr= getattr(n,self.attrName)
      tmp= None
      if hasattr(attr,"__getitem__"):
        tmp= attr[self.attrComponent]
      elif callable(attr):
        if(attr.__name__!='getProp'):
          tmp= attr(self.name)
        elif(n.hasProp(self.name)):
          tmp= attr(self.name)
        else:
          tmp= 0.0
      else:
        tmp= attr
      if hasattr(tmp,"__getitem__"):
        tmp= tmp[self.attrComponent]
      tmp*= self.fUnitConv
      self.updateMinMax(tmp)
      self.arr.SetTuple1(n.getIdx,tmp)
    return self.arr

  def setupOnGrid(self,uGrid):
    uGrid.GetPointData().SetScalars(self.arr)
    uGrid.GetPointData().SetActiveScalars(self.name)
    uGrid.Modified()

  def setupOnMapper(self,gridMapper):
    gridMapper.SetScalarRange(self.valMin,self.valMax)
    gridMapper.SetLookupTable(self.lookUpTable)
    gridMapper.SetScalarModeToUsePointData()
    gridMapper.ScalarVisibilityOn()


class ExtrapolatedScalarField(ScalarField):
  '''Scalar field defined at nodes.'''
  def __init__(self,name, functionName, xcSet, component= None, fUnitConv= 1.0):
    super(ExtrapolatedScalarField,self).__init__(name,functionName,component,fUnitConv)
    self.xcSet= xcSet

  def display(self,defDisplay,fName= None,caption= ''):
    defDisplay.displayMesh(self.xcSet,self,None,fName,caption)
  def plot(self,defDisplay,fName= None,caption= ''):
    lmsg.warning('ExtrapolatedScalarField.plot is DEPRECATED use display.')
    self.display(defDisplay,fName,caption)

class ExtrapolatedProperty(ExtrapolatedScalarField):
  '''Scalar field defined as property value at nodes.'''
  def __init__(self,name,functionName,xcSet, component= None,fUnitConv= 1.0):
    super(ExtrapolatedProperty,self).__init__(name,functionName, xcSet, component, fUnitConv)

  def extrapolate(self):
    extrapolate_elem_attr.extrapolate_elem_function_attr(self.xcSet.getElements,self.name,"getProp", self.name)

  def display(self,defDisplay,fName= None,caption= ''):
    self.extrapolate()
    defDisplay.displayMesh(self.xcSet,self,None,fName,caption)
  def plot(self,defDisplay,fName= None,caption= ''):
    lmsg.warning('ExtrapolatedProperty.plot is DEPRECATED use display.')
    self.display(defDisplay,fName,caption)

def getScalarFieldFromControlVar(attributeName,argument,xcSet,component,fUnitConv):
  nodePropName= cv.extrapolate_control_var(xcSet.getElements,attributeName,argument)
  return ExtrapolatedScalarField(nodePropName,"getProp",xcSet,component,fUnitConv)
