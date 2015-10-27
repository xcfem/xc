# -*- coding: utf-8 -*-

from __future__ import division
import xc_base
import geom
import xc
import math

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
        n.setProp(attributeName,initialValue)
      else:
        nodeTags[tag]+=1
  return nodeTags

def extrapolate_elem_function_attr(preprocessor,elemSet,attributeName,function, argument):
  nodeTags= create_attribute_at_nodes(elemSet,attributeName,0.0)
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
  for tag in nodeTags:
    n= preprocessor.getNodeLoader.getNode(tag)
    denom= nodeTags[tag]
    n.setProp(attributeName,n.getProp(attributeName)/denom)
