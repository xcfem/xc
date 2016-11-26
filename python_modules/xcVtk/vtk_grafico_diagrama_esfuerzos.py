# -*- coding: utf-8 -*-

''' Representación de diagramas de esfuerzos (o de otras magnitudes)
   sobre elementos lineales. '''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import geom
import vtk
from xcVtk import ColoredDiagram as cd
from postprocess import ControlVars as cv

class RecordDefDiagramaEsf(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,component):
    ''' Diagram constructor

        :ivar scale:     scale factor for the diagram (can be negative too).
        :ivar fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
        :ivar sets:      list of element sets for which the diagram will be displayed.
        :ivar component: property to be displayed 
                         (possible arguments: 'N', 'My', 'Mz'Vz,...)
    '''

    super(RecordDefDiagramaEsf,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.component= component
    if((self.component == 'N') or (self.component.endswith('_N'))):
      self.diagramRoutineName= self.agregaDatosADiagramaAxiles
    elif((self.component == 'Qy') or (self.component.endswith('_Qy'))): 
      self.diagramRoutineName= self.agregaDatosADiagramaVY
    elif((self.component == 'Qz') or (self.component.endswith('_Qz'))): 
      self.diagramRoutineName= self.agregaDatosADiagramaVZ
    elif((self.component == 'T') or (self.component.endswith('_T'))): 
      self.diagramRoutineName= self.agregaDatosADiagramaT
    elif((self.component == 'My') or (self.component.endswith('_My'))): 
      self.diagramRoutineName= self.agregaDatosADiagramaMomEjeY
    elif((self.component == 'Mz') or (self.component.endswith('_Mz'))): 
      self.diagramRoutineName= self.agregaDatosADiagramaMomEjeZ
    else:
      print "component :'", self.component, "' unknown."



  def agregaDatosADiagramaAxiles(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirEjeDebilGlobales()
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getN1,e.getN2)

  def agregaDatosADiagramaMomEjeY(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getKVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getMy1,-e.getMy2)

  def agregaDatosADiagramaMomEjeZ(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getJVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,-e.getMz1,e.getMz2)

  def agregaDatosADiagramaVY(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getJVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getVy1,-e.getVy2)

  def agregaDatosADiagramaVZ(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getCoordTransf.getKVector 
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getVz1,-e.getVz2)


  def agregaDatosADiagramaT(self, eSet,indxDiagrama):
    #Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirEjeDebilGlobales
      e.getResistingForce()
      indxDiagrama= self.agregaDatosADiagrama(e,indxDiagrama,e.getT1,-e.getT2)

  def agregaDiagrama(self):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagrama= 0
    indiceSet= 0
    numSetsDiagrama= len(self.conjuntos)
    for s in self.conjuntos:
      self.diagramRoutineName(s,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()


def getDiagramFromControlVar(attributeName,argument,xcSet,component,scaleFactor,fUnitConv):
  ''':returns: a diagram that represents the control var over the elements in the set

     :param attributeName: name of the element's property that has the 
                           control var in it for example as in 
                           elem.getProp(attributeName).eval(argument).
     :param argument: name of the control var to represent.
     :param xcSet: represent the field over those element sets.
     :param component: (FUTURE USE!) component of the control var to represent.
     :param scaleFactor: scale factor (size of the diagram).
     :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3).
  '''
  if(component):
    lmsg.warning('component not yet in use. Ignored.')
  nodePropName= cv.extrapolate_control_var(xcSet.getElements,attributeName,argument)
  return RecordDefDiagramaEsf(scaleFactor,fUnitConv,[xcSet],nodePropName)

