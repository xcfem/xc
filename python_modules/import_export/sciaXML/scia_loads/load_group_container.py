# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from import_export.sciaXML.xml_basics import scxml_table_container as ctr
from import_export.sciaXML.xml_basics import scxml_table_xmlnodes as tb
from import_export.sciaXML.xml_basics import scxml_object as obj
from import_export.sciaXML.xml_basics import scxml_object_item as oI
from import_export.sciaXML.xml_basics import scxml_row as rw
from import_export.sciaXML import node_container as nc
import load_group_properties as lgp

idLoadGroupContainer= lgp.containerId
tLoadGroupContainer= lgp.tbProgId
idLoadGroupContainerTb= lgp.tbId
tLoadGroupContainerTb= lgp.tbProgId
loadGroupPrefix= 'LG'

def getLoadGroupObject(loadGroup):
  retval= obj.SCXMLObject()
  id= str(loadGroup.id)
  retval.setId(id)
  name= loadGroupPrefix+id
  retval.setNm(name)
  retval.setP0(oI.SCXMLObjectItem(name)) #Name
  tmp= None
  if(loadGroup.permanent):
    tmp= oI.SCXMLObjectItem('0')
    tmp.t= 'Permanent'
  else:
    tmp= oI.SCXMLObjectItem('1')
    tmp.t= 'Variable' 
  retval.setP1(tmp)
  return retval

class LoadGroupContainer(ctr.SCXMLTableContainer):
  def __init__(self,loadGroupsDict):
    super(LoadGroupContainer,self).__init__(idLoadGroupContainer,tLoadGroupContainer)
    loadGroups= list()
    for key in sorted(loadGroupsDict):
      ns= loadGroupsDict[key]
      loadGroups.append(getLoadGroupObject(ns))
    self.appendTable(tb.SCXMLTableXMLNodes(idLoadGroupContainerTb,tLoadGroupContainerTb, 'Load groups', None,loadGroups))
  
