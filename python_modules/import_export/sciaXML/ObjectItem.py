# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Header as hdr
import xml.etree.cElementTree as ET

class ObjectItem(object):
	
  def __init__(self, v= '', i= '', n= '', t= '', h= None, rws= None):
    self.v= v
    self.i= i
    self.n= n
    self.t= t
    self.h= h
    if rws is not None:
      self.rows= rws
    else:
      self.rows= []

  def getV(self):
    return self.v
  def setV(self, v):
    self.v= v

  def getI(self):
    return self.i
  def setI(self, i):
    self.i= i
  

  def getN(self):
    return self.n
  def setN(self, n):
    self.n= n

  def setT(self, t):
    self.t= t
  def getT(self):
    return self.t
  

  def getH(self):
    return self.h
  def setH(self, h):
    self.h= h
  

  def hasRows(self):
    retval= False
    if self.rows is not None:
      retval= (len(self.rows)>0)
    return retval

  def getXMLElement(self,parent, cont):
    itemId= 'p'+str(cont) 
    p= ET.SubElement(parent,itemId)
    if self.v is not None:
      if self.v != '':
        p.set("v",self.v)
    if self.i is not None:
      if self.i != '':
        p.set("i",self.i)
    if self.n is not None:
      if self.n != '':
        p.set("n",self.n)
    if self.t is not None:
      if self.t != '':
        p.set("t",self.t)
    if self.h is not None:
      hh= self.h.getXMLElement(p)
    if self.hasRows():
      for r in self.rows:
        rr= r.getXMLElement(p)
    return p

	
  

  
	

