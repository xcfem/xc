# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scxml_base as b
import xml.etree.cElementTree as ET

class SCXMLTableBase(b.SCXMLBase):
  '''Base class for al sciaXML tables.'''

  def __init__(self,id= '',t= '',name= ''):
    super(SCXMLTableBase,self).__init__(id,t)
    self.name = name

  def getName(self):
    return self.name
  def setName(self,name):
    self.name = name
    
  def populateXMLElement(self,xmlElement):
    super(SCXMLTableBase,self).populateXMLElement(xmlElement)
    if(self.name!=''):
      xmlElement.set("name",self.name)

    
    
    

