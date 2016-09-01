# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016 LCPT"
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
import xml.etree.cElementTree as ET
import LoadCaseProperties as lcp
import NodeLoadProperties as nlp
import PointForceFreeProperties as pffp

containerId= "{3E5FFA16-D1A4-4589-AD5A-4A0FC555E8B8}"
containerClsId= containerId
tbName= "DataAddLoad.EP_SurfaceForceFree.1"
tbProgId= tbName
tbId= "4DD6C19E-7578-4591-A547-7B0362419697"
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

class SurfacePressureFreeProperties(ctr.PropertiesContainer):
  '''Free point force properties in SCIA XML file.''' 
  def __init__(self):
    '''Constructor.'''
    super(SurfacePressureFreeProperties,self).__init__(containerId,containerClsId,tbProgId)
    self.tableProp= propTable.PropertiesTable(tbId,"XML\default","vertical",tbClsId,tbProgId)
    propRefLoadCase= prop.Property("0","Load case","ref",idLoadCase)
    propRefLoadCase.value= rf.Ref(idLoadCaseRef,lcp.tbName)
    propName= prop.Property("1","Name","string",idName)
    propUniqueID= prop.Property('2','UniqueID','string',idUniqueId)
    propDirection= prop.Property("3","Direction","enum",idDirection,"262144")
    propDirection.value= enum.Enum([eI.EnumItem("0","X"),eI.EnumItem("1","Y"),eI.EnumItem("2","Z")])
    propType= prop.Property("4","Type","enum",idType,"262144")
    propType.value= enum.Enum([eI.EnumItem("0","Force")])
    propDistribution= prop.Property('5','Distribution','enum',idDistribution)
    propDistribution.value= enum.Enum([eI.EnumItem("0","Uniform"),eI.EnumItem("1","Dir X"),eI.EnumItem("2","Dir Y"),eI.EnumItem("3","3 points")])
    propQ= prop.Property('6','q','param',idQ,"33619968")
    propValidity= prop.Property('7',"Validity","enum",idValidity,"262144")
    propValidity.value= enum.Enum([eI.EnumItem("0","Tout"), eI.EnumItem("1", "-Z"), eI.EnumItem("2", "+Z"), eI.EnumItem("3", "Intervalle"), eI.EnumItem("4", "Z=0"), eI.EnumItem("5", "-Z (yc. 0)"), eI.EnumItem("6", "+Z (yc. 0)")])
    propSelect= prop.Property('8',"Select","enum",idSelect)
    propSelect.value= enum.Enum([eI.EnumItem("0","Auto"),eI.EnumItem("1","Select")])
    propSystem= prop.Property('9',"System","enum",idSystem)
    propSystem.value= enum.Enum([eI.EnumItem("0","GCS"),eI.EnumItem("1","Member LCS"),eI.EnumItem("2","Load LCS")])
    propLocation= prop.Property('10','Location','enum',idLocation)
    propLocation.value= enum.Enum([eI.EnumItem("0","Length"),eI.EnumItem("1","Projection")])
    propTableOfGeometry= prop.Property("11","Table of geometry","table",idTableOfGeometry)
    propTableOfGeometry.value= propSubTable.PropertiesSubTable('','',"vertical",'','')
    propTableOfGeometry.value.properties.append(prop.Property("0","Node","string",idNode))
    propPointDefinition= prop.Property("1","Point definition","enum",idPointDefinition)
    propPointDefinition.value= enum.Enum([eI.EnumItem("0","Standard"),eI.EnumItem("1","Linear Combination"),eI.EnumItem("2","Distance from node")])
    propTableOfGeometry.value.properties.append(propPointDefinition)
    propTableOfGeometry.value.properties.append(prop.Property("2","Coord X","param",idCoordXA))
    propTableOfGeometry.value.properties.append(prop.Property("3","Coord Y","param",idCoordYA))
    propTableOfGeometry.value.properties.append(prop.Property("4","Coord Z","param",idCoordZA))
    propTableOfGeometry.value.properties.append(prop.Property("5","Coord X","param",idCoordXB))
    propTableOfGeometry.value.properties.append(prop.Property("6","Coord Y","param",idCoordYB))
    propTableOfGeometry.value.properties.append(prop.Property("7","Coord Z","param",idCoordZB))
    propTableOfGeometry.value.properties.append(prop.Property("8","First point weight","real",idFirstPointWeight))
    propTableOfGeometry.value.properties.append(prop.Property("9","Coord X","param",idCoordXC))
    propTableOfGeometry.value.properties.append(prop.Property("10","Coord Y","param",idCoordYC))
    propTableOfGeometry.value.properties.append(prop.Property("11","Coord Z","param",idCoordZC))
    propTableOfGeometry.value.properties.append(prop.Property("12","Second point weight","real",idSecondPointWeight))
    propTableOfGeometry.value.properties.append(prop.Property("13","Distance","real",idDistance))
    propTableOfGeometry.value.properties.append(prop.Property("14","Vector X","real",idVectorX))
    propTableOfGeometry.value.properties.append(prop.Property("15","Vector Y","real",idVectorY))
    propTableOfGeometry.value.properties.append(prop.Property("16","Vector Z","real",idVectorZ))
    propReferencedEntityA= prop.Property("17","Referenced Entity","enum",idReferencedEntityA)
    propReferencedEntityA.value= enum.Enum([eI.EnumItem("0","Existing Node"),eI.EnumItem("1","New Point")])
    propTableOfGeometry.value.properties.append(propReferencedEntityA)
    propReferencedEntityB= prop.Property("18","Referenced Entity","enum",idReferencedEntityB)
    propReferencedEntityB.value= enum.Enum([eI.EnumItem("0","Existing Node"),eI.EnumItem("1","New Point")])
    propTableOfGeometry.value.properties.append(propReferencedEntityB)
    propTableOfGeometry.value.properties.append(prop.Property("19","Coord ux","real",idCoordUX))
    propTableOfGeometry.value.properties.append(prop.Property("20","Coord uy","real",idCoordUY))
    propTableOfGeometry.value.properties.append(prop.Property("21","Coord uz","real",idCoordUZ))
    propTableOfGeometry.value.properties.append(prop.Property("22","Edge","string",idEdge))

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

