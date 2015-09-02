# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
import ElementLoadProperties as elp
import LoadCaseContainer as lcc
import EPPlaneContainer as ec

idElementLoadContainer= "{BC16B3CA-F464-11D4-94D3-000000000000}"
tElementLoadContainer= elp.tbName
idElementLoadContainerTb= "FC7CC6EF-1D04-492E-B6B2-C70DD56DE898"
tElementLoadContainerTb= elp.tbName
elementLoadPrefix= 'F'

def getElementLoadObject(elementLoadComponent):
  retval= obj.Object()
  id= str(elementLoadComponent.id)
  retval.setId(id)
  name= elementLoadComponentPrefix+id
  retval.setNm(name)
  elementId= str(elementLoadComponent.elementId) # Load name.
  retval.setP0(oI.ObjectItem(name)) #Name
  lcId= str(elementLoadComponent.loadCaseId) #Reference to load case.
  lcName= lcc.loadCasePrefix+lcId
  tmp= oI.ObjectItem('',lcId)
  tmp.n= lcName
  XXX
  retval.setP1(tmp) 
  oi2= oI.ObjectItem()
  elementId= str(elementLoadComponent.elementId)
  name= nc.elementPrefix+elementId
  row= rw.RowP0123("0", oI.ObjectItem(nc.idElementContainer, None, None, None,None,None), oI.ObjectItem(nc.progIdElements, None, None, None,None,None), oI.ObjectItem(elementId, None, None, None,None,None), oI.ObjectItem(name, None, None, None,None,None))
  oi2.rows.append(row)

  retval.setP2(oi2) #Reference to element.
  retval.setP3(oI.ObjectItem(str(elementLoadComponent.direction))) #Direction X, Y or Z
  retval.setP4(oI.ObjectItem('0')) #Type 0 -> Force.
  retval.setP5(oI.ObjectItem(str(elementLoadComponent.value))) #Value
  retval.setP6(oI.ObjectItem('0')) #System 0 -> GCS
  return retval

class ElementLoadContainer(ctr.Container):
  def __init__(self,elementLoadsDict):
    super(ElementLoadContainer,self).__init__(idElementLoadContainer,tElementLoadContainer)
    elementLoads=[]
    for key in elementLoadsDict:
      ns= elementLoadsDict[key]
      elementLoads.append(getElementLoadObject(ns))
    self.table= tb.TableNode(idElementLoadContainerTb,tElementLoadContainerTb, 'Forces on surface', None,elementLoads)
  
