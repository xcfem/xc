# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class EnumItem(object):
  '''Item of an enumeration.'''

  def __init__(self,v,t):
    ''' Constructor.
        Parameters:
        v: v field
        t: t field
    '''
    self.v = v
    self.t = t

  def getXMLElement(self,parent):
    '''Returns an XML element for the object.'''
    ei= ET.SubElement(parent,"it_enum")
    if(self.v!=''):
      ei.set("v",self.v)
    if(self.t!=''):
      ei.set("t",self.t)
    return ei
        

