# -*- coding: utf-8 -*-
''' Test computation of shrinkage and creep parameters according to Model Code
    10.

Based on
'''
from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.mc10 import MC10_materials

concrete= EC2_materials.EC2Concrete("test", -27e6, 1.5)

parameters= MC10_materials.ShrinkageAndCreepParameters(concrete= concrete, cement= '42.5R', h0= 312.5e-3, T= 21, RH= 50, ts= 7, t0= 7.0)
t= 1000 # time at analysis.
# Shrinkage related parameters.
alpha_bs= parameters.get_alpha_bs()
epsilon_cbs0= parameters.get_epsilon_cbs0()
alpha_ds1= parameters.get_alpha_ds1()
alpha_ds2= parameters.get_alpha_ds2()
epsilon_cds0= parameters.get_epsilon_cds0()
beta_s1= parameters.get_beta_s1()
beta_as= parameters.get_beta_as(t= t) # time at analysis.
beta_rh_shrinkage= parameters.get_beta_rh_shrinkage()
beta_ds= parameters.get_beta_ds(t= t) # time at analysis.
epsilon_cbs= parameters.get_epsilon_cbs(t= t) # time at analysis.
epsilon_cds= parameters.get_epsilon_cds(t= t) # time at analysis.
epsilon_cs= parameters.get_epsilon_cs(t= t) # time at analysis.

opensees_epsba= parameters.get_opensees_epsba()
ratio1= abs(opensees_epsba+4.943247416289858e-05)/4.943247416289858e-05
opensees_epsbb= parameters.get_opensees_epsbb()
ratio2= abs(opensees_epsbb-1)
opensees_epsda= parameters.get_opensees_epsda()
ratio3= abs(opensees_epsda+0.0007841853794492701)/0.0007841853794492701
opensees_epsdb= parameters.get_opensees_epsdb()
ratio4= abs(opensees_epsdb-3417.9687500000005)/3417.9687500000005

# Creep related parameters.
t0_T_adj= parameters.get_t0_T_adj(t= t) # time at analysis.
beta_h= parameters.get_beta_h()
beta_bc_fcm= parameters.get_beta_bc_fcm()
beta_bc= parameters.get_beta_bc(t= t) # time at analysis.
beta_dc_fcm= parameters.get_beta_dc_fcm()
beta_rh_creep= parameters.get_beta_rh_creep()
beta_dc_t0= parameters.get_beta_dc_t0()
gamma_t0= parameters.get_gamma_t0()
beta_dc= parameters.get_beta_dc(t= t) # time at analysis.
phi_bc= parameters.get_phi_bc(t= t) # time at analysis.
phi_dc= parameters.get_phi_dc(t= t) # time at analysis.
phi= parameters.get_phi(t= t) # time at analysis.

opensees_phiba= parameters.get_opensees_phiba()
ratio5= abs(opensees_phiba-0.14942555378970301)/0.14942555378970301
opensees_phibb= parameters.get_opensees_phibb()
ratio6= abs(opensees_phibb-1)
opensees_phida= parameters.get_opensees_phida()
ratio7= abs(opensees_phida-2.091969191559629)/2.091969191559629
opensees_phidb= parameters.get_opensees_phidb()
ratio8= abs(opensees_phidb-718.75)/718.75
opensees_cem= parameters.get_opensees_cem()
ratio9= abs(opensees_cem-1)

'''
print('alpha_bs= ', alpha_bs)
print('epsilon_cbs0= ', epsilon_cbs0*1e6)
print('alpha_ds1= ', alpha_ds1)
print('alpha_ds2= ', alpha_ds2)
print('epsilon_cds0= ', epsilon_cds0*1e6)
print('beta_s1= ', beta_s1)
print('beta_as= ', beta_as)
print('beta_rh_shrinkage= ', beta_rh_shrinkage)
print('beta_ds= ', beta_ds)
print('epsilon_cbs= ', epsilon_cbs*1e6)
print('epsilon_cds= ', epsilon_cds*1e6)
print('epsilon_cs= ', epsilon_cs*1e6)

print('\nopensees_epsba= ', opensees_epsba, ratio1)
print('opensees_epsbb= ', opensees_epsbb, ratio2)
print('opensees_epsda= ', opensees_epsda, ratio3)
print('opensees_epsdb= ', opensees_epsdb, ratio4)

print('\nt0_T_adj= ', t0_T_adj)
print('beta_h= ', beta_h)
print('beta_bc_fcm= ', beta_bc_fcm)
print('beta_bc= ', beta_bc)
print('beta_dc_fcm= ', beta_dc_fcm)
print('beta_rh_creep= ', beta_rh_creep)
print('beta_dc_t0= ', beta_dc_t0)
print('gamma_t0= ', gamma_t0)
print('beta_dc= ', beta_dc)
print('phi_bc= ', phi_bc)
print('phi_dc= ', phi_dc)
print('phi= ', phi)

print('\nopensees_phiba= ', opensees_phiba, ratio5)
print('opensees_phibb= ', opensees_phibb, ratio6)
print('opensees_phida= ', opensees_phida, ratio7)
print('opensees_phidb= ', opensees_phidb, ratio8)
print('opensees_cem= ', opensees_cem, ratio9)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio4)<1e-10) & (abs(ratio5)<1e-10) & (abs(ratio6)<1e-10) & (abs(ratio7)<1e-10) & (abs(ratio8)<1e-10) & (abs(ratio9)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
