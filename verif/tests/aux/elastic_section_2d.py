# -*- coding: utf-8 -*-
#Very simple fiber section (only four fibers)
#made to develop tests over it (mostly about sign of strains/stress/curvatures/internal forces)
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

# Material properties
E= 2.1e6 # Elastic modulus (Pa)

# Cross section properties
widthOverZ= 4
depthOverY= 2
A= widthOverZ*depthOverY # Area (m2)
Iy= 1/12.0*dephtOverY*widthOverZ**3 # Cross section moment of inertia (m4)
Iz= 1/12.0*widthOverZ*dephtOverY**3 # Cross section moment of inertia (m4)
# Materials definition
elasticSection2d= typical_materials.defElasticSection2d(preprocessor,"elasticSection2d",A,E,Iz)

y1= -depthOverY/2.0
z1= -widthOverZ/2.0
p1= geom.Pos3d([y1,z1,epsilon1]))
p2= geom.Pos3d([-y1,z1,epsilon2]))
p3= geom.Pos3d([-y1,-z1,epsilon3]))
p4= geom.Pos3d([y1,-z1,epsilon4]))

elasticSection2d.sectionDeformation= xc.Vector([epsilon



f1.getMaterial().setTrialStrain(epsilon1,0.0)
f2.getMaterial().setTrialStrain(epsilon2,0.0)
f3.getMaterial().setTrialStrain(epsilon3,0.0)
f4.getMaterial().setTrialStrain(epsilon4,0.0)

N0= fourFibersSection.getFibers().getResultant()
My0= fourFibersSection.getFibers().getMy(0.0)
Mz0= fourFibersSection.getFibers().getMz(0.0)

fourFibersSection.setupFibers()
RR= fourFibersSection.getStressResultant()
R0= xc.Vector([RR[0],RR[2],RR[1]]) # N= RR[0], My= RR[2], Mz= RR[1]
deformationPlane0= fourFibersSection.getFibers().getDeformationPlane()
fourFibersSection.setTrialDeformationPlane(deformationPlane0)
#fourFibersSection.setTrialSectionDeformation(xc.Vector([epsilon,0.0,0.0]))
DD= fourFibersSection.getSectionDeformation()
D0= xc.Vector([DD[0],DD[2],DD[1]]) # epsilon= DD[0], Ky= DD[2], Kz= DD[1]
N0S= fourFibersSection.getN()
My0S= fourFibersSection.getMy()
Mz0S= fourFibersSection.getMz()
