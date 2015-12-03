# -*- coding: utf-8 -*-

'''LoadContainer.py: extract loads from XC load patterns and put the in neutral data structure.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from import_export import NeutralLoadDescription as nld

class LoadContainer(object):
  def __init__(self):
    self.nodalLoadCounter= 1
    self.elementalLoadCounter= 1
    #Loads
    self.loads= nld.LoadData()
    lg= self.loads.loadGroups
    lg0= nld.LoadGroup(0,'Permanent loads')
    lg0.permanent= True
    lg["LG0"]= lg0
    lg["LG1"]= nld.LoadGroup(1,'Variable loads')
    self.mapLoadCases= dict()
  def getLoadCombinationDict(self):
    return self.loads.loadCombs
  def dumpNodalLoads(self, lp, destLoadCase):
    '''Dump loads over nodes.'''
    lIter= lp.getNodalLoadIter
    nl= lIter.next()
    while nl:
      pLoad= nld.PunctualLoadRecord(destLoadCase, self.nodalLoadCounter,None,1.0)
      force= nl.getForce 
      pLoad.value= force.Norm()
      pLoad.vDir= force.Normalized()
      pLoad.tag= nl.getNodeTag
      destLoadCase.loads.punctualLoads.append(pLoad)
      self.nodalLoadCounter+=1
      nl= lIter.next()
  def dumpElementalLoads(self, lp, destLoadCase):
    '''Dump loads over elements.'''
    lIter= lp.getElementalLoadIter
    el= lIter.next()
    while el:
      eLoad= nld.ElementLoadRecord(destLoadCase,self.elementalLoadCounter,1.0)
      if(hasattr(el,"getGlobalForces")):
        eLoad.mode= False # Referred to local coordinate system.
        lf= xc.Vector([el.Wx,el.Wy,el.Wz])
        eLoad.value= lf.Norm()
        if(eLoad.value>=1e-3):
          eLoad.vDir= [el.Wx/eLoad.value,el.Wy/eLoad.value,el.Wz/eLoad.value]
          tags= el.elementTags
          for i in range(0,len(tags)):
            eLoad.tags.append(tags[i])
          destLoadCase.loads.surfaceLoads.append(eLoad)
        else:
          print "loads2Neutral: vDir vector very small: ", vDir, " load ignored."
      self.elementalLoadCounter+=1
      el= lIter.next()
  def loads2Neutral(self,preprocessor,permanentLoadCaseNames):
    loadPatterns= preprocessor.getLoadLoader.getLoadPatterns
    lc= self.loads.loadCases
    counter= 1
    for lpName in loadPatterns.getKeys():
      loadCaseId= counter
      loadCaseName= lpName
      lp= loadPatterns[lpName] #Load pattern from
      descr= ''#lp.description
      newLoadCase= None
      if(loadCaseName in permanentLoadCaseNames):
        newLoadCase= nld.LoadCase(loadCaseId,loadCaseName, descr,0,1)
      else:
        newLoadCase= nld.LoadCase(loadCaseId,loadCaseName, descr,1,1)
        newLoadCase.actionType= 'Variable'
      lc[loadCaseId]= newLoadCase
      self.mapLoadCases[loadCaseName]= newLoadCase
      counter+= 1
    
      preprocessor.resetLoadCase()
      loadPatterns.addToDomain(lpName)
      self.dumpNodalLoads(lp,newLoadCase) #Dump nodal loads
      self.dumpElementalLoads(lp,newLoadCase) #Dump loads over elements
      loadPatterns.removeFromDomain(lpName)
  def readLoadsFromXC(self,preprocessor,permanentLoadCaseNames):
    self.loads2Neutral(preprocessor,permanentLoadCaseNames)
  def readLoadCombsFromXC(self,combContainer):
    self.loads.readLoadCombsFromXC(combContainer,self.mapLoadCases)
  def readFromXC(self,preprocessor,permanentLoadCaseNames,combContainer):
    self.loads2Neutral(preprocessor,permanentLoadCaseNames)
    self.loads.readLoadCombsFromXC(combContainer,self.mapLoadCases)
