# -*- coding: utf-8 -*-
'''Calculation of creep and shrinkage on a prestressed concrete cs,
subject to horizontal prestressing force.

Data taken from SOFiSTiK 2020 | Benchmark No. 21 "Real Creep and Shrinkage 
Calculation of a T-Beam Prestressed CS"
'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials
from materials.mc10 import MC10_materials
from materials import concrete_base

time_steps= [100, 300, 11250]

t0= 7 # minimum age of concrete for loading.
ts= 3 # age of concrete at start of drying shrinkage (end of curing).

# Type of concrete used in the T beam
concrete= EC2_materials.EC2Concrete("C35/45",-35e6,1.5)
concrete.cemType= 'N' # class N cement
RH= 80 # Relative humidity (%).

# Geometric properties.
h= 1.20 # beam depth.
b_eff= 2.8 # effective width.
hf= 0.4 # flange height.
bw= 0.6 # web width (error in table 1; see line 18 of real_creep_shrinkage.dat).
hw= h-hf
A= b_eff*hf+hw*bw # area of the concrete slab (m2)
u= bw+2*h+2*b_eff-bw # perimeter exposed to drying (m)
h0=2*A/u  #notional size of the member h0.

prestressingSteel= concrete_base.PrestressingSteel('Y1770',fpk= 1171e6,fmax= 1770e6)
As= 178.568e-4 # Prestressing steel area.
Es= 200e9
Ac= A-As # Concrete area.

L= 20.0 # beam span (simply supported).

# Prestressing load.
NEd= -900e3 # at x= 1.0 (middle of the span).
## Computation of stresses at x= 10.0 midspan:
Ecm= concrete.getEcm()
fcm= concrete.getFcm()
# Computation of the stresses at midspan.
sigma_c= NEd/A # concrete stress between t= 100 and t= 300.

# 1) Calculating the shrinkage displacements before loading.
# (self weight per length is neglected).

t= time_steps[0] # age of concrete at the moment considered

## Calculating creep:
epsilon_cc= 0.0 # Because sigma_c = 0 (before loading), creep deformation is neglected and εcc = 0
## Calculating shrinkage
### Drying shrinkage.
epsilon_cd_7= concrete.getShrEpscd(t= t0,ts= ts, RH= RH, h0= h0) # Drying shrinkage strain
epsilon_cd_100= concrete.getShrEpscd(t= t,ts= ts, RH= RH, h0= h0) # Drying shrinkage strain
epsilon_cd_93= epsilon_cd_100-epsilon_cd_7
ratio1= abs(epsilon_cd_93+4.04e-5)/4.04e-5
### Autogenours shrinkage.
epsilon_ca_100= concrete.getShrEpsca(t= t, t0= t0) # autogenous shrinkage strain.
### Total shrinkage.
epsilon_cs_100= concrete.getShrEpscs(t,ts,RH,h0, t0= t0) # Total shrinkage
epsilon_cs_93= epsilon_cs_100-epsilon_cd_7 # Total shrinkage
ratio2= abs(epsilon_cs_93+6.881e-5)/6.881e-5
### Calculate displacement at midspan.
Delta_l1_cs= epsilon_cs_93 * L/2.0

# 2) Calculate displacement when loading occurs at time t1 = 100 days at
# x= 10.0 m midspan.
Ecs= Ecm+As/Ac*Es # calculated ”ideal” cross section modulus of elasticity
                  # for concrete and reinforcement steel.
epsilon_t_100= sigma_c/Ecs
### Calculate displacement at midspan.
Delta_l2= epsilon_t_100 * L/2.0

# 3) Calculating the displacement (creep and shrinkage) at time before the
# deactivation of the prestressing load (t2 ≈ 300 and t2 < 300 days)
t0= time_steps[0]
t= time_steps[1]

## Calculate shrinkage.
## Calculating shrinkage
### Drying shrinkage.
epsilon_cd_300= concrete.getShrEpscd(t= t,ts= ts, RH= RH, h0= h0) # Drying shrinkage strain
epsilon_cd_t300_t100= epsilon_cd_300-epsilon_cd_100
### Autogenours shrinkage.
epsilon_ca_t300= concrete.getShrEpsca(t= t)
epsilon_ca_t300_t100= concrete.getShrEpsca(t= t, t0= t0) # autogenous shrinkage strain.
### Total shrinkage.
epsilon_cs_300= concrete.getShrEpscs(t,ts,RH,h0, t0= t0) # Total shrinkage
epsilon_cs_t300_t100= epsilon_cs_300-epsilon_cd_100 # Total shrinkage
ratio3= abs(epsilon_cs_t300_t100+5.218e-5)/5.218e-5

### Displacement increment.
Delta_l3_cs= epsilon_cs_t300_t100 * L/2.0


## Calculating creep.
gamma_t0= MC10_materials.gamma_t0(t0= t0)
betactt0= concrete.getCreepBetactt0(t,t0,RH,h0,gamma_t0= gamma_t0)
                                                     # coefficient to describe
                                                     # the development of creep
                                                     # with time after loading.
fi_t300_t100= concrete.getCreepFitt0(t,t0,RH,h0,gamma_t0= gamma_t0)
ratio4= abs(fi_t300_t100-0.57)/0.57
# According to EN, the creep value is related to the tangent Young’s
# modulus Ec, where Ec being deﬁned as 1.05*Ecm. To account for
# this, SOFiSTiK adopts this scaling for the computed creep coefﬁcient.
fi_eff_t300_t100= fi_t300_t100/1.05

### Calculating the displacement increment:
epsilon_cc_t300_t100= fi_t300_t100*sigma_c/Ecs
Delta_l3_cc= epsilon_cc_t300_t100 * L/2.0

# 4) Calculating the displacement at time when the load is removed (t2 ≈ 300 and t2 > 300 days).
Delta_l4= -Delta_l2

# 5) Calculating the displacement at time t3 = 30 years.
t0= time_steps[1]
t= time_steps[2]
### Drying shrinkage.
epsilon_cd_30y= concrete.getShrEpscd(t= t,ts= ts, RH= RH, h0= h0) # Drying shrinkage strain
epsilon_cd_t30y_t300= epsilon_cd_30y-epsilon_cd_300
### Autogenours shrinkage.
epsilon_ca_t30y_t300= concrete.getShrEpsca(t= t, t0= t0) # autogenous shrinkage strain.
### Total shrinkage.
epsilon_cs_30y= concrete.getShrEpscs(t,ts,RH,h0, t0= t0) # Total shrinkage
epsilon_cs_t30y_t300= epsilon_cs_30y-epsilon_cd_300 # Total shrinkage
ratio5= abs(epsilon_cs_t30y_t300+9.212e-5)/9.212e-5
### Displacement increment.
Delta_l5_cs= epsilon_cs_t30y_t300 * L/2.0

## Calculating creep.
gamma_t0= MC10_materials.gamma_t0(t0= t0)
betactt0= concrete.getCreepBetactt0(t,t0,RH,h0,gamma_t0= gamma_t0)
                                                     # coefficient to describe
                                                     # the development of creep
                                                     # with time after loading.
fi_t30y_t300= concrete.getCreepFitt0(t,t0,RH,h0,gamma_t0= gamma_t0)
ratio6= abs(fi_t30y_t300-0.91)/0.91
# According to EN, the creep value is related to the tangent Young’s
# modulus Ec, where Ec being deﬁned as 1.05*Ecm. To account for
# this, SOFiSTiK adopts this scaling for the computed creep coefﬁcient.
fi_eff_t30y_t300= fi_t30y_t300/1.05

### Calculating the displacement increment:
epsilon_cc_t30y_t300= fi_t30y_t300*-sigma_c/Ecs
Delta_l5_cc= epsilon_cc_t30y_t300 * L/2.0
ratio7= abs(Delta_l5_cc-.1408e-3)/.1408e-3

totalDisp= Delta_l1_cs+ Delta_l2+ Delta_l3_cs+ Delta_l3_cc+Delta_l4+Delta_l5_cs+Delta_l5_cc
ratio8= abs(totalDisp+2.08e-3)/2.08e-3


'''
print('A= '+'{:.1f}'.format(A)+' m2')
print('h0= '+'{:.2f}'.format(h0)+' m')
print('Ecm= '+'{:.1f}'.format(Ecm/1e6)+' MPa')
print('fcm= '+'{:.1f}'.format(fcm/1e6)+' MPa')
print('sigma_c= '+'{:.4f}'.format(sigma_c/1e6)+' MPa')
print('\n1) Calculating the shrinkage displacements before loading.')
print('   Drying shrinkage strain: epsilon_cd_100= '+'{:.2f}'.format(epsilon_cd_93*1e5)+'e-5', ratio1)
print('   Autogenous shrinkage strain: epsilon_ca_100= '+'{:.2f}'.format(epsilon_ca_100*1e5)+'e-5')
print('   Total shrinkage strain: epsilon_cs_100= '+'{:.3f}'.format(epsilon_cs_93*1e5)+'e-5', ratio2)
print('   Displacement increment at midspan: Delta_l1_cs= '+'{:.4f}'.format(Delta_l1_cs*1e3)+' mm')
print('\n2) Calculate displacement when loading occurs at time t1 = 100 days at x= 10.0 m midspan.')
print('    Concrete average modulus of elasticity: Ecm= '+'{:.2f}'.format(Ecm/1e6)+' MPa')
print('    Prestressing steel modulus of elasticity: Es= '+'{:.2f}'.format(Es/1e6)+' MPa')
print('    Cross section modulus of elasticity for concrete and reinforcement steel: Ecs= '+'{:.2f}'.format(Ecs/1e6)+' MPa')
print('    Strain: epsilon_t_100= '+'{:.2f}'.format(epsilon_t_100*1e5)+'e-5')
print('    Displacement increment at midspan: Delta_l2= '+'{:.3f}'.format(Delta_l2*1e3)+' mm')
print('\n3) Calculating the displacement (creep and shrinkage) at time before the deactivation of the prestressing load (t2 ≈ 300 and t2 < 300 days)')
print('   Drying shrinkage strain: epsilon_cd_300= '+'{:.2f}'.format(epsilon_cd_t300_t100*1e5)+'e-5')
print('   Autogenous shrinkage strain: epsilon_ca_300= '+'{:.6f}'.format(epsilon_ca_t300_t100*1e6)+'e-6')
print('   Total shrinkage strain: epsilon_cs_300= '+'{:.3f}'.format(epsilon_cs_t300_t100*1e5)+'e-5', ratio3)
print('   Displacement increment at midspan due to shrinkage: Delta_l3_cs= '+'{:.4f}'.format(Delta_l3_cs*1e3)+' mm')
print('   Creep coefficient: fi_t300_t100= '+'{:.2f}'.format(fi_t300_t100), ratio4)
print('   Effective creep coefficient: fi_eff_t300_t100= '+'{:.4f}'.format(fi_eff_t300_t100))
print('   Strain due to creep: fi_t300_t100= '+'{:.5f}'.format(epsilon_cc_t300_t100*1e6)+'e-6')
print('   Displacement increment at midspan due to creep: Delta_l3_cc= '+'{:.5f}'.format(Delta_l3_cc*1e3)+' mm')
print('4) Calculating the displacement at time when the load is removed (t2 ≈ 300 and t2 > 300 days).')
print('    Displacement increment at midspan: Delta_l4= '+'{:.3f}'.format(Delta_l4*1e3)+' mm')
print('\n5) Calculating the displacement at time t3 = 30 years.')
print('   Drying shrinkage strain: epsilon_cd_30y= '+'{:.2f}'.format(epsilon_cd_t30y_t300*1e5)+'e-5')
print('   Autogenous shrinkage strain: epsilon_ca_30y= '+'{:.6f}'.format(epsilon_ca_t30y_t300*1e6)+'e-6')
print('   Total shrinkage strain: epsilon_cs_30y= '+'{:.3f}'.format(epsilon_cs_t30y_t300*1e5)+'e-5', ratio5)
print('   Displacement increment at midspan due to shrinkage: Delta_l5_cs= '+'{:.4f}'.format(Delta_l5_cs*1e3)+' mm')
print('   Creep coefficient: fi_t30y_t300= '+'{:.2f}'.format(fi_t30y_t300), ratio6)
print('   Effective creep coefficient: fi_eff_t30y_t300= '+'{:.4f}'.format(fi_eff_t30y_t300))
print('   Strain due to creep: fi_t30y_t300= '+'{:.5f}'.format(epsilon_cc_t30y_t300*1e6)+'e-6')
print('   Displacement increment at midspan due to creep: Delta_l5_cc= '+'{:.5f}'.format(Delta_l5_cc*1e3)+' mm', ratio7)
print('Total displacement at midspan due to creep: Delta_l5_cc= '+'{:.5f}'.format(totalDisp*1e3)+' mm', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-2) and (ratio2<1e-2) and (ratio3<1e-2) and (ratio4<1e-2) and (ratio5<1e-2) and (ratio6<1e-2) and (ratio7<1e-2) and (ratio8<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

