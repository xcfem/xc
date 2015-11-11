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

def getNodeSupportObject(nodeSupport):
  retval= obj.Object()
  id= str(nodeSupport.id)
  retval.setId(id)
  name= nodeSupportPrefix+id
  retval.setNm(name)
  nodeId= str(nodeSupport.nodeId) # Support name.
  retval.setP0(oI.ObjectItem(name)) #Name
  retval.setP1(oI.ObjectItem(nodeId+' - ', nodeId)) #Ref Node.
  retval.setP2(oI.ObjectItem('0')) #Support type.
  retval.setP3(oI.ObjectItem(str(nodeSupport.xComp.k))) #x component stiffness.
  retval.setP4(oI.ObjectItem(nodeSupport.xComp.getTypeCode())) #x component constraint.
  retval.setP5(oI.ObjectItem(str(nodeSupport.yComp.k))) #y component stiffness.
  retval.setP6(oI.ObjectItem(nodeSupport.yComp.getTypeCode())) #y component constraint.
  retval.setP7(oI.ObjectItem(str(nodeSupport.zComp.k))) #z component stiffness.
  retval.setP8(oI.ObjectItem(nodeSupport.zComp.getTypeCode())) #z component constraint.
  retval.setP9(oI.ObjectItem(str(nodeSupport.rxComp.k))) #rx component stiffness.
  retval.setP10(oI.ObjectItem(nodeSupport.rxComp.getTypeCode())) #rx component constraint.
  retval.setP11(oI.ObjectItem(str(nodeSupport.ryComp.k))) #ry component stiffness.
  retval.setP12(oI.ObjectItem(nodeSupport.ryComp.getTypeCode())) #ry component constraint.

  retval.setP13(oI.ObjectItem(str(nodeSupport.rzComp.k))) #rz component stiffness.
  retval.setP14(oI.ObjectItem(nodeSupport.rzComp.getTypeCode())) #rz component constraint.
  return retval

class NodeSupportContainer(ctr.Container):
  def __init__(self,nodeSupportsDict):
    super(NodeSupportContainer,self).__init__(idNodeSupportContainer,tNodeSupportContainer)
    nodeSupports= list()
    for key in nodeSupportsDict:
      ns= nodeSupportsDict[key]
      nodeSupports.append(getNodeSupportObject(ns))
    self.table= tb.TableNode(idNodeSupportContainerTb,tNodeSupportContainerTb, '', None,nodeSupports)
  
