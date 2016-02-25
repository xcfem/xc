 # -*- coding: utf-8 -*-

''' Diagram display a property defined over linear elements. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import logging
import vtk
from xcVtk import ColoredDiagram as cd
from postprocess import extrapolate_elem_attr

logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

class ElementPropertyDiagram(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,propertyName):
    super(ElementPropertyDiagram,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.propertyName= propertyName

  def appendDataToDiagram(self, eSet,indxDiagrama):
    # Append property values to diagram .
    #   eSet: Element set.
    elems= eSet.getElements
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
