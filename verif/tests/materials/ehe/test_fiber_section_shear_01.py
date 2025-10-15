# -*- coding: utf-8 -*-
''' Shear checking of a reinforced concrete section.
    Some results are compared with those of the exercise ER-CS-01 from
    www.areadecalculo.com. According to EHE-08».
'''
from __future__ import division
from __future__ import print_function


import math
from misc import scc3d_testing_bench
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import xc
from solution import predefined_solutions
from model import predefined_spaces
from postprocess import limit_state_data as lsd

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 0.3 # Section width expressed in meters.
depth= 0.4 # Section depth expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.

NDato= -400e3 # Axial force for shear checking.
MyDato= 1e5 # Bending moment for shear checking.
MzDato= 1e3 # Bending moment for shear checking.

rebarArea= 4e-4
rebarDiam= 2.0*math.sqrt(rebarArea/math.pi)
numOfRebars= 3
lowerRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)
upperRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concr= EHE_materials.HA25
concr.alfacc=0.85    # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
steel= EHE_materials.B500S

# Section geometry
section= def_simple_RC_section.RCRectangularSection(name='test', width= width, depth= depth, concrType= concr, reinfSteelType= steel)
## Put the reinforcement in the section.
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([lowerRow])
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([upperRow])
# section.shReinfY= shearReinf

fiberSectionName= section.defRCSection(preprocessor, matDiagType= 'd')
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, fiberSectionName)


# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,MyDato,MzDato]))
lp1= modelSpace.newLoadPattern(name= '1')
lp1.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,MzDato/10.0,MyDato/10.0]))
lp2= modelSpace.newLoadPattern(name= '2')
lp2.newNodalLoad(nodB.tag,xc.Vector([NDato,0,0,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)
if(analOk!=0):
    print("Error!; failed to converge.")
    exit()


limitState= EHE_limit_state_checking.shearResistance
shearController= limitState.getController()



scc= zlElement.getSection()
rcSets= shearController.extractFiberData(scc, EHE_materials.HA25, EHE_materials.B500S)
shearController.calcVuEHE08(scc,"",NDato,math.sqrt(MyDato**2+MzDato**2),0,0, rcSets= rcSets)


Vu2A= shearController.Vu2


loadHandler= preprocessor.getLoadHandler.removeFromDomain("0") # Remove this load.
preprocessor.resetLoadCase()
loadHandler= preprocessor.getLoadHandler.addToDomain("1") # Add the other one.


analOk= analysis.analyze(10)
if(analOk!=0):
    print("Error!; failed to converge.")
    exit()


shearController.calcVuEHE08(scc,"",0,0,0,0, rcSets= rcSets)

Vu2B= shearController.Vu2


loadHandler= preprocessor.getLoadHandler.removeFromDomain("1") # Remove this load.
preprocessor.resetLoadCase()
loadHandler= preprocessor.getLoadHandler.addToDomain("2") # Add another one.


analOk= analysis.analyze(10)
if(analOk!=0):
    print("Error!; failed to converge.")
    exit()


shearController.calcVuEHE08(scc,"",0,0,0,0, rcSets= rcSets)

Vu2C= shearController.Vu2


''' Instead of taking Vu2= 127.638 kN as stated in the reference
document, we take 117.052 kN, that's because here we take into account
the axial force in the reinforcement so average stress in the web
is smaller. '''
Vu2ATeor= 117.052e3
ratio1= ((Vu2A-Vu2ATeor)/Vu2ATeor)
Vu2BTeor= 84630.2
ratio2= ((Vu2B-Vu2BTeor)/Vu2BTeor)
Vu2CTeor= 92301.8
ratio3= ((Vu2C-Vu2CTeor)/Vu2CTeor)


'''
section.plot(preprocessor) # Display section geometry
print("Vu2A= ",Vu2A/1e3," kN")
print("Vu2ATeor= ",Vu2ATeor/1e3," kN")
print("ratio1= ",ratio1)
print("\nVu2B= ",Vu2B/1e3," kN")
print("Vu2BTeor= ",Vu2BTeor/1e3," kN")
print("ratio2= ",ratio2)
print("\nVu2C= ",Vu2C/1e3," kN")
print("Vu2CTeor= ",Vu2CTeor/1e3," kN")
print("ratio3= ",ratio3)
 '''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<0.03) & (abs(ratio3)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
