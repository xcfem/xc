# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Properties for constraint nodes.

from import_export.sciaXML.xml_basics import PropertiesContainer as ctr
from import_export.sciaXML.xml_basics import Property as prop
from import_export.sciaXML.xml_basics import Ref as rf
from import_export.sciaXML.xml_basics import EnumItem as eI
from import_export.sciaXML.xml_basics import Enum as enum
from import_export.sciaXML.xml_basics import PropertiesTable as propTable
from import_export.sciaXML.xml_basics import PropertiesSubTable as propSubTable
import load_case_properties as lcp
import xml.etree.cElementTree as ET

containerId= "{C0FBF7E1-4A71-11D4-AB86-00C06C452330}"
containerClsId= containerId
conainerProgId= "DataSetSciaTom.EP_LoadCombi.1"
tbName= "XML\default"
tbProgId= "DataSetSciaTom.EP_LoadCombi.1"
tbId= "2B68D91B-7B72-4386-824B-4CC9834281A9"
tbClsId= containerClsId

idName= "{4364BC01-AAB7-11D4-B3D9-00104BC3B531}"
idUniqueId= "{EB2C0F85-7275-4F94-9EA7-E8C3DBFB0FA6}"
idDescription= "{B4E52219-83EE-11D4-B37D-000000000000}"
idCombType= "{B4E52211-83EE-11D4-B37D-000000000000}"
idLoadCases= "{1DC504C3-85BE-4323-B1EB-BADB2FA6A82B}"
idLoadCaseName= "{389B00F7-5EE9-428B-BE8A-15727A8E57D1}"
idLoadCaseCoeff= "{138BF88D-1C80-4702-BE41-9E068C4F6131}"
idLoadCaseId= "{138BF88E-1C80-4702-BE41-9E068C4F6131}"
idLoadCaseRef= "{0908D21F-481F-11D4-AB84-00C06C452330}"
idExplode= "{1DC504C4-85BE-4323-B1EB-BADB2FA6A82B}"
idPhase= "{1DC504C5-85BE-4323-B1EB-BADB2FA6A82B}"
idMasterID= "{1DC504C6-85BE-4323-B1EB-BADB2FA6A82B}"

class LoadCombProperties(ctr.PropertiesContainer):
  tableProp= None
  def __init__(self):
    super(LoadCombProperties,self).__init__(containerId,containerClsId,tbProgId)
    self.tableProp= propTable.PropertiesTable(tbId,tbName,"vertical",tbClsId,tbProgId)
    propName= prop.Property("0","Name","string",idName)
    propUniqueId= prop.Property("1","UniqueID","string",idUniqueId)
    propDescription= prop.Property("2","Description","string",idDescription)
    propCombType= prop.Property("3","Type","enum",idCombType)
    propCombType.value= enum.Enum([eI.EnumItem("0","Envelope - ultimate"),eI.EnumItem("1","Envelope - serviceability"),eI.EnumItem("2","Linear - ultimate"),eI.EnumItem("3","Linear - serviceability"),eI.EnumItem("4","France - ultimate"),eI.EnumItem("5","France - serviceability"),eI.EnumItem("6","BAEL - ultimate"),eI.EnumItem("7","BAEL - serviceability")])
    propRefTable= prop.Property("4","Load cases","table",idLoadCases)
    propRefTable.value= propSubTable.PropertiesSubTable('','',"vertical",'','')
    propRefTable.value.properties.append(prop.Property("0","Name","string",idLoadCaseName))
    propRefTable.value.properties.append(prop.Property("1","Coeff.","param",idLoadCaseCoeff))
    propRefLoadCases= prop.Property("2","Load case ID","ref",idLoadCaseId)
    propRefLoadCases.value= rf.Ref(idLoadCaseRef,lcp.tbName)
    propRefTable.value.properties.append(propRefLoadCases)

    propExplode= prop.Property("5","Explode","integer",idExplode)
    propPhase= prop.Property("6","Phase","integer",idPhase)
    propMasterID= prop.Property("7","Master ID","integer",idMasterID)

    self.tableProp.properties.append(propName) #0
    self.tableProp.properties.append(propUniqueId) #1
    self.tableProp.properties.append(propDescription) #2
    self.tableProp.properties.append(propCombType) #3
    self.tableProp.properties.append(propRefTable) #4
    self.tableProp.properties.append(propExplode) #5
    self.tableProp.properties.append(propPhase) #6
    self.tableProp.properties.append(propMasterID) #7

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the load combination properties.
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

