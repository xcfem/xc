# -*- coding: utf-8 -*-
''' Display of diagrams (forces, moments, ...) on linear elements'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import geom
import vtk
from postprocess.xcVtk.diagrams import colored_diagram as cd
from postprocess import control_vars as cv
from misc_utils import log_messages as lmsg

class ControlVarDiagram(cd.ColoredDiagram):
    '''Diagram of control var values (capacity factor values, 
    internal forces values (N,My,Mz,T,Vy,Vz), crack width values,...

    :ivar scaleFactor:     scale factor for the diagram (can be negative  
          too).
    :ivar fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
    :ivar sets: list of element sets for which the diagram will be 
          displayed.
    :param attributeName: name of the element's property that has the 
           control var in it for example as in 
           elem.getProp(attributeName).component.
    :ivar component: property to be displayed 
          (possible arguments: 'CF', 'N', 'My', 'Mz', 'wk',...)
    '''
    def __init__(self, scaleFactor, fUnitConv, sets, attributeName, component):
        ''' Diagram constructor
        '''
        super(ControlVarDiagram,self).__init__(scaleFactor,fUnitConv)
        self.conjuntos= sets
        self.attributeName= attributeName 
        self.component= component

    def getElementComponentData(self,elem):
        '''Return the data to use to represent the diagram over the element

        :param elem: element to deal with.
        :param component: component to show. 
               Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
               'T'
        '''
        # default values
        value1= 0.0
        value2= 0.0
        elemVDir= elem.getJVector3d(True) #initialGeometry= True
        if self.attributeName <> "intForce":
            attributeNameSect1= self.attributeName + 'Sect1' # Values in the start node.
            attributeNameSect2= self.attributeName + 'Sect2' # Values in the end node.
            prop1= elem.getProp(attributeNameSect1)
            prop2= elem.getProp(attributeNameSect2)
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
        else:
            values= elem.getValuesAtNodes(self.component)
            if(len(values)>1): # component found.
                value1= values[0]; value2= values[1]
        if((self.component == 'Qy') or (self.component == 'Vy')):
            elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        elif((self.component == 'Qz') or (self.component == 'Vz')):
            elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'My'):
            elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'Mz'):
            elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        return [elemVDir,value1,value2]

    def getMaxAbsComp(self):
        '''Return the maximum absolute value of the component.
        It is used only for calculating auto-scale parameter, so 
        we compare only values of the component in first node of the elements
        '''
        maxV=0
        for s in self.conjuntos:
          for e in s.elements:
            e.getResistingForce()
            componentData= abs(self.getElementComponentData(e)[1])
            if componentData > maxV:
                maxV=componentData
        return maxV
            

    def addDiagram(self):
        '''Add diagram to the scene
        '''
        self.creaEstrucDatosDiagrama()
        self.creaLookUpTable()
        self.creaActorDiagrama()
        indxDiagrama= 0
        indiceSet= 0
        numSetsDiagrama= len(self.conjuntos)
        for s in self.conjuntos:
          for e in s.elements:
            e.getResistingForce()
            componentData= self.getElementComponentData(e)
            indxDiagrama= self.appendDataFromElementEnds(componentData[0],e,indxDiagrama,componentData[1],componentData[2])
        self.updateLookUpTable()
        self.updateActorDiagrama()



