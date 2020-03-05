# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import scxml_table_base as tBase
import xml.etree.cElementTree as ET

class SCXMLPropertiesTable(tBase.SCXMLTableBase):

  def __init__(self,id= '',name= '',typo= '',clsid='',progid=''):
    super(SCXMLPropertiesTable,self).__init__(id,'',name)
    self.typo= typo
    self.clsid= clsid
    self.progid= progid
    self.properties= list()

  def getXMLElementLabel(self):
    return "def_table"

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the table.
       Parameters:
       parent: owner of this object in the XML structure.'''
    propTable= ET.SubElement(parent,self.getXMLElementLabel())
    super(SCXMLPropertiesTable,self).populateXMLElement(propTable)
    if(self.typo!=''):
      propTable.set("type",self.typo)
    if(self.clsid!=''):
      propTable.set("clsid",self.clsid)
    if(self.progid!=''):
      propTable.set("progid",self.progid)
    size= str(len(self.properties))
    propTable.set("size",size)
    for p in self.properties:
      e= p.getXMLElement(propTable)
    return propTable
    

