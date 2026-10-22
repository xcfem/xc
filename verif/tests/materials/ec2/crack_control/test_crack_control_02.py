# -*- coding: utf-8 -*-
''' Crack width calculation according to clause 7.3.4 of EC2:2004.
 
    Test based on an example calculated with https://eurocodeapplied.com/design/en1992/sls-crack-control-rectangular-section
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from misc import scc3d_testing_bench
from model import predefined_spaces
from solution import predefined_solutions # Solution procedure


# Geometry.
b= 1.0 # Section width (m).
h= 0.4 # Section depth (m).
cover= 50e-3+12e-3 # Concrete cover (m).
numRebars= 7 # number of rebars in each layer.
# Compute the diameter of the bars so we get the same steel area
# that the one in the reference document.
halfRefAs= 19.73*1e-4
rebarDiameter= 2*math.sqrt(halfRefAs/numRebars/math.pi)

# Materials.
concrete= EC2_materials.C30 # concrete.
steel= EC2_materials.S500B # reinforcing steel.

# Concrete section.
rcSection= def_simple_RC_section.RCRectangularSection(name='rcSection', width= b, depth= h, concrType= concrete, reinfSteelType= steel)
# Concrete section reinforcement.
bottomRow= def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiameter, width= b, nRebars= numRebars, nominalCover= cover)
topRow= def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiameter, width= b, nRebars= numRebars, nominalCover= cover)
rcSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([bottomRow])
rcSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([topRow])

# Trivial FE model
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Fiber-section material.
templateSection= rcSection.defRCSection(preprocessor,matDiagType= 'k')
## Zero-length element.
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, templateSection.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)
modelSpace.fixNodeF00_0FF(nodB.tag)

# Load definition.
Nload= 253e3 # Axial force for crack control checking.
Myload= 50 # Bending moment force for crack control checking.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag, xc.Vector([Nload,0,0,0,Myload,0]))
modelSpace.addLoadCaseToDomain(lp0.name) # Add the load case to domain.

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(1)

# Create crack controller.
limitState= EC2_limit_state_checking.quasiPermanentLoadsCrackControl # Crack control under quasi-permanent loads.
crackController= limitState.getController(shortTermLoading= False, wk_lim= 0.1e-3)
# Check crack control.
fiberSection= zlElement.getSection()
CF, wk, s_rmax, sigma_s, sigma_c, N, My, Mz= crackController.checkSection(sct= fiberSection)

# Check results.
# help(fiberSection)
As1= bottomRow.getAs()
As2= topRow.getAs()
As= As1+As2
Ac= fiberSection.getArea()-As
refAc= b*h
ratio1= abs(Ac-refAc)/refAc
testOK= abs(ratio1)<1e-6
Ec= concrete.Ecm()

EIy= templateSection.EIy() # Inertia of the undeformed section.
Iy= EIy/Ec
refIy= 1/12.0*b*h**3
ratio2= abs(Iy-refIy)/refIy
testOK&= abs(ratio2)<0.05 # Fibers sections underestimate inertia.
refAs= 2*halfRefAs
ratio3= abs(As-refAs)/refAs
testOK&= abs(ratio3)<1e-6
# Check equilibrium of cross-section.
crackedSectionIy= fiberSection.EIy()/Ec
ref_sigma_s= Nload/As+Myload/crackedSectionIy
ratio4= abs(sigma_s-ref_sigma_s)/ref_sigma_s
testOK&= abs(ratio4)<1e-3
# Auxiliary quantities.
## Factorependent on the load duration.
refKt= 0.4 # Long-term loads.
kt= crackController.getKt()
ratio5= abs(kt-refKt)/refKt
testOK&= abs(ratio5)<1e-15
## Secant modulus of elasticity of concrete
refEcm= 32837e6
ratio6= abs(Ec-refEcm)/refEcm
testOK&= abs(ratio6)<1e-4
## Modulus of elasticity of reinforcement steel
Es= steel.Es
refEs= 200*1e9 # Pa
ratio7= abs(Es-refEs)/refEs
testOK&= abs(ratio7)<1e-6
## Mean tensile strength of concrete
fct_eff= concrete.fctm()
ref_fct_eff= 2.9e6 # Pa
ratio8= abs(fct_eff-ref_fct_eff)/ref_fct_eff
testOK&= abs(ratio8)<5e-3
## Tensile zone e(ective for crack control.
sectionCrackProperties= crackController.getPropertiesFromFiberSection(fiberSection)
h= sectionCrackProperties.h
ref_h= 0.4
d= sectionCrackProperties.d
ref_d= h-0.068
x= max(sectionCrackProperties.x, 0) # Depth of the compressed zone.
ref_x= 0.0
ref_hceff= min(2.5*(ref_h-ref_d), (ref_h-ref_x)/3.0, ref_h/2.0) 
hceff= crackController.EC2_hceff(sectionCrackProperties.h,sectionCrackProperties.d, x)
ratio9= math.sqrt((h-ref_h)**2+(d-ref_d)**2+(x-ref_x)**2+(hceff-ref_hceff)**2)
testOK&= abs(ratio9)<1e-2 # d is slightly different.
ratio10= abs(hceff-ref_hceff)/ref_hceff
testOK&= abs(ratio10)<1e-8
## Maximum crack spacing.
### Effective reinforcement ratio for crack control verification.
ref_Ac_eff= b*ref_hceff
rho_p_eff= As1/ref_Ac_eff
k1= 0.8
k2= 1.0
k3= 3.4
k4= 0.425
c= sectionCrackProperties.cover
spacing= sectionCrackProperties.spacing
ref_s_rmax= k3*c+k1*k2*k4*rebarDiameter/rho_p_eff
influenceWidth= 5.0*(c+rebarDiameter/2.0)
ratio11= math.sqrt((c-0.062329096988310594)**2+(spacing-0.15350934862294402)**2+(influenceWidth-0.35900525559739166)**2)
testOK&= abs(ratio11)<1e-8
testOK&= (influenceWidth>spacing) # No need to apply EN 1992-1-1 eq. (7.14)!!!
ratio12= abs(s_rmax-ref_s_rmax)/ref_s_rmax
testOK&= abs(ratio12)<0.005
# Compute mean strain difference using equation 7.9 of EC2.
meanStrainDifference= crackController.meanStrainDifference(sigma_s= sigma_s, steel= steel, concrete= concrete, ro_eff= rho_p_eff)
ref_meanStrainDifference= 0.6*sigma_s/Es
ratio13= abs(meanStrainDifference-ref_meanStrainDifference)/ref_meanStrainDifference
testOK&= abs(ratio13)<1e-8
ref_wk= ref_meanStrainDifference*ref_s_rmax
ratio14= abs(wk-ref_wk)/ref_wk
testOK&= abs(ratio14)<.005
'''
print('Properties of the cross-section')
print('  Concrete area: Ac= ', Ac, 'm2', 'ratio1= ', ratio1)
print('  Bottom reinforcement layer: As1= ', As1*1e4, 'cm2')
print('  Top reinforcement layer: As2= ', As2*1e4, 'cm2')
print('  Second moment of the area of gross cross-section: Iy= ', Iy, 'm4 ratio2= ', ratio2)
print('  Total area of longitudinal reinforcement: As= ', As*1e4, 'cm2 ratio3=', ratio3)
print('  Rebar diameter: ', rebarDiameter*1e3, 'mm')
print('  Geometric reinforcement ratio: rho_l= ', As/Ac*100, '%')
print('Equilibrium of cross-section')
print('  Tensile steel stress= ', sigma_s/1e6, 'MPa, ratio4=', ratio4)
print('Auxiliary quantites')
print('  Factor dependent on the load duration: kt= ', kt, ' ratio5= ', ratio5)
print('  Secant modulus of elasticity of concrete: Ecm= ', Ec/1e9, 'GPa, ratio6= ', ratio6)
print('  Modulus of elasticity of reinforcement steel: Es= ', Es/1e9, 'GPa, ratio7= ', ratio7)
print('  Ratio of Es to Ec: alpha_m= ', Es/Ec)
print('  Mean tensile strength of concrete: fctm= ', fct_eff/1e6, 'MPa ratio8= ', ratio8)
print('  Height of the tensile zone e9ective for crack control, hceff= ', hceff, ' m; ratio9= ', ratio9)
print('Axial load: N= ', N/1e3, 'kN')
print('Bending moments: My= ', My/1e3, 'kN.m', ' Mz= ', Mz/1e3, 'kN.m')
print('Maximum final crack spacing s_rmax= ', s_rmax*1e3, 'mm; ratio12= ', ratio12)
print('Mean strain difference: (eps_cm-eps_sm)= ', meanStrainDifference*1e3, '‰; ratio13= ', ratio13)
print('Stress in the tension reinforcement assuming a cracked section: sigma_s= ', sigma_s/1e6, 'MPa')
print('Crack width: Wk= ', wk*1e3, 'mm')
print('Capacity factor: CF= ', CF)
print(testOK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
