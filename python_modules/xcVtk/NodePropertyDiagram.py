 # -*- coding: utf-8 -*-

''' Diagram to display a property defined at nodes over linear elements. '''

import geom
import vtk
from xcVtk import ColoredDiagram as cd



class NodePropertyDiagram(cd.ColoredDiagram):
  '''Diagram to display a property defined at nodes over linear elements.'''
  def __init__(self,scaleFactor,fUnitConv,sets,attributeName):
    super(NodePropertyDiagram,self).__init__(scaleFactor,fUnitConv)
    self.conjuntos= sets
    self.propertyName= attributeName

  def getValueForNode(self,node):
    if self.propertyName == 'uX':
      return node.getDispXYZ[0]
    elif self.propertyName == 'uY':
      return node.getDispXYZ[1]
    elif self.propertyName == 'uZ':
      return node.getDispXYZ[2]
    elif self.propertyName == 'rotX':
      return node.getRotXYZ[0]
    elif self.propertyName == 'rotY':
      return node.getRotXYZ[1]
    elif self.propertyName == 'rotZ':
      return node.getRotXYZ[2]
    else:
      return node.getProp(self.propertyName)

  def appendDataToDiagram(self, eSet,indxDiagrama,deform=False):
    ''' Append property values to diagram .
       :param eSet: Element set.
       :param deform: =True for display of current/deformed shape (defaults
                       to False, i.e. display of initial/undeformed shape)
    ''' 
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirWeakAxisGlobalCoord()
      v0= self.getValueForNode(e.getNodes[0])
      v1= self.getValueForNode(e.getNodes[1])
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

