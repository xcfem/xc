# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

'''Graphic representation of fields over the mesh.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


from postprocess.xcVtk.fields import field_base as fb
from postprocess import extrapolate_elem_attr
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg
from vtk.vtkCommonCore import vtkDoubleArray

class ScalarField(fb.FieldBase):
    '''Scalar field defined at nodes.

      :ivar rgMinMax: range (vmin,vmax) with the maximum and minimum values of 
                the field to be represented. All the values less than vmin are 
                displayed in blue and those greater than vmax iqn red
                (defaults to None)

    '''
    def __init__(self, name, functionName, component=None, fUnitConv= 1.0,rgMinMax= None):
        ''' Constructor.

        :param name: node property that contains the value to represent.
        :param functionName: name of the node method to obtain the value (usually 'getProp').
        :param component: component of the control var to represent (None if the property
                          is already an scalar).
        :param fUnitConv: unit conversion factor.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All 
                         the values less than vmin are displayed in blue and 
                         those greater than vmax in red (defaults to None)
        '''
        super(ScalarField,self).__init__(name= name, fUnitConv= fUnitConv)
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

    def getPropertyName(self):
        ''' Return the name of the property that will store the
            field values.'''
        return self.name

    def _fill_vtk_double_array(self, nodeSet):
        '''Creates an vtkDoubleArray filled with the proper values.

        :param nodeSet: set of nodes that will provide the values
                        to populate the array.
        '''
        # Scalar values.
        self.arr= vtkDoubleArray()
        self.arr.SetName(self.name)
        self.arr.SetNumberOfTuples(len(nodeSet))
        self.arr.SetNumberOfComponents(1)
        propertyName= self.getPropertyName()
        for n in nodeSet:
            attr= getattr(n,self.attrName)
            tmp= None
            if hasattr(attr,"__getitem__"):
                tmp= attr[self.attrComponent]
            elif callable(attr):
                if(attr.__name__!='getProp'):
                    tmp= attr(propertyName)
                elif(n.hasProp(propertyName)):
                    tmp= attr(propertyName)
                else:
                    tmp= 0.0
            else:
                tmp= attr
            if(hasattr(tmp,"__getitem__")):
                tmp= tmp[self.attrComponent]
            tmp*= self.fUnitConv
            if not(self.rgMinMax):
                self.updateMinMax(tmp)
            else:
                self.updateMinMaxWithinRange(tmp,self.rgMinMax)
                lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+"), so they don't correspond to the computed ones.")
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

    '''
    def __init__(self,name, functionName, xcSet, component= None, fUnitConv= 1.0,rgMinMax=None):
        ''' Constructor.

        :param name: node property that contains the value to represent.
        :param functionName: name of the node method to obtain the value (usually 'getProp').
        :param xcSet: elements in the field domain.
        :param component: component of the control var to represent.
        :param fUnitConv: unit conversion factor.
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                         of the scalar field (if any) to be represented. All the values 
                         less than vmin are displayed in blue and those greater than vmax 
                         in red (defaults to None)
        '''
        super(ExtrapolatedScalarField,self).__init__(name,functionName,component,fUnitConv,rgMinMax)
        self.xcSet= xcSet

    def display(self, displaySettings, caption= '', unitDescription= '', fileName= None, defFScale=0.0):
        '''Display the scalar field graphic.

        :param fileName: name of the graphic file to create 
                         (if None -> screen window).
        :param caption: text to display in the graphic.
        :param unitDescription: description of the units.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        displaySettings.displayMesh(xcSets= self.xcSet, field= self, diagrams= None, caption= caption, unitDescription= unitDescription, fileName= fileName, defFScale= defFScale)
    
class ExtrapolatedProperty(ExtrapolatedScalarField):
    '''Scalar field defined as property value at nodes.'''
    def __init__(self,name,functionName,xcSet, component= None,fUnitConv= 1.0,rgMinMax=None):
        super(ExtrapolatedProperty,self).__init__(name,functionName, xcSet, component, fUnitConv,rgMinMax)

    def extrapolate(self):
        extrapolate_elem_attr.extrapolate_elem_function_attr(self.xcSet.elements, self.name, "getProp", self.name)

    def display(self,displaySettings, caption= '', unitDescription= '', fileName= None, defFScale=0.0):
        ''' Display the field.

        :param fileName: name of the graphic file to create (if None -> screen window).
        :param caption: text to display in the graphic.
        :param unitDescription: description of the units.
	:param defFScale: deformation scale factor. Factor to apply to the
			  current displacement of the nodes so that the 
			  displayed position of each node equals to
			  the initial position plus its displacement 
			  multiplied by this factor. (Defaults to 0.0, i.e. 
			  display the initial/undeformed shape).
        '''
        self.extrapolate()
        displaySettings.displayMesh(self.xcSet, field= self, diagrams= None, caption= caption, unitDescription= unitDescription, fileName= fileName, defFScale= defFScale)

def get_scalar_field_from_control_var(attributeName, argument, xcSet, component, fUnitConv, rgMinMax):
    '''return an scalar field that represents the control var over the 
                 elements in the set.

    :param attributeName: name of the element's property that has the 
                          control var in it, for example as in 
                          elem.getProp(attributeName).eval(argument).
    :param argument: name of the control var to represent.
    :param xcSet: represent the field over those elements.
    :param component: component of the control var to represent.
    :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
    :param rgMinMax: range (vmin,vmax) with the maximum and minimum values  
                of the scalar field (if any) to be represented  (in units 
                of calculation, not units of display). All the values 
                less than vmin are displayed in blue and those greater than vmax 
                in red (defaults to None)
    '''
    nodePropName= cv.extrapolate_control_var(elemSet= xcSet, propName= attributeName, argument= argument)
    return ExtrapolatedScalarField(nodePropName,"getProp",xcSet,component,fUnitConv,rgMinMax)
