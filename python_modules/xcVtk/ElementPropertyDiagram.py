 # -*- coding: utf-8 -*-

''' Diagram display a property defined over linear elements. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
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

  def __init__(self,scale,fUnitConv,sets,propertyName):
    super(ElementPropertyDiagram,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.propertyName= propertyName

  def appendDataToDiagram(self, eSet,indxDiagrama):
    # Append property values to diagram .
    #   eSet: Element set.
    elems= eSet.getElements
    if(self.propertyName=='FCTNCP'):
      for e in elems:
        self.vDir= e.getVDirEjeDebilGlobales()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='FCVCP'):
      for e in elems:
        self.vDir= e.getVDirEjeDebilGlobales()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='N+'):
      for e in elems:
        self.vDir= e.getVDirEjeDebilGlobales()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='N-'):
      for e in elems:
        self.vDir= e.getVDirEjeDebilGlobales()
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='My+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='My-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Mz+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Mz-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Vy+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Vy-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getJVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Vz+'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    elif(self.propertyName=='Vz-'):
      for e in elems:
        self.vDir= e.getCoordTransf.getKVector 
        values= e.getProp(self.propertyName) # [back node value, front node value]
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,values[0],values[1])      
    else:
      extrapolate_elem_attr.extrapolate_elem_function_attr(elems,self.propertyName,"getProp", self.propertyName)
      for e in elems:
        self.vDir= e.getVDirEjeDebilGlobales()
        v0= e.getNodes[0].getProp(self.propertyName)
        v1= e.getNodes[1].getProp(self.propertyName)
        indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,v0,v1)

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
