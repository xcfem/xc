# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
import math
import logging

def flatten_attribute(elemSet,attributeName, treshold, limit):
  '''reduce higher values which hide attribute variation over the model.'''
  flattened= 0
  for e in elemSet:
    v= e.getProp(attributeName)
    if(v>treshold):
      vCorr= 2*math.atan(v)/math.pi*(limit-treshold)+treshold
      #print "v= ", v, " vCorr=", vCorr
      e.setProp(attributeName,vCorr)
      flattened+= 1
  if(flattened>0):
    print "flattened ", flattened, 'values over', len(elemSet)

def create_attribute_at_nodes(elemSet,attributeName,initialValue):
  nodeTags= {}
  for e in elemSet:
    elemNodes= e.getNodes
    sz= len(elemNodes)
    for i in range(0,sz):
      n= elemNodes[i]
      tag= n.tag
      if tag not in nodeTags:
        nodeTags[tag]= 1
        if(n.hasProp(attributeName)):
          logging.warning('node: '+ str(n.tag) + ' already has a property named: \'' + attributeName +'\'.')
        n.setProp(attributeName,initialValue)
      else:
        nodeTags[tag]+=1
  return nodeTags

def extrapolate_elem_function_attr(elemSet,attributeName,function, argument,initialValue= 0.0):
  nodeTags= create_attribute_at_nodes(elemSet,attributeName,initialValue)
  #Calculate totals.
  for e in elemSet:
    elemNodes= e.getNodes
    sz= len(elemNodes)
    for i in range(0,sz):
      n= elemNodes[i]
      f= getattr(e,function)
      value= f(argument)
      oldValue= n.getProp(attributeName)
      n.setProp(attributeName,oldValue+value)
  #Divide by number of elements in the set that touch the node.
  preprocessor= elemSet.owner.getPreprocessor
  for tag in nodeTags:
    n= preprocessor.getNodeLoader.getNode(tag)
    denom= nodeTags[tag]
    n.setProp(attributeName,n.getProp(attributeName)/denom)
