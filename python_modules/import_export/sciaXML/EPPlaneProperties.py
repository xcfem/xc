# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from xml_basics import PropertiesContainer as ctr
from xml_basics import Property as prop
from xml_basics import Ref as rf
from xml_basics import EnumItem as eI
from xml_basics import Enum as enum
from xml_basics import PropertiesTable as propTable
from xml_basics import PropertiesSubTable as propSubTable
import NodeContainer as nc
import xml.etree.cElementTree as ET

containerId= "{8708ed31-8e66-11d4-ad94-f6f5de2be344}"
containerClsId= containerId
tbName="EP_DSG_Elements.EP_Plane.1"
tbProgId= tbName
tbId="915fdfe9-b6a9-446b-9f33-857a00d31679"
tbClsId= containerClsId

idRef= "{d30c2dd3-95de-439b-a30d-0b1c27f9d9ac}"
idType= "{9e0940df-9062-456a-af36-7f16ad031295}"
idLayerRef= "{35e4a750-e358-11d4-ae4c-000000000000}"
idFEMModel= "{323f8707-f52c-409a-bfaf-7a6eb0715295}"
idThicknessType= "{82644ab6-d9dd-4636-b771-c6ed05fa9144}"
idMemberSystemPlaneAtProp= "{54de517c-f821-4fda-9d7c-4e552ca89126}"
idDirection= "{82644ab6-d9dd-4636-b771-c6ed05fa9145}"
idPoint1= "{c9650e93-b8ed-4e59-b201-6da692d1ec4e}"
idPoint2= "{de5cbf71-ea0b-41ef-a6e9-da364edb741e}"
idGeomTable= "{748a0933-f05e-4b33-b4d6-4854fcd612c8}"
idInternalNodes= "{ae6aa708-ce5c-4465-b54b-214413adac4d}"

class EPPlaneProperties(ctr.PropertiesContainer):
  tableProp= None;
  def __init__(self):
    super(EPPlaneProperties,self).__init__(containerId,containerClsId,'')
    self.tableProp= propTable.PropertiesTable(tbId,tbName,"vertical",tbClsId,tbProgId)
    propMat= prop.Property("0","_MATERIAL_","ref",idRef,"131072")
    propMat.value= rf.Ref("{77705284-eeb9-11d4-b450-00104bc3b531}","EP_Material.EP_Material.1")
    propType= prop.Property("1","_TYPE_","enum",idType,"0")
    propType.value= enum.Enum([eI.EnumItem("0","plate"),eI.EnumItem("1","wall"),eI.EnumItem("2","shell")])
    propLayerRef= prop.Property("2","_LAYERREF_","ref",idLayerRef,"131072")
    propLayerRef.value= rf.Ref("{d3885ec4-bae5-11d4-b3fa-00104bc3b531}","EP_DSG_Elements.EP_DataLayer.1")
    propFEMModel= prop.Property("3","_FEM_MODEL_","enum",idFEMModel,"0")
    propFEMModel.value= enum.Enum([eI.EnumItem("0", "Isotropic"), eI.EnumItem("1", "Orthotropic"), eI.EnumItem("2", "Membrane"), eI.EnumItem("3", "Load_panel"), eI.EnumItem("4", "Isotropic_with_beam"), eI.EnumItem("5", "Isotropic_from_beam"), eI.EnumItem("6", "Orthotropic_from_beam"), eI.EnumItem("7", "Membrane_with_beam"), eI.EnumItem("8", "Load_panel_with_beam"), eI.EnumItem("9", "Orthotropic_with_beam")])
    propThicknessType= prop.Property("4","_THICKNESS_TYPE_","enum",idThicknessType,"0")
    propThicknessType.value= enum.Enum([eI.EnumItem("0","constant"),eI.EnumItem("1","variable")])
    propMemberSystemPlaneAtProp= prop.Property("5","_MEMBER_SYSTEM_PLANE_AT_PROP_","enum",idMemberSystemPlaneAtProp,"0")
    propMemberSystemPlaneAtProp.value= enum.Enum([eI.EnumItem("1","centre"),eI.EnumItem("2","top"),eI.EnumItem("4","bottom")])
    propDirection= prop.Property("8","_DIRECTION_","enum",idDirection,"0")
    propDirection.value= enum.Enum([eI.EnumItem("0","X"),eI.EnumItem("1","Y"),eI.EnumItem("2","Z")])
    propPoint1= prop.Property("10","_POINT1_","ref",idPoint1,"0")
    propPoint1.value= rf.Ref("{39a7f468-a0d4-4dff-8e5c-5843e1807d13}",nc.progIdNodes)
    propPoint2= prop.Property("11","_POINT2_","ref",idPoint2,"0")
    propPoint2.value= rf.Ref("{39a7f468-a0d4-4dff-8e5c-5843e1807d13}",nc.progIdNodes)
    propGeomTable= prop.Property("12","_TABLE_OF_GEOMETRY_","table",idGeomTable,"0")
    propGeomTable.value= propSubTable.PropertiesSubTable('','',"vertical",'','')
    propGeomTable.value.properties.append(prop.Property("0","_CLOSED_CURVE_","bool","{e0dd8a0c-8e62-49bf-b2ff-cf737a0b65e5}","0"))
    subPropNode= prop.Property("1","_NODE_","ref","{f140e9f9-e8c9-48f1-a305-7f1eb39eeb65}","0")
    subPropNode.value= rf.Ref("{39a7f468-a0d4-4dff-8e5c-5843e1807d13}",nc.progIdNodes)
    subPropEdge= prop.Property("2","_EDGE_","enum","{406832d0-2125-4389-94e2-fd17d97bd027}","0")
    subPropEdge.value= enum.Enum([eI.EnumItem("0", "Line"), eI.EnumItem("1", "Circle_arc"), eI.EnumItem("2", "Circle_by_centre_and_vertex"), eI.EnumItem("4", "Circle_by_3_pts"), eI.EnumItem("5", "Parabolic_arc"), eI.EnumItem("6", "Bezier"), eI.EnumItem("7", "Spline"), eI.EnumItem("9", "Linestrip")])
    propGeomTable.value.properties.append(subPropNode)
    propGeomTable.value.properties.append(subPropEdge)
    propGeomTable.value.properties.append(prop.Property("3","_WEIGHT_","real","{23292b43-824e-4cc1-9376-83da8bac58db}","0"))
    propInternalNodes= prop.Property("13","_INTERNAL_NODES_","table",idInternalNodes,"0")
    propInternalNodes.value= propSubTable.PropertiesSubTable('','',"vertical",'','')
    propInternalNodes.value.properties.append(prop.Property("0","_INDEX_","integer","{24262aad-7b4d-42ed-b3f7-7d237b2a5484}","0"))
    subPropNode= prop.Property("1","_NODE_","ref","{66c5a1c2-2989-4d02-847b-63e69b346911}","0")
    subPropNode.value= rf.Ref("{39a7f468-a0d4-4dff-8e5c-5843e1807d13}",nc.progIdNodes)
    propInternalNodes.value.properties.append(subPropNode)

    self.tableProp.properties.append(propMat) #0
    self.tableProp.properties.append(propType) #1
    self.tableProp.properties.append(propLayerRef) #2
    self.tableProp.properties.append(propFEMModel) #3
    self.tableProp.properties.append(propThicknessType) #4
    self.tableProp.properties.append(propMemberSystemPlaneAtProp) #5
    self.tableProp.properties.append(prop.Property("6","_ECCENTRICITY_Z_","param","{92faa4d4-fe27-49ea-9d21-91a7ec47e12e}","33619968"))
    self.tableProp.properties.append(prop.Property("7","_THICKNESS_","param","{82644ab6-d9dd-4636-b771-c6ed05fa9171}","33619968"))
    self.tableProp.properties.append(propDirection) #8
    self.tableProp.properties.append(prop.Property("9","_THICKNESS2_","param","{82644ab6-d9dd-4636-b771-c6ed05fa9172}","33619968"))
    self.tableProp.properties.append(propPoint1) #10
    self.tableProp.properties.append(propPoint2) #11
    self.tableProp.properties.append(propGeomTable) #12
    self.tableProp.properties.append(propInternalNodes) #13

  def getXMLElement(self,parent):
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

