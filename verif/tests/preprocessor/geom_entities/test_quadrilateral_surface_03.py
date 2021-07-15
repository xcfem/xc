# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

# Home made test: trivial tests about quadrilateral faces definition

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

feProblem.logFileName= "/tmp/erase.log" # Nicely avoid warning messages.
NumDivI= 3
NumDivJ= 2
CooMax= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

#               
#  4 +--------------------+ 3
#    |                    |
#    |                    |
#    |                    |
#  1 +--------------------+ 2
#

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(CooMax,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(CooMax,CooMax,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,CooMax,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)

seq1= s.getKPoints()
s.reverse()
seq2= s.getKPoints()
r_seq1= seq1.getReversed()
diff= (seq2[0]-r_seq1[0])**2+(seq2[1]-r_seq1[1])**2+(seq2[2]-r_seq1[2])**2+(seq2[3]-r_seq1[3])**2

ratio2= 0.0
for side in s.getSides:
    p1= side.getEdge.getCentroid()
    oppEdge= s.getOppositeEdge(side.getEdge)
    p2= oppEdge.getCentroid()
    d= p1.dist(p2)
    ratio2+= (d-CooMax)**2

ratio2= math.sqrt(ratio2)

# print('seq1= ', seq1)
# print('seq2= ', seq2)
# print('r_seq1= ', r_seq1)
# print('diff= ', diff)
# print('ratio2= ', ratio2)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((diff<=1e-20) and (ratio2<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

