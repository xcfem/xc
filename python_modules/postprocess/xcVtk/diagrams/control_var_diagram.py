# -*- coding: utf-8 -*-
''' Display of diagrams (forces, moments, ...) on linear elements'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import sys
from postprocess.xcVtk.diagrams import colored_diagram as cd
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg

class ControlVarDiagram(cd.ColoredDiagram):
    '''Diagram of control var values (capacity factor values,
    internal forces values (N,My,Mz,T,Vy,Vz), crack width values,...

    :ivar sets: list of element sets for which the diagram will be
          displayed.
    :ivar attributeName: name of the element's property that has the
           control var in it for example as in
           elem.getProp(attributeName).component.
    :ivar component: property to be displayed
          (possible arguments: 'CF', 'N', 'My', 'Mz', 'wk',...)
    :ivar defaultDirection: default direction of the diagram (J: element local
                            j vector or K: element local K vector).
    :ivar rgMinMax: range (vmin,vmax) with the maximum and minimum values of
              the field to be represented. All the values less than vmin are
              displayed in blue and those greater than vmax iqn red
              (defaults to None)
    :ivar lRefModSize: reference length of the model (how big the model is).
                       Used to autoscale the diagram.
    '''
    def __init__(self, scaleFactor, fUnitConv, sets, attributeName, component, defaultDirection= 'J', rgMinMax= None, lRefModSize= 1.0):
        ''' Diagram constructor

        :param scaleFactor: scale factor for the diagram (can be negative too).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :param sets:      list of element sets for which the diagram will be displayed.
        :param attributeName: name of the property that containst the control
                              variable.
        :param component: component of the control variable to be displayed
                         (possible arguments: 'N', 'My', 'Mz'Vz,...).
        :param defaultDirection: default direction of the diagram (J: element
                                 local j vector or K: element local K vector).
        :param rgMinMax: range (vmin,vmax) with the maximum and minimum values
                         of the scalar field (if any) to be represented. All
                         the values less than vmin are displayed in blue and
                         those greater than vmax in red (defaults to None)
        :param lRefModSize: reference length of the model (how big the model
                            is). Used to autoscale the diagram.
        '''
        super(ControlVarDiagram,self).__init__(scaleFactor= scaleFactor,fUnitConv= fUnitConv, rgMinMax= rgMinMax)
        self.elemSets= sets
        self.attributeName= attributeName
        self.component= component
        self.defaultDirection= defaultDirection
        self.lRefModSize= lRefModSize

    def getPropertyNames(self):
        ''' Return the names of the properties used to construct the diagram.'''
        retval= list()
        if(self.attributeName != "intForce"): # not internal forces.
            retval.append(self.attributeName + 'Sect1') # Value at start node.
            retval.append(self.attributeName + 'Sect2') # Value at end node.
        return retval;

    def getElementComponentData(self, elem, propertyNames):
        '''Return the data to be used to represent the diagram over the element

        :param elem: element to get the components for.
        :param propertyNames: names of the properties that contain the values
                              to represent in the diagram.
        '''
        # default values
        elemVDir= None
        if(self.defaultDirection):
            if(self.defaultDirection=='J'): # local J vector.
                elemVDir= elem.getJVector3d(True) #initialGeometry= True
            else: # local K vector.
                elemVDir= elem.getKVector3d(True) #initialGeometry= True
        if(len(propertyNames)>0):
            prop1= elem.getProp(propertyNames[0])
            prop2= elem.getProp(propertyNames[1])
            if(prop1 and prop2):
                if('crack' in self.attributeName.lower()):
                    if (self.component == 'getCF'):
                        value1=prop1.getCF()
                        value2=prop2.getCF()
                    elif (self.component == 'getMaxSteelStress'):
                        value1=prop1.getMaxSteelStress()
                        value2=prop2.getMaxSteelStress()
                    else:
                        value1= getattr(prop1, self.component)
                        value2= getattr(prop2, self.component)
                else:
                    value1= getattr(prop1, self.component)
                    value2= getattr(prop2, self.component)
            else:
                lmsg.error('attribute name: '+ self.attributeName+ ' not found.')
                value1= None
                value2= None
        else: # internal forces.
            [elemVDir,value1,value2]= cv.get_element_internal_force_component_data(elem, self.component, self.defaultDirection)
        return [elemVDir,value1,value2]

    def filterElements(self, propertyNames):
        ''' Return the elements that actually have the required properties.

        :param propertyNames: names of the properties that contain the values
                              to represent in the diagram.
        '''
        retval= list()
        refused_elements= list()
        if(len(propertyNames)>0):
            for s in self.elemSets:
                for e in s.elements:
                    addElement= True
                    for propName in propertyNames:
                        if(not e.hasProp(propName)):
                            addElement= False
                            break;
                    if(addElement):
                        retval.append(e)
                    else:
                        refused_elements.append(e)
        else:
            for s in self.elemSets:
                for e in s.elements:
                    retval.append(e)
        if(len(refused_elements)>0):
            tag_lst= list()
            for e in refused_elements:
                tag_lst.append(e.tag)
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            warningMsg= '; elements: '+str(tag_lst) + ' do not have the required properties: '+str(propertyNames)
            lmsg.warning(className+'.'+methodName+warningMsg)
        return retval

    def computeDiagramValues(self):
        ''' Compute the values needed to create the diagram representation.'''
        self.dirVectors= list()
        self.valueCouples= list()
        self.elements= list()
        propertyNames= self.getPropertyNames()
        filtered_elements= self.filterElements(propertyNames= propertyNames)
        for e in filtered_elements:
            e.getResistingForce()
            componentData= self.getElementComponentData(e, propertyNames= propertyNames)
            self.dirVectors.append(componentData[0])
            self.valueCouples.append((componentData[1], componentData[2]))
            self.elements.append(e)
        if(self.rgMinMax): # filter values.
            self.valueCouples= self.filterValueCouples(self.valueCouples)
            lmsg.warning('Displayed values have been clipped whitin the range: ('+str(self.rgMinMax[0])+', '+str(self.rgMinMax[1])+"), so they don't correspond to the computed ones.")
        self.autoScale() # Update scale.
        # Return the number of computed values.
        return len(self.valueCouples)

    def getMaxAbsComp(self):
        '''Return the maximum absolute value of the component.
        It is used only for calculating auto-scale parameter, so
        we compare only values of the component in first node of the elements
        '''
        maxV= 0.0
        for (v0, v1) in self.valueCouples:
            maxV= max(abs(v0), maxV)
            maxV= max(abs(v1), maxV)
        return maxV

    def autoScale(self):
        ''' Autoscale the diagram. '''
        maxAbsComp= self.getMaxAbsComp()
        if(maxAbsComp>0):
            self.scaleFactor*= 0.15*self.lRefModSize/(maxAbsComp*self.fUnitConv)

    def clear(self):
        ''' Clear the diagram data.'''
        self.dirVectors.clear()
        self.elements.clear()
        self.valueCouples.clear()

    def addDiagram(self):
        '''Add diagram to the scene.'''
        numberOfComputedValues= self.computeDiagramValues()

        if(numberOfComputedValues>0):
            self.createDiagramDataStructure()
            diagramIndex= 0
            diagramIndex= self.appendDataFromElementEnds(dirVectors= self.dirVectors, elements= self.elements, diagramIndex= diagramIndex, valueCouples= self.valueCouples)
            self.createLookUpTable()
            self.createDiagramActor()
            self.clear()
            retval= True
        else:
            retval= False
        return retval
