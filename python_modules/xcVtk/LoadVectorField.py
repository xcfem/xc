 # -*- coding: utf-8 -*-

''' Loads represented as vectors. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import vtk
from miscUtils import LogMessages as lmsg
from xcVtk import VectorField as vf


class LoadVectorField(vf.VectorField):
  '''Draws a load over a points on nodes and on elements.'''
  def __init__(self,loadPatternName,fUnitConv= 1e-3,scaleFactor= 1.0,showPushing= True,components= [0,1,2],multiplyByElementArea= True):
    '''
    Parameters:
      loadPatternName: name of the load pattern to display.
      fUnitConv: unit conversion factor.
      scaleFactor: scale factor for the size of the vectors.
      showPushing: true if the loads push the loaded point (as oppssed to pull). Default: True
      components: index of the components of the load. Default: [0,1,2] 
      multiplyByElementArea: for loads over elements (default= True).
    '''
    super(LoadVectorField,self).__init__(loadPatternName,fUnitConv,scaleFactor,showPushing)
    self.lpName= loadPatternName 
    self.components= components
    self.multiplyByElementArea= multiplyByElementArea

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
        vLoad= elem.getCoordTransf.getVectorGlobalCoordFromLocal(elementLoad.getLocalForce())
        if(self.multiplyByElementArea):
          vLoad*=elem.getArea(True)
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
      nTag= nl.getNodeTag
      node= preprocessor.getNodeLoader.getNode(nTag)
      p= node.getInitialPos3d
      vLoad= nl.getForce
      vx= vLoad[0]; vy= vLoad[1]; vz= vLoad[2]
      self.insertNextPair(p.x,p.y,p.z,vx,vy,vz)
      nl= lIter.next()
      count+= 1
    return count

  def dumpLoads(self, preprocessor):
    preprocessor.resetLoadCase()
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    loadPatterns.addToDomain(self.lpName)
    lp= loadPatterns[self.lpName]
    if(lp):
      #Iterate over loaded elements.
      count= self.dumpElementalLoads(preprocessor,lp)
      #Iterate over loaded nodes.
      count+= self.dumpNodalLoads(preprocessor,lp)
      if(count==0):
        lmsg.warning('LoadVectorField.dumpLoads: no loads defined.')
      loadPatterns.removeFromDomain(self.lpName)
    else:
      lmsg.error('Load pattern: '+ self.lpName + ' not found.')

