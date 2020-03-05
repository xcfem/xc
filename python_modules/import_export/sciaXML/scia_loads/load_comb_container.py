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
import load_group_container as lgc
import load_comb_properties as lcp
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
  p0= oI.SCXMLObjectItem(lcName) #Load case name
  p1= oI.SCXMLObjectItem(str(combComp.coef)) #Load case coefficient
  p2= oI.SCXMLObjectItem('',str(combComp.loadCaseId))
  p2.n= lcName
  return rw.SCXMLRowP012(id,p0,p1,p2)
  retval.rows.append(row)

  return retval

def getDescompObject(descomp):
  retval= oI.SCXMLObjectItem()
  for cc in descomp:
    retval.rows.append(getLoadCombComponentRow(cc))
  return retval

def getLoadCombObject(loadComb):
  retval= obj.SCXMLObject()
  id= str(loadComb.id)
  retval.setId(id)
  name= loadComb.name
  if(name==''):
    name= loadCombPrefix+id
  retval.setNm(name)
  retval.setP0(oI.SCXMLObjectItem(name)) #Name
  retval.setP1(oI.SCXMLObjectItem('{'+str(uuid.uuid4())+'}')) # Unique id
  retval.setP2(oI.SCXMLObjectItem(loadComb.desc)) #Description
  ctyp= getLoadCombType(loadComb.typ)
  ctypName= getLoadCombTypeName(ctyp)
  tmp= oI.SCXMLObjectItem(str(ctyp))
  tmp.t= ctypName
  retval.setP3(tmp)
  retval.setP4(getDescompObject(loadComb.descomp))
  retval.setP5(oI.SCXMLObjectItem('0')) #Explode
  retval.setP6(oI.SCXMLObjectItem('-1')) #Phase
  retval.setP7(oI.SCXMLObjectItem('0')) #Master ID
  return retval


class LoadCombContainer(ctr.SCXMLTableContainer):
  def __init__(self,loadCombsDict):
    super(LoadCombContainer,self).__init__(idLoadCombContainer,tLoadCombContainer)
    loadCombs= list()
    for key in sorted(loadCombsDict):
      ns= loadCombsDict[key]
      loadCombs.append(getLoadCombObject(ns))
    self.appendTable(tb.SCXMLTableXMLNodes(idLoadCombContainerTb,tLoadCombContainerTb, 'Combinations', None,loadCombs))
  
