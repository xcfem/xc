# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from import_export.sciaXML.xml_basics import Container as ctr
from import_export.sciaXML.xml_basics import TableXMLNodes  as tb
from import_export.sciaXML.xml_basics import Header as hdr
from import_export.sciaXML.xml_basics import HeaderItem as hi
from import_export.sciaXML.xml_basics import Object as obj
from import_export.sciaXML.xml_basics import ObjectItem as oI
from import_export.sciaXML.xml_basics import Row as rw
from import_export.sciaXML import node_container as nc
import LoadGroupProperties as lgp

idLoadGroupContainer= lgp.containerId
tLoadGroupContainer= lgp.tbProgId
idLoadGroupContainerTb= lgp.tbId
tLoadGroupContainerTb= lgp.tbProgId
loadGroupPrefix= 'LG'

def getLoadGroupObject(loadGroup):
  retval= obj.Object()
  id= str(loadGroup.id)
  retval.setId(id)
  name= loadGroupPrefix+id
  retval.setNm(name)
  retval.setP0(oI.ObjectItem(name)) #Name
  tmp= None
  if(loadGroup.permanent):
    tmp= oI.ObjectItem('0')
    tmp.t= 'Permanent'
  else:
    tmp= oI.ObjectItem('1')
    tmp.t= 'Variable' 
  retval.setP1(tmp)
  return retval

class LoadGroupContainer(ctr.Container):
  def __init__(self,loadGroupsDict):
    super(LoadGroupContainer,self).__init__(idLoadGroupContainer,tLoadGroupContainer)
    loadGroups= list()
    for key in sorted(loadGroupsDict):
      ns= loadGroupsDict[key]
      loadGroups.append(getLoadGroupObject(ns))
    self.appendTable(tb.TableXMLNodes(idLoadGroupContainerTb,tLoadGroupContainerTb, 'Load groups', None,loadGroups))
  
