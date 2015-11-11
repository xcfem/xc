# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI

idNodeContainer= '{39A7F468-A0D4-4DFF-8E5C-5843E1807D13}'
progIdNodes= 'EP_DSG_Elements.EP_StructNode.1'
STRUCT_NODE_HEADER_H0 = "Name"
STRUCT_NODE_HEADER_H1 = "XCoord"
STRUCT_NODE_HEADER_H2 = "YCoord"
STRUCT_NODE_HEADER_H3 = "ZCoord"
STRUCT_NODE_TABLE= progIdNodes
#STRUCT_NODE_TABLE_ID = "C4D79AFB-6512-4D56-BD04-E1BEA1C3B085" java sources
STRUCT_NODE_TABLE_ID = "F67B790F-88D4-41ED-A3B9-3D9F9744B5AF"
STRUCT_NODE_TABLE_NAME = "Node"
nodePrefix = "N"

def getNodeObject(nr):
  retval= obj.Object()
  id= str(nr.id)
  retval.setId(id)
  name= nodePrefix+id
  retval.setNm(name)
  retval.setP0(oI.ObjectItem(name, None, None, None, None, None))
  retval.setP1(oI.ObjectItem(str(nr.getX()), None, None, None, None, None))
  retval.setP2(oI.ObjectItem(str(nr.getY()), None, None, None, None, None))
  retval.setP3(oI.ObjectItem(str(nr.getZ()), None, None, None, None, None))
  return retval

class NodeContainer(ctr.Container):
  def __init__(self,nodesDict):
    super(NodeContainer,self).__init__(idNodeContainer,progIdNodes)
    header= self.getDefaultStructNodeHeader()
    nodes= list()
    for key in nodesDict:
      nr= nodesDict[key]
      nodes.append(getNodeObject(nr))
    self.table= tb.TableNode(STRUCT_NODE_TABLE_ID,STRUCT_NODE_TABLE, STRUCT_NODE_TABLE_NAME, header,nodes)
  def getDefaultStructNodeHeader(self):
    retorno= hdr.Header()
    retorno.setH0(hi.HeaderItem(STRUCT_NODE_HEADER_H0))
    retorno.setH1(hi.HeaderItem(STRUCT_NODE_HEADER_H1))
    retorno.setH2(hi.HeaderItem(STRUCT_NODE_HEADER_H2))
    retorno.setH3(hi.HeaderItem(STRUCT_NODE_HEADER_H3))
    return retorno
  
