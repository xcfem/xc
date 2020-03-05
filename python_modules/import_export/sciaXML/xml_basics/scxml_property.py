# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xml.etree.cElementTree as ET

class SCXMLProperty(object):  
  '''SCIA XML property.'''
  def __init__(self, x= '', name= '', typo= '', id= '', flag= ''):
    self.x= x
    self.name= name
    self.typo= typo
    self.id= id
    self.flag= flag
    self.value= None
  
  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the property.
       Parameters:
       parent: owner of this object in the XML structure.'''
    property= ET.SubElement(parent,"def_property")
    if(self.x!=''):
      property.set("x",self.x)
    if(self.name!=''):
      property.set("name",self.name)
    if(self.typo!=''):
      property.set("type",self.typo)
    if(self.id!=''):
      property.set("id",self.id)
    if(self.flag!=''):
      property.set("flag",self.flag)
    if self.value is not None:
      self.value.getXMLElement(property)
    return property
