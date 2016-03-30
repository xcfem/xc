 # -*- coding: utf-8 -*-

''' Representación de cargas sobre elementos lineales. '''

import geom
import vtk
from miscUtils import LogMessages as lmsg
from xcVtk import VectorField as vf


class LoadVectorField(vf.VectorField):
  '''Draws a load over a points on nodes and on elements.'''
  def __init__(self,fUnitConv,loadPatternName,components= [0,1,2]):
    super(LoadVectorField,self).__init__(loadPatternName,fUnitConv)
    self.lpName= loadPatternName
    self.components= components

  def dumpElementalLoads(self,preprocessor,lp):
    ''' Iterate over loaded elements dumping its loads into the graphic.'''
    lIter= lp.getElementalLoadIter
    elementLoad= lIter.next()
    i= self.components[0]; j= self.components[1]; k= self.components[2]
    count= 0 
    while(elementLoad):
      tags= elementLoad.elementTags
      for i in range(0,len(tags)):
        eTag= tags[i]
        elem= preprocessor.getElementLoader.getElement(eTag)
        area= elem.getArea(True)
        vLoad= elem.getCoordTransf.getVectorGlobalCoordFromLocal(elementLoad.getLocalForce())*area
        vx= vLoad[i]; vy= vLoad[j]; vz= vLoad[k]
        p= elem.getPosCentroid(True)
        self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      elementLoad= lIter.next()
      count+= 1
    return count

  def dumpNodalLoads(self,preprocessor,lp):
    ''' Iterate over loaded nodes dumping its loads into the graphic.'''
    lIter= lp.getNodalLoadIter
    nl= lIter.next()
    count= 0 
    while(nl):
      tags= nl.nodeTags
      for i in range(0,len(tags)):
        nTag= tags[i]
        node= preprocessor.getElementLoader.getElement(nTag)
        p= node.getInitialPos3d()
        vLoad= nl.getLoadVectorOnNode(node)
        vx= vLoad[i]; vy= vLoad[j]; vz= vLoad[k]
        self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      nl= lIter.next()
      count+= 1
    return count

  def dumpLoads(self, preprocessor):
    preprocessor.resetLoadCase()
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    loadPatterns.addToDomain(self.lpName)
    lp= loadPatterns[self.lpName]
    #Iterate over loaded elements.
    count= self.dumpElementalLoads(preprocessor,lp)
    #Iterate over loaded nodes.
    count+= self.dumpNodalLoads(preprocessor,lp)
    if(count==0):
      lmsg.warning('LoadVectorField.dumpLoads: no loads defined.')

