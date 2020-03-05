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
import node_load_properties as nlp
import load_case_container as lcc
from import_export.sciaXML import node_container as nc
import load_component_base as lcb

idNodeLoadContainer= "{F8371A21-F459-11D4-94D3-000000000000}"
tNodeLoadContainer= nlp.tbName
idNodeLoadContainerTb= "18E5DA6A-A48A-4522-A210-E961AF296D93"
tNodeLoadContainerTb= nlp.tbName
nodeLoadPrefix= 'F'

class NodeLoadComponent(lcb.LoadComponentBase):
  ''' Each of the components (X, Z or Z) of a punctual load '''
  counter= 0
  memberType= nc.idNodeContainer
  memberTypeName= nc.progIdNodes
  def __init__(self, nodeId, loadCaseId, loadCaseName, direction, value):
    super(NodeLoadComponent,self).__init__(loadCaseId, loadCaseName, direction, value)
    NodeLoadComponent.counter+=1
    self.nodeLoadCompId= NodeLoadComponent.counter 
    self.nodeId= nodeId

  def getNodeReferenceItem(self):
    retval= oI.SCXMLObjectItem()
    nodeId= str(self.nodeId)
    name= nc.nodePrefix+nodeId
    row= rw.SCXMLRowP012("0", oI.SCXMLObjectItem(nc.idNodeContainer, None, None, None,None,None), oI.SCXMLObjectItem(nc.progIdNodes, None, None, None,None,None), oI.SCXMLObjectItem(name, None, None, None,None,None))
    retval.rows.append(row)
    return retval
    
  def getObject(self):
    retval= obj.SCXMLObject()
    loadCompId= str(self.nodeLoadCompId)
    retval.setId(loadCompId)
    name= nodeLoadPrefix+loadCompId
    retval.setNm(name)
    nodeId= str(self.nodeId) # Node id
    retval.setP0(oI.SCXMLObjectItem(name)) #Name
    retval.setP1(self.getLoadCaseReferenceItem()) #Reference to load case.
    retval.setP2(self.getNodeReferenceItem()) #Reference to node.
    retval.setP3(self.getDirectionObjectItem()) #Direction X, Y or Z
    retval.setP4(oI.SCXMLObjectItem('0','','','Force')) #Type 0 -> Force.
    retval.setP5(self.getValueObjectItem()) #Value
    retval.setP6(self.getSystemItem()) #System 0 -> GCS, 1 -> LCS
    return retval


def getNodeLoadComponents(nodeLoad):
  '''get NodeLoadComponent objects from the 3D nodeLoad argument.'''
  retval= list()
  vDir= nodeLoad.vDir
  nodeId= nodeLoad.tag
  loadCaseId= nodeLoad.loadCaseId
  loadCaseName= nodeLoad.loadCaseName
  value= nodeLoad.value
  if(vDir[0]!=0.0):
    retval.append(NodeLoadComponent(nodeId, loadCaseId, loadCaseName, 'X',value*vDir[0])) #X component.
  if(vDir[1]!=0.0):
    retval.append(NodeLoadComponent(nodeId, loadCaseId, loadCaseName, 'Y',value*vDir[1])) #Y component.
  if(vDir[2]!=0.0):
    retval.append(NodeLoadComponent(nodeId, loadCaseId, loadCaseName, 'Z',value*vDir[2])) #Z component.
  return retval
  
def getNodeLoadObjects(nl):
  components= getNodeLoadComponents(nl)
  retval= list()
  for c in components:
    retval.append(c.getObject())
  return retval

class NodeLoadContainer(ctr.SCXMLTableContainer):
  def __init__(self,nodeLoadsDict):
    super(NodeLoadContainer,self).__init__(idNodeLoadContainer,tNodeLoadContainer)
    nodeLoads= list()
    for nl in nodeLoadsDict:
      compObjects= getNodeLoadObjects(nl)
      for c in compObjects:
        nodeLoads.append(c)
    self.appendTable(tb.SCXMLTableXMLNodes(idNodeLoadContainerTb,tNodeLoadContainerTb, 'Point forces in node', None,nodeLoads))
  
