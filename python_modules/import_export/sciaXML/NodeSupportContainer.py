# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
import NodeContainer as nc

idNodeSupportContainer= "{1cbca4de-355b-40f7-a91d-8efd26a6404d}"
tNodeSupportContainer= "1cbca4de-355b-40f7-a91d-8efd26a6404d"
idNodeSupportContainerTb= "6b8df80b-aad0-4570-84dd-4af1d7c683d2"
tNodeSupportContainerTb= "DataAddSupport.EP_PointSupportPoint.1"
nodeSupportPrefix= 'Sn'

def getNodeSupportObject(nodeConstraint):
  retval= obj.Object()
  id= str(cell.id)
  #retval.setId(id)
  name= nodeSupportPrefix+id
  retval.setNm(name)
  nodeId= str(cell.nodeId)
  retval.setP1(oI.ObjectItem(nodeId,nodeId+' - ')) #Ref Node.
  return retval

class NodeSupportContainer(ctr.Container):
  def __init__(self,nodeSupportsDict):
    super(NodeSupportContainer,self).__init__(idNodeSupportContainer,tNodeSupportContainer)
    nodeSupports=[]
    for key in nodeSupportsDict:
      cell= nodeSupportsDict[key]
      nodeSupports.append(getNodeSupportObject(cell))
    self.table= tb.TableNode(idNodeSupportContainerTb,tNodeSupportContainerTb, '', None,nodeSupports)
  
