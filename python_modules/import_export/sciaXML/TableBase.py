# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Base as b
import Header as hdr
import xml.etree.cElementTree as ET

class TableBase(b.Base):

  def __init__(self,id= '',t= '',name= ''):
    super(TableBase,self).__init__(id,t)
    self.name = name

  def getName(self):
    return self.name
  def setName(self,name):
    self.name = name
    
  def populateXMLElement(self,xmlElement):
    super(TableBase,self).populateXMLElement(xmlElement)
    if(self.name!=''):
      xmlElement.set("name",self.name)

    
    
    

