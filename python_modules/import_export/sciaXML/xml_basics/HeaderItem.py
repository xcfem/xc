# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class HeaderItem(object):
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

  def getXMLElement(self,parent, cont):
    '''Returns an XML element for the object.'''
    itemId= 'h'+str(cont) 
    hi= ET.SubElement(parent,itemId)
    hi.set("t",self.t)
    return hi
