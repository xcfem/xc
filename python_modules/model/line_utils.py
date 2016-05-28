# -*- coding: utf-8 -*-

''' Utilities for line related manipulations. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
from postprocess import extrapolate_elem_attr as extrap

class LineWrapper(object):
  def __init__(self,line):
    self.line= line

  def getOrderedNodes(self):
    '''Returns line's interior nodes.'''
    nNodes= self.line.getNumNodes
    retval= []
    for i in range(0,nNodes):
      retval.append(self.line.getNodeI(i+1))
    return retval

  def getElements(self):
    return self.line.getElements()

  def getNodeAbcissae(self):
    '''Returns node's abcissae; first node x= 0.0 last node x= lengthOfLine.'''
    retval= list()
    nodes= self.getOrderedNodes()
    for n in nodes:
      retval.append(self.line.getLambda(n.getInitialPos3d))
    return retval

  def getNodeAttrOrdinates(self,attributeName):
    '''Returns a list with the values of attribute at each node.'''
    retval= list()
    nodes= self.getOrderedNodes()
    for n in nodes:
      retval.append(n.getProp(attributeName))
    return retval

  def getElementFunctionOrdinates(self,factor0, funcName0, factor1, funcName1):
    '''Returns a list with the values of attribute extrapolated at each node.'''
    retval= list()
    indices= dict()
    nodes= self.getOrderedNodes()
    count= 0
    for n in nodes:
      retval.append(0.0) #Initialize values
      indices[n.tag]= count
      count+= 1
    for e in self.getElements():
      f0= factor0*getattr(e, funcName0)
      f1= factor1*getattr(e, funcName1)
      i0= indices[e.getNodes[0].tag]
      retval[i0]+= 0.5*f0
      i1= indices[e.getNodes[1].tag]
      retval[i1]+= 0.5*f1
    retval[0]*=2 #Extreme nodes.
    retval[-1]*=2
    return retval

  def getInteriorNodes(self):
    '''Returns line's interior nodes.'''
    nNodes= self.line.getNumNodes
    retval= []
    for i in range(2,nNodes):
      n= self.line.getNodeI(i)
      retval.append(n)
    return retval

  def getFirstNode(self):
    '''Returns line's first node.'''
    return self.line.getNodeI(1)

  def getLastNode(self):
    '''Returns line's last node.'''
    nNodes= self.line.getNumNodes
    return self.line.getNodeI(nNodes-1)
  

def getInteriorNodes(line):
  '''DEPRECATED; returns line's interior nodes.'''
  lmsg.warning('getInteriorNodes DEPRECATED; use LineWrapper.getInteriorNodes.')
  lw= LineWrapper(line)
  return lw.getInteriorNodes()

def getFirstNode(line):
  '''DEPRECATED; returns line's first node.'''
  lmsg.warning('getFirstNode DEPRECATED; use LineWrapper.getFirstNode.')
  lw= LineWrapper(line)
  return lw.getFirstNode()

# Returns line's last node.
def getLastNode(line):
  '''DEPRECATED; returns line's last node.'''
  lmsg.warning('getLastNode DEPRECATED; use LineWrapper.getLastNode.')
  lw= LineWrapper(line)
  return lw.getLastNode()

