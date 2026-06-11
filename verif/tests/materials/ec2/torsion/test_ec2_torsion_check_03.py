# -*- coding: utf-8 -*-
''' Check torsion strength of reinforced concrete section according to clause
    6.3 of Eurocode 2.

Reference values obtained from the worked example in:
https://structville.com/2021/01/worked-example-design-of-rc-beams-for-torsion-en-1992-12004.html
'''

from __future__ import print_function
from __future__ import division
from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import xc
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data as lsd

concrete= EC2_materials.EC2Concrete("C28-35",-28e6,1.5, alphacc= 0.85)
steel= EC2_materials.S500B

# Define reinforced concrete section.
cover= 25e-3
section= def_simple_RC_section.RCRectangularSection(name='test', sectionDescr= 'dummy section for testing torsion ULS checking', width= 0.4, depth= 0.6, concrType= concrete, reinfSteelType= steel)
## Main reinforcement.
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 4e-3, nRebars= 2, nominalCover= cover)])
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 25e-3, nRebars= 4, nominalCover= cover)])
## Torsion reinforcement.
angThetaConcrStruts= math.radians(21.802) # Concrete strut angle.
## Shear reinforcement
shearReinfDiam= 4e-3
shearReinfArea= math.pi*(shearReinfDiam/2.0)**2
nBranches= 2
section.shReinfY= def_simple_RC_section.ShearReinforcement(familyName= "sh", nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.3, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= angThetaConcrStruts)

# Design forces.
NEd= 0.0
TEd_max= 55e3
VEd_max= 225e3

## Compute section geometry
### XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
### Compute section geometry.
section.defSectionGeometry(preprocessor= preprocessor, matDiagType= 'd')

# Compute torsion parameters.
torsionParameters= EC2_limit_state_checking.compute_effective_hollow_section_parameters_rc_section(section)

# Check values of torsion parameters.
## Get effetive thickness
tef= torsionParameters.tef() # effective thickness of the wall of the design section.
tef_ref= 0.12 # See spreadshee with the same file name in this folder.
ratio1= abs(tef-tef_ref)/tef_ref
Ac= torsionParameters.A() # Concrete area.
Ak= torsionParameters.Ak() # Area enclosed by the middle line of the design effective hollow section (figure 6.11 of EC2).
Ak_ref= 134400e-6
ratio2= abs(Ak-Ak_ref)/Ak_ref
uk= torsionParameters.uk() # perimeter of the middle line in the design effective hollow section Ak.
uk_ref= 1520e-3
ratio3= abs(uk-uk_ref)/uk_ref

# Check the value of the shear strength reduction factor.
nu1= concrete.getShearStrengthReductionFactor()
nu1_ref= 0.6*(1-28/250)
ratio4= abs(nu1-nu1_ref)/nu1_ref

# Check the value of the torsional shear stress in wall (exp. 6.26 of EC2).
t_tEd= torsionParameters.getShearStressDueToPureTorsionalMoment(TEd= TEd_max)
t_tEd_ref= TEd_max/(2*0.1344*0.12)
ratio5= abs(t_tEd-t_tEd_ref)/t_tEd_ref

# Max design value of torsional resistance according to expr. (6.30) of EC2.
limitState= EC2_limit_state_checking.torsionResistance # Torsion strength limit state.
torsionController= limitState.getController()
TRd_max= torsionController.calcTRd_max(rcSection= section, NEd= 0.0, Ac= Ac, Ak= Ak, tef= tef, theta= angThetaConcrStruts)
TRD_max_ref= 94.0242742234122e3 # See spreadshee with the same file name in this
                                # folder.
ratio6= abs(TRd_max-TRD_max_ref)/TRD_max_ref

# Max design shear force ccording to expr. (6.9) of EC2.
VRdt_max= torsionController.getMaximumShearWebStrutCrushing(rcSection= section, NEd= NEd, bw= section.b, z= 0.9*0.45)
tanTheta= math.tan(angThetaConcrStruts)
cotTheta= 1/tanTheta
VRdt_max_ref= 1.0*0.4*0.9*0.45*nu1_ref*28e6/1.5/(cotTheta+tanTheta)
ratio7= abs(VRdt_max-VRdt_max_ref)/VRdt_max_ref

# Interaction formula; expression (6.29) of EC2.
struts_cf= TEd_max/TRd_max + VEd_max/VRdt_max
struts_cf_ref= 0.99
ratio8= abs(struts_cf-struts_cf_ref)/struts_cf_ref

# Torsional and shear resistance of the concrete alone.
## Compute maximum torsional strength with no shear reinforcement
## according to clause 6.3.2.
TRdc= torsionController.calcTrdc(rcSection= section, Ak= Ak, tef= tef)
fctd= 1.0*0.7*0.3*math.pow(28, 2/3.0)/1.5*1e6
TRdc_ref= 2 * Ak_ref * fctd * tef_ref
ratio9= abs(TRdc-TRdc_ref)/TRdc_ref

## Compute maximum torsional strength with no shear reinforcement
## according to expression (6.2a).
VRdc= torsionController.getShearResistanceCrackedNoShearReinf(rcSection= section, NEd= NEd, bw= section.b, d= 0.45)
VRdc_ref= 0.12*5/3.0*pow(100*0.011047934165124106*28, 1/3.0)*0.4*0.45*1e6
ratio10= abs(VRdc-VRdc_ref)/VRdc_ref

# Interaction formula; expression (6.31) of EC2.
cf1= TEd_max/TRdc + VEd_max/VRdc
cf1_ref= 3.32
ratio11= abs(cf1-cf1_ref)/cf1_ref

# Compute required torsional reinforcement.
## Longitudinal.
Asl_req= EC2_limit_state_checking.TorsionController.getAsl_req(rcSection= section, TEd= TEd_max, Ak= Ak, uk= uk, limitMaxStress= None)
Asl_req_ref= 1788e-6
ratio12= abs(Asl_req-Asl_req_ref)/Asl_req_ref
## Transverse.
Asw_req= EC2_limit_state_checking.TorsionController.getAsw_req(rcSection= section, TEd= TEd_max, Ak= Ak, limitMaxStress= None)
Asw_req_ref= 188e-6
ratio13= abs(Asw_req-Asw_req_ref)/Asw_req_ref
Asw_max_spacing= EC2_limit_state_checking.TorsionController.getAsw_max_spacing(rcSection= section, uk= uk)
Asw_max_spacing_ref= min(uk/8.0, 0.6, 0.4)
ratio14= abs(Asw_max_spacing-Asw_max_spacing_ref)/Asw_max_spacing_ref

'''
print('NEd= ', NEd)
print('TEd_max=', TEd_max/1e3)
print('VEd_max= ', VEd_max/1e3)
print('tef= ', tef*1e3, 'mm, ratio1= ', ratio1)
print('Ak= ', Ak*1e6, 'mm2, ratio2= ', ratio2)
print('uk= ', uk*1e3, 'mm, ratio3= ', ratio3)
print('nu1= ', nu1, ', ratio4= ', ratio4)
print('t_tEd= ', t_tEd/1e6, ', ratio5= ', ratio5)
print('TRd_max= ', TRd_max/1e3, 'kN.m, ratio6= ', ratio6)
print('VRdt_max= ', VRdt_max/1e3, 'kN, ratio7= ', ratio7)
print('struts_cf= ', struts_cf, 'ratio8= ', ratio8)
print('TRdc= ', TRdc/1e3, 'ratio9= ', ratio9)
print('VRdc= ', VRdc/1e3, 'ratio10= ', ratio10)
print('cf1= ', cf1, 'ratio11= ', ratio11)
print('Asl_req= ', Asl_req, 'ratio12= ', ratio12)
print('Asw_req= ', Asw_req, 'ratio13= ', ratio13)
print('Asw_max_spacing= ', Asw_max_spacing, 'ratio14= ', ratio14)
'''

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if ((abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12) and (abs(ratio5)<1e-12) and (abs(ratio6)<1e-4) and (abs(ratio7)<1e-12) and (abs(ratio8)<1e-3) and (abs(ratio9)<1e-12) and (abs(ratio10)<1e-12) and (abs(ratio11)<1e-2) and (abs(ratio12)<1e-3) and (abs(ratio13)<1e-2) and (abs(ratio14)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
