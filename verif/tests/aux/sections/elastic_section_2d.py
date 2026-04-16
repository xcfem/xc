# -*- coding: utf-8 -*-
#2D Elastic section made to develop tests over it (mostly about sign of strains/stress/curvatures/internal forces)
#Section scheme:

#             y
#             ^
#             |
#             |
#     3 +-----|-----+ 2   -
#       |     |     |     ^
# z<----|-----+     |     | depthOverY
#       |           |     |
#     4 +-----------+ 1   -
#
#       |<--------->|
#          widthOverZ 

# STATIC SIGN CONVENTION: forces and stresses are positive or negative according to their directions along the coordinate axes. 
# DEFORMATION SIGN CONVENTION: forces and stresses are positive if they produce elongation (or tension).

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
import xc
from materials import typical_materials

def def_elastic_section(preprocessor, A, E, Iz, y1, z1, epsilon1, epsilon2, epsilon3, epsilon4):
    # Materials definition
    elasticSection2d= typical_materials.defElasticSection2d(preprocessor,"elasticSection2d",A,E,Iz)

    # Elongation in 1, 2, 3 and 4
    # STATIC SIGN CONVENTION: N and epsilon positive.
    # XC SIGN CONVENTION: N and epsilon positive.
    p1= geom.Pos3d(epsilon1,y1,z1)
    p2= geom.Pos3d(epsilon2,-y1,z1)
    p3= geom.Pos3d(epsilon3,-y1,-z1)
    p4= geom.Pos3d(epsilon4,y1,-z1)

    elasticSection2d.setTrialDeformationPlane(xc.DeformationPlane(p1,p2,p3))
    return elasticSection2d

def compute_internal_forces(preprocessor, A, E, Iz, y1, z1, epsilon, epsilon1, epsilon2, epsilon3, epsilon4):
    elasticSection2d= def_elastic_section(preprocessor, A, E, Iz, y1, z1, epsilon1, epsilon2, epsilon3, epsilon4)
    N0= elasticSection2d.getN()
    My0= elasticSection2d.getMy()
    Mz0= elasticSection2d.getMz()

    RR= elasticSection2d.getStressResultant()
    R0= xc.Vector([RR[0],RR[1]]) # N= RR[0], Mz= RR[1]
    elasticSection2d.sectionDeformation= xc.Vector([epsilon,0.0])
    DD= elasticSection2d.getSectionDeformation()
    D0= xc.Vector([DD[0],DD[1]]) # epsilon= DD[0], Kz= DD[1]
    N0S= elasticSection2d.getN()
    My0S= elasticSection2d.getMy()
    Mz0S= elasticSection2d.getMz()
    sigma= E*epsilon
    return elasticSection2d, N0, My0, Mz0, RR, R0, DD, D0, N0S, My0S, Mz0S, sigma
