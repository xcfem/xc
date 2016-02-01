 # -*- coding: utf-8 -*-

''' Representación de diagramas de esfuerzos (o de otras magnitudes)
   sobre elementos lineales. '''

import geom
import vtk
from xcVtk import ColoredDiagram as cd

class RecordDefDiagramaEsf(cd.ColoredDiagram):
  '''Diagram of internal forces (N,My,Mz,T,Vy,Vz)'''
  def __init__(self,scale,fUnitConv,sets,component):
    super(RecordDefDiagramaEsf,self).__init__(scale,fUnitConv)
    self.conjuntos= sets
    if(component == "N"):
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaAxiles
    elif(component == "Qy"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVY
    elif(component == "Qz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaVZ
    elif(component == "T"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaT
    elif(component == "My"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeY
    elif(component == "Mz"): 
      self.nmbRutinaDiagrama= self.agregaDatosADiagramaMomEjeZ
    else:
      print "component :'", component, "' unknown."



  def agregaDatosADiagramaAxiles(self, eSet,indxDiagrama):
    # Agrega a el diagrama los valores obtenidos del conjunto que se pasa como parámetro.
    #   eSet: Conjunto de elementos cuyos valore se incorporan al diagrama.
    elems= eSet.getElements
    for e in elems:
      self.vDir= e.getVDirEjeDebilGlobales
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
