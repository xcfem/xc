# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import os
from xml_basics import Definition as df
import NodeContainer as nCtr
import EPPlaneContainer as eppc
import NodeSupportContainer as nsc
import LoadGroupContainer as lgc
import LoadCaseContainer as lcc
import LoadCombContainer as lcmb
import NodeLoadContainer as nlc
import ElementLoadContainer as elc
import ProjectProperties as prjDef
import xml.etree.cElementTree as ET
import sXMLBase as base

class SXMLMesh(base.SXMLBase):
  def __init__(self,xmlns, mesh,loadContainer):
    super(SXMLMesh,self).__init__(xmlns,mesh,loadContainer)
    self.nodeContainer= nCtr.NodeContainer(mesh.nodes)
    self.epPlaneContainer= eppc.EPPlaneContainer(mesh.cells)
    self.nodeSupportContainer= nsc.NodeSupportContainer(mesh.nodeSupports)
    loads= loadContainer.loads
    self.nodeLoadContainers= list()
    self.elementLoadContainers= list()
    for key in sorted(loads.loadCases):
      lc= loads.loadCases[key]
      pl= lc.loads.punctualLoads
      if(pl):
        self.nodeLoadContainers.append(nlc.NodeLoadContainer(pl))
      sl= lc.loads.surfaceLoads
      if(sl):
        self.elementLoadContainers.append(elc.ElementLoadContainer(sl))

  def getContainers(self):
    retval= [self.nodeContainer,self.epPlaneContainer, self.nodeSupportContainer]
    retval.extend(super(SXMLMesh,self).getContainers())
    retval.extend(self.nodeLoadContainers)
    retval.extend(self.elementLoadContainers)
    return retval

