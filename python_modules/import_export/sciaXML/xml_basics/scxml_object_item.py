# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class SCXMLObjectItem(object):
  ''' Item of a table.'''	
  def __init__(self, v= '', i= '', n= '', t= '', h= None, rws= None):
    ''' Constructor.

        Parameters:
        :param v: v field.
        :param i: i field.
        :param n: n field.
        :param h: h field.
        :param rws: rows.
    '''
    self.v= v
    self.i= i
    self.n= n
    self.t= t
    self.h= h
    if rws:
      self.rows= rws
    else:
      self.rows= list()

  def getV(self):
    '''Returns v field.'''
    return self.v
  def setV(self, v):
    '''Sets v field.'''
    self.v= v

  def getI(self):
    '''Returns i field.'''
    return self.i
  def setI(self, i):
    '''Sets i field.'''
    self.i= i
  

  def getN(self):
    '''Returns n field.'''
    return self.n
  def setN(self, n):
    '''Sets n field.'''
    self.n= n

  def setT(self, t):
    '''Returns t field.'''
    self.t= t
  def getT(self):
    '''Sets t field.'''
    return self.t
  

  def getH(self):
    '''Returns h field.'''
    return self.h
  def setH(self, h):
    '''Sets h field.'''
    self.h= h
  

  def hasRows(self):
    '''Returns true if rows are not empty.'''
    retval= False
    if self.rows is not None:
      retval= (len(self.rows)>0)
    return retval

  def getXMLElement(self,parent, count):
    '''Returns the corresponding XML element for the object.

       :param parent: owner of this object in the XML structure.
       :param count:  counter.'''
    itemId= 'p'+str(count) 
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

	
  

  
	

