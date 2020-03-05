# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from xml_basics import scxml_table_container as ctr
from xml_basics import scxml_table_xmlnodes as tb
from xml_basics import scxml_object as obj
from xml_basics import scxml_object_item as oI
from xml_basics import scxml_row as rw
import node_container as nc

idEPPlaneContainer= '{8708ed31-8e66-11d4-ad94-f6f5de2be344}'
tEPPlaneContainer= "8708ed31-8e66-11d4-ad94-f6f5de2be344"
idEPPlaneContainerTb= '915fdfe9-b6a9-446b-9f33-857a00d31679'
tEPPlaneContainerTb= 'EP_DSG_Elements.EP_Plane.1'
elementPrefix= 'EPPlane'

def getEPPlaneObject(cell):
  retval= obj.SCXMLObject()
  id= str(cell.id)
  #retval.setId(id)
  name= elementPrefix+id
  retval.setNm(name)

  retval.setP1(oI.SCXMLObjectItem('2')) #Shell
  retval.setP3(oI.SCXMLObjectItem('0'))
  retval.setP4(oI.SCXMLObjectItem('0')) #Constant thickness.
  retval.setP5(oI.SCXMLObjectItem('1')) #Member system plane at prop (center).
  retval.setP6(oI.SCXMLObjectItem('0.0')) #Eccentricity Z.
  retval.setP7(oI.SCXMLObjectItem(str(cell.thickness))) #Thickness.
  retval.setP8(oI.SCXMLObjectItem('0')) #Direction.
  oi12= oI.SCXMLObjectItem()
  row= rw.SCXMLRowP012("0", oI.SCXMLObjectItem("1", None, None, None,None,None), oI.SCXMLObjectItem('', str(cell.nodeIds[0]), nc.nodePrefix+str(cell.nodeIds[0]), None,None,None), oI.SCXMLObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.SCXMLRow("1",oI.SCXMLObjectItem('', str(cell.nodeIds[1]), nc.nodePrefix+str(cell.nodeIds[1]), None,None,None), oI.SCXMLObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.SCXMLRow("2",oI.SCXMLObjectItem('', str(cell.nodeIds[2]), nc.nodePrefix+str(cell.nodeIds[2]), None,None,None), oI.SCXMLObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.SCXMLRow("3",oI.SCXMLObjectItem('', str(cell.nodeIds[3]), nc.nodePrefix+str(cell.nodeIds[3]), None,None,None), oI.SCXMLObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  retval.setP12(oi12) #Cell nodes.
  return retval

class EPPlaneContainer(ctr.SCXMLTableContainer):
  def __init__(self,cellsDict):
    super(EPPlaneContainer,self).__init__(idEPPlaneContainer,tEPPlaneContainer)
    cells= list()
    for key in cellsDict:
      cell= cellsDict[key]
      cells.append(getEPPlaneObject(cell))
    self.appendTable(tb.SCXMLTableXMLNodes(idEPPlaneContainerTb,tEPPlaneContainerTb, '', None,cells))
  
