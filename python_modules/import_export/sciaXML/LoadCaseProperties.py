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
import xml.etree.cElementTree as ET

containerId= "{0908D21F-481F-11D4-AB84-00C06C452330}"
containerClsId= containerId
tbName="DataSetScia.EP_LoadCase.1"
tbProgId= tbName
tbId= "AAEC76F5-9C0D-4340-98D7-330ACB62D905"
tbClsId= containerClsId

idName= "{4364BC01-AAB7-11D4-B3D9-00104BC3B531}"
idUniqueId= "{EB2C0F85-7275-4F94-9EA7-E8C3DBFB0FA6}"
idActionType= "{11D48DC0-E0F4-11D4-A482-00C06C542707}"
idDescription= "{11D48DC8-E0F4-11D4-A482-00C06C542707}"
idLoadGroup= "{AA694EDC-83EA-11D4-B37D-000000000000}"
idLoadGroupReference= "{F9D4AA72-49D5-11D4-A3CF-000000000000}"
idLoadType= "{11D48DC1-E0F4-11D4-A482-00C06C542707}"
idDirection= "{11D48DC3-E0F4-11D4-A482-00C06C542707}"

class LoadCaseProperties(ctr.PropertiesContainer):
  def __init__(self):
    super(LoadCaseProperties,self).__init__(containerId,containerClsId,tbProgId)
    self.tableProp= propTable.PropertiesTable(tbId,'',"vertical",tbClsId,tbProgId)
    propName= prop.Property("0","Name","string",idName)
    propUniqueId= prop.Property("1","UniqueID","string",idUniqueId)
    propActionType= prop.Property("2","Action type","enum",idActionType,"262144")
    propActionType.value= enum.Enum([eI.EnumItem("0","Permanent"),eI.EnumItem("1","Variable")])
    propDescription= prop.Property("3","Description","string",idDescription)
    propLoadGroupReference= prop.Property("4","LoadGroup","ref",idLoadGroup,"131072")
    propLoadGroupReference.value= rf.Ref(idLoadGroupReference,"DataSetScia.EP_LoadGroup.1")
    propLoadType= prop.Property("5","Load type","enum",idLoadType,"262144")
    propLoadType.value= enum.Enum([eI.EnumItem("0","Poids propre"),eI.EnumItem("1","Standard"),eI.EnumItem("2","Effet primaire")])
    propDirection= prop.Property("6","Direction","enum",idDirection,"262144")
    propDirection.value= enum.Enum([eI.EnumItem("0","-Z"),eI.EnumItem("1","+Z"),eI.EnumItem("2","-Y"),eI.EnumItem("3","+Y"),eI.EnumItem("4","-X"),eI.EnumItem("5","+X")])

    self.tableProp.properties.append(propName) #0
    self.tableProp.properties.append(propUniqueId) #1
    self.tableProp.properties.append(propActionType) #2
    self.tableProp.properties.append(propDescription) #3
    self.tableProp.properties.append(propLoadGroupReference) #4
    self.tableProp.properties.append(propLoadType) #5
    self.tableProp.properties.append(propDirection) #6

  def getXMLElement(self,parent):
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

