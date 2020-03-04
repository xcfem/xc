# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Properties for constraint nodes.

from xml_basics import PropertiesContainer as ctr
from xml_basics import Property as prop
from xml_basics import Ref as rf
from xml_basics import EnumItem as eI
from xml_basics import Enum as enum
from xml_basics import PropertiesTable as propTable
import node_container as nc
import xml.etree.cElementTree as ET

containerId= "{1cbca4de-355b-40f7-a91d-8efd26a6404d}"
containerClsId= containerId
tbName="DataAddSupport.EP_PointSupportPoint.1"
tbProgId= tbName
tbId="6b8df80b-aad0-4570-84dd-4af1d7c683d2"
tbClsId= containerClsId

idName= "{4364bc01-aab7-11d4-b3d9-00104bc3b531}"
idNodeRef= "{5be4e6fe-0434-4af7-8a3f-036b057ccb1e}"
idType= "{7c7589ca-c90c-11d4-a46a-00c06c542707}"
idStiffnessX= "{49f80190-a4e5-11d4-a43a-000000000000}"
idStiffnessY= "{49f80193-a4e5-11d4-a43a-000000000000}"
idStiffnessZ= "{49f80196-a4e5-11d4-a43a-000000000000}"
idStiffnessRX= "{49f80199-a4e5-11d4-a43a-000000000000}"
idStiffnessRY= "{49f8019c-a4e5-11d4-a43a-000000000000}"
idStiffnessRZ= "{49f8019f-a4e5-11d4-a43a-000000000000}"

idTypeX= "{49f8018f-a4e5-11d4-a43a-000000000000}"
idTypeY= "{49f80192-a4e5-11d4-a43a-000000000000}"
idTypeZ= "{49f80195-a4e5-11d4-a43a-000000000000}"
idTypeRX= "{49f80198-a4e5-11d4-a43a-000000000000}"
idTypeRY= "{49f8019b-a4e5-11d4-a43a-000000000000}"
idTypeRZ= "{49f8019e-a4e5-11d4-a43a-000000000000}"

class NodeSupportProperties(ctr.PropertiesContainer):
  tableProp= None
  def __init__(self):
    super(NodeSupportProperties,self).__init__(containerId,containerClsId,'')
    self.tableProp= propTable.PropertiesTable(tbId,tbName,"vertical",tbClsId,tbProgId)
    propName= prop.Property("0","_NAME_","string",idName,"0")
    propRefNode= prop.Property("1","_NODE_","ref",idNodeRef,"0")
    propRefNode.value= rf.Ref("{39a7f468-a0d4-4dff-8e5c-5843e1807d13}",nc.progIdNodes)
    propType= prop.Property("2","_TYPE_","enum",idType,"262144")
    propType.value= enum.Enum([eI.EnumItem("0","Standard"),eI.EnumItem("1","Column")])

    propKX= prop.Property("3","_STIFFNESSX_","param",idStiffnessX,"33619968")
    propTypeX= prop.Property("4","_TYPEX_","enum",idTypeX,"262144")
    propTypeX.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])
    propKY= prop.Property("5","_STIFFNESSY_","param",idStiffnessY,"33619968")
    propTypeY= prop.Property("6","_TYPEY_","enum",idTypeY,"262144")
    propTypeY.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])
    propKZ= prop.Property("7","_STIFFNESSZ_","param",idStiffnessZ,"33619968")
    propTypeZ= prop.Property("8","_TYPEZ_","enum",idTypeZ,"262144")
    propTypeZ.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])
    propKRX= prop.Property("9","_STIFFNESSRX_","real",idStiffnessRX,"0")
    propTypeRX= prop.Property("10","_TYPERX_","enum",idTypeRX,"262144")
    propTypeRX.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])
    propKRY= prop.Property("11","_STIFFNESSRY_","real",idStiffnessRY,"0")
    propTypeRY= prop.Property("12","_TYPERY_","enum",idTypeRY,"262144")
    propTypeRY.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])
    propKRZ= prop.Property("13","_STIFFNESSRZ_","real",idStiffnessRZ,"0")
    propTypeRZ= prop.Property("14","_TYPERZ_","enum",idTypeRZ,"262144")
    propTypeRZ.value= enum.Enum([eI.EnumItem("0","Free"),eI.EnumItem("1","Rigid"),eI.EnumItem("2","Flexible")])

    self.tableProp.properties.append(propName) #0
    self.tableProp.properties.append(propRefNode) #1
    self.tableProp.properties.append(propType) #2
    self.tableProp.properties.append(propKX) #3
    self.tableProp.properties.append(propTypeX) #4
    self.tableProp.properties.append(propKY) #5
    self.tableProp.properties.append(propTypeY) #6
    self.tableProp.properties.append(propKZ) #7
    self.tableProp.properties.append(propTypeZ) #8
    self.tableProp.properties.append(propKRX) #9
    self.tableProp.properties.append(propTypeRX) #10
    self.tableProp.properties.append(propKRY) #11
    self.tableProp.properties.append(propTypeRY) #12
    self.tableProp.properties.append(propKRZ) #13
    self.tableProp.properties.append(propTypeRZ) #14

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the node support.
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

