# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test empleando
   el material fiber_section_GJ. '''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import os
import sys
import xc
from misc import scc3d_testing_bench
from materials.ehe import EHE_materials
from model import predefined_spaces
from solution import predefined_solutions

barDiam= 16e-3 # Rebar diameter.
areaFi16= 2.01e-4 # Rebar area expressed in square meters.
leverArm= 0.5 # Rebar lever arm with respect to section axis.

MzDato= 15e3
NDato= 0.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.B500S.defDiagD(preprocessor)

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../../aux/")
import barsSectionGeometry as bsg

barsSectionGeometry, reinforcementInf, reinforcementSup= bsg.define_section(preprocessor, leverArm, barDiam, areaFi16)

barsSection= preprocessor.getMaterialHandler.newMaterial("fiber_section_GJ","barsSection")
fiberSectionRepr= barsSection.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(barsSectionGeometry.name)
barsSection.setupFibers()

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, "barsSection")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_00F(nodB.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([0,0,0,0,0,MzDato]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(1)


nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RN= nodA.getReaction[0] 
RM= nodA.getReaction[5] 
RN2= nodB.getReaction[0] 

scc= zlElement.getSection()
esfN= scc.getFibers().getResultant()
esfMy= scc.getFibers().getMy(0.0)
esfMz= scc.getFibers().getMz(0.0)
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)

''' 
print("RN= ",(RN/1e3)," kN \n")
print("RN2= ",(RN2/1e3)," kN \n")
print("N= ",(esfN/1e3)," kN \n")
print("My= ",(esfMy/1e3)," kN m\n")
print("Mz= ",(esfMz/1e3)," kN m\n")
print("defMz= ",(defMz))
print("defN= ",(defN))
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
  '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(RN)<1e-10) & (abs(RN2)<1e-10) & (abs(esfMy)<1e-10) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= barsSectionGeometry)
# plt.show()
