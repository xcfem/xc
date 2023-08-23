# -*- coding: utf-8 -*-
''' Rectangular reinforced concrete section under bending. Based on the
    document «Example 1: Simply supported beam design - Rectangular»
    from Dr. Sharifah Maszura Syed Mohsin Faculty of Civil Engineering and Earth    Resources Malaysia'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from model import predefined_spaces
from misc import scc3d_testing_bench
from solution import predefined_solutions # Solution procedure
from materials.sections.fiber_section import fiber_sets
from colorama import Fore
from colorama import Style

okString= Fore.GREEN+'OK'+Style.RESET_ALL
koString= Fore.RED+'KO'+Style.RESET_ALL

# Cross-section geometry
b= .2 # beam width.
h= .5 # beam depth.
d= 447e-3 # effective depth.
nominalCover= h-d

# Materials.
concrete= EC2_materials.C30
steel= EC2_materials.S500C

# Design bending moment
MEd= 160e3 

# EC2 design
K= -MEd/b/d**2/concrete.fcd() # Relative flexural stress.
z= 0.86*d 
reqAs= MEd/0.87/steel.fyk/z

# Main reinforcement
bars= EC2_materials.rebarsEC2['fi20']
numberOfBars= 3
rebarArea= bars['area']
As= numberOfBars*rebarArea

# Define reinforce concrete cross-section.
## Reinforcement.
lowerRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= b, nRebars= numberOfBars, nominalCover= nominalCover, nominalLatCover= nominalCover)
## Concrete section template.
sectionTemplate= def_simple_RC_section.RCRectangularSection(name='test', width= b, depth= h, concrType= concrete, reinfSteelType= steel)
sectionTemplate.positvRebarRows= def_simple_RC_section.LongReinfLayers([lowerRow])

# Create trivial FE model.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
modelSpace= predefined_spaces.StructuralMechanics3D(preprocessor.getNodeHandler)
## Fiber-section material.
sectionTemplate.defRCSection(preprocessor, matDiagType= 'd')
## Zero-length element.
zlElement, nodA, nodB= scc3d_testing_bench.sectionModel(preprocessor, sectionTemplate.name)
## Constraints
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0FF(nodB.tag)
## Load definition.
nSteps= 1
ts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag, xc.Vector([0,0,0,0,MEd/nSteps,0]))
modelSpace.addLoadCaseToDomain(lp0.name)
## Solve
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(nSteps)
## Get element section.
fiberSection= zlElement.getSection()

# Results.
## Bending moment resisted by the fibers (must be equal to the sollicitation).
My= fiberSection.getMy()

## Concrete strains.
concrFibers= fiber_sets.FiberSet(fiberSection,'concrete',concrete.matTagD)
fibraCEpsMin= concrFibers.getFiberWithMinStrain()
epsCMin= fibraCEpsMin.getMaterial().getStrain() # Minimum concrete strain.
checkConcreteStrain= okString if(-epsCMin<=3.5e-3) else koString

fibraCEpsMax= concrFibers.getFiberWithMaxStrain()
epsCMax= fibraCEpsMax.getMaterial().getStrain() # Maximum concrete strain.
## Steel strains.
reinfFibers= fiber_sets.FiberSet(fiberSection,"reinforcement",steel.matTagD)
fibraSEpsMax= reinfFibers.getFiberWithMaxStrain()
epsSMax= fibraSEpsMax.getMaterial().getStrain() # Maximum steel strain
checkSteelStrain= okString if(epsSMax<=10e-3) else koString
## Lever arm.
xc_z= fiberSection.getLeverArmSegment()
## Neutral axis depth.
xc_x= -fiberSection.getNeutralAxisDepth()
checkNeutralAxisDepth= okString if(xc_x<=0.617*d) else koString
## Stress distribution
stressDistribution= fiberSection.getStrClaseEsfuerzo(1e-4)

# EC2:2004 7.3.2 Minimum reinforcement areas
Act= b*(h-xc_x)
AsMinCrackControl= EC2_limit_state_checking.getAsMinCrackControl(concrete, steel, h, Act, sigmaC= 0.0)
checkAsMinCrackControl= okString if(As>=AsMinCrackControl) else koString
# EC2:2004 9.2.1.1 Minimum reinforcement area for beams
AsMinBeams= EC2_limit_state_checking.getAsMinBeams(concrete, steel, h= h, z= xc_z, bt= b, d= d, nationalAnnex= None)
checkAsMinBeams= okString if(As>=AsMinBeams) else koString
# EC2:2004 9.2.1.1 Maximum reinforcement area for beams
AsMaxBeams= EC2_limit_state_checking.getAsMaxBeams(Ac= b*h)
checkAsMaxBeams= okString if(As<=AsMaxBeams) else koString

ratio1= abs(My-MEd)/MEd
ratio2= abs(xc_z.getLength()-0.38340327940475566)/0.38340327940475566
ratio3= abs(AsMinCrackControl-148.09476688305716e-6)/148.09476688305716e-6
ratio4= abs(AsMinBeams-134.65101153463957e-6)/134.65101153463957e-6
ratio5= abs(AsMaxBeams-4000e-6)/4000e-6

'''
print('MEd= ', MEd/1e3, ' kN.m')
print('ratio1= ', ratio1)
print('K= ', K)
print('z= ', z, xc_z.getLength(), ' m')
print('ratio2= ', ratio2)
print('required As= ', reqAs*1e6, ' mm2')
print('My= ', My/1e3, ' kN.m')
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('Check bending strength.')
print('  Area of reinforcing steel As= ', As*1e6, ' mm2')
print('  Stress distribution: ', stressDistribution)
print('  Minimum concrete strain: ',epsCMin*1e3,'\u2030', checkConcreteStrain)
print('  Maximum concrete strain: ',epsCMax*1e3,'\u2030 (info)')
print('  Maximum rebar strain: ',epsSMax*1e3,'\u2030', checkSteelStrain)
print('  Inner lever arm: ',xc_z.getLength(),'m')
print('  Neutral axis depth: ',xc_x,'m',checkNeutralAxisDepth)
print('Check minimum and maximum reinforcement.')
print('  Crack control minimum reinforcement (EC2:2004 7.3.2): ', AsMinCrackControl*1e6, 'mm2',  checkAsMinCrackControl)
print('  Minimum reinforcement for beams (EC2:2004 9.2.1.1): ', AsMinBeams*1e6, 'mm2',  checkAsMinBeams)
print('  Maximum reinforcement for beams (EC2:2004 9.2.1.1): ', AsMaxBeams*1e6, 'mm2',  checkAsMaxBeams)
'''
#section.plot(preprocessor)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-6) and (ratio2<1e-6) and (ratio3<1e-6) and (ratio4<1e-6) and (ratio5<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

