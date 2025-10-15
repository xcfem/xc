# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.
   Results are compared with those of the «Prontuario informático del hormigón armado». '''

from __future__ import division
from __future__ import print_function

import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure


from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

width= 1.0 # Section width expressed in meters.
depth= 0.25 # Section depth expressed in meters.
cover= 0.035 # Concrete cover expressed in meters.
rebarDiam= 12e-3 # Rebar diameter.
rebarArea= 1.13e-4 # Rebar area.
numOfRebars= 7 # Number of rebars.

NDato= 0.0 # Axial force for crack control checking.
MyDato= 40e3 # Bending moment force for crack control checking.

# Material definition
concr= EHE_materials.HA25
steel= EHE_materials.B500S

# Reinforcement
lowerRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover)
# Section geometry
section= def_simple_RC_section.RCRectangularSection(name='test', width= width, depth= depth, concrType= concr, reinfSteelType= steel)
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([lowerRow])
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([])

# Trivial FE model
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Fiber-section material.
fiberSectionName= section.defRCSection(preprocessor,matDiagType= 'k')
## Zero-length element.
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, fiberSectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0FF(nodB.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag, xc.Vector([NDato,0,0,0,MyDato,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)

secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')

elements= preprocessor.getElementHandler
scc= zlElement.getSection()
secHAParamsFis.computeWk(scc,concr.matTagK,steel.matTagK,concr.fctm())

wkReference= 0.226250241526506e-3 # Avg of computed values in different computers.
ratio1= (abs(secHAParamsFis.Wk-wkReference)/wkReference)
ratio2= ((secHAParamsFis.Wk-0.21e-3)/0.21e-3)

'''
section.plot(preprocessor) # Display section geometry
secHAParamsFis.printParams()
print("Wk= ", secHAParamsFis.Wk*1e3,"mm")
print("ratio1= ", ratio1)
print("ratio2= ", ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-4) and (abs(ratio2)<0.1)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
