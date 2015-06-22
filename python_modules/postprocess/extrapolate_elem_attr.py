# -*- coding: utf-8 -*-

from __future__ import division
import xc_base
import geom
import xc

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

def extrapolate_elem_function_attr(mdlr,elemSet,attributeName,function, argument):
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
    n= mdlr.getNodeLoader.getNode(tag)
    denom= nodeTags[tag]
    n.setProp(attributeName,n.getProp(attributeName)/denom)
