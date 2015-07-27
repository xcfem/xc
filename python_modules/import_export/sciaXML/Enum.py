# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import xml.etree.cElementTree as ET

class Enum(object):
  items= []

  def __init__(self,items):
    self.items= items
  def getXMLElement(self,parent):
    tb= ET.SubElement(parent,"def_enum")
    tb.set("size",str(len(self.items)))
    for i in self.items:
      it= i.getXMLElement(tb)
    return tb

    
    
    

