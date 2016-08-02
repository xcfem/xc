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
from scia_loads import PointForceFreeContainer as pffc
from scia_loads import SurfacePressureFreeContainer as spf
import sXMLBase as base

class SXMLBlockTopology(base.SXMLBase):
  '''Export block topology (kPoints, lines, surfaces, volumes), supports
     and load to SCIA XML.'''
  def __init__(self,xmlns, blocks,loadContainer):
    super(SXMLBlockTopology,self).__init__(xmlns,blocks,loadContainer)
    self.pointContainer= nCtr.NodeContainer(blocks.points)
    self.blockContainer= eppc.EPPlaneContainer(blocks.blocks)
    self.pointSupportContainer= nsc.NodeSupportContainer(blocks.pointSupports)
    loads= loadContainer.loads
    self.pointForceFreeContainers= list()
    self.surfacePressureFreeContainers= list()
    for key in sorted(loads.loadCases):
      lc= loads.loadCases[key]
      pl= lc.loads.freePunctualLoads
      if(pl):
        self.pointForceFreeContainers.append(pffc.PointForceFreeContainer(pl))
      sl= lc.loads.freeSurfaceLoads
      if(sl):
        self.surfacePressureFreeContainers.append(elc.ElementLoadContainer(sl))

  def getContainers(self):
    retval= [self.pointContainer,self.blockContainer,self.pointSupportContainer]
    retval.extend(super(SXMLBlockTopology,self).getContainers())
    return retval

