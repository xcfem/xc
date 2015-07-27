# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub
import xml.etree.cElementTree as ET

class EnumItem(object):
  v= ''
  t= ''

  def __init__(self,v,t):
    self.v = v
    self.t = t

  def getXMLElement(self,parent):
    ei= ET.SubElement(parent,"it_enum")
    if(self.v!=''):
      ei.set("v",self.v)
    if(self.t!=''):
      ei.set("t",self.t)
    return ei
        

