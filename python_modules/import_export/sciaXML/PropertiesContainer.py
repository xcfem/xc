# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Base as b
import xml.etree.cElementTree as ET


class PropertiesContainer(b.Base):
  clsid= ''
  progid= ''
  def __init__(self,cId,clsid,progid):
    super(PropertiesContainer,self).__init__(cId,'')
    self.clsid= clsid
    self.progid= progid
  
  def populateXMLElement(self, element):
    super(PropertiesContainer,self).populateXMLElement(element)
    element.set("clsid",self.clsid)
    if(self.progid!=''):
      element.set("progid",self.progid)
      

  
