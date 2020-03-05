# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
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
import node_load_properties as nlp
import point_force_free_properties as pffp
import uuid

containerId= "{3E5FFA16-D1A4-4589-AD5A-4A0FC555E8B8}"
containerClsId= containerId
tbName= "DataAddLoad.EP_SurfaceForceFree.1"
tbProgId= tbName
tbId= str(uuid.uuid4()) #"4DD6C19E-7578-4591-A547-7B0362419697"
tbClsId= containerClsId

idLoadCase= nlp.idLoadCase
idLoadCaseRef= nlp.idLoadCaseRef
idName= pffp.idName
idUniqueId= pffp.idUniqueId
idDirection= pffp.idDirection
idType= pffp.idType
idDistribution= "{77B38320-D8EF-444A-B97D-64EE94ACCD39}"
idQ= "{2B793726-6A90-4FAD-925D-E8F6D8973976}"
idValidity= pffp.idValidity
idSelect= pffp.idSelect
idSystem= pffp.idSystem
idLocation= "{E2B671A4-2A15-41E5-84BD-5A2A5B827A40}"
idTableOfGeometry= "{7F9F0012-4FBC-4731-808E-84A6A78DA584}"
idNode= "{69A30F79-122B-44FD-9D8E-C20A07FED8EB}"
idPointDefinition= "{21458FDE-FCCE-42F4-99BE-4D125E8F6496}"
idCoordXA= pffp.idCoordX
idCoordYA= pffp.idCoordY
idCoordZA= pffp.idCoordZ
idCoordXB= "{5C72E956-E131-43AD-B289-38769EDB7612}"
idCoordYB= "{96F88865-C491-4B49-9705-C404FD6C4F14}"
idCoordZB= "{EAEEDC01-3E4B-41BB-9934-99112F13C437}"
idFirstPointWeight= "{794EB3ED-8749-476D-8AE3-9DB0160347E4}"
idCoordXC= "{8BDB8A2C-A112-4A8A-9CD0-07567EB16281}"
idCoordYC= "{71B0D0C6-E334-4132-8612-B16CB2D23E65}"
idCoordZC= "{C23AFC05-B7FD-4CC9-A97B-CF9038BFB78F}"
idSecondPointWeight= "{6816A8F1-6049-411F-87A0-3D2D0E738D05}"
idDistance= "{75DC4AAF-00BC-47AB-96CA-BD9402457753}"
idVectorX= "{056DBA2C-44AC-418D-B891-FBC587D1E0BA}"
idVectorY= "{5019F985-7FA2-4DAB-AC36-62C75DF20E72}"
idVectorZ= "{6B466680-68F2-484E-BD20-E00A39E81C4F}"
idReferencedEntityA= "{EA11631F-7A6E-466B-A658-FE19AE705499}"
idReferencedEntityB= "{AEA22588-8EF1-4D75-B66D-1B0171E77D72}"
idCoordUX= pffp.idCoordUX
idCoordUY= pffp.idCoordUY
idCoordUZ= pffp.idCoordUZ
idEdge= "{ECDF6620-F0FD-4476-B095-3610E641C35C}"

class SurfacePressureFreeProperties(ctr.SCXMLPropertiesContainer):
  '''Free point force properties in SCIA XML file.''' 
  def __init__(self):
    '''Constructor.'''
    super(SurfacePressureFreeProperties,self).__init__(containerId,containerClsId,tbProgId)
    self.tableProp= propTable.SCXMLPropertiesTable(tbId,"XML\default","vertical",tbClsId,tbProgId)
    propRefLoadCase= prop.SCXMLProperty("0","Load case","ref",idLoadCase)
    propRefLoadCase.value= rf.SCXMLRef(idLoadCaseRef,lcp.tbName)
    propName= prop.SCXMLProperty("1","Name","string",idName)
    propUniqueID= prop.SCXMLProperty('2','UniqueID','string',idUniqueId)
    propDirection= prop.SCXMLProperty("3","Direction","enum",idDirection,"262144")
    propDirection.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","X"),eI.SCXMLEnumItem("1","Y"),eI.SCXMLEnumItem("2","Z")])
    propType= prop.SCXMLProperty("4","Type","enum",idType,"262144")
    propType.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Force")])
    propDistribution= prop.SCXMLProperty('5','Distribution','enum',idDistribution)
    propDistribution.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Uniform"),eI.SCXMLEnumItem("1","Dir X"),eI.SCXMLEnumItem("2","Dir Y"),eI.SCXMLEnumItem("3","3 points")])
    propQ= prop.SCXMLProperty('6','q','param',idQ,"33619968")
    propValidity= prop.SCXMLProperty('7',"Validity","enum",idValidity,"262144")
    propValidity.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Tout"), eI.SCXMLEnumItem("1", "-Z"), eI.SCXMLEnumItem("2", "+Z"), eI.SCXMLEnumItem("3", "Intervalle"), eI.SCXMLEnumItem("4", "Z=0"), eI.SCXMLEnumItem("5", "-Z (yc. 0)"), eI.SCXMLEnumItem("6", "+Z (yc. 0)")])
    propSelect= prop.SCXMLProperty('8',"Select","enum",idSelect)
    propSelect.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Auto"),eI.SCXMLEnumItem("1","Select")])
    propSystem= prop.SCXMLProperty('9',"System","enum",idSystem)
    propSystem.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","GCS"),eI.SCXMLEnumItem("1","Member LCS"),eI.SCXMLEnumItem("2","Load LCS")])
    propLocation= prop.SCXMLProperty('10','Location','enum',idLocation)
    propLocation.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Length"),eI.SCXMLEnumItem("1","Projection")])
    propTableOfGeometry= prop.SCXMLProperty("11","Table of geometry","table",idTableOfGeometry)
    propTableOfGeometry.value= propSubTable.SCXMLPropertiesSubTable('','',"vertical",'','')
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("0","Node","string",idNode))
    propPointDefinition= prop.SCXMLProperty("1","Point definition","enum",idPointDefinition)
    propPointDefinition.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Standard"),eI.SCXMLEnumItem("1","Linear Combination"),eI.SCXMLEnumItem("2","Distance from node")])
    propTableOfGeometry.value.properties.append(propPointDefinition)
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("2","Coord X","param",idCoordXA))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("3","Coord Y","param",idCoordYA))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("4","Coord Z","param",idCoordZA))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("5","Coord X","param",idCoordXB))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("6","Coord Y","param",idCoordYB))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("7","Coord Z","param",idCoordZB))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("8","First point weight","real",idFirstPointWeight))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("9","Coord X","param",idCoordXC))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("10","Coord Y","param",idCoordYC))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("11","Coord Z","param",idCoordZC))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("12","Second point weight","real",idSecondPointWeight))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("13","Distance","real",idDistance))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("14","Vector X","real",idVectorX))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("15","Vector Y","real",idVectorY))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("16","Vector Z","real",idVectorZ))
    propReferencedEntityA= prop.SCXMLProperty("17","Referenced Entity","enum",idReferencedEntityA)
    propReferencedEntityA.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Existing Node"),eI.SCXMLEnumItem("1","New Point")])
    propTableOfGeometry.value.properties.append(propReferencedEntityA)
    propReferencedEntityB= prop.SCXMLProperty("18","Referenced Entity","enum",idReferencedEntityB)
    propReferencedEntityB.value= enum.SCXMLEnum([eI.SCXMLEnumItem("0","Existing Node"),eI.SCXMLEnumItem("1","New Point")])
    propTableOfGeometry.value.properties.append(propReferencedEntityB)
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("19","Coord ux","real",idCoordUX))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("20","Coord uy","real",idCoordUY))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("21","Coord uz","real",idCoordUZ))
    propTableOfGeometry.value.properties.append(prop.SCXMLProperty("22","Edge","string",idEdge))

    self.tableProp.properties.append(propRefLoadCase) #0
    self.tableProp.properties.append(propName) #1
    self.tableProp.properties.append(propUniqueID) #2
    self.tableProp.properties.append(propDirection) #3
    self.tableProp.properties.append(propType) #4
    self.tableProp.properties.append(propDistribution) #5
    self.tableProp.properties.append(propQ) #6
    self.tableProp.properties.append(propValidity) #7
    self.tableProp.properties.append(propSelect) #8
    self.tableProp.properties.append(propSystem) #9
    self.tableProp.properties.append(propLocation) #10
    self.tableProp.properties.append(propTableOfGeometry) #11


  def getXMLElement(self,parent):
    '''Returns the corresponding XML element..
       Parameters:
       parent: owner of this object in the XML structure.'''
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableProp.getXMLElement(container)
    return container

