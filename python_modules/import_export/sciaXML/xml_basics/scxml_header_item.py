# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class SCXMLHeaderItem(object):
  def __init__(self,t= ''):
    ''' Constructor.
        Parameters:
        t: t field
    '''
    self.t = t

  def getT(self):
    return self.t
  def setT(self,t):
    self.t= t

  def getXMLElement(self,parent, count):
    '''Returns the corresponding XML element for the object.
       Parameters:
       parent: owner of this object in the XML structure.
       count: counter.'''
    itemId= 'h'+str(count) 
    hi= ET.SubElement(parent,itemId)
    hi.set("t",self.t)
    return hi
