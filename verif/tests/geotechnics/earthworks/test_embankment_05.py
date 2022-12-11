# -*- coding: utf-8 -*-
''' Test Embankment getEarthPressuresOnPointGrid method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from geotechnics.earthworks import embankment
from geotechnics import frictional_soil as fs
from scipy.constants import g

# Embankment points.
#     p2 +--------+ p3
#       /          \
#      /            \
#     /              \
# p1 +                + p4  ---> X axis
# 

# Vertex list.
embankmentHeight= 10.0
vertices= [geom.Pos3d(-50, 0, 0), geom.Pos3d(-40, 0, embankmentHeight), geom.Pos3d(40, 0, embankmentHeight), geom.Pos3d(50, 0, 0)]

# Define polyline.
plane_pline_3d= geom.PlanePolyline3d(vertices)

# Define soil.
soilGamma= 20e3 # N/m3
soilRho= soilGamma/g
soil= fs.FrictionalSoil(phi= math.radians(30), rho= soilRho)
soilLoadSpreadingRatio= 2/1 # 2 vertical : 1 horizontal.
# We define a natural terrain without weight to consider only the weight
# of the embankment over this soil 
naturalSoil= fs.FrictionalSoil(phi= math.radians(30), rho= 0.0) # No mass.


# Construct embankment.
embankment= embankment.EmbankmentCrossSection()
embankment.appendLayer(soil= soil, loadSpreadingRatio= soilLoadSpreadingRatio, layerSurfacePoints= vertices)
embankment.appendLayer(soil= naturalSoil, loadSpreadingRatio= soilLoadSpreadingRatio, layerSurfacePoints= [vertices[0], vertices[-1]])

# Create loaded quadrilateral.
#            Top
#  p4 +----------------+ p3
#     |                |
#     |                |
#     |                |
#     |                |
#  p1 +----------------+ p2
#           Bottom
p1= geom.Pos3d(-5,0,-10)
p2= geom.Pos3d(5,0,-10)
p3= geom.Pos3d(5,0,0)
p4= geom.Pos3d(-5,0,0)
loadedQuadrilateral= geom.Quadrilateral3d(p1, p2, p3, p4)

# Integrate lateral pressure.
## Compute point grid (2x2 coarse mesh).
pointGrid= loadedQuadrilateral.getBilinearGrid(2,2)
unitVectorDirs= pointGrid.getQuadNormals()
## Compute pressures on grid centroids.
k0= naturalSoil.K0Jaky() # Earth pressure at rest
earthPressures= embankment.getEarthPressuresOnPointGrid(pointGrid= pointGrid, unitVectorDirs= unitVectorDirs, k= k0)
print(earthPressures)
areas= pointGrid.getQuadAreas()
## Integrate pressure.
E= geom.Vector3d(0.0,0,0)
for pressure, area in zip(earthPressures, areas):
    E+= pressure*area


# Guia de cimentaciones en obras de carretera simplified formula in section
# 5.6.3
p= embankmentHeight*soilGamma
refE= k0*p*loadedQuadrilateral.getArea()
err= abs(E.y-refE)/refE

'''
print('E= ', E*1e-3, 'kN')
print('p= ', p/1e3, 'kN/m2')
print('area of loaded quadrilateral A= ', loadedQuadrilateral.getArea(), 'm2')
print('refE= ', refE/1e3, 'kN')
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-6):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')


    
    



