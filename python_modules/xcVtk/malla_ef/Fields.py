# -*- coding: utf-8 -*-

'''Graphic representation of fields over the mesh.'''

import vtk
from xcVtk import vtk_lut_field
from postprocess import extrapolate_elem_attr

class ScalarField(vtk_lut_field.LUTField):
  '''Scalar field defined at nodes.'''
  def __init__(self,name,vExpr,component,fUnitConv):
    super(ScalarField,self).__init__(fUnitConv)
    self.name= name
    self.attrName= vExpr
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
      tmp*= self.fConvUnidades
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
  def __init__(self,name,vExpr,component,fUnitConv,elementSetName):
    super(ExtrapolatedScalarField,self).__init__(name,vExpr,component,fUnitConv)
    self.elementSetName= elementSetName

  def plot(self,preprocessor,defDisplay,fName):
    elementSet= preprocessor.getSets.getSet(self.elementSetName).getElements
    extrapolate_elem_attr.extrapolate_elem_function_attr(elementSet,self.name,"getProp", self.name)
    defDisplay.displayMesh(preprocessor, self.elementSetName,self,None,fName)
