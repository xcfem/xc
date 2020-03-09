# -*- coding: utf-8 -*-

'''load_container.py: extract loads from XC load patterns and put the in neutral data structure.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from import_export import neutral_load_description as nld
from miscUtils import log_messages as lmsg

class LoadContainerBase(object):
  '''Base for XML SCIA load containers.'''
  def __init__(self):
    self.pointLoadCounter= 1
    self.surfaceLoadCounter= 1
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
  def readLoadCombsFromXC(self,combContainer):
    self.loads.readLoadCombsFromXC(combContainer,self.mapLoadCases)
  def dumpLoadPattern(self, counter, lpName, loadPatterns,permanentLoadCaseNames):
    loadCaseId= counter
    loadCaseName= lpName
    descr= ''#lp.description
    newLoadCase= None
    if(loadCaseName in permanentLoadCaseNames):
      newLoadCase= nld.LoadCase(loadCaseId,loadCaseName, descr,0,1)
    else:
      newLoadCase= nld.LoadCase(loadCaseId,loadCaseName, descr,1,1)
      newLoadCase.actionType= 'Variable'
    self.loads.loadCases[loadCaseId]= newLoadCase
    self.mapLoadCases[loadCaseName]= newLoadCase
    return newLoadCase 
  def loads2Neutral(self,preprocessor,permanentLoadCaseNames):
    loadPatterns= preprocessor.getLoadHandler.getLoadPatterns
    lc= self.loads.loadCases
    counter= 1
    for lpName in loadPatterns.getKeys():
      newLoadCase= self.dumpLoadPattern(counter,lpName,loadPatterns, permanentLoadCaseNames)
      counter+= 1
      preprocessor.resetLoadCase()
      loadPatterns.addToDomain(lpName)
      lp= loadPatterns[lpName] #Load pattern from
      self.dumpPointLoads(lp,newLoadCase) #Dump nodal loads
      self.dumpSurfaceLoads(lp,newLoadCase) #Dump loads over elements
      loadPatterns.removeFromDomain(lpName)
  def readLoadsFromXC(self,preprocessor,permanentLoadCaseNames):
    self.loads2Neutral(preprocessor,permanentLoadCaseNames)
  def readFromXC(self,preprocessor,permanentLoadCaseNames,combContainer):
    self.loads2Neutral(preprocessor,permanentLoadCaseNames)
    self.loads.readLoadCombsFromXC(combContainer,self.mapLoadCases)


class LoadContainer(LoadContainerBase):
  '''Container for loads over mesh nodes and elements.'''
  def dumpPointLoads(self, lp, destLoadCase):
    '''Dump loads over nodes.'''
    lIter= lp.loads.getNodalLoadIter
    nl= lIter.next()
    while nl:
      pLoad= nld.NodalLoadRecord(destLoadCase, self.pointLoadCounter,None,1.0)
      force= nl.getForce 
      pLoad.value= force.Norm()
      pLoad.vDir= force.Normalized()
      pLoad.tag= nl.getNodeTag
      destLoadCase.loads.punctualLoads.append(pLoad)
      self.pointLoadCounter+=1
      nl= lIter.next()
  def dumpSurfaceLoads(self, lp, destLoadCase):
    '''Dump loads over elements.'''
    eLoadIter= lp.loads.getElementalLoadIter
    el= eLoadIter.next()
    while el:
      eLoad= nld.ElementLoadRecord(destLoadCase,self.surfaceLoadCounter,1.0)
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
          lmsg.warning('loads2Neutral: vDir vector very small: "+ str(vDir) + " load ignored.')
      self.surfaceLoadCounter+=1
      el= eLoadIter.next()
    


class FreeLoadContainer(LoadContainerBase):
  '''Container for free loads over mesh points and surfaces
     (used for export a block model (kPoints, lines and surfaces)
     into XML SCIA) .'''

  def dumpPointLoads(self, lp, destLoadCase):
    '''Dump loads over nodes as free punctual loads.'''
    lIter= lp.loads.getNodalLoadIter
    nl= lIter.next()
    while nl:
      node= nl.getNode
      pLoad= nld.PointForceRecord(destLoadCase, self.pointLoadCounter,node.getInitialPos3d,1.0)
      force= nl.getForce 
      pLoad.value= force.Norm()
      pLoad.vDir= force.Normalized()
      pLoad.tag= nl.getNodeTag
      destLoadCase.loads.punctualLoads.append(pLoad)
      self.pointLoadCounter+=1
      nl= lIter.next()

  def dumpSurfaceLoads(self, lp, destLoadCase):
    '''Dump loads over surfaces as free surface loads.'''
    domain= lp.getDomain
    preprocessor= lp.getDomain.getPreprocessor
    eLoadIter= lp.loads.getElementalLoadIter
    eLoad= eLoadIter.next()
    loadSets= []
    while eLoad:
      setName= 'surfaceLoadSet'+str(eLoad.tag)
      surfaceLoadSet= preprocessor.getSets.defSet(setName)
      surfaceLoadSet.elementalLoad= eLoad
      elemTags= eLoad.elementTags
      resultant= eLoad.getResultant(geom.Pos3d(0,0,0),True) #Total force over the elements.
      totalForce= geom.Vector3d(resultant.x,resultant.y,resultant.z)
      totalForceModulus= totalForce.getModulus()
      numberOfLoadedElements= len(elemTags)
      if(numberOfLoadedElements>1):
        #It seems there is a bug in 
        totalArea= 0.0
        for tag in elemTags:
          elem= domain.getMesh.getElement(tag)
          totalArea+= elem.getArea(True)
          if(elem):
            surfaceLoadSet.elements.append(elem)
          else:
            lmsg.error('element: '+ str(tag) + ' not found.')
        surfaceLoadSet.value= totalForceModulus
        surfaceLoadSet.vDir= [totalForce.x/surfaceLoadSet.value,totalForce.y/surfaceLoadSet.value,totalForce.z/surfaceLoadSet.value]
        surfaceLoadSet.value/= totalArea
        elementContours= surfaceLoadSet.elements.getContours(0.0)
        if(len(elementContours)>1):
          lmsg.error('surface load set: '+ setName + ' has more than one contour.  Contours others than first are ignored.')
        surfaceLoadSet.polygon= elementContours[0]
        surfaceLoadSet.polygon.simplify(.01) #Deletes unnecesary vertices.
        loadSets.append(surfaceLoadSet)
      else:
        elem= domain.getMesh.getElement(elemTags[0]) #Only one element...
        pLoad= nld.PointForceRecord(destLoadCase, self.pointLoadCounter,elem.getPosCentroid(True),1.0)
        pLoad.value= totalForceModulus
        pLoad.vDir= [totalForce.x/pLoad.value,totalForce.y/pLoad.value,totalForce.z/pLoad.value]
        pLoad.tag= eLoad.tag
        destLoadCase.loads.punctualLoads.append(pLoad)
        self.pointLoadCounter+=1
      eLoad= eLoadIter.next()        
    for s in loadSets:
      sLoad= nld.SurfaceLoadRecord(destLoadCase, self.surfaceLoadCounter)
      sLoad.polygon= s.polygon
      sLoad.value= s.value
      sLoad.vDir= s.vDir
      destLoadCase.loads.surfaceLoads.append(sLoad)
      self.surfaceLoadCounter+=1
      

