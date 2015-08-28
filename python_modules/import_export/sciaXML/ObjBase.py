# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import ObjectItem as oi
import xml.etree.cElementTree as ET

class ObjBase(object):
	
  def __init__(self, id, p1= oi.ObjectItem(), p2= oi.ObjectItem()):
    self.id= id
    self.p1 = p1
    self.p2 = p2
	
  def getP1(self):
    return self.p1
  def setP1(self, p1):
    self.p1 = p1

  def getP2(self):
    return self.p2
  def setP2(self, p2):
    self.p2 = p2

  def getId(self):
    return self.id
  def setId(self, id):
    self.id = id

  def populateXMLElement(self,xmlElement):
    pp1= self.p1.getXMLElement(xmlElement,1)
    pp2= self.p2.getXMLElement(xmlElement,2)

  def getXMLElement(self,parent):
    oo= ET.SubElement(parent,'row')
    oo.set("id",self.id)
    self.populateXMLElement(oo)
    return oo
