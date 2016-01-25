# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
import MaterialProperties as mp
import uuid

idMaterialContainer= mp.containerId
tMaterialContainer= mp.containerProgId

class MaterialTable(tb.TableNode):
  def __init__(self,matProperties):
    tableId= matProperties.TableId
    tableProgId= matProperties.TableProgId
    super(MaterialTable,self).__init__(tableId,'Materials')
    self.progid= tableProgId
  def populate(self,matDict):
    print "XXX write implementation!"
    

class MaterialContainer(ctr.Container):
  def __init__(self):
    super(MaterialContainer,self).__init__(idMaterialContainer,tMaterialContainer)
    self.tableGeneric= MaterialTable(mp.MaterialProperties.tableGeneric)
    self.tableSteelSIA= MaterialTable(mp.MaterialProperties.tableSteelSIA)
    self.tableConcreteSIA= MaterialTable(mp.MaterialProperties.tableConcreteSIA)

  
