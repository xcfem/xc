# -*- coding: utf-8 -*-
#Very simple fiber section (only four fibers)
#made to develop tests over it (mostly about sign of strains/stress/curvatures/internal forces)
#Section scheme:

#             y
#             ^
#             |
#             |
#       o 3   |     o 2   -
#             |           ^
# z<----------+           | depthOverY
#                         |
#       o 4         o 1   -
#
#       |<--------->|
#          widthOverZ 

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Material properties
E= 2.1e6 # Elastic modulus (Pa)

# Cross section properties
widthOverZ= 4
depthOverY= 2
fiberArea= 1e-4 # Area of each fiber (m2)
A= 4*fiberArea # Total area (m2)
Iy= 4*(fiberArea*(widthOverZ/2.0)**2) # Cross section moment of inertia (m4)
Iz= 4*(fiberArea*(depthOverY/2.0)**2) # Cross section moment of inertia (m4)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

# Fibers
y1= -depthOverY/2.0
z1= -widthOverZ/2.0
fourFibersSection= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","fourFibersSection")

f1= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([y1,z1]))
f2= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([-y1,z1]))
f3= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([-y1,-z1]))
f4= fourFibersSection.addFiber("elast",fiberArea,xc.Vector([y1,-z1]))


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
