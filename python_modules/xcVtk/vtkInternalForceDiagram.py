# -*- coding: utf-8 -*-

''' Display of diagrams (forces, moments, ...) on linear elements'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega@ciccp.es, l.pereztato@ciccp.es"

import geom
import vtk
from xcVtk import ColoredDiagram as cd
from postprocess import ControlVars as cv
from miscUtils import LogMessages as lmsg

class InternalForceDiagram(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,component):
    ''' Diagram constructor

        :ivar scale:     scale factor for the diagram (can be negative too).
        :ivar fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :ivar sets:      list of element sets for which the diagram will be displayed.
        :ivar component: property to be displayed 
                         (possible arguments: 'N', 'My', 'Mz'Vz,...)
    '''

    super(InternalForceDiagram,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.component= component

  def getElementComponentData(self,elem):
    '''Returns the data to use to represent the diagram over the element
 
       :param elem: element to deal with.
       :param component: component to represent:
    '''
    # default values.
    elemVDir= elem.getJVector3d(True) #initialGeometry= True
    value1= 0.0
    value2= 0.0
    if(self.component == 'N'):
      value1= elem.getN1
      value2= elem.getN2
    elif(self.component == 'Qy'):
      elemVDir= elem.elem.getJVector3d(True) # initialGeometry= True 
      value1= elem.getVy1
      value2= elem.getVy2
    elif(self.component == 'Qz'):
      elemVDir= elem.elem.getKVector3d(True) # initialGeometry= True 
      value1= elem.getVz1
      value2= elem.getVz2
    elif(self.component == 'T'):
      value1= elem.getT1
      value2= elem.getT2
    elif(self.component == 'My'):
      elemVDir= elem.elem.getKVector3d(True) # initialGeometry= True 
      value1= elem.getMy1
      value2= elem.getMy2
    elif(self.component == 'Mz'):
      elemVDir= elem.elem.getJVector3d(True) # initialGeometry= True 
      value1= elem.getMz1
      value2= elem.getMz2
    else:
      lmsg.warning("'component :'"+ self.component+ "' unknown.")
    return [elemVDir,value1,value2]
    

  def agregaDiagrama(self):
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



