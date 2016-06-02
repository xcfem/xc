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

class SXMLBlockTopology(base.SXMLBase):
  def __init__(self,xmlns, blocks,loadContainer):
    super(SXMLBlockTopology,self).__init__(xmlns,blocks,loadContainer)
    self.pointContainer= nCtr.NodeContainer(blocks.points)
    self.blockContainer= eppc.EPPlaneContainer(blocks.blocks)
    self.pointSupportContainer= nsc.NodeSupportContainer(blocks.pointSupports)
    print 'XXX add free loads.'

  def getContainers(self):
    retval= [self.pointContainer,self.blockContainer,self.pointSupportContainer]
    retval.extend(super(SXMLBlockTopology,self).getContainers())
    return retval

