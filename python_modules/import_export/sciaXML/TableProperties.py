# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import TableBase as tBase
import Header as hdr
import xml.etree.cElementTree as ET

class TableProperties(tBase.TableBase):

  def __init__(self,id= '',name= '',typo= '',clsid='',progid=''):
    super(TableProperties,self).__init__(id,'',name)
    self.typo= typo
    self.clsid= clsid
    self.progid= progid
    self.properties= list()

  def getXMLElementLabel(self):
    return "def_table"

  def getXMLElement(self,parent):
    tbProp= ET.SubElement(parent,self.getXMLElementLabel())
    super(TableProperties,self).populateXMLElement(tbProp)
    if(self.typo!=''):
      tbProp.set("type",self.typo)
    if(self.clsid!=''):
      tbProp.set("clsid",self.clsid)
    if(self.progid!=''):
      tbProp.set("progid",self.progid)
    size= str(len(self.properties))
    tbProp.set("size",size)
    for p in self.properties:
      e= p.getXMLElement(tbProp)
    return tbProp
    

