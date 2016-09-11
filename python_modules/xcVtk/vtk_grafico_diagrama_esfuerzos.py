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

class RecordDefDiagramaEsf(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,component):
    ''' Diagram constructor

        :param scale: scale factor for the diagram (can be negative too).
        :param fUnitConv: unit conversion factor (i.e N->kN => fUnitConv= 1e-3.
        :param sets: list of element sets where the diagram will be displayed.
        :param component: magnitude to display (N, My, Vz,...)
    '''

    super(RecordDefDiagramaEsf,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    self.component= component
    if(self.component == "N"):
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaAxiles
    elif(self.component == "Qy"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVY
    elif(self.component == "Qz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVZ
    elif(self.component == "T"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaT
    elif(self.component == "My"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeY
    elif(self.component == "Mz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeZ
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
      self.nmbRutinaDiagrama(s,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()
