# -*- coding: utf-8 -*-

''' Display of diagrams (forces, moments, ...) on linear elements'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import geom
import vtk
from postprocess.xcVtk import colored_diagram as cd
from postprocess import control_vars as cv
from miscUtils import LogMessages as lmsg

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
        elemVDir= elem.getJVector3d(True) #initialGeometry= True
        if self.attributeName <> "intForce":
          attributeNameSect1= self.attributeName + 'Sect1' # Values in the start node.
          attributeNameSect2= self.attributeName + 'Sect2' # Values in the end node.
          if ('crack' in self.attributeName.lower()):
            if (self.component == 'getCF'):
              value1=elem.getProp(attributeNameSect1).getCF()
              value2=elem.getProp(attributeNameSect2).getCF()
            elif (self.component == 'getMaxSteelStress'):
              value1=elem.getProp(attributeNameSect1).getMaxSteelStress()
              value2=elem.getProp(attributeNameSect2).getMaxSteelStress()
            else:
              print 'component ',self.component,' not implemented'
          else:
            value1= getattr(elem.getProp(attributeNameSect1), self.component)
            value2= getattr(elem.getProp(attributeNameSect2), self.component)
        else:
          if(self.component == 'N'):
            value1=elem.getN1
            value2=elem.getN2
          elif((self.component == 'Qy') or (self.component == 'Vy')):
            value1=elem.getVy1
            value2=elem.getVy2
          elif((self.component == 'Qz') or (self.component == 'Vz')):
            value1=elem.getVz1
            value2=elem.getVz2
          elif(self.component == 'My'):
            value1=elem.getMy1
            value2=elem.getMy2
          elif(self.component == 'Mz'):
            value1=elem.getMz1
            value2=elem.getMz2
          elif(self.component == 'T'):
            value1=elem.getT1
            value2=elem.getT2
        if((self.component == 'Qy') or (self.component == 'Vy')):
          elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        elif((self.component == 'Qz') or (self.component == 'Vz')):
          elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'My'):
          elemVDir= elem.getKVector3d(True) # initialGeometry= True 
        elif(self.component == 'Mz'):
          elemVDir= elem.getJVector3d(True) # initialGeometry= True 
        return [elemVDir,value1,value2]

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
          for e in s.getElements:
            e.getResistingForce()
            componentData= self.getElementComponentData(e)
            indxDiagrama= self.appendDataFromElementEnds(componentData[0],e,indxDiagrama,componentData[1],componentData[2])
        self.updateLookUpTable()
        self.updateActorDiagrama()



