# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

from xml_basics import Container as ctr
from xml_basics import TableXMLNodes  as tb
from xml_basics import Header as hdr
from xml_basics import HeaderItem as hi
from xml_basics import Object as obj
from xml_basics import ObjectItem as oI
from xml_basics import Row as rw
import NodeContainer as nc

idEPPlaneContainer= '{8708ed31-8e66-11d4-ad94-f6f5de2be344}'
tEPPlaneContainer= "8708ed31-8e66-11d4-ad94-f6f5de2be344"
idEPPlaneContainerTb= '915fdfe9-b6a9-446b-9f33-857a00d31679'
tEPPlaneContainerTb= 'EP_DSG_Elements.EP_Plane.1'
elementPrefix= 'EPPlane'

def getEPPlaneObject(cell):
  retval= obj.Object()
  id= str(cell.id)
  #retval.setId(id)
  name= elementPrefix+id
  retval.setNm(name)

  retval.setP1(oI.ObjectItem('2')) #Shell
  retval.setP3(oI.ObjectItem('0'))
  retval.setP4(oI.ObjectItem('0')) #Constant thickness.
  retval.setP5(oI.ObjectItem('1')) #Member system plane at prop (center).
  retval.setP6(oI.ObjectItem('0.0')) #Eccentricity Z.
  retval.setP7(oI.ObjectItem(str(cell.thickness))) #Thickness.
  retval.setP8(oI.ObjectItem('0')) #Direction.
  oi12= oI.ObjectItem()
  row= rw.RowP012("0", oI.ObjectItem("1", None, None, None,None,None), oI.ObjectItem('', str(cell.nodeIds[0]), nc.nodePrefix+str(cell.nodeIds[0]), None,None,None), oI.ObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.Row("1",oI.ObjectItem('', str(cell.nodeIds[1]), nc.nodePrefix+str(cell.nodeIds[1]), None,None,None), oI.ObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.Row("2",oI.ObjectItem('', str(cell.nodeIds[2]), nc.nodePrefix+str(cell.nodeIds[2]), None,None,None), oI.ObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  row= rw.Row("3",oI.ObjectItem('', str(cell.nodeIds[3]), nc.nodePrefix+str(cell.nodeIds[3]), None,None,None), oI.ObjectItem("0", None, None, None,None,None))
  oi12.rows.append(row)
  retval.setP12(oi12) #Cell nodes.
  return retval

class EPPlaneContainer(ctr.Container):
  def __init__(self,cellsDict):
    super(EPPlaneContainer,self).__init__(idEPPlaneContainer,tEPPlaneContainer)
    cells= list()
    for key in cellsDict:
      cell= cellsDict[key]
      cells.append(getEPPlaneObject(cell))
    self.appendTable(tb.TableXMLNodes(idEPPlaneContainerTb,tEPPlaneContainerTb, '', None,cells))
  
