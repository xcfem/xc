# -*- coding: utf-8 -*-

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

from xml_basics import Container as ctr
from xml_basics import TableXMLNodes  as tb
from xml_basics import Header as hdr
from xml_basics import HeaderItem as hi
from xml_basics import Object as obj
from xml_basics import ObjectItem as oI
from xml_basics import Row as rw
import LoadGroupContainer as lgc
import LoadCaseProperties as lcp
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
    print actionType, "Unknown action type (not permanent, not variable)."
    return 0

def getLoadCaseObject(loadCase):
  retval= obj.Object()
  id= str(loadCase.id)
  retval.setId(id)
  name= loadCase.name
  if(name==''):
    name= loadCasePrefix+id
  retval.setNm(name)
  retval.setP0(oI.ObjectItem(name)) #Name
  retval.setP1(oI.ObjectItem('{'+str(uuid.uuid4())+'}')) # Unique id
  tmp= oI.ObjectItem(str(getActionTypeCode(loadCase.actionType)))
  tmp.t= loadCase.actionType
  retval.setP2(tmp) #??
  retval.setP3(oI.ObjectItem(loadCase.desc)) #Description
  gId= str(loadCase.loadGroupId)
  gName= lgc.loadGroupPrefix+gId
  tmp= oI.ObjectItem('',gId)
  tmp.n= gName
  retval.setP4(tmp)
  ltyp= loadCase.ltyp
  ltypName= getLoadTypeName(ltyp)
  tmp= oI.ObjectItem(str(ltyp))
  tmp.t= ltypName
  retval.setP5(tmp) #??
  return retval

class LoadCaseContainer(ctr.Container):
  def __init__(self,loadCasesDict):
    super(LoadCaseContainer,self).__init__(idLoadCaseContainer,tLoadCaseContainer)
    loadCases= list()
    for key in sorted(loadCasesDict):
      ns= loadCasesDict[key]
      loadCases.append(getLoadCaseObject(ns))
    self.appendTable(tb.TableXMLNodes(idLoadCaseContainerTb,tLoadCaseContainerTb, 'Load cases', None,loadCases))
  
