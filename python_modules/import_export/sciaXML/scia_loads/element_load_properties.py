# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

#Properties for constraint nodes.

from import_export.sciaXML.xml_basics import scxml_properties_container as ctr
from import_export.sciaXML.xml_basics import scxml_property as prop
from import_export.sciaXML.xml_basics import scxml_ref as rf
from import_export.sciaXML.xml_basics import scxml_enum_item as eI
from import_export.sciaXML.xml_basics import scxml_enum as enum
from import_export.sciaXML.xml_basics import scxml_properties_table as propTable
from import_export.sciaXML.xml_basics import scxml_properties_sub_table as propSubTable
import xml.etree.cElementTree as ET
import load_case_properties as lcp

containerId= "{BC16B3CA-F464-11D4-94D3-000000000000}"
containerClsId= containerId
tbName= "DataAddLoad.EP_SurfaceForceSurface.1"
tbProgId= tbName
tbId="FC7CC6EF-1D04-492E-B6B2-C70DD56DE898"
tbClsId= containerClsId

idRefTable= "{8F96D00B-2125-411D-9218-EF0038D0056F}"
idMemberType= "{3334884C-E566-404D-A135-4093CC88257D}"
idMemberTypeName= "{FF14758E-BC03-4650-88B5-5EA8F1016952}"
idMemberId= "{DD496603-20BD-4CF1-8C72-351306D89D08}"
idMemberName= "{66D67076-ED1C-44EC-BA07-93E993F493CB}"
idLoadCase= "{AACF96D6-D986-421C-9FF2-D0B696A6B1F6}"
idLoadCaseRef= "{0908D21F-481F-11D4-AB84-00C06C452330}"
idName= "{4364BC01-AAB7-11D4-B3D9-00104BC3B531}"
idUniqueId= "{EB2C0F85-7275-4F94-9EA7-E8C3DBFB0FA6}"
idDirection= "{49F8017D-A4E5-11D4-A43A-000000000000}"
idType= "{49F8017C-A4E5-11D4-A43A-000000000000}"
idValue= "{49F80183-A4E5-11D4-A43A-000000000000}"
idSystem= "{ACC6FEC0-B7FC-11D4-ADD9-000000000000}"
idLocation= "{F68C16F1-8E6D-11D4-A419-000000000000}"

class ElementLoadProperties(ctr.SCXMLPropertiesContainer):
  tableProp= None
  def __init__(self):
    super(ElementLoadProperties,self).__init__(containerId,containerClsId,tbProgId)
    self.tableProp= propTable.SCXMLPropertiesTable(tbId,"XML\default","vertical",tbClsId,tbProgId)
    propRefTable= prop.SCXMLProperty("0","Reference Table","table",idRefTable)
    propRefTable.value= propSubTable.SCXMLPropertiesSubTable('','',"vertical",'','')
    propRefTable.value.properties.append(prop.SCXMLProperty("0","Member Type","string",idMemberType))
    propRefTable.value.properties.append(prop.SCXMLProperty("1","Member Type Name","string",idMemberTypeName))
    propRefTable.value.properties.append(prop.SCXMLProperty("2","Member Id","integer",idMemberId))
    propRefTable.value.properties.append(prop.SCXMLProperty("3","Member Name","string",idMemberName))
    propRefLoadCase= prop.SCXMLProperty("1","Load case","ref",idLoadCase)
    propRefLoadCase.value= rf.SCXMLRef(idLoadCaseRef,lcp.tbName)
    propName= prop.SCXMLProperty("2","Name","string",idName)
    propUniqueId= prop.SCXMLProperty("3","UniqueID","string",idUniqueId)
    propDirection= prop.SCXMLProperty("4","Direction","enum",idDirection,"262144")
    propDirection.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","X"),eI.SCXMLEnumItem("1","Y"),eI.SCXMLEnumItem("2","Z")])
    propType= prop.SCXMLProperty("5","Type","enum",idType,"262144")
    propType.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Force"),eI.SCXMLEnumItem("3","Poids propre")])
    propValue= prop.SCXMLProperty("6","Value","param",idValue,"33619968")
    propSystem= prop.SCXMLProperty("7","System","enum",idSystem)
    propSystem.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","GCS"),eI.SCXMLEnumItem("1","LCS")])
    propLocation= prop.SCXMLProperty("8","Location","enum",idLocation)
    propLocation.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Length"),eI.SCXMLEnumItem("1","Projection")])

    self.tableProp.properties.append(propRefTable) #0
    self.tableProp.properties.append(propRefLoadCase) #1
    self.tableProp.properties.append(propName) #2
    self.tableProp.properties.append(propUniqueId) #3
    self.tableProp.properties.append(propDirection) #4
    self.tableProp.properties.append(propType) #5
    self.tableProp.properties.append(propValue) #6
    self.tableProp.properties.append(propSystem) #7
    self.tableProp.properties.append(propLocation) #8

  def getXMLElement(self,parent):
    '''Returns the corresponding XML element for the property.
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

