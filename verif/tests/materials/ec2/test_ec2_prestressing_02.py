# -*- coding: utf-8 -*-
''' Test calculation of prestressing losses to the following clauses of
    EC2:2004:

    - Stress-strain curves for concrete and prestressing steel
      (Section 3.1.7, 3.3.6).
    - Veriﬁcation by the partial factor method - Design values (Section 2.4.2).
    - Prestressing force (Section 5.10.2, 5.10.3)

    Inspired on: "DCE-EN18 Creep and Shrinkage Calculation of a Rectangular 
    Prestressed Concrete CS" from the SOFiSTiK verification manual.
    URL: https://docs.sofistik.com/2023/en/verification/_static/verification/pdf/dce-en18.pdf
'''

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from scipy.constants import g
from materials.ec2 import EC2_materials
from materials.mc10 import MC10_materials
from materials.sections import section_properties as sp
from materials.prestressing import prestressed_concrete
from geom_utils import parabola
import numpy as np
from scipy.constants import g
from scipy.interpolate import interp1d
from misc_utils import log_messages as lmsg

# National annex.
nationalAnnex= 'Germany' # DIN EN 1992-1-1:2004 (German National Annex).

# Section geometry.
## Concrete.
concreteSection= sp.RectangularSection(name= 'concreteSection', b= 1.0, h= 1.0)
## Beam.
L= 20.0 # beam span.
## Duct.
ductDiam= 97e-3 # duct diameter.
ductArea= math.pi*(ductDiam/2.0)**2 # duct area.
eEnds= 0.0 # eccentricity of duct at both ends of the beam
eDuctMidspan=-0.396 # eccentricity of the duct at midspan [m]
ductHole= sp.Hole(name= 'ductHole', section= sp.CircularSection(name= 'duct', Rext= ductDiam/2.0, Rint=0.0))
## Prestressing
Ap= 28.5e-4 # prestressing steel area.
eTendonMidspan= -.3901 # See computation of Mp1 (first equation on page 8).
angl_Parab_XZ= 0.0 # angle between the vertical plane that contains the parabola and the plane XZ.
mu= 0.19 # coefficient of friction between the cables and their sheating
k= mu*0.0035 # wobble coefficient per meter length of cable (adjusted to match
             # the value of Np= 3653.0 kN fixed in the verification test).
             # IMPORTANT: we multiply by the friction coefficient
             # because of the way the Python expression in getLossFriction is written:
             # getLossFriction:  (1-math.exp(-mu*cum_angl[i]-k*cum_len[i]))
             # Eurocode 2: expression (5.45): (1-math.exp(-mu*(cum_angl[i]+k*cum_len[i]))
## Tendon definition, layout and friction losses
n_points_rough= 5 # number of points provided to the interpolation algorithm
n_points_fine= 101 # number of points interpolated
a,b,c= parabola.fit_parabola(x=np.array([0,L/2.0,L]), y=np.array([eEnds,eDuctMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough= parabola.eq_points_parabola(0,L,n_points_rough,a,b,c,angl_Parab_XZ)
### Tendon geometry.
tendon= prestressed_concrete.PrestressTendon([])
tendon.roughCoordMtr= np.array([x_parab_rough,y_parab_rough,z_parab_rough])
## Anchorage slip
deltaL= 2.722e-3 # anchorage draw-in ((adjusted to match the value of
                 # Np= 3653.0 kN fixed in the verification test).
                 
# Interpolated 3D spline 
tendon.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)

# Materials.
concrete= EC2_materials.C35 # concrete.
prestressingSteel= EC2_materials.Y1770 # prestressig steel.

ratioAlphaEp= prestressingSteel.Es/concrete.getEcm()
tendonEquivalentArea= (ratioAlphaEp-1)*Ap
tendonEquivalentRadius= math.sqrt(tendonEquivalentArea/math.pi)
tendonEquivalentSection= sp.CircularSection(name= 'tendonEquivalentSection', Rext= tendonEquivalentRadius, Rint=0.0)

# Prestressing.
sigma_p_max= prestressingSteel.getMaximumStressingStress(nationalAnnex= nationalAnnex)
## Losses of prestressing due to friction
frictionLosses= tendon.getLossFriction(coefFric= mu, k=k, sigmaP0_extr1= 0.973*sigma_p_max, sigmaP0_extr2= 0.0)
## Losses of prestressing due to anchorage slip (loss due to friction must be
## previously calculated
anchorageSlipLosses= tendon.getLossAnchor(Ep= prestressingSteel.Es, anc_slip_extr1= deltaL, anc_slip_extr2=0.0)
## effective length of tendon affected by the anchorage slip in extremity 1 [m]
Laffected= tendon.projXYcoordZeroAnchLoss[0]
initialStresses= tendon.stressAfterLossFriction-anchorageSlipLosses #loss along tendon
initialStressFunc= interp1d(tendon.fineProjXYcoord, initialStresses)
midSpanInitialStress= initialStressFunc(10.0)

# Load actions.
## Self weight.
selfWeightLoad= concrete.density()*concreteSection.A()*10#*g
Mg= selfWeightLoad*L**2/8.0 # bending moment at mid-span.
## Prestressing axial load.
Np= midSpanInitialStress*Ap # initial prestressing force at mid-span.

# Prestress and self-weight at construction stage section 0 (P+G cs0)
## Construction stage 0 section:
positions= [geom.Pos2d(0,0), geom.Pos2d(0,eDuctMidspan)]
sectionList= list()
for p, section in zip(positions, [concreteSection, ductHole]):
    sectionList.append([p, section])
cs0Section= sp.CompoundSection('cs0Section',sectionList, Iw= 0.0)
## Bending moment at mid-span.
zp= eTendonMidspan-cs0Section.yCenterOfMass()
Mp= Np*zp
My= Mg+Mp
## Stress at the top of the mid-span section in case II.
yTop= concreteSection.h/2.0+cs0Section.yCenterOfMass()
sigma_c_qp= -Np/cs0Section.A()+My/cs0Section.Iz()*yTop
ratio1= abs(sigma_c_qp+4.82e6)/4.82e6

# Calculation of creep and shrinkage at x= 10.0 m midspan:
t0= 28 # minimun age of concrete for loading.
ts= 0 # age of concrete at start of drying shrinkage.
t= 1000000+28 # age of concrete at the moment considered.
u= 4
RH= 80
h0= 2*concreteSection.A()/u
# Shrinkage strain
## Drying shrinkage strain before prestressing.
epsilon_cd_t0= concrete.getShrEpscd(t= t0,ts= ts, RH= RH, h0= h0)
## Drying shrinkage strain at time t.
epsilon_cd_t= concrete.getShrEpscd(t= t, ts= ts, RH= RH, h0= h0) 
## Drying shrinkage strain "seen" by the prestressing load.
epsilon_cd= epsilon_cd_t-epsilon_cd_t0
ratio2= abs(epsilon_cd+1.668e-4)/1.668e-4
## Autogenous shrinkage strain "seen" by the prestressing load.
epsilon_ca= concrete.getShrEpsca(t= t, t0= t0) # autogenous shrinkage strain.
ratio3= abs(epsilon_ca+2.169e-5)/2.169e-5
## Total shrinkage "seen" by the prestressing load.
epsilon_cs= epsilon_cd+epsilon_ca
# Creep
gamma_t0= MC10_materials.gamma_t0(t0= t0)
fi_t_t0= concrete.getCreepFitt0(t= t, t0= t0, RH= RH, h0= h0,gamma_t0= gamma_t0)
# According to EN, the creep value is related to the tangent Young’s
# modulus Ec, where Ec being deﬁned as 1.05*Ecm. To account for
# this, SOFiSTiK adopts this scaling for the computed creep coefﬁcient.
fi_t_t0/=1.05

# Time dependent losses of prestress according to 5.10.6
delta_sigma_pr= 0.0 # reduction of stress due to relaxation (Δσpr) is ignored.

# Ap: cross-sectional area of the tendon(s).
# concrete: concrete material.
# Ac: area of the concrete section.
# Ic: second moment of area of the concrete section.
# z_cp: distance between the centre of gravity of the concrete
#                     section and the tendons.
# epsilon_cs: estimated shrinkage strain according to 3.1.4(6).
# fi_t_t0: creep coefficient at a time t and load application 
#                        at time t0.
# delta_sigma_pr: absolute value of the variation of stress in 
#                 the tendons at location x, at time t, due to 
#                 the relaxation of the prestressing steel.
# sigma_c_qp: stress in the concrete adjacent to the tendons, due
#             to self-weight and initial prestress and other 
#             quasi-permanent actions where relevant.

deltaSigma= prestressingSteel.getTimeDependentStressLosses(Ap= Ap, concrete= concrete, Ac= concreteSection.A(), Ic= concreteSection.Iz(), z_cp= eTendonMidspan, epsilon_cs= epsilon_cs, fi_t_t0= fi_t_t0, delta_sigma_pr= delta_sigma_pr, sigma_c_qp= sigma_c_qp)
deltaP= deltaSigma*Ap
ratio4= abs(deltaP+195.11e3)/195.11e3

'''
print('\n1) Calculating the shrinkage displacements «seen» by the prestressing load.')
print('   Drying shrinkage strain: epsilon_cd= '+'{:.2f}'.format(epsilon_cd*1e4)+'e-4', ' ratio2= ', ratio2)
print('   Autogeonous shrinkage strain: epsilon_ca= '+'{:.2f}'.format(epsilon_ca*1e5)+'e-5', ' ratio3= ', ratio3)
print('   Total shrinkage strain: epsilon_cs= '+'{:.2f}'.format(epsilon_cs*1e5)+'e-5')
print('\n2) Calculating creep.')
print('   Creep coefficient fi(t, t0)= '+'{:.3f}'.format(fi_t_t0))
print('\n3) Calculating losses due to creep and shrinkage (relaxation ignored).')
print('   Stress losses due to creep and shrinkage delta_sigma= '+'{:.2f}'.format(deltaSigma/1e6)+' MPa')
print('   Force losses due to creep and shrinkage deltaP= '+'{:.2f}'.format(deltaP/1e3)+' kN', ' ratio4= ', ratio4)
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-3) and (ratio3<1e-4) and (ratio3<1e-4) and (ratio4<1e-3)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
