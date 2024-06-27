# -*- coding: utf-8 -*-
''' Test of reinforced rectangular elastomeric bearing design according
    to EN 1337-3:2005.

    Data obtained from a calcuation sample from Mageba.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from misc_utils import log_messages as lmsg
from materials.en_1337 import en_1337_3 as en
    
# Maximum displacements.
vxd= 141.8e-3 # m
vyd= 33.5e-3 # m
# Rotations.
alpha_ad= 0.0030 # rad
alpha_bd= 0.0 # rad
# Loads.
Fzd= 2746.8e3 # design vertical load.
Vmin= 1226.3e3 # minimum vertical load.
Vperm= 1589.2e3 # design permanent load.
Fxd= 166.8e3 # design horizontal load along x.
Fyd= 49.1e3 # design horizontal load along y.

bearing= en.RectangularLaminatedBearing(a= 0.55, b= 0.5, tb= 0.172, ti= 0.011, ts= 2e-3, Te= 0.146, tso= 0.0, Tb= 0.172, n= 12, C= 5e-3, ted= 7e-3)

# Compute effective area.
effectiveArea= bearing.getEffectiveArea()



ratio1= abs(effectiveArea-264600.0e-6)/264600.0e-6
# Compute reduced effective area.
reducedArea= bearing.getReducedEffectiveArea(vxd= vxd, vyd= vyd)
ratio2= abs(reducedArea-177027e-6)/177027e-6
# Shape factor.
S1= bearing.getShapeFactorS1()
ratio3= abs(S1-11.676963812886143)/11.676963812886143
S2= bearing.getShapeFactorS2()
ratio4= abs(S2-13.106796116504853)/13.106796116504853
# Compute compressive strain
eps_cd= bearing.getCompressiveStrain(vxd= vxd, vyd= vyd, Fzd= Fzd)
ratio5= abs(eps_cd-2.2146438317891426)/2.2146438317891426
# Compute shear strain.
eps_qd= bearing.getShearStrain(vxd= vxd, vyd= vyd)
ratio6= abs(eps_qd-0.997968710058754)/0.997968710058754
# Compute strain due to angular rotation.
eps_alphad= bearing.getAngularRotationStrain(alpha_ad= alpha_ad, alpha_bd= alpha_bd)
ratio7= abs(eps_alphad-0.24803719008264463)/0.24803719008264463
# Compute total strain.
eps_td= bearing.getTotalStrain(vxd= vxd, vyd= vyd, Fzd= Fzd, alpha_ad= alpha_ad, alpha_bd= alpha_bd)
ratio8= abs(eps_td-5.433696054797232)/5.433696054797232

# Compute minimum thickness of steel laminate.
thk= bearing.getStrictReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= False)
minThk= bearing.getRequiredReinforcedPlateThickness(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= False)
ratio9= abs(thk-1.8883532788898882e-3)/1.8883532788898882e-3
thkCF= bearing.getPlateThicknessEfficiency(vxd= vxd, vyd= vyd, Fzd= Fzd, withHoles= False)
ratio9+= abs(thkCF-1)

# Check buckling stability.
buckLimStress= bearing.getBucklingLimitStress()
buckCF= bearing.getBucklingEfficiency(vxd= vxd, vyd= vyd, Fzd= Fzd)
buckCFRef= 15.5/26.0 # from the Mageba report.
ratio10= abs(buckCF-buckCFRef)/buckCFRef

# Compute total vertical deflection.
Vzd= bearing.getTotalVerticalDeflection(vxd= vxd, vyd= vyd, Fzd= Fzd)
VzdRef= 4.362070755417235e-3
ratio11= abs(Vzd-VzdRef)/VzdRef

# Compute rotational deflection.
rotDef= bearing.getRotationalDeflection(alpha_ad= alpha_ad, alpha_bd= alpha_bd)
rotCF= bearing.getRotationalLimitEfficiency(vxd= vxd, vyd= vyd, Fzd= Fzd, alpha_ad= alpha_ad, alpha_bd= alpha_bd)
rotCFRef= 0.54/4.8 # from the Mageba report.
ratio12= abs(rotCF-rotCFRef)/rotCFRef

# Friction check.
sigma_perm= bearing.getCompressiveStress(vxd= vxd, vyd= vyd, Fzd= Vperm)
sgPermCF= bearing.getPermCompressiveStressEfficiency(vxd= vxd, vyd= vyd, Fzd_min_perm= Vperm)
sgPermCFRef= 3/8.98 # from the Mageba report.
ratio13= abs(sgPermCF-sgPermCFRef)/sgPermCFRef
frictionCoef= bearing.getFrictionCoefficient(vxd= vxd, vyd= vyd, Fzd_min= Vmin, concreteBedding= True)
frictionForce= bearing.getFrictionForce(vxd= vxd, vyd= vyd, Fzd_min= Vmin, concreteBedding= True)
slidingConditionCF= bearing.getSlidingConditionEfficiency(vxd= vxd, vyd= vyd, Fxd= Fxd, Fyd= Fyd, Fzd_min= Vmin, concreteBedding= True)
slidingConditionCFRef= math.sqrt(Fxd**2+Fyd**2)/281.95e3
ratio14= abs(slidingConditionCF-slidingConditionCFRef)/slidingConditionCFRef

'''
print('Effective area: Ae= '+'{:.3f}'.format(effectiveArea)+' m2')
print('ratio1= '+str(ratio1))
print('Reduced effective area: Ar= '+'{:.3f}'.format(reducedArea)+' m2')
print('ratio2= '+str(ratio2))
print('Shape factor considering inner layer: S1= '+'{:.2f}'.format(S1))
print('ratio3= '+str(ratio3))
print('Shape factor considering outer layer: S2= '+'{:.2f}'.format(S2))
print('ratio4= '+str(ratio4))
print('Compressive strain (cl. no. 5.3.3.1 & 5.3.3.2): eps_cd= '+'{:.3f}'.format(eps_cd))
print('ratio5= '+str(ratio5))
print('Shear strain (cl. no. 5.3.3.3): eps_qd= '+'{:.3f}'.format(eps_qd))
print('ratio6= '+str(ratio6))
print('Strain due to angular rotation(cl. no. 5.3.3.4): eps_alphad= '+'{:.3f}'.format(eps_alphad))
print('ratio7= '+str(ratio7))
print('Total design strain (cl. no. 5.3.3a): eps_td= '+'{:.2f}'.format(eps_td))
print('ratio8= '+str(ratio8))
print('Strict reinforcing plate thickness (cl. no. 5.3.3.5): t_s= '+'{:.2f}'.format(thk*1e3))
print('ratio9= '+str(ratio9))
print('Minimum reinforcing plate thickness (cl. no. 5.3.3.5): t_s= '+'{:.2f}'.format(minThk*1e3))
print('Buckling stability limit stress (cl. no. 5.3.3.6): sigma_buck= '+'{:.1f}'.format(buckLimStress/1e6))
print('Buckling stability efficiency (cl. no. 5.3.3.6): CF_buck= '+'{:.1f}'.format(buckCF))
print('ratio10= '+str(ratio10))
print('Total vertical deflection: (cl. no. 5.3.3.7): Vzd= '+'{:.1f}'.format(Vzd*1e3))
print('ratio11= '+str(ratio11))
print('Rotational deflection: (cl. no. 5.3.3.7): rotDef= '+'{:.2f}'.format(rotDef*1e3))
print('Efficiency with respect to the rotational deflection limit (cl. no. 5.3.3.7): rotCF= '+'{:.2f}'.format(rotCF))
print('ratio12= '+str(ratio12))
print('Minimum compressive stress under permanent loads: sigma_min_perm= '+'{:.2f}'.format(sigma_perm/1e6)+' MPa')
print('Efficiency with respect to the minimum compressive stress under permanent loads: sfPermCF= '+'{:.2f}'.format(sgPermCF))
print('ratio13= '+str(ratio13))
print('Efficiency with respect to the sliding condition: slidingConditionCF= '+'{:.2f}'.format(slidingConditionCF))
print('ratio14= '+str(ratio14))
'''

fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-6) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-6) & (abs(ratio4)<1e-6) & (abs(ratio5)<1e-6) & (abs(ratio6)<1e-6) & (abs(ratio7)<1e-6) & (abs(ratio8)<1e-6) & (abs(ratio9)<1e-6) & (abs(ratio10)<1e-2)& (abs(ratio11)<1e-6) & (abs(ratio12)<1e-2) & (abs(ratio13)<1e-3) & (abs(ratio14)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
