# -*- coding: utf-8 -*-
# home made test
# Sign convention for 2D elastic sections.

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials
from postprocess import prop_statistics
import math

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   

#Constant positive strain.
epsilon= 3.5e-3
epsilon1= epsilon
epsilon2= epsilon
epsilon3= epsilon
epsilon4= epsilon


#Read section definition from file.
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/../../aux/elastic_section_2d.py")

sigma= E*epsilon
N0Teor= (epsilon1+epsilon2+epsilon3+epsilon4)/4.0*E*A
Mz0Teor= 0.0
R0Teor=xc.Vector([N0Teor,Mz0Teor])
D0Teor=xc.Vector([epsilon,0.0])


ratioN0= abs(N0-N0Teor)/N0Teor
ratioN0S= abs(N0S-N0Teor)/N0Teor
ratioMz0= abs(Mz0-Mz0Teor)
ratioMz0S= abs(Mz0S-Mz0Teor)
ratioR0= (R0Teor-R0).Norm()
ratioD0= (D0Teor-D0).Norm()

# Shortening in 1 and 4
# Elongation in 2 and 3
# STATIC SIGN CONVENTION: Mz and kZ negative (Right hand rule).
# XC SIGN CONVENTION: Mz and kZ POSITIVE (it is annoying Yeah!)
elasticSection2d.revertToStart()
epsilon1= -epsilon
epsilon2= epsilon
epsilon3= epsilon
epsilon4= -epsilon
p1= geom.Pos3d(epsilon1,y1,z1)
p2= geom.Pos3d(epsilon2,-y1,z1)
p3= geom.Pos3d(epsilon3,-y1,-z1)
p4= geom.Pos3d(epsilon4,y1,-z1)
elasticSection2d.setTrialDeformationPlane(xc.DeformationPlane(p1,p2,p3))
DD= elasticSection2d.getSectionDeformation()
N1= elasticSection2d.getN()
Mz1= elasticSection2d.getMz()
RR= elasticSection2d.getStressResultant()
R1= xc.Vector([RR[0],RR[1]]) # N= RR[0], Mz= RR[1]
epsilonTeor= (epsilon1+epsilon2+epsilon3+epsilon4)/4.0
kZ1Teor= -((epsilon4-epsilon3)+(epsilon1-epsilon2))/depthOverY/2.0
elasticSection2d.sectionDeformation= xc.Vector([epsilonTeor,kZ1Teor])
D1= xc.Vector([DD[0],DD[1]]) # epsilon= DD[0], Kz= DD[1]
N1S= elasticSection2d.getN()
Mz1S= elasticSection2d.getMz()

N1Teor= epsilonTeor*E*A
Mz1Teor= kZ1Teor*E*Iz
R1Teor=xc.Vector([N1Teor,Mz1Teor])
D1Teor=xc.Vector([0.0,kZ1Teor])

ratioN1= abs(N1-N1Teor)
ratioN1S= abs(N1S-N1Teor)
ratioMz1= abs(Mz1-Mz1Teor)
ratioMz1S= abs(Mz1S-Mz1Teor)
ratioR1= (R1Teor-R1).Norm()
ratioD1= (D1Teor-D1).Norm()

# Elongations in 1 and 4
# Shortening in 2 and 3
# STATIC SIGN CONVENTION: Mz and kZ positive (Right hand rule).
# XC SIGN CONVENTION: Mz and kZ NEGATIVE (it is annoying Yeah!)
elasticSection2d.revertToStart()
epsilon1= epsilon
epsilon2= -epsilon
epsilon3= -epsilon
epsilon4= epsilon
p1= geom.Pos3d(epsilon1,y1,z1)
p2= geom.Pos3d(epsilon2,-y1,z1)
p3= geom.Pos3d(epsilon3,-y1,-z1)
p4= geom.Pos3d(epsilon4,y1,-z1)
elasticSection2d.setTrialDeformationPlane(xc.DeformationPlane(p1,p2,p3))
DD= elasticSection2d.getSectionDeformation()
N2= elasticSection2d.getN()
Mz2= elasticSection2d.getMz()
RR= elasticSection2d.getStressResultant()
R2= xc.Vector([RR[0],RR[1]]) # N= RR[0], Mz= RR[1]
epsilonTeor= (epsilon1+epsilon2+epsilon3+epsilon4)/4.0
kZ1Teor= -((epsilon4-epsilon3)+(epsilon1-epsilon2))/depthOverY/2.0
elasticSection2d.sectionDeformation= xc.Vector([epsilonTeor,kZ1Teor])
D2= xc.Vector([DD[0],DD[1]]) # epsilon= DD[0], kZ= DD[1]
N2S= elasticSection2d.getN()
Mz2S= elasticSection2d.getMz()

N2Teor= epsilonTeor*E*A
Mz2Teor= kZ1Teor*E*Iz 
R2Teor=xc.Vector([N2Teor,Mz2Teor])
kZ2Teor= -((epsilon4-epsilon3)+(epsilon1-epsilon2))/depthOverY/2.0
D2Teor=xc.Vector([0.0,kZ2Teor]) #Negative ???

ratioN2= abs(N2-N2Teor)
ratioN2S= abs(N2S-N2Teor)
ratioMz2= abs(Mz2-Mz2Teor)/Mz2Teor
ratioMz2S= abs(Mz2S-Mz2Teor)/Mz2Teor
ratioR2= (R2Teor-R2).Norm()
ratioD2= (D2Teor-D2).Norm()

import math
error= math.sqrt(ratioN0**2+ratioMz0**2+ratioN0S**2+ratioMz0S**2+ratioR0**2+ratioD0**2+ratioN1**2+ratioMz1**2+ratioN1S**2+ratioMz1S**2+ratioR1**2+ratioD1**2+ratioN2**2+ratioMz2**2+ratioN2S**2+ratioMz2S**2+ratioR2**2+ratioD2**2)

'''
print "Elongation in 1, 2, 3 and 4"
print "STATIC SIGN CONVENTION: N and epsilon positive."
print "XC SIGN CONVENTION: N and epsilon positive."
print '  N0= ', N0, ' N0S= ', N0S, ' N0Teor= ', N0Teor, ' ratioN0= ', ratioN0, ' ratioN0S= ', ratioN0S
print '  Mz0= ', Mz0, ' Mz0S= ', Mz0S, ' Mz0Teor= ', Mz0Teor, ' ratioMz0= ', ratioMz0, ' ratioMz0S= ', ratioMz0S
print '  R0= ', R0, ' R0Teor= ', R0Teor, ' ratioR0= ', ratioR0
print '  D0= ', D0, ' D0Teor= ', D0Teor, ' ratioD0= ', ratioD0, '\n'

print 'Shortening in 1 and 4, elongation in 2 and 3'
print 'STATIC SIGN CONVENTION: Mz and kZ negative (Right hand rule).'
print 'XC SIGN CONVENTION: Mz and kZ POSITIVE (it is annoying Yeah!)'
print '  N1= ', N1, ' N1S= ', N1S, ' N1Teor= ', N1Teor, ' ratioN1= ', ratioN1, ' ratioN1S= ', ratioN1S
print '  Mz1= ', Mz1, ' Mz1S= ', Mz1S, ' Mz1Teor= ', Mz1Teor, ' ratioMz1= ', ratioMz1, ' ratioMz1S= ', ratioMz1S
print '  R1= ', R1, ' R1Teor= ', R1Teor, ' ratioR1= ', ratioR1
print '  D1= ', D1, ' D1Teor= ', D1Teor, ' ratioD1= ', ratioD1 , '\n'

print 'Elongations in 1 and 4, shortening in 2 and 3'
print 'STATIC SIGN CONVENTION: Mz and kZ positive (Right hand rule).'
print 'XC SIGN CONVENTION: Mz and kZ NEGATIVE (it is annoying Yeah!)'
print '  N2= ', N2, ' N2S= ', N2S, ' N2Teor= ', N2Teor, ' ratioN2= ', ratioN2, ' ratioN2S= ', ratioN2S
print '  Mz2= ', Mz2, ' Mz2S= ', Mz2S, ' Mz2Teor= ', Mz2Teor, ' ratioMz2= ', ratioMz2, ' ratioMz2S= ', ratioMz2S
print '  R2= ', R2, ' R2Teor= ', R2Teor, ' ratioR2= ', ratioR2
print '  D2= ', D2, ' D2Teor= ', D2Teor, ' ratioD2= ', ratioD2, '\n'

print 'error= ', error
'''

fname= os.path.basename(__file__)
if (error < 1e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

