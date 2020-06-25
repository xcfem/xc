# -*- coding: utf-8 -*-
from __future__ import print_function


#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import uuid
from import_export.sciaXML.xml_basics import scxml_table_container as ctr
import surface_pressure_free_properties as spfp
from import_export.sciaXML.xml_basics import scxml_row as rw
import load_component_base as lcb
from import_export.sciaXML.xml_basics import scxml_object as obj
from import_export.sciaXML.xml_basics import scxml_object_item as oI
from import_export.sciaXML.xml_basics import scxml_table_xmlnodes as tb
import xml.etree.cElementTree as ET

class PolygonPointRow(rw.SCXMLRowP0123):
  '''SCIA XML object for each of the points
     that define the polygon in which the load acts.'''
  def __init__(self, id, x, y, z):
    p0= oI.SCXMLObjectItem(v= 'Head') #Node
    p1= oI.SCXMLObjectItem('0','','','Standard') #Point definition
    p2= oI.SCXMLObjectItem(str(x),'','','') #X coordinate.
    p3= oI.SCXMLObjectItem(str(y),'','','') #Y coordinate.
    super(PolygonPointRow,self).__init__(id,p0,p1,p2,p3)
    self.p4= oI.SCXMLObjectItem(str(z),'','','') #Z coordinate.
    self.p22= oI.SCXMLObjectItem(v= 'Ligne') #Edge

  def populateXMLElement(self,xmlElement):
    super(PolygonPointRow,self).populateXMLElement(xmlElement)
    pp4= self.p4.getXMLElement(xmlElement,4)
    pp22= self.p22.getXMLElement(xmlElement,22)
 
  def getXMLElement(self,parent):
    oo= ET.SubElement(parent,'row')
    oo.set("id",self.id)
    self.populateXMLElement(oo)
    return oo


idSurfacePressureFreeContainer= spfp.containerId
tSurfacePressureFreeContainer= spfp.tbName
idSurfacePressureFreeContainerTb= spfp.tbId
tSurfacePressureFreeContainerTb= spfp.tbName
surfacePressureFreePrefix= 'SF'
class SurfacePressureFreeComponent(lcb.LoadComponentBase):
  ''' Each of the components (X, Z or Z) of an surface pressure.'''
  counter= 0
  def __init__(self, loadCaseId, loadCaseName, direction, distribution, value, polygon, globalCooSys):
    super(SurfacePressureFreeComponent,self).__init__(loadCaseId, loadCaseName, direction, value, globalCooSys)
    self.distribution= distribution
    self.polygon= polygon
    SurfacePressureFreeComponent.counter+=1
    self.surfacePressureFreeCompId= SurfacePressureFreeComponent.counter
    #print("loadCompId= ", self.surfacePressureFreeCompId )

  def getPolygonRows(self,polygon):
    retval= oI.SCXMLObjectItem()
    counter= 0
    for point in polygon:
      retval.rows.append(PolygonPointRow(str(counter),point.x,point.y,point.z))
      counter+= 1
    return retval    

  def getObject(self):
    retval= obj.SCXMLObject()
    loadCompId= str(self.surfacePressureFreeCompId)
    retval.setId(loadCompId)
    name= surfacePressureFreePrefix+loadCompId
    retval.setNm(name)
    retval.setP0(self.getLoadCaseReferenceItem()) #Reference to load case.
    retval.setP1(oI.SCXMLObjectItem(name)) #Name
    retval.setP2(oI.SCXMLObjectItem('{'+str(uuid.uuid4())+'}')) #Unique id
    retval.setP3(self.getDirectionObjectItem()) #Direction X, Y or Z
    retval.setP4(oI.SCXMLObjectItem('0','','','Force')) #Type 0 -> Force.
    retval.setP5(self.getDistributionObjectItem()) #Distribution (uniform,...)
    retval.setP6(self.getValueObjectItem()) #Value
    retval.setP7(oI.SCXMLObjectItem('4','','','Z= 0')) #Validity
    retval.setP8(oI.SCXMLObjectItem('0','','','Auto')) #Select
    retval.setP9(self.getSystemItem()) #System 0 -> GCS, 1 -> LCS
    retval.setP10(oI.SCXMLObjectItem('0','','','Length')) #Location.
    retval.setP11(self.getPolygonRows(self.polygon)) #Reference to node.
    return retval

def getSurfacePressureFreeComponents(surfacePressureFree):
  '''get SurfacePressureFreeComponent objects from the 3D surfacePressureFree argument.'''
  retval= list()
  vDir= surfacePressureFree.vDir
  normVDir= math.sqrt(vDir[0]**2+vDir[1]**2+vDir[2]**2)
  if(normVDir<1e-3):
    print("getSurfacePressureFreeComponents: vDir vector very small: ", vDir, " load ignored.")
  else:
    loadCaseId= surfacePressureFree.loadCaseId
    loadCaseName= surfacePressureFree.loadCaseName
    distrib= 'Uniform'
    value= surfacePressureFree.value
    polygon= surfacePressureFree.polygon
    globalCooSys= True #True if referred to global coordinate system.
    if(vDir[0]!=0.0):
      vComp= value*vDir[0]
      retval.append(SurfacePressureFreeComponent(loadCaseId, loadCaseName, 'X', distrib, vComp, polygon, globalCooSys)) #X component.
    if(vDir[1]!=0.0):
      vComp= value*vDir[1]
      retval.append(SurfacePressureFreeComponent(loadCaseId, loadCaseName, 'Y', distrib, vComp, polygon, globalCooSys)) #Y component.
    if(vDir[2]!=0.0):
      vComp= value*vDir[2]
      retval.append(SurfacePressureFreeComponent(loadCaseId, loadCaseName, 'Z', distrib, vComp, polygon, globalCooSys)) #Z component.
  return retval

def getSurfacePressureFreeObjects(nl):
  components= getSurfacePressureFreeComponents(nl)
  retval= list()
  for c in components:
    retval.append(c.getObject())
  return retval

class SurfacePressureFreeContainer(ctr.SCXMLTableContainer):
  def __init__(self,surfacePressureFreesDict):
    super(SurfacePressureFreeContainer,self).__init__(idSurfacePressureFreeContainer,tSurfacePressureFreeContainer)
    surfacePressureFrees= list()
    for el in surfacePressureFreesDict:
      compObjects= getSurfacePressureFreeObjects(el)
      for c in compObjects:
        surfacePressureFrees.append(c)
    self.appendTable(tb.SCXMLTableXMLNodes(idSurfacePressureFreeContainerTb,tSurfacePressureFreeContainerTb, 'Free surface load', None,surfacePressureFrees))

  def __len__(self):
    return len(self.table)
