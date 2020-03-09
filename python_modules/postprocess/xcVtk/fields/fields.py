# -*- coding: utf-8 -*-

'''Graphic representation of fields over the mesh.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import vtk
from miscUtils import log_messages as lmsg
from postprocess.xcVtk.fields import field_base as fb
from postprocess import extrapolate_elem_attr
from postprocess import control_vars as cv

class ScalarField(fb.FieldBase):
  '''Scalar field defined at nodes.

    :ivar rgMinMax: range (vmin,vmax) with the maximum and minimum values of 
              the field to be represented. All the values less than vmin are 
              displayed in blue and those greater than vmax iqn red
              (defaults to None)

  '''
  def __init__(self,name, functionName, component=None, fUnitConv= 1.0,rgMinMax=None):
    super(ScalarField,self).__init__(name,fUnitConv)
    self.attrName= functionName
    self.attrComponent= component
    if(rgMinMax):
      self.rgMinMax= [1.0,1.0]
      # Convert the range units.
      self.rgMinMax[0]= rgMinMax[0]*fUnitConv
      self.rgMinMax[1]= rgMinMax[1]*fUnitConv
    else:
      self.rgMinMax= None
    self.arr= None

  def fillArray(self, nodeSet):
    '''Creates an vtkDoubleArray filled with the proper values.
 
   '''
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
      if not(self.rgMinMax):
        self.updateMinMax(tmp)
      else:
        self.updateMinMaxWithinRange(tmp,self.rgMinMax)
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
  '''Scalar field defined at nodes.
  :ivar rgMinMax: range (vmin,vmax) with the maximum and minimum values  
              of the scalar field (if any) to be represented. All the values 
              less than vmin are displayed in blue and those greater than vmax 
              in red (defaults to None)
  '''
  def __init__(self,name, functionName, xcSet, component= None, fUnitConv= 1.0,rgMinMax=None):
    super(ExtrapolatedScalarField,self).__init__(name,functionName,component,fUnitConv,rgMinMax)
    self.xcSet= xcSet

  def display(self,defDisplay,caption= '',fileName= None, defFScale=0.0):
    '''Display the scalar field graphic.

      :param fileName: name of the graphic file to create (if None -> screen window).
      :param caption: text to display in the graphic.
      :param defFScale: factor to apply to current displacement of nodes 
                  so that the display position of each node equals to
                  the initial position plus its displacement multiplied
                  by this factor. (Defaults to 0.0, i.e. display of 
                  initial/undeformed shape)
    '''
    defDisplay.displayMesh(xcSets= self.xcSet,field= self, diagrams= None,caption= caption, fileName= fileName, defFScale= defFScale)
    
class ExtrapolatedProperty(ExtrapolatedScalarField):
  '''Scalar field defined as property value at nodes.'''
  def __init__(self,name,functionName,xcSet, component= None,fUnitConv= 1.0,rgMinMax=None):
    super(ExtrapolatedProperty,self).__init__(name,functionName, xcSet, component, fUnitConv,rgMinMax)

  def extrapolate(self):
    extrapolate_elem_attr.extrapolate_elem_function_attr(self.xcSet.elements,self.name,"getProp", self.name)

  def display(self,defDisplay,caption= '',fileName= None, defFScale=0.0):
    ''' Display the field.

       :param fileName: name of the graphic file to create (if None -> screen window).
      :param caption: text to display in the graphic.
    '''

    self.extrapolate()
    defDisplay.displayMesh(self.xcSet,field= self, diagrams= None, caption= caption, fileName= fileName, defFScale= defFScale)

def getScalarFieldFromControlVar(attributeName,argument,xcSet,component,fUnitConv,rgMinMax):
  '''return an scalar field that represents the control var over the 
               elements in the set.

     :param attributeName: name of the element's property that has the 
                           control var in it for example as in 
                           elem.getProp(attributeName).eval(argument).
     :param argument: name of the control var to represent.
     :param xcSet: represent the field over those elements.
     :param component: component of the control var to represent.
     :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
  '''
  nodePropName= cv.extrapolate_control_var(xcSet.elements,attributeName,argument)
  return ExtrapolatedScalarField(nodePropName,"getProp",xcSet,component,fUnitConv,rgMinMax)
