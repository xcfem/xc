# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
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
    loadCases=[]
    for key in sorted(loadCasesDict):
      ns= loadCasesDict[key]
      loadCases.append(getLoadCaseObject(ns))
    self.table= tb.TableNode(idLoadCaseContainerTb,tLoadCaseContainerTb, 'Load cases', None,loadCases)
  
