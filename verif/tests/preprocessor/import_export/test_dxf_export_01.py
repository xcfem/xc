# Home made test
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import xc_base
import geom
import xc
from model import predefined_spaces
from import_export import block_topology_entities as bte

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

feProblem.logFileName= "/tmp/erase.log" # Nicely avoid warning messages.
outputFileName= '/tmp/test.dxf'

#               
#  4 +--------------------+ 3
#    |                    |
#    |                    |
#    |                    |
#  1 +--------------------+ 2
#

points= preprocessor.getMultiBlockTopology.getPoints
CooMax= 10.0
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(CooMax,CooMax,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,CooMax,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)

xcTotalSet= modelSpace.getTotalSet()
import ezdxf
bdTotal= bte.BlockData(verbosity= 0)
numEntitiesRead= bdTotal.readFromXCSet(xcTotalSet)
ok1= (abs(numEntitiesRead-8)==0)
bdTotal.writeDxfFile(outputFileName)
import os
fileSize= os.path.getsize(outputFileName)
ok2= (fileSize>3000) and (fileSize<12000) # file exists and has a size that
                                          # seems reasonable.

'''
print(numEntitiesRead)
print(fileSize)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ok1 & ok2):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')

