# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import TableBase as tBase
import Header as hdr
import xml.etree.cElementTree as ET

class TableNode(tBase.TableBase):

  def __init__(self,id= '',t= '',name= '',h= hdr.Header(),objects= None):
    super(TableNode,self).__init__(id,t,name)
    self.h = h
    if objects is not None:
      self.objects= objects
    else:
      self.objects= []

  def getH(self):
    return self.h
  def setH(self,h):
    self.h = h

  def getObjects(self):
    return self.objects
  def setObjects(self, objects):
    self.objects = objects
    
  def getXMLElement(self,parent):
    tb= ET.SubElement(parent,"table")
    super(TableNode,self).populateXMLElement(tb)
    if(self.h is not None):
      header= self.h.getXMLElement(tb)
    for o in self.objects:
      ob= o.getXMLElement(tb)
    return tb

    
    
    

