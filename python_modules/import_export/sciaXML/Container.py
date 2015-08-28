# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Base as b
import TableNode as tn
import xml.etree.cElementTree as ET

class Container(b.Base):
  table= tn.TableNode()

  def __init__(self,id,t,table= None):
    super(Container,self).__init__(id,t)
    self.table= table

  def getTable(self):
    return self.table
  def setTable(self,table):
    self.table= table
        
  def getXMLElement(self,parent):
    container= ET.SubElement(parent,"container")
    super(Container,self).populateXMLElement(container)
    tb= self.table.getXMLElement(container)
    return container

  
