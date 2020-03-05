# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from xml_basics import scxml_table_container as ctr
from xml_basics import scxml_table_xmlnodes as tb
from xml_basics import scxml_header_item as hi
from xml_basics import scxml_object as obj
from xml_basics import scxml_object_item as oI
from xml_basics import scxml_row as rw
import node_container as nc

idNodeSupportContainer= "{1cbca4de-355b-40f7-a91d-8efd26a6404d}"
tNodeSupportContainer= "1cbca4de-355b-40f7-a91d-8efd26a6404d"
idNodeSupportContainerTb= "6b8df80b-aad0-4570-84dd-4af1d7c683d2"
tNodeSupportContainerTb= "DataAddSupport.EP_PointSupportPoint.1"
nodeSupportPrefix= 'Sn'

def getNodeSupportObject(nodeSupport):
  retval= obj.SCXMLObject()
  id= str(nodeSupport.id)
  retval.setId(id)
  name= nodeSupportPrefix+id
  retval.setNm(name)
  nodeId= str(nodeSupport.nodeId) # Support name.
  retval.setP0(oI.SCXMLObjectItem(name)) #Name
  retval.setP1(oI.SCXMLObjectItem(nodeId+' - ', nodeId)) #Ref Node.
  retval.setP2(oI.SCXMLObjectItem('0')) #Support type.
  retval.setP3(oI.SCXMLObjectItem(str(nodeSupport.xComp.k))) #x component stiffness.
  retval.setP4(oI.SCXMLObjectItem(nodeSupport.xComp.getTypeCode())) #x component constraint.
  retval.setP5(oI.SCXMLObjectItem(str(nodeSupport.yComp.k))) #y component stiffness.
  retval.setP6(oI.SCXMLObjectItem(nodeSupport.yComp.getTypeCode())) #y component constraint.
  retval.setP7(oI.SCXMLObjectItem(str(nodeSupport.zComp.k))) #z component stiffness.
  retval.setP8(oI.SCXMLObjectItem(nodeSupport.zComp.getTypeCode())) #z component constraint.
  retval.setP9(oI.SCXMLObjectItem(str(nodeSupport.rxComp.k))) #rx component stiffness.
  retval.setP10(oI.SCXMLObjectItem(nodeSupport.rxComp.getTypeCode())) #rx component constraint.
  retval.setP11(oI.SCXMLObjectItem(str(nodeSupport.ryComp.k))) #ry component stiffness.
  retval.setP12(oI.SCXMLObjectItem(nodeSupport.ryComp.getTypeCode())) #ry component constraint.

  retval.setP13(oI.SCXMLObjectItem(str(nodeSupport.rzComp.k))) #rz component stiffness.
  retval.setP14(oI.SCXMLObjectItem(nodeSupport.rzComp.getTypeCode())) #rz component constraint.
  return retval

class NodeSupportContainer(ctr.SCXMLTableContainer):
  def __init__(self,nodeSupportsDict):
    super(NodeSupportContainer,self).__init__(idNodeSupportContainer,tNodeSupportContainer)
    nodeSupports= list()
    for key in nodeSupportsDict:
      ns= nodeSupportsDict[key]
      nodeSupports.append(getNodeSupportObject(ns))
    self.appendTable(tb.SCXMLTableXMLNodes(idNodeSupportContainerTb,tNodeSupportContainerTb, '', None,nodeSupports))
  
