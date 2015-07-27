# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import ObjBase as ob
import xml.etree.cElementTree as ET


class Row(ob.ObjBase):
  def __init__(self, id, p1, p2):
    super(Row,self).__init__(id,p1,p2)


class RowP012(Row):
  def __init__(self, id, p0, p1, p2):
    super(RowP012,self).__init__(id,p1,p2)
    self.p0= p0

  def populateXMLElement(self,xmlElement):
    pp0= self.p0.getXMLElement(xmlElement,0)
    super(RowP012,self).populateXMLElement(xmlElement)

  def getXMLElement(self,parent):
    oo= ET.SubElement(parent,'row')
    oo.set("id",self.id)
    self.populateXMLElement(oo)
    return oo
