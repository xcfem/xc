# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Header as hdr
import xml.etree.cElementTree as ET

class Property(object):  
  x= ''  
  name= ''
  typo= ''	
  id= ''
  flag= ''
  value= None
	
  def __init__(self, x= '', name= '', typo= '', id= '', flag= ''):
    self.x= x
    self.name= name
    self.typo= typo
    self.id= id
    self.flag= flag
    self.value= None
  
  def getXMLElement(self,parent):
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
