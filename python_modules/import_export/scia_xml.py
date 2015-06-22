# -*- coding: utf-8 -*-

import xml.etree.cElementTree as ET
import scia_xml_base

nodePrefix = "N"
beamPrefix = "B"
supportPrefix = "Sn"

class sciaXML(scia_xml_base.sciaXMLBase):
  title= "Scia XML automation"
  project= None

  idMaterialContainer="{77705284-EEB9-11D4-B450-00104BC3B531}"
  idMaterialTable="281EAF18-2434-4B29-A24E-062B96AC1C16"
  idMaterialCls= "{A29D6A7B-DB63-4CC0-90F6-69270E56846D}"
  progIdMaterials= "EP_MaterialCSN.EP_MaterialCrtCSN6206.1"
  idNodeContainer= '{39A7F468-A0D4-4DFF-8E5C-5843E1807D13}'
  idNodeTable= 'F67B790F-88D4-41ED-A3B9-3D9F9744B5AF' #sin llavecitas
  progIdNodes= 'EP_DSG_Elements.EP_StructNode.1'
  begNodeName= 'Beg. node'
  endNodeName= 'End node'
  idName= '{4364BC01-AAB7-11D4-B3D9-00104BC3B531}'
  idXCoord= '{C1DD759A-4291-481B-B819-92E3AA9E04B7}'
  idYCoord= '{A84A15AC-FEC2-4C27-84B4-CFA676615934}'
  idZCoord= '{1017EA1E-F76B-4D4B-8A4C-7E58FDED1B02}'
  idBeamContainer= '{ECB5D684-7357-11D4-9F6C-00104BC3B443}'


  def defEnumBeam(self,parent):
    enumBeam= ET.SubElement(parent,"def_enum")
    enumBeam.set("size","2")
    it_enum0= ET.SubElement(enumBeam,"it_enum")
    it_enum0.set("v","0")
    it_enum0.set("t","Standard")
    it_enum1= ET.SubElement(enumBeam,"it_enum")
    it_enum1.set("v","1")
    it_enum1.set("t","Pilar")
    return enumBeam

  def defEnumFRF(self,parent):
    enumFRF= ET.SubElement(parent,"def_enum")
    enumFRF.set("size","3")
    it_enum0= ET.SubElement(enumFRF,"it_enum")
    it_enum0.set("v","0")
    it_enum0.set("t","Free")
    it_enum1= ET.SubElement(enumFRF,"it_enum")
    it_enum1.set("v","1")
    it_enum1.set("t","Rigid")
    it_enum2= ET.SubElement(enumFRF,"it_enum")
    it_enum2.set("v","2")
    it_enum2.set("t","Flexible")
    return enumFRF

  def defMaterialContainer(self,prj):
    materialContainer= self.containerDef(prj,self.idMaterialContainer,self.idMaterialContainer,"EP_Material.EP_Material.1")
    materialContainer.set("library","yes")
    materialContainer.set("readonly","no")
    materialContainer.set("hetero","yes")
    tbMaterials= self.tableDef(materialContainer,self.idMaterialTable,"XML\default","vertical",self.idMaterialCls,self.progIdMaterials,"37")
    prop0= self.defProperty(tbMaterials,"0","Nom","string","{4364BC01-AAB7-11D4-B3D9-00104BC3B531}")
    prop1= self.defProperty(tbMaterials,"1","UniqueID","string","{EB2C0F85-7275-4F94-9EA7-E8C3DBFB0FA6}")        
    prop2= self.defProperty(tbMaterials,"2","Type de materiau","string","{C7421D33-AF0C-11D4-BAE2-006008A4560F}")        
    prop3= self.defProperty(tbMaterials,"3","Dilatation thermique ","real","{0BE3B3D1-86F1-11D4-B3AC-00104BC3B531}")        
    prop4= self.defProperty(tbMaterials,"4","Masse volumique","real","{FD667020-AB1A-11D4-A0DA-0001029E5330}")        
    prop5= self.defProperty(tbMaterials,"5","Module E","real","{C7421D31-AF0C-11D4-BAE2-006008A4560F}")        
    prop6= self.defProperty(tbMaterials,"6","Coeff. de Poisson","real","{C7421D32-AF0C-11D4-BAE2-006008A4560F}")        
    prop7= self.defProperty(tbMaterials,"7","Module G independant","bool","{97671FB7-B5BA-4919-AB8D-F5C4EEB88E9D}")        
    prop8= self.defProperty(tbMaterials,"8","Module G","real","{EFE9E250-A71F-4CE9-8981-372A1AD5A321}")        
    prop9= self.defProperty(tbMaterials,"9","Log. decrement (non-uniform damping only)","real","{AAD0FAB0-2ACF-44E9-845A-12E43260524E}")        
    prop10= self.defProperty(tbMaterials,"10","Couleur","integer","{5FF9EA16-063B-41AE-83DD-F3208D528BBE}")        
    prop11= self.defProperty(tbMaterials,"11","Chaleur specifique","real","{FEC68EE3-07B2-4D4F-9E83-D3C468458371}")        
    prop12= self.defProperty(tbMaterials,"12","Conductivite thermique","real","{573962BE-2376-48E6-9527-098F1A4D736C}")        
    prop13= self.defProperty(tbMaterials,"13","Ordre dans la norme","integer","{F782ADE0-2E50-46B7-A5E4-8484A0CD145B}")        
    prop14= self.defProperty(tbMaterials,"14","Compression centree","real","{F2736F6C-639E-49CE-9511-74DEF7413348}")        
    prop15= self.defProperty(tbMaterials,"15","Flexion composee, compression excentree","real","{A2C71CF6-3B92-4FB1-A0BE-ADC582DB627A}")        
    prop16= self.defProperty(tbMaterials,"16","Flexion composee, compression excentree sur section rectangulaire.[&gt;20cm]","real","{8D945BBC-3FCB-47DE-8EAE-92CA90F30275}")        
    prop17= self.defProperty(tbMaterials,"17","Traction due a une compression excentree","real","{0EE0B748-1A07-44EC-A8DF-2F92BDAD925B}")        
    prop18= self.defProperty(tbMaterials,"18","Traction principale","real","{276E2849-63B5-4D26-9504-21FF99FC8607}")        
    prop19= self.defProperty(tbMaterials,"19","Cohesion [10216,11373,11375]","real","{8C9F651E-FE8B-4F06-80AD-A39DCB5F4E91}")        
    prop20= self.defProperty(tbMaterials,"20","Cohesion [10425,10505,10555]","real","{7E636A45-B392-4940-A38D-4F8339EAC686}")
    return materialContainer

  def defNodeContainer(self,prj):
    nodeContainer= self.containerDef(prj,self.idNodeContainer,self.idNodeContainer,self.progIdNodes)
    tbNodes= self.tableDef(nodeContainer,self.idNodeTable,"XML\default","vertical",self.idNodeContainer,self.progIdNodes,"4")

    name= self.defProperty(tbNodes,"0","Name","string",self.idName)
    xCoord= self.defProperty(tbNodes,"1","XCoord","param",self.idXCoord)
    xCoord.set("flag","33619968")
    yCoord= self.defProperty(tbNodes,"2","YCoord","param",self.idYCoord)
    yCoord.set("flag","33619968")
    zCoord= self.defProperty(tbNodes,"3","ZCoord","param",self.idZCoord)
    zCoord.set("flag","33619968")
    return nodeContainer

  def defBeamContainer(self,prj):
    beamContainer= self.containerDef(prj,self.idBeamContainer,self.idBeamContainer,"EP_DSG_Elements.EP_Beam.1")
    tbBeams= self.tableDef(beamContainer,"818BBBEB-BD42-473A-8475-9A5EDAEA530D","XML\default","vertical",self.idBeamContainer,"EP_DSG_Elements.EP_Beam.1","3")

    name= self.defProperty(tbBeams,"0","Name","string",self.idName)
    begNode= self.defProperty(tbBeams,"1",self.begNodeName,"ref","{771CB578-EB31-4374-93CA-632E3A6BF147}")
    begNode.set("flag","131072")
    refBegNode= self.defRef(begNode,self.idNodeContainer,self.progIdNodes)
    endNode= self.defProperty(tbBeams,"2",self.endNodeName,"ref","{275774D1-1C02-401A-9521-55B87438C694}")
    endNode.set("flag","131072")
    refEndNode= self.defRef(endNode,self.idNodeContainer,self.progIdNodes)
    return beamContainer

  def defSupportContainer(self,prj):
    supportContainer= self.containerDef(prj,"{1CBCA4DE-355B-40F7-A91D-8EFD26A6404D}","{1CBCA4DE-355B-40F7-A91D-8EFD26A6404D}","DataAddSupport.EP_PointSupportPoint.1")
    tbSupports= self.tableDef(supportContainer,"197586B2-82F0-4E32-8C52-7A4492BC1AD5","XML\default","vertical","{1CBCA4DE-355B-40F7-A91D-8EFD26A6404D}","DataAddSupport.EP_PointSupportPoint.1","13")

    name= self.defProperty(tbSupports,"0","Name","string",self.idName)
    tbRefs= self.defProperty(tbSupports,"1","Reference Table","table","{8F96D00B-2125-411D-9218-EF0038D0056F}")
    subTb= ET.SubElement(tbRefs,"def_subtable")
    subTb.set("size","4")
    subTb.set("type","vertical")
    memberType= self.defProperty(subTb,"0","Member Type","string","{3334884C-E566-404D-A135-4093CC88257D}")
    memberTypeName= self.defProperty(subTb,"1","Member Type Name","string","{FF14758E-BC03-4650-88B5-5EA8F1016952}")
    memberTypeName.set("readonly","yes")
    memberId= self.defProperty(subTb,"2","Member Id","integer","{DD496603-20BD-4CF1-8C72-351306D89D08}")
    memberName= self.defProperty(subTb,"3","Member Name","string","{66D67076-ED1C-44EC-BA07-93E993F493CB}")

    beamType= self.defProperty(tbSupports,"2","Type","enum","{7C7589CA-C90C-11D4-A46A-00C06C542707}")
    beamType.set("flag","262144")
    enumBeam= self.defEnumBeam(beamType)

    xSupport= self.defProperty(tbSupports,"3","X","enum","{49F8018F-A4E5-11D4-A43A-000000000000}")
    xSupport.set("flag","262144")
    enumXSupport= self.defEnumFRF(xSupport)

    ySupport= self.defProperty(tbSupports,"4","Y","enum","{49F80192-A4E5-11D4-A43A-000000000000}")
    ySupport.set("flag","262144")
    enumYSupport= self.defEnumFRF(ySupport)

    zSupport= self.defProperty(tbSupports,"5","Z","enum","{49F80195-A4E5-11D4-A43A-000000000000}")
    zSupport.set("flag","262144")
    enumZSupport= self.defEnumFRF(zSupport)

    rxSupport= self.defProperty(tbSupports,"6","Rx","enum","{49F80198-A4E5-11D4-A43A-000000000000}")
    rxSupport.set("flag","262144")
    enumRXSupport= self.defEnumFRF(rxSupport)

    rySupport= self.defProperty(tbSupports,"7","Ry","enum","{49F8019B-A4E5-11D4-A43A-000000000000}")
    rySupport.set("flag","262144")
    enumRYSupport= self.defEnumFRF(rySupport)

    rzSupport= self.defProperty(tbSupports,"8","Rz","enum","{49F8019E-A4E5-11D4-A43A-000000000000}")
    rzSupport.set("flag","262144")
    enumRZSupport= self.defEnumFRF(rzSupport)
    return supportContainer

  def writeDefFile(self, defFileName):
    f= open(defFileName,"w")

    prj= self.projectDef()
   
    materialContainer= self.defMaterialContainer(prj)
    nodeContainer= self.defNodeContainer(prj)
    beamContainer= self.defBeamContainer(prj)
    supportContainer= self.defSupportContainer(prj)

    treeDef = ET.ElementTree(prj)
    treeDef.write(defFileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")

  def __init__(self, defFileName):
    #datos genéricos del xml 1 encoding UTF-8
    # Print #f, "<?xml version=""1.0"" encoding=""UTF-8"" standalone=""yes""?>"
    self.project= ET.Element("project")
    self.project.set("xmlns","http://www.scia.cz")
    df= ET.SubElement(self.project,"def")
    df.set("uri",defFileName)
    self.writeDefFile(defFileName)

  def newNodeContainer(self):
    # contenedor de nodos
    contNodos= self.newContainer(self.idNodeContainer,self.progIdNodes)
    table= self.newTable(contNodos,"F67B790F-88D4-41ED-A3B9-3D9F9744B5AF",self.progIdNodes,"Node")
    recordDef= ['Name','XCoord','YCoord','ZCoord']
    h= self.defTableRecord(table,recordDef)
    return table

  def newNode(self, table, tag, x, y, z):
    idNodo= nodePrefix+str(tag)
    obj= ET.SubElement(table,"obj")
    obj.set("nm",idNodo)
    self.addRecord(obj,[{'v':idNodo},{'v':str(x)},{'v':str(y)},{'v':str(z)}])
    return obj

  def newBeamContainer(self):
    beamCont= self.newContainer(self.idBeamContainer,"EP_DSG_Elements.EP_Beam.1")
    table= self.newTable(beamCont,"818BBBEB-BD42-473A-8475-9A5EDAEA530D","EP_DSG_Elements.EP_Beam.1","Member 1D")
    h= self.defTableRecord(table,['Name',self.begNodeName,self.endNodeName])
    return table

  def newBeam(self, table, tag, n_ini, n_fin):
    idBeam= beamPrefix+str(tag)
    idNIni= nodePrefix+str(n_ini)
    idNFin= nodePrefix+str(n_fin)
    obj= ET.SubElement(table,"obj")
    obj.set("id",str(tag))
    obj.set("nm",idBeam)
    self.addRecord(obj,[{'v':idBeam},{'i':str(n_ini),'n':str(idNIni)},{'i':str(n_fin),'n':str(idNFin)}])
    return obj

  def strSupportType(self, boolValue):
    if(boolValue):
      return "Rigid"
    else:
      return "Free"

  def supportLine(self, obj, name, boolValue):
    retval= ET.SubElement(obj,name)
    retval.set("v",self.strBool(boolValue))
    retval.set("t",self.strSupportType(boolValue))
    return retval

  def newSupportContainer(self):
    beamCont= self.newContainer("{1CBCA4DE-355B-40F7-A91D-8EFD26A6404D}","DataAddSupport.EP_PointSupportPoint.1")
    table= self.newTable(beamCont,"197586B2-82F0-4E32-8C52-7A4492BC1AD5","DataAddSupport.EP_PointSupportPoint.1","Supports in node")
    h= self.defTableRecord(table,['Name',"Reference Table","Type","X","Y","Z","Rx","Ry","Rz"])
    return table

  def newSupport(self, table, tag, nodeTag, xFixed, yFixed, zFixed, rxFixed, ryFixed, rzFixed):
    #coacciones true:fijo, false:libre
    idSupport= supportPrefix+str(tag)
    idNodo= nodePrefix+str(nodeTag)
    obj= ET.SubElement(table,"obj")
    obj.set("id",str(nodeTag))
    obj.set("nm",idSupport)
    p0= ET.SubElement(obj,"p0")
    p0.set("v",idSupport)
    p1= ET.SubElement(obj,"p1")
    p1.set("t","")
    p2= ET.SubElement(obj,"p2")
    p2.set("v","0")
    p2.set("t","Standard")
    p3= self.supportLine(obj,"p3",xFixed)
    p4= self.supportLine(obj,"p4",yFixed)
    p5= self.supportLine(obj,"p5",zFixed)
    p6= self.supportLine(obj,"p6",rxFixed)
    p7= self.supportLine(obj,"p7",ryFixed)
    p8= self.supportLine(obj,"p8",rzFixed)
    h= self.defTableRecord(p1,["Member Type","Member Type Name","Member Id","Member Name"])
    row= ET.SubElement(p1,"row")
    row.set("id",str(0))
    self.addRecord(row,[{'v':self.idNodeContainer},{'v':self.progIdNodes},{'v':str(nodeTag)},{'v':idNodo}])
    return obj

  def writeXMLFile(self, fileName): 
    tree = ET.ElementTree(self.project)
    tree.write(fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")
