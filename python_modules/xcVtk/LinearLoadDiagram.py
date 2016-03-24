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

  def dumpElementalLoads(self,preprocessor,lp,indxDiagram):
    ''' Iterate over loaded elements dumping its loads into the graphic.'''
    lIter= lp.getElementalLoadIter
    el= lIter.next()
    while(el):
      tags= el.elementTags
      for i in range(0,len(tags)):
        eTag= tags[i]
        elem= preprocessor.getElementLoader.getElement(eTag)
        if(self.component=='axialComponent'):
          self.vDir= elem.getVDirEjeDebilGlobales
          indxDiagram= self.agregaDatosADiagrama(elem,indxDiagram,el.axialComponent,el.axialComponent)
        elif(self.component=='transComponent'):
          self.vDir= elem.getCoordTransf.getJVector 
          indxDiagram= self.agregaDatosADiagrama(elem,indxDiagram,el.transComponent,el.transComponent)
        elif(self.component=='transYComponent'):
          self.vDir= elem.getCoordTransf.getJVector 
          indxDiagram= self.agregaDatosADiagrama(elem,indxDiagram,el.transYComponent,el.transYComponent)
        elif(self.component=='transZComponent'):
          self.vDir= elem.getCoordTransf.getKVector 
          indxDiagram= self.agregaDatosADiagrama(elem,indxDiagram,el.transZComponent,el.transZComponent)
        else:
          print "LinearLoadDiagram :'", self.component, "' unknown."        
      el= lIter.next()

  def dumpLoads(self, preprocessor, indxDiagram):
    preprocessor.resetLoadCase()
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    loadPatterns.addToDomain(self.lpName)
    lp= loadPatterns[self.lpName]
    #Iterate over loaded elements.
    self.dumpElementalLoads(preprocessor,lp,indxDiagram)

  def agregaDiagrama(self,preprocessor):
    self.creaEstrucDatosDiagrama()
    self.creaLookUpTable()
    self.creaActorDiagrama()

    indxDiagram= 0
    self.dumpLoads(preprocessor,indxDiagram)

    self.updateLookUpTable()
    self.updateActorDiagrama()
