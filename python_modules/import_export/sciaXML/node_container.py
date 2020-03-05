# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from xml_basics import scxml_table_container as ctr
from xml_basics import scxml_table_xmlnodes as tb
from xml_basics import scxml_header as hdr
from xml_basics import scxml_header_item as hi
from xml_basics import scxml_object as obj
from xml_basics import scxml_object_item as oI

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
  retval= obj.SCXMLObject()
  id= str(nr.id)
  retval.setId(id)
  name= nodePrefix+id
  retval.setNm(name)
  retval.setP0(oI.SCXMLObjectItem(name, None, None, None, None, None))
  retval.setP1(oI.SCXMLObjectItem(str(nr.getX()), None, None, None, None, None))
  retval.setP2(oI.SCXMLObjectItem(str(nr.getY()), None, None, None, None, None))
  retval.setP3(oI.SCXMLObjectItem(str(nr.getZ()), None, None, None, None, None))
  return retval

class NodeContainer(ctr.SCXMLTableContainer):
  def __init__(self,nodesDict):
    super(NodeContainer,self).__init__(idNodeContainer,progIdNodes)
    header= self.getDefaultStructNodeHeader()
    nodes= list()
    for key in nodesDict:
      nr= nodesDict[key]
      nodes.append(getNodeObject(nr))
    self.appendTable(tb.SCXMLTableXMLNodes(STRUCT_NODE_TABLE_ID,STRUCT_NODE_TABLE, STRUCT_NODE_TABLE_NAME, header,nodes))
  def getDefaultStructNodeHeader(self):
    retorno= hdr.SCXMLHeader()
    retorno.setH0(hi.SCXMLHeaderItem(STRUCT_NODE_HEADER_H0))
    retorno.setH1(hi.SCXMLHeaderItem(STRUCT_NODE_HEADER_H1))
    retorno.setH2(hi.SCXMLHeaderItem(STRUCT_NODE_HEADER_H2))
    retorno.setH3(hi.SCXMLHeaderItem(STRUCT_NODE_HEADER_H3))
    return retorno
  
