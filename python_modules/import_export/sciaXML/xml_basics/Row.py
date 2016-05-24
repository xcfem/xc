# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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

class RowP0123(RowP012):
  def __init__(self, id, p0, p1, p2, p3):
    super(RowP0123,self).__init__(id,p0,p1,p2)
    self.p3= p3

  def populateXMLElement(self,xmlElement):
    super(RowP0123,self).populateXMLElement(xmlElement)
    pp3= self.p3.getXMLElement(xmlElement,3)

  def getXMLElement(self,parent):
    oo= ET.SubElement(parent,'row')
    oo.set("id",self.id)
    self.populateXMLElement(oo)
    return oo
