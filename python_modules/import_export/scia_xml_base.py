# -*- coding: utf-8 -*-

import xml.etree.cElementTree as ET
import SciaXMLBase

nodePrefix = "N"
beamPrefix = "B"
supportPrefix = "Sn"

class sciaXMLBase(object):
  title= "Scia XML automation"

  def projectDef(self):
    cDef= ET.Element("def_project")
    cDef.set("xmlns","http://www.scia.cz")
    return cDef

  def containerDef(self,parent,cId,clsid,progid):
    cDef= ET.SubElement(parent,"def_container")
    cDef.set("id",cId)
    cDef.set("clsid",clsid)
    cDef.set("progid",progid)
    return cDef

  def tableDef(self,container,tbId,tbName,tbType,clsid,progid,size):
    tbDef= ET.SubElement(container,"def_table")
    tbDef.set("id",tbId)
    tbDef.set("name",tbName)
    tbDef.set("type",tbType)
    tbDef.set("clsid",clsid)
    tbDef.set("progid",progid)
    tbDef.set("size",size)
    return tbDef

  def defProperty(self,parent,x,name,type,id):
    property= ET.SubElement(parent,"def_property")
    property.set("x",x)
    property.set("name",name)
    property.set("type",type)
    property.set("id",id)
    return property

  def defRef(self,parent,id,progId):
    ref= ET.SubElement(parent,"def_ref")
    ref.set("id",id)
    ref.set("progid",progId)
    return set

  def newContainer(self,id,t):
    # container
    contenedor= ET.SubElement(self.project,"container")
    contenedor.set("id",id)
    contenedor.set("t",t)
    return contenedor

  def newTable(self,container,id,t,name):
    table= ET.SubElement(container,"table")
    table.set("id",id)
    table.set("t",t)
    table.set("name",name)
    return table

  def defTableRecord(self,table,recordDef):
    h= ET.SubElement(table,"h")
    cont= 0
    for key in recordDef:
      itemId= 'h'+str(cont) 
      tbItem= ET.SubElement(h,itemId)
      tbItem.set("t",key)
      cont+= 1
    return h

  def addRecord(self, obj, recordDef):
    cont= 0
    for mapItems in recordDef:
      itemId= 'p'+str(cont) 
      p= ET.SubElement(obj,itemId)
      for key in mapItems:
        p.set(key,mapItems[key])
      cont+= 1

  def strBool(self, boolValue):
    if(boolValue):
      return "1"
    else:
      return "0"

