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
import ElementLoadProperties as elp
import LoadCaseContainer as lcc
from import_export.sciaXML import ep_plane_container as ec
import LoadComponentBase as lcb
import uuid
import math

idElementLoadContainer= "{BC16B3CA-F464-11D4-94D3-000000000000}"
tElementLoadContainer= elp.tbName
idElementLoadContainerTb= "FC7CC6EF-1D04-492E-B6B2-C70DD56DE898"
tElementLoadContainerTb= elp.tbName
elementLoadPrefix= 'SF'
class ElementLoadComponent(lcb.LoadComponentBase):
  ''' Each of the components (X, Z or Z) of an elemental load '''
  counter= 0
  memberType= ec.idEPPlaneContainer
  memberTypeName= ec.tEPPlaneContainerTb
  def __init__(self, elementId, loadCaseId, loadCaseName, direction, value, globalCooSys):
    super(ElementLoadComponent,self).__init__(loadCaseId, loadCaseName, direction, value, globalCooSys)
    ElementLoadComponent.counter+=1
    self.elementLoadCompId= ElementLoadComponent.counter
    #print "loadCompId= ", self.elementLoadCompId 
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
  retval= list()
  vDir= elementLoad.vDir
  normVDir= math.sqrt(vDir[0]**2+vDir[1]**2+vDir[2]**2)
  if(normVDir<1e-3):
    print "getElementLoadComponents: vDir vector very small: ", vDir, " load ignored."
  else:
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
  retval= list()
  for c in components:
    retval.append(c.getObject())
  return retval

class ElementLoadContainer(ctr.Container):
  def __init__(self,elementLoadsDict):
    super(ElementLoadContainer,self).__init__(idElementLoadContainer,tElementLoadContainer)
    elementLoads= list()
    for el in elementLoadsDict:
      compObjects= getElementLoadObjects(el)
      for c in compObjects:
        elementLoads.append(c)
    self.appendTable(tb.TableXMLNodes(idElementLoadContainerTb,tElementLoadContainerTb, 'Forces on surface', None,elementLoads))

  def __len__(self):
    return len(self.table)
