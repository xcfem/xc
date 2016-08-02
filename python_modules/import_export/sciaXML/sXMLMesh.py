# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import NodeContainer as nCtr
import EPPlaneContainer as eppc
import NodeSupportContainer as nsc
from scia_loads import NodeLoadContainer as nlc
from scia_loads import ElementLoadContainer as elc
import sXMLBase as base

class SXMLMesh(base.SXMLBase):
  '''Export mesh (nodes, elements, supports and loads) to
     SCIA XML.'''
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

