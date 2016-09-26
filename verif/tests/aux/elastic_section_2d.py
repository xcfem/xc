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

# Material properties
E= 2.1e6 # Elastic modulus (Pa)

# Cross section properties
widthOverZ= 4
depthOverY= 2
A= widthOverZ*depthOverY # Area (m2)
Iy= 1/12.0*depthOverY*widthOverZ**3 # Cross section moment of inertia (m4)
Iz= 1/12.0*widthOverZ*depthOverY**3 # Cross section moment of inertia (m4)
# Materials definition
elasticSection2d= typical_materials.defElasticSection2d(preprocessor,"elasticSection2d",A,E,Iz)

y1= -depthOverY/2.0
z1= -widthOverZ/2.0

# Elongation in 1, 2, 3 and 4
# STATIC SIGN CONVENTION: N and epsilon positive.
# XC SIGN CONVENTION: N and epsilon positive.
p1= geom.Pos3d(epsilon1,y1,z1)
p2= geom.Pos3d(epsilon2,-y1,z1)
p3= geom.Pos3d(epsilon3,-y1,-z1)
p4= geom.Pos3d(epsilon4,y1,-z1)

elasticSection2d.setTrialDeformationPlane(xc.DeformationPlane(p1,p2,p3))

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
