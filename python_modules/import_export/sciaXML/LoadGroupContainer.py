# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import Container as ctr
import TableNode as tb
import Header as hdr
import HeaderItem as hi
import Object as obj
import ObjectItem as oI
import Row as rw
import NodeContainer as nc
import LoadGroupProperties as lgp

idLoadGroupContainer= lgp.containerId
tLoadGroupContainer= lgp.tbProgId
idLoadGroupContainerTb= lgp.tbId
tLoadGroupContainerTb= lgp.tbProgId
loadGroupPrefix= 'LG'

def getLoadGroupObject(loadGroup):
  retval= obj.Object()
  id= str(loadGroup.id)
  retval.setId(id)
  name= loadGroupPrefix+id
  retval.setNm(name)
  retval.setP0(oI.ObjectItem(name)) #Name
  tmp= oI.ObjectItem('0')
  tmp.t= 'Permanent'
  retval.setP1(tmp) #??
  return retval

class LoadGroupContainer(ctr.Container):
  def __init__(self,loadGroupsDict):
    super(LoadGroupContainer,self).__init__(idLoadGroupContainer,tLoadGroupContainer)
    loadGroups=[]
    for key in sorted(loadGroupsDict):
      ns= loadGroupsDict[key]
      loadGroups.append(getLoadGroupObject(ns))
    self.table= tb.TableNode(idLoadGroupContainerTb,tLoadGroupContainerTb, 'Load groups', None,loadGroups)
  
