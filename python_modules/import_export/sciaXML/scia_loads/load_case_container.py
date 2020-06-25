# -*- coding: utf-8 -*-
from __future__ import print_function


#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
import load_group_container as lgc
import load_case_properties as lcp
import uuid

idLoadCaseContainer= lcp.containerId
tLoadCaseContainer= lcp.tbProgId
idLoadCaseContainerTb= lcp.tbId
tLoadCaseContainerTb= lcp.tbProgId
loadCasePrefix= 'LC'

def getLoadTypeName(ltype):
  if(ltype==0):
    return 'Poids propre'
  elif(ltype==1):
    return 'Standard'
  elif(ltype==2):
    return 'Effet primaire'

def getActionTypeCode(actionType):
  if(actionType=='Permanent'):
    return 0
  elif(actionType=='Variable'):
    return 1
  else:
    print(actionType, "Unknown action type (not permanent, not variable).")
    return 0

def getLoadCaseObject(loadCase):
  retval= obj.SCXMLObject()
  id= str(loadCase.id)
  retval.setId(id)
  name= loadCase.name
  if(name==''):
    name= loadCasePrefix+id
  retval.setNm(name)
  retval.setP0(oI.SCXMLObjectItem(name)) #Name
  retval.setP1(oI.SCXMLObjectItem('{'+str(uuid.uuid4())+'}')) # Unique id
  tmp= oI.SCXMLObjectItem(str(getActionTypeCode(loadCase.actionType)))
  tmp.t= loadCase.actionType
  retval.setP2(tmp) #??
  retval.setP3(oI.SCXMLObjectItem(loadCase.desc)) #Description
  gId= str(loadCase.loadGroupId)
  gName= lgc.loadGroupPrefix+gId
  tmp= oI.SCXMLObjectItem('',gId)
  tmp.n= gName
  retval.setP4(tmp)
  ltyp= loadCase.ltyp
  ltypName= getLoadTypeName(ltyp)
  tmp= oI.SCXMLObjectItem(str(ltyp))
  tmp.t= ltypName
  retval.setP5(tmp) #??
  return retval

class LoadCaseContainer(ctr.SCXMLTableContainer):
  def __init__(self,loadCasesDict):
    super(LoadCaseContainer,self).__init__(idLoadCaseContainer,tLoadCaseContainer)
    loadCases= list()
    for key in sorted(loadCasesDict):
      ns= loadCasesDict[key]
      loadCases.append(getLoadCaseObject(ns))
    self.appendTable(tb.SCXMLTableXMLNodes(idLoadCaseContainerTb,tLoadCaseContainerTb, 'Load cases', None,loadCases))
  
