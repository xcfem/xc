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
import LoadComponentBase as lcb
import uuid

idElementLoadContainer= "{BC16B3CA-F464-11D4-94D3-000000000000}"
tElementLoadContainer= elp.tbName
idElementLoadContainerTb= "FC7CC6EF-1D04-492E-B6B2-C70DD56DE898"
tElementLoadContainerTb= elp.tbName
elementLoadPrefix= 'SF'
class ElementLoadComponent(lcb.LoadComponentBase):
  ''' Each of the components (X, Z or Z) of a punctual load '''
  counter= 0
  memberType= ec.idEPPlaneContainer
  memberTypeName= ec.tEPPlaneContainerTb
  def __init__(self, elementId, loadCaseId, loadCaseName, direction, value, globalCooSys):
    super(ElementLoadComponent,self).__init__(loadCaseId, loadCaseName, direction, value, globalCooSys)
    ElementLoadComponent.counter+=1
    self.elementLoadCompId= ElementLoadComponent.counter
    print "loadCompId= ", self.elementLoadCompId 
    self.elementId= elementId

  def getElementReferenceItem(self):
    retval= oI.ObjectItem()
    elementId= str(self.elementId)
    name= ec.elementPrefix+elementId
    row= rw.RowP0123("0", oI.ObjectItem(ec.idEPPlaneContainer, None, None, None,None,None), oI.ObjectItem(ec.tEPPlaneContainerTb, None, None, None,None,None), oI.ObjectItem(elementId, None, None, None,None,None), oI.ObjectItem(name, None, None, None,None,None))
    retval.rows.append(row)
    return retval
    
  def getObject(self):
    retval= obj.Object()
    loadCompId= str(self.elementLoadCompId)
    retval.setId(loadCompId)
    name= elementLoadPrefix+loadCompId
    retval.setNm(name)
    elementId= str(self.elementId) # Load name.
    retval.setP0(self.getElementReferenceItem()) #Reference to element.
    retval.setP1(self.getLoadCaseReferenceItem()) #Reference to load case.
    retval.setP2(oI.ObjectItem(name)) #Name
    retval.setP3(oI.ObjectItem('{'+str(uuid.uuid4())+'}')) #Unique id
    retval.setP4(self.getDirectionObjectItem()) #Direction X, Y or Z
    retval.setP5(oI.ObjectItem('0','','','Force')) #Type 0 -> Force.
    retval.setP6(self.getValueObjectItem()) #Value
    retval.setP7(self.getSystemItem()) #System 0 -> GCS, 1 -> LCS
    retval.setP8(oI.ObjectItem('0','','','Length')) #Location.
    return retval

def getElementLoadComponents(elementLoad):
  '''get ElementLoadComponent objects from the 3D elementLoad argument.'''
  retval= []
  vDir= elementLoad.vDir
  loadCaseId= elementLoad.loadCaseId
  loadCaseName= elementLoad.loadCaseName
  value= elementLoad.value
  tags= elementLoad.tags
  mode= elementLoad.mode #True if referred to global coordinate system.
  for elementId in tags:
    if(vDir[0]!=0.0):
      vComp= value*vDir[0]
      retval.append(ElementLoadComponent(elementId,loadCaseId, loadCaseName, 'X',vComp, mode)) #X component.
    if(vDir[1]!=0.0):
      vComp= value*vDir[1]
      retval.append(ElementLoadComponent(elementId,loadCaseId, loadCaseName, 'Y',vComp, mode)) #Y component.
    if(vDir[2]!=0.0):
      vComp= value*vDir[2]
      retval.append(ElementLoadComponent(elementId,loadCaseId, loadCaseName, 'Z',vComp, mode)) #Z component.
  return retval

def getElementLoadObjects(nl):
  components= getElementLoadComponents(nl)
  retval= []
  for c in components:
    retval.append(c.getObject())
  return retval

class ElementLoadContainer(ctr.Container):
  def __init__(self,elementLoadsDict):
    super(ElementLoadContainer,self).__init__(idElementLoadContainer,tElementLoadContainer)
    elementLoads=[]
    for el in elementLoadsDict:
      compObjects= getElementLoadObjects(el)
      for c in compObjects:
        elementLoads.append(c)
    self.table= tb.TableNode(idElementLoadContainerTb,tElementLoadContainerTb, 'Forces on surface', None,elementLoads)
  
