# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test.
   results are compared with those of the prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM).

The results obtained are acceptable considering that:

- The "prontuario" does not take into account that the maximum value of resistance is 1.05 times that of the characteristic resistance.
- This program does not discount the holes in the concrete occupied by the reinforcement.
- Fiber model discretization introduces an error.
 '''

# feProblem.logFileName= "/tmp/erase.log"  # Ignore warning messages

from __future__ import print_function
from __future__ import division
import xc_base
import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions

from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from materials.sections import section_properties
from model import predefined_spaces

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

CurvZ= 11.474e-3 # Curvature test 1.
x1= 0.0997 # Neutral axis depth 1.
MzDato= 55.949206e3 # Bending moment that makes tension reinforcement to yield.
NDato= 0.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
reinfMatTag= EHE_materials.B500S.defDiagD(preprocessor)
dgDB500S= EHE_materials.B500S.getDiagD(preprocessor)
concr= EHE_materials.HA25
concr.alfacc=0.85    # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concreteTag= concr.defDiagD(preprocessor)

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
# print("pth= ", pth)
exec(open(pth+"/concrete_section_01.py").read())
secHA= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(concreteSectionGeom01.name)
secHA.setupFibers()

scc3d_testing_bench.sectionModel(preprocessor, "secHA")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,MzDato]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(1)

IEIz= 0.0
IEIy= 0.0
TEIz= 0.0
TEIy= 0.0
x= 0.0

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
fibras= scc.getFibers()
sumAreas= fibras.getArea(1.0)
Iz= fibras.getIz(1.0,0.0)
IEIy= scc.getInitialTangentStiffness().at(3,3)
IEIz= scc.getInitialTangentStiffness().at(2,2)
TEIy= scc.getTangentStiffness().at(3,3)
TEIz= scc.getTangentStiffness().at(2,2)
Iy= fibras.getIy(1.0,0.0)
esfN= fibras.getResultant()
esfMy= fibras.getMy(0.0)
esfMz= fibras.getMz(0.0)
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")
x= scc.getNeutralAxisDepth()
Resul= scc.getStressResultant()
Deform= scc.getSectionDeformation()
setsRC= fiber_sets.fiberSectionSetupRCSets(scc,EHE_materials.HA25.matTagD,'concrete',EHE_materials.B500S.matTagD,"reinforcement")


fibraCEpsMin= -1
fMin= setsRC.concrFibers.getFiberWithMinStrain()

epsCMin= fMin.getMaterial().getStrain() # Minimum strain in concrete.
sgCMin= fMin.getMaterial().getStress() # Minimum stress in concrete.
YepsCMin= fMin.getPos().x # Y Z coordinates for the minimum concrete strain.
ZepsCMin= fMin.getPos().y

fMin= setsRC.reinfFibers.getFiberWithMinStrain()

epsSMin= fMin.getMaterial().getStrain() # Minimum strain in steel.
sgSMin= fMin.getMaterial().getStress() # Minimum stress in steel.
YepsSMin= fMin.getPos().x # Y Z coordinates for the minimum steel strain.
ZepsSMin= fMin.getPos().y 

fMax= setsRC.reinfFibers.getFiberWithMaxStrain()

epsSMax= fMax.getMaterial().getStrain() # Maximum strain in steel.
sgSMax= fMax.getMaterial().getStress() # Maximum stress in steel.
YepsSMax= fMax.getPos().x # Y Z coordinates for the maximum steel strain.
ZepsSMax= fMax.getPos().y


from materials.sections import section_properties
from materials.ehe import EHE_limit_state_checking
solicitationType= section_properties.solicitationType(epsCMin,epsSMax)
solicitationTypeString= section_properties.solicitationTypeString(solicitationType)
cumpleFT= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.bendingOK(epsCMin,epsSMax)
aprovSecc= EHE_materials.ReinforcedConcreteLimitStrainsEHE08.getBendingEfficiency(epsCMin,epsSMax)

ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)
ratio4= (epsSMin+0.5e-3)/0.5e-3
ratio5= (Resul[1]-MzDato)/MzDato
ratio6= (epsSMax-10e-3)/10e-3

'''
print("sumAreas= ",(sumAreas))
print("Iz= ",(Iz))
print("IEIz= ",IEIz)
print("TEIz= ",TEIz)
print("E1= ",(IEIz/Iz))
print("Iy= ",(Iy))
print("IEIy= ",IEIy)
print("TEIy= ",TEIy)
print("E2= ",(IEIy/Iy))
print("Profundidad de la fibra neutra= ",x," m")
print("Resul= ",Resul*1e-3," kN")
print("Deform= ",Deform*1e3,"E-3")

print("\nMinimum strain in concrete: ",(epsCMin*1E3),"E-3")
print("Minimum stress in concrete: ",(sgCMin/1e6),"E6")
print("y coordinate for minimum concrete strain: ",(YepsCMin))
print("z coordinate for minimum concrete strain: ",(ZepsCMin))

print("\nMinimum strain en la reinforcement: ",(epsSMin*1E3),"E-3")
print("Minimum stress in steel: ",(sgSMin/1e6),"E6")
print("y coordinate for minimum rebar strain: ",(YepsSMin))
print("z coordinate for minimum rebar strain: ",(ZepsSMin))

print("\nMaximum strain en la reinforcement: ",(epsSMax*1E3),"E-3")
print("Maximum stress in steel: ",(sgSMax/1e6),"E6")
print("y coordinate for maximum rebar strain: ",(YepsSMax))
print("z coordinate for maximum rebar strain: ",(ZepsSMax))
print("\nSolicitation type: ",solicitationTypeString," (",(solicitationType),") ")
print("Cumple a ",solicitationTypeString,": ",(cumpleFT))
print("Aprovechamiento a ",solicitationTypeString,": ",(aprovSecc))
print("RN= ",(RN/1e3))
print("RN2= ",(RN2/1e3))
print("N= ",(esfN/1e3))
print("My= ",(esfMy/1e3))
print("Mz= ",(esfMz/1e3))
print("defMz= ",(defMz))
print("defN= ",(defN))
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))
print("ratio6= ",(ratio6))
 '''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-9) & (abs(RN)<1e-9) & (abs(RN2)<1e-9) & (abs(esfMy)<1e-9) & (solicitationType == 2) & (abs(ratio4)<0.06) & (abs(ratio5)<1e-6) & (abs(ratio6)<1e-6)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
