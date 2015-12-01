# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

#Properties for constraint nodes.

import PropertiesContainer as ctr
import Property as prop
import Ref as rf
import EnumItem as eI
import Enum as enum
import PropertiesTable as propTable
import xml.etree.cElementTree as ET

containerId= "{77705284-EEB9-11D4-B450-00104BC3B531}"
containerClsId= containerId
containerProgId= "EP_Material.EP_Material.1"

class BasicMaterialProperties(propTable.PropertiesTable):
  idName= "{4364BC01-AAB7-11D4-B3D9-00104BC3B531}"
  idUniqueId= "{EB2C0F85-7275-4F94-9EA7-E8C3DBFB0FA6}"
  idMaterialType= "{C7421D33-AF0C-11D4-BAE2-006008A4560F}"
  idThermalExpansion= "{0BE3B3D1-86F1-11D4-B3AC-00104BC3B531}"
  idUnitMass= "{FD667020-AB1A-11D4-A0DA-0001029E5330}"
  idYoungModulus= "{C7421D31-AF0C-11D4-BAE2-006008A4560F}"
  idPoissonCoeff= "{C7421D32-AF0C-11D4-BAE2-006008A4560F}"
  idIndependentGModulus= "{97671FB7-B5BA-4919-AB8D-F5C4EEB88E9D}"
  idGModulus= "{EFE9E250-A71F-4CE9-8981-372A1AD5A321}"
  idLogDecrement= "{AAD0FAB0-2ACF-44E9-845A-12E43260524E}"
  idColour= "{5FF9EA16-063B-41AE-83DD-F3208D528BBE}"
  idSpecificHeat= "{FEC68EE3-07B2-4D4F-9E83-D3C468458371}"
  idThermalConductivity= "{573962BE-2376-48E6-9527-098F1A4D736C}"
  def __init__(self,id,name,typo,clsid,progid):
    super(BasicMaterialProperties,self).__init__(id,name,typo,clsid,progid)
    propName= prop.Property("0","Name","string",self.idName)
    propUniqueId= prop.Property("1","UniqueID","string",self.idUniqueId)
    propMatType= prop.Property("2","Material type","enum",self.idMaterialType,"262144")
    propMatType.value= enum.Enum([])
    propThermalExpansion= prop.Property("3","Thermal expansion ","real",self.idThermalExpansion)
    propUnitMass= prop.Property("4","Unit mass","real",self.idUnitMass)
    propYoungModulus= prop.Property("5","Young Modulus","real",self.idYoungModulus)
    propPoissonCoeff= prop.Property("6","Poisson coeff.","real",self.idPoissonCoeff)
    propIndependentGModulus= prop.Property("7","Independent G modulus","real",self.idIndependentGModulus)
    propGModulus= prop.Property("8","G modulus","real",self.idGModulus)
    propLogDecrement= prop.Property("9","Log. decrement (non-uniform damping only)","real",self.idLogDecrement)
    propColour= prop.Property("10","Colour","integer",self.idColour)

    self.properties.append(propName) #0
    self.properties.append(propUniqueId) #1
    self.properties.append(propMatType) #2
    self.properties.append(propThermalExpansion) #3
    self.properties.append(propUnitMass) #4
    self.properties.append(propYoungModulus) #5
    self.properties.append(propPoissonCoeff) #6
    self.properties.append(propIndependentGModulus) #7
    self.properties.append(propGModulus) #8
    self.properties.append(propLogDecrement) #9
    self.properties.append(propColour) #10



class GenericMaterialProperties(BasicMaterialProperties):
  TableId= "1AC47140-DA71-47B3-9644-B8959DE2744D"
  TableClsId= "{EDAF68F9-AA38-11D4-B3D9-00104BC3B531}"
  TableProgId= "EP_Material.EP_Material.1"
  def __init__(self):
    super(GenericMaterialProperties,self).__init__(self.TableId,"XML\default","vertical",self.TableClsId,self.TableProgId)
    propSpecificHeat= prop.Property("11","Specific heat","real",self.idSpecificHeat)
    propThermalConductivity= prop.Property("12","Thermal conductivity","real",self.idThermalConductivity)
    self.properties.append(propSpecificHeat) #11
    self.properties.append(propThermalConductivity) #12

class SteelSIAProperties(BasicMaterialProperties):
  TableId= "E533E4EC-A104-41C2-B970-6264B819D66A"
  TableClsId= "{75D7A485-8E2B-4DD2-85A9-83BE9983061B}"
  TableProgId= "EP_MaterialSIA.EP_MaterialSteelSIA.1"
  idThermalExpansionFireR= "{AC3DB8F9-EF9F-40B8-96A3-CE7815BBF8E7}"
  idUltimateStrength= "{0BE3B3D1-86FA-11D4-B3AC-00104BC3B531}"
  idYieldStrength= "{0BE3B3D2-86FA-11D4-B3AC-00104BC3B531}"
  def __init__(self):
    super(SteelSIAProperties,self).__init__(self.TableId,"XML\default","vertical",self.TableClsId,self.TableProgId)
    propThermalExpansionFireR= prop.Property("11","Thermal expansion (for fire resistance) ","real",self.idThermalExpansionFireR)
    propSpecificHeat= prop.Property("12","Specific heat","real",self.idSpecificHeat)
    propThermalConductivity= prop.Property("13","Thermal conductivity","real",self.idThermalConductivity)
    propUltimateStrength= prop.Property("14","Ultimate strength","real",self.idUltimateStrength)
    propYieldStrength= prop.Property("15","Yield strength","real",self.idYieldStrength)

    self.properties.append(propThermalExpansionFireR) #11
    self.properties.append(propSpecificHeat) #12
    self.properties.append(propThermalConductivity) #13
    self.properties.append(propUltimateStrength) #14
    self.properties.append(propYieldStrength) #15

class ConcreteSIAProperties(BasicMaterialProperties):
  TableId= "FEB99679-36DA-4388-A6EF-396D7021E8A8"
  TableClsId= "{48C81A90-3016-4DF5-90AD-B1DE2917858E}"
  TableProgId= "EP_MaterialSIA.EP_MaterialCrtSIA.1"
  idOrderInCode= "{F782ADE0-2E50-46B7-A5E4-8484A0CD145B}"
  idStoneDiameter= "{0BE3B3D3-86FA-11D4-B3AC-00104BC3B531}"
  idCementClass= "{0BE3B3D4-86FA-11D4-B3AC-00104BC3B531}"
  idFck= "{1762A457-B515-4DD1-8C80-8805C8EC8041}"
  idFctm= "{0631DEEC-3CD3-409C-AB46-48ED353EC1F6}"
  idFcd= "{177072D9-FD3F-4495-9C07-5924590B6B8B}"
  idMVMCS= "{4C74CF90-7EC1-45FA-8A23-B40CED5857FA}"
  def __init__(self):
    super(ConcreteSIAProperties,self).__init__(self.TableId,"XML\default","vertical",self.TableClsId,self.TableProgId)
    propSpecificHeat= prop.Property("11","Specific heat","real",self.idSpecificHeat)
    propThermalConductivity= prop.Property("12","Thermal conductivity","real",self.idThermalConductivity)
    propOrderInCode= prop.Property("13","Order in code","integer",self.idOrderInCode)
    propStoneDiameter= prop.Property("14","Stone diameter (dg)","real",self.idStoneDiameter)
    propCementClass= prop.Property("15","Cement class","enum",self.idCementClass,"262144")
    propCementClass.value= enum.Enum([])
    propFck= prop.Property("16","Characteristic compressive  cylinder strength [28](Fck)","real",self.idFck)
    propFctm= prop.Property("17","Mean tensile strength [28](Fctm)","real",self.idFctm)
    propFcd= prop.Property("18","fcd","real",self.idFcd)
    propMVMCS= prop.Property("19","Measured values of mean compressive strength (influence of ageing)","real",self.idMVMCS)

    self.properties.append(propSpecificHeat) #11
    self.properties.append(propThermalConductivity) #12
    self.properties.append(propOrderInCode) #13
    self.properties.append(propStoneDiameter) #14
    self.properties.append(propCementClass) #15
    self.properties.append(propFck) #16
    self.properties.append(propFctm) #17
    self.properties.append(propFcd) #18
    self.properties.append(propMVMCS) #19

class MaterialProperties(ctr.PropertiesContainer):
  def __init__(self):
    super(MaterialProperties,self).__init__(containerId,containerClsId,containerProgId)
    self.tableGeneric= GenericMaterialProperties()
    self.tableSteelSIA= SteelSIAProperties()
    self.tableConcreteSIA= ConcreteSIAProperties()

  def getXMLElement(self,parent):
    container= ET.SubElement(parent,"def_container")
    self.populateXMLElement(container)
    self.tableGeneric.getXMLElement(container)
    self.tableSteelSIA.getXMLElement(container)
    self.tableConcreteSIA.getXMLElement(container)
    return container

