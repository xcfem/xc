# -*- coding: utf-8 -*-
'''Home made test. Verification of getLeftChunk method. There was an error
   in the getChunk method that made the returned chunk to be even longer than
   the original polyline. This test controls that the error does not occur
   again.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import ezdxf
from geom_utils import dxf_document

# Rails.
railPts= [geom.Pos3d(17.6095, 61.706, 13.8412), geom.Pos3d(7.48256, 89.1976, 14.2031), geom.Pos3d(-4.58163, 119.62, 14.486)]
rail= geom.PlanePolyline3d(railPts)

# Intersection point computed elsewhere.
intPoint= geom.Pos3d(1.17226,105.11,14.3511)
# Get the rails that are outside the locomotive.
railFromPoint= rail.getFromPoint()
railLength= rail.getLength()

targetChunk= rail.getLeftChunk(intPoint,.01)
targetChunkLength= targetChunk.getLength()
ratio= abs(targetChunkLength-46.41832030589508)/46.41832030589508

# print('railFromPoint= ', railFromPoint)
# print('rail: ', rail.getVertices(), railLength)
# print('chunk: ', targetChunk.getVertices(), targetChunkLength)
# print('ratio: ', ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio)<1e-10:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


