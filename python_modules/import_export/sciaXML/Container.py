# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Base as b
import TableNode as tn
import xml.etree.cElementTree as ET

class Container(b.Base):

  def __init__(self,id,t,table= None):
    super(Container,self).__init__(id,t)
    self.tables= list()
    if(tables):
      self.tables.append(table)

  def getTables(self):
    return self.tables
  def appendTable(self,table):
    self.tables.append(table)
        
  def getXMLElement(self,parent):
    container= ET.SubElement(parent,"container")
    super(Container,self).populateXMLElement(container)
    for tb in self.tables:
      xmlTB= tb.getXMLElement(container)
    return container

  
