 # -*- coding: utf-8 -*-

''' Diagram display a property defined at nodes over linear elements. '''

import geom
import vtk
from xcVtk import ColoredDiagram as cd

class NodePropertyDiagram(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,propertyName):
    super(NodePropertyDiagram,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.propertyName= propertyName

  def appendDataToDiagram(self, eSet,indxDiagrama):
    # Append property values to diagram .
    #   eSet: Element set.
    elems= eSet.getElements
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
