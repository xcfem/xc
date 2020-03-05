# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scxml_obj_base as ob
import xml.etree.cElementTree as ET


class SCXMLRow(ob.SCXMLObjBase):
  def __init__(self, id, p1, p2):
    super(SCXMLRow,self).__init__(id,p1,p2)


class SCXMLRowP012(SCXMLRow):
  def __init__(self, id, p0, p1, p2):
    super(SCXMLRowP012,self).__init__(id,p1,p2)
    self.p0= p0

  def populateXMLElement(self,xmlElement):
    pp0= self.p0.getXMLElement(xmlElement,0)
    super(SCXMLRowP012,self).populateXMLElement(xmlElement)

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the row.
       Parameters:
       parent: owner of this object in the XML structure.'''
    oo= ET.SubElement(parent,'row')
    oo.set("id",self.id)
    self.populateXMLElement(oo)
    return oo

class SCXMLRowP0123(SCXMLRowP012):
  def __init__(self, id, p0, p1, p2, p3):
    super(SCXMLRowP0123,self).__init__(id,p0,p1,p2)
    self.p3= p3

  def populateXMLElement(self,xmlElement):
    super(SCXMLRowP0123,self).populateXMLElement(xmlElement)
    pp3= self.p3.getXMLElement(xmlElement,3)

