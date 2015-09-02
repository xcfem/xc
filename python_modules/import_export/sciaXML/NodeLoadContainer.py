# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
import NodeLoadProperties as nlp
import LoadCaseContainer as lcc
import NodeContainer as nc

idNodeLoadContainer= "{F8371A21-F459-11D4-94D3-000000000000}"
tNodeLoadContainer= nlp.tbName
idNodeLoadContainerTb= "18E5DA6A-A48A-4522-A210-E961AF296D93"
tNodeLoadContainerTb= nlp.tbName
nodeLoadPrefix= 'F'

class NodelLoadComponent(object):
  counter= 0
  memberType= nc.idNodeContainer
  memberTypeName= nc.progIdNodes
  ''' Each of the components (X, Z or Z) of a punctual load '''
  def __init__(self, nodeId, loadCaseId, direction, value):
    self.counter+=1
    self.nodeLoadCompId= self.counter 
    self.nodeId= nodeId
    self.loadCaseId= loadCaseId
    self.direction= direction
    self.value= value
  def getDirectionObjectItem(self):
    '''returns an item which represents load component direction.'''
    dirId= '0'
    if(self.direction=='Y'):
      dirId= '1'
    elif(self.direction=='Z'):
      dirId= '2'
    return oI.ObjectItem(dirId,'','',self.direction) #Direction X, Y or Z
    


def getNodeLoadComponents(nodeLoad):
  '''get NodeLoadComponent objects from the 3D nodeLoad argument.'''
  retval= []
  vDir= nodeLoad.vDir
  nodeId= nodeLoad.tag
  loadCaseId= nodeLoad.loadCaseName
  value= nodeLoad.value
  if(vDir[0]!=0.0):
    retval.append(NodelLoadComponent(nodeId,loadCaseId,'X',value*vDir[0])) #X component.
  if(vDir[1]!=0.0):
    retval.append(NodelLoadComponent(nodeId,loadCaseId,'Y',value*vDir[1])) #Y component.
  if(vDir[2]!=0.0):
    retval.append(NodelLoadComponent(nodeId,loadCaseId,'Z',value*vDir[2])) #Z component.
  return retval
  
def getNodeLoadComponentObject(nodeLoadComponent):
  retval= obj.Object()
  loadCompId= str(nodeLoadComponent.nodeLoadCompId)
  retval.setId(loadCompId)
  name= nodeLoadPrefix+loadCompId
  retval.setNm(name)
  nodeId= str(nodeLoadComponent.nodeId) # Load name.
  retval.setP0(oI.ObjectItem(name)) #Name
  lcId= str(nodeLoadComponent.loadCaseId) #Reference to load case.
  lcName= lcc.loadCasePrefix+lcId
  tmp= oI.ObjectItem('',lcId)
  tmp.n= lcName
  retval.setP1(tmp) 
  oi2= oI.ObjectItem()
  nodeId= str(nodeLoadComponent.nodeId)
  name= nc.nodePrefix+nodeId
  row= rw.RowP012("0", oI.ObjectItem(nc.idNodeContainer, None, None, None,None,None), oI.ObjectItem(nc.progIdNodes, None, None, None,None,None), oI.ObjectItem(name, None, None, None,None,None))
  oi2.rows.append(row)

  retval.setP2(oi2) #Reference to node.
  retval.setP3(nodeLoadComponent.getDirectionObjectItem()) #Direction X, Y or Z
  retval.setP4(oI.ObjectItem('0','','','Force')) #Type 0 -> Force.
  retval.setP5(oI.ObjectItem(str(nodeLoadComponent.value))) #Value
  retval.setP6(oI.ObjectItem('0','','','GCS')) #System 0 -> GCS
  return retval


def getNodeLoadObjects(nl):
  components= getNodeLoadComponents(nl)
  retval= []
  for c in components:
    retval.append(getNodeLoadComponentObject(c))
  return retval

class NodeLoadContainer(ctr.Container):
  def __init__(self,nodeLoadsDict):
    super(NodeLoadContainer,self).__init__(idNodeLoadContainer,tNodeLoadContainer)
    nodeLoads=[]
    for nl in nodeLoadsDict:
      compObjects= getNodeLoadObjects(nl)
      for c in compObjects:
        nodeLoads.append(c)
    self.table= tb.TableNode(idNodeLoadContainerTb,tNodeLoadContainerTb, 'Point forces in node', None,nodeLoads)
  
