 # -*- coding: utf-8 -*-

''' Diagram display a property defined over linear elements. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
import vtk
from xcVtk import ColoredDiagram as cd
from postprocess import extrapolate_elem_attr




class ElementPropertyDiagram(cd.ColoredDiagram):
  '''Diagram of element properties'''
  envelopes= set(['N+','N-','My+','My-','Mz+','Mz-','Vy+','Vy-','Vz+','Vz-','T+','T-'])

  def __init__(self,scaleFactor,fUnitConv,sets,propertyName):
    '''Diagram that represents a property value over several sets of elements.

       :param scaleFactor: scale factor (size of the diagram).
       :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
       :param propertyName: name of the element's property to represent.
       :param sets: represent the field over those element sets.
    '''
    super(ElementPropertyDiagram,self).__init__(scaleFactor,fUnitConv)
    self.conjuntos= sets
    self.propertyName= propertyName

  def appendDataToDiagram(self, eSet,indxDiagrama,deform=False):
    ''' Append property values to diagram .
        eSet: Element set.
       :param deform: =True for display of current/deformed shape (defaults
                       to False, i.e. display of initial/undeformed shape) 
    '''
    elems= eSet.getElements
    if(self.propertyName=='FCTNCP'):
      for e in elems:
        self.vDir= e.getVDirWeakAxisGlobalCoord()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='FCVCP'):
      for e in elems:
        self.vDir= e.getVDirWeakAxisGlobalCoord()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='N+'):
      for e in elems:
        self.vDir= e.getVDirWeakAxisGlobalCoord()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='N-'):
      for e in elems:
        self.vDir= e.getVDirWeakAxisGlobalCoord()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='My+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='My-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Mz+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Mz-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Vy+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Vy-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Vz+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    elif(self.propertyName=='Vz-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1],deform)      
    else:
      extrapolate_elem_attr.extrapolate_elem_function_attr(elems,self.propertyName,"getProp", self.propertyName)
      for e in elems:
        self.vDir= e.getVDirWeakAxisGlobalCoord()
        v0= e.getNodes[0].getProp(self.propertyName)
        v1= e.getNodes[1].getProp(self.propertyName)
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,v0,v1,deform)

  def agregaDiagrama(self):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagrama= 0
    indiceSet= 0
    numSetsDiagrama= len(self.conjuntos)
    for s in self.conjuntos:
      self.appendDataToDiagram(s,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()

