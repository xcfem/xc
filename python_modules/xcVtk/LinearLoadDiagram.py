 # -*- coding: utf-8 -*-

''' Representación de cargas sobre elementos lineales. '''

import geom
import vtk
from xcVtk import ColoredDiagram as cd


class LinearLoadDiagram(cd.ColoredDiagram):
  '''Draws a load over a linear element (qx,qy,qz,...)'''
  def __init__(self,scale,fUnitConv,loadPatternName,component):
    super(LinearLoadDiagram,self).__init__(scale,fUnitConv)
    self.lpName= loadPatternName
    self.component= component

  def dumpLoads(self, preprocessor, indxDiagrama):
    preprocessor.resetLoadCase()
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    loadPatterns.addToDomain(self.lpName)
    lp= loadPatterns[self.lpName]
    #Iterate over loaded elements.
    lIter= lp.getElementalLoadIter
    el= lIter.next()
    while(el):
      tags= el.elementTags
      for i in range(0,len(tags)):
        eTag= tags[i]
        elem= preprocessor.getElementLoader.getElement(eTag)
        if(self.component=='axialComponent'):
          self.vDir= elem.getVDirEjeDebilGlobales
          indxDiagrama= self.agregaDatosADiagrama(elem,indxDiagrama,el.axialComponent,el.axialComponent)
        elif(self.component=='transComponent'):
          self.vDir= elem.getCoordTransf.getJVector 
          indxDiagrama= self.agregaDatosADiagrama(elem,indxDiagrama,el.transComponent,el.transComponent)
        elif(self.component=='transYComponent'):
          self.vDir= elem.getCoordTransf.getJVector 
          indxDiagrama= self.agregaDatosADiagrama(elem,indxDiagrama,el.transYComponent,el.transYComponent)
        elif(self.component=='transZComponent'):
          self.vDir= elem.getCoordTransf.getKVector 
          indxDiagrama= self.agregaDatosADiagrama(elem,indxDiagrama,el.transZComponent,el.transZComponent)
        else:
          print "LinearLoadDiagram :'", self.component, "' unknown."        
      el= lIter.next()

  def agregaDiagrama(self,preprocessor):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagrama= 0
    self.dumpLoads(preprocessor,indxDiagrama)

    self.updateLookUpTable()
    self.updateActorDiagrama()
