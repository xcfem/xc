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
import LoadCombProperties as lcp
import uuid

idLoadCombContainer= lcp.containerId
tLoadCombContainer= lcp.tbProgId
idLoadCombContainerTb= lcp.tbId
tLoadCombContainerTb= lcp.tbProgId
loadCombPrefix= 'LC'

def getLoadCombTypeName(ltype):
  if(ltype==0):
    return 'Envelope - ultimate'
  elif(ltype==1):
    return 'Envelope - serviceability'
  elif(ltype==2):
    return 'Linear - ultimate'
  elif(ltype==3):
    return 'Linear - serviceability'
  elif(ltype==4):
    return 'France - ultimate'
  elif(ltype==5):
    return 'France - serviceability'
  elif(ltype==6):
    return 'BAEL - ultimate'
  elif(ltype==7):
    return 'BAEL - serviceability'
  else:
    return 'Unknown'

def getLoadCombType(typ):
  if(typ.startswith('ELS')):
    return 3
  elif(typ.startswith('ELU')):
    return 2
  else:
    return -1

def getLoadCombComponentRow(combComp):
  id= str(combComp.id)
  lcName= combComp.loadCaseName
  if(lcName==''):
    lcName= loadCombPrefix+id
  p0= oI.ObjectItem(lcName) #Load case name
  p1= oI.ObjectItem(str(combComp.coef)) #Load case coefficient
  p2= oI.ObjectItem('',str(combComp.loadCaseId))
  p2.n= lcName
  return rw.RowP012(id,p0,p1,p2)
  retval.rows.append(row)

  return retval

def getDescompObject(descomp):
  retval= oI.ObjectItem()
  for cc in descomp:
    retval.rows.append(getLoadCombComponentRow(cc))
  return retval

def getLoadCombObject(loadComb):
  retval= obj.Object()
  id= str(loadComb.id)
  retval.setId(id)
  name= loadComb.name
  if(name==''):
    name= loadCombPrefix+id
  retval.setNm(name)
  retval.setP0(oI.ObjectItem(name)) #Name
  retval.setP1(oI.ObjectItem('{'+str(uuid.uuid4())+'}')) # Unique id
  retval.setP2(oI.ObjectItem(loadComb.desc)) #Description
  ctyp= getLoadCombType(loadComb.typ)
  ctypName= getLoadCombTypeName(ctyp)
  tmp= oI.ObjectItem(str(ctyp))
  tmp.t= ctypName
  retval.setP3(tmp)
  retval.setP4(getDescompObject(loadComb.descomp))
  retval.setP5(oI.ObjectItem('0')) #Explode
  retval.setP6(oI.ObjectItem('-1')) #Phase
  retval.setP7(oI.ObjectItem('0')) #Master ID
  return retval


class LoadCombContainer(ctr.Container):
  def __init__(self,loadCombsDict):
    super(LoadCombContainer,self).__init__(idLoadCombContainer,tLoadCombContainer)
    loadCombs=[]
    for key in sorted(loadCombsDict):
      ns= loadCombsDict[key]
      loadCombs.append(getLoadCombObject(ns))
    self.table= tb.TableNode(idLoadCombContainerTb,tLoadCombContainerTb, 'Combinations', None,loadCombs)
  
