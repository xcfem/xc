# -*- coding: utf-8 -*-
from __future__ import division

''' Loads represented as vectors. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import geom
import xc
import vtk
from miscUtils import LogMessages as lmsg
from postprocess.xcVtk import vector_field as vf


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

  def sumElementalLoads(self,preprocessor,lp):
    ''' Iterate over loaded elements and cumulate their loads.'''
    lIter= lp.loads.getElementalLoadIter
    elementLoad= lIter.next()
    comp_i= self.components[0]; comp_j= self.components[1]; comp_k= self.components[2]
    retval= dict()
    while(elementLoad):
      tags= elementLoad.elementTags
      for i in range(0,len(tags)):
        eTag= tags[i]
        elem= preprocessor.getElementHandler.getElement(eTag)
        if(elem.getDimension==2):
          vLoad= elem.getCoordTransf.getVectorGlobalCoordFromLocal(elementLoad.getLocalForce())
          if(self.multiplyByElementArea):
            vLoad*=elem.getArea(True)
          v= xc.Vector([vLoad[comp_i],vLoad[comp_j],vLoad[comp_k]])
          if eTag in retval:
            retval[eTag]+= v
          else:
            retval[eTag]= v
#        else:
#          lmsg.warning('displaying of loads over 1D elements not yet implemented')
      elementLoad= lIter.next()
    return retval


  
  def getMaxElementalLoad(self,preprocessor,lp):
    ''' Calculate the maximum absolute value of elemental loads in load pattern lp.'''
    loadValues= self.sumElementalLoads(preprocessor,lp)
    for eTag in loadValues.keys():
      elem= preprocessor.getElementHandler.getElement(eTag)
      if(elem.getDimension==2):
        vLoad= loadValues[eTag]
        self.data.insertNextLoadVector(vLoad[0],vLoad[1],vLoad[2])
      else:
        lmsg.warning('displaying of loads over 1D elements not yet implemented')
    self.data.calculateLengths(self.fUnitConv)
    rgMaxMin=self.data.lengths.GetRange()
    return abs(max(rgMaxMin, key=abs))

  def dumpElementalLoads(self,preprocessor,lp):
    ''' Iterate over cumulated loads dumping them into the graphic.'''
    loadValues= self.sumElementalLoads(preprocessor,lp)
    for eTag in loadValues.keys():
      elem= preprocessor.getElementHandler.getElement(eTag)
      if(elem.getDimension==2):
        vLoad= loadValues[eTag]
        p= elem.getPosCentroid(True)
        self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
      else:
        lmsg.warning('displaying of loads over 1D elements not yet implemented')
    return len(loadValues)

  def sumNodalLoads(self,preprocessor,lp):
    ''' Iterate over loaded nodes to cumulate their loads.

    :param lp: load pattern
    '''
    lIter= lp.loads.getNodalLoadIter
    nl= lIter.next()
    retval= dict()
    while(nl):
      nTag= nl.getNodeTag
      node= preprocessor.getNodeHandler.getNode(nTag)
      vLoad= nl.getForce
      v= xc.Vector([vLoad[0], vLoad[1], vLoad[2]])
      if nTag in retval:
        retval[nTag]+= v
      else:
        retval[nTag]= v
      nl= lIter.next()
    return retval

  def getMaxNodalLoad(self,preprocessor,lp):
    ''' Calculate maximum absolute value of nodal loads in load pattern lp.'''
    loadValues= self.sumNodalLoads(preprocessor,lp)
    for nTag in loadValues.keys():
      node= preprocessor.getNodeHandler.getNode(nTag)
      vLoad= loadValues[nTag]
      self.data.insertNextLoadVector(vLoad[0],vLoad[1],vLoad[2])
    self.data.calculateLengths(self.fUnitConv)
    rgMaxMin=self.data.lengths.GetRange()
    return abs(max(rgMaxMin, key=abs))
    
  def dumpNodalLoads(self,preprocessor,lp,defFScale):
    ''' Iterate over loaded nodes dumping its loads into the graphic.

    :param lp: load pattern
    :param defFScale: factor to apply to current displacement of nodes 
              so that the display position of each node equals to
              the initial position plus its displacement multiplied
              by this factor.    
    '''
    loadValues= self.sumNodalLoads(preprocessor,lp)
    for nTag in loadValues.keys():
      node= preprocessor.getNodeHandler.getNode(nTag)
      p= node.getCurrentPos3d(defFScale)
      vLoad= loadValues[nTag]
      self.data.insertNextPair(p.x,p.y,p.z,vLoad[0],vLoad[1],vLoad[2],self.fUnitConv,self.showPushing)
    return len(loadValues)

  
  
  def dumpLoads(self, preprocessor,defFScale):
    preprocessor.resetLoadCase()
    loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
    loadPatterns.addToDomain(self.lpName)
    lp= loadPatterns[self.lpName]
    if(lp):
      maxElLd=self.getMaxElementalLoad(preprocessor,lp)
      maxNdLd=self.getMaxNodalLoad(preprocessor,lp)
      maxLd=max(maxElLd,maxNdLd)
      self.data.scaleFactor/=maxLd
      #Iterate over loaded elements.
      count= self.dumpElementalLoads(preprocessor,lp)
      #Iterate over loaded nodes.
      count+= self.dumpNodalLoads(preprocessor,lp,defFScale)
      if(count==0):
        lmsg.warning('LoadVectorField.dumpLoads: no loads defined.')
      loadPatterns.removeFromDomain(self.lpName)
    else:
      lmsg.error('Load pattern: '+ self.lpName + ' not found.')

