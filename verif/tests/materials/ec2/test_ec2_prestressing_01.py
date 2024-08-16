# -*- coding: utf-8 -*-
''' Test calculation of prestressing losses to the following clauses of
    EC2:2004:

    - Stress-strain curves for concrete and prestressing steel
      (Section 3.1.7, 3.3.6).
    - Veriﬁcation by the partial factor method - Design values (Section 2.4.2).
    - Prestressing force (Section 5.10.2, 5.10.3)

    Inspired on: "DCE-EN17 Stress Calculation at a Rectangular Prestressed 
    Concrete CS" from the SOFiSTiK verification manual.
    URL: https://docs.sofistik.com/2023/en/verification/_static/verification/pdf/dce-en17.pdf
'''

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from materials.ec2 import EC2_materials
from materials.sections import section_properties as sp
from materials.prestressing import prestressed_concrete
from model.geometry import geom_utils
import numpy as np
from scipy.constants import g
from scipy.interpolate import interp1d
from misc_utils import log_messages as lmsg

# National annex.
nationalAnnex= 'Germany' # DIN EN 1992-1-1:2004 (German National Annex).

# Section geometry.
## Concrete section.
concreteSection= sp.RectangularSection(name= 'concreteSection', b= 1.0, h= 1.0)
d= 0.95 # effective depth.
## Beam.
L= 20.0 # beam span.
# Duct.
ductDiam= 97e-3 # duct diameter.
ductArea= math.pi*(ductDiam/2.0)**2 # duct area.
eEnds= 0.0 # eccentricity of duct at both ends of the beam
eDuctMidspan=-0.396 # eccentricity of the duct at midspan [m]
ductHole= sp.Hole(name= 'ductHole', section= sp.CircularSection(name= 'duct', Rext= ductDiam/2.0, Rint=0.0))

# Prestressing
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
a,b,c=geom_utils.fit_parabola(x=np.array([0,L/2.0,L]), y=np.array([eEnds,eDuctMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough= geom_utils.eq_points_parabola(0,L,n_points_rough,a,b,c,angl_Parab_XZ)
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
Ac_ideal= concreteSection.A()+tendonEquivalentSection.A()

# 5.10.2.1 (1)P: Prestress force during tensioning - Maximum stressing force.
sigma_p_max= prestressingSteel.getMaximumStressingStress(nationalAnnex= nationalAnnex)
Pmax= prestressingSteel.getMaximumStressingForce(Ap= Ap, nationalAnnex= nationalAnnex)
ratio1= abs(Pmax-3898.8e3)/3898.8e3

# 5.10.3 (2): Prestress force
sigma_p_m_0= prestressingSteel.getMaximumInitialStress()
Pm0= prestressingSteel.getMaximumInitialForce(Ap= Ap)
ratio2= abs(Pm0-3682.2e3)/3682.2e3

# variation of initial prestress force along a post-tensioned member
## Losses of prestressing due to friction
frictionLosses= tendon.getLossFriction(coefFric= mu, k=k, sigmaP0_extr1= 0.973*sigma_p_max, sigmaP0_extr2= 0.0)
## Losses of prestressing due to anchorage slip (loss due to friction must be
## previously calculated
anchorageSlipLosses= tendon.getLossAnchor(Ep= prestressingSteel.Es, anc_slip_extr1= deltaL, anc_slip_extr2=0.0)
## effective length of tendon affected by the anchorage slip in extremity 1 [m]
Laffected= tendon.projXYcoordZeroAnchLoss[0]
initialStresses= tendon.stressAfterLossFriction-anchorageSlipLosses #loss along tendon
xi= tendon.fineProjXYcoord
for x, stress in zip(xi, initialStresses):
    ratio= (stress-sigma_p_m_0)/sigma_p_m_0
    if(ratio>.01):
        errMsg= 'Maximum stress ('+'{:.1f}'.format(sigma_p_m_0/1e6)+' MPa) exceeded at point x= '+'{:.1f}'.format(x)
        errMsg+= '; stress value: '+'{:.1f}'.format(stress/1e6)+' {:.3f}'.format(ratio)
        lmsg.error(errMsg)
    
initialStressFunc= interp1d(xi, initialStresses)
midSpanInitialStress= initialStressFunc(10.0)


# Load actions.
selfWeightLoad= concrete.density()*concreteSection.A()*10#*g
Mg= selfWeightLoad*L**2/8.0 # bending moment at mid-span.
Np= midSpanInitialStress*Ap # initial prestressing force at mid-span.
ratio3= abs(Np-3653e3)/3653e3

# Case I: Prestress at construction stage section 0 (P cs0)
## Construction stage 0 section:
positions= [geom.Pos2d(0,0), geom.Pos2d(0,eDuctMidspan)]
sectionList= list()
for p, section in zip(positions, [concreteSection, ductHole]):
    sectionList.append([p, section])
cs0Section= sp.CompoundSection('cs0Section',sectionList, Iw= 0.0)
### Center of gravity of the concrete section with the hollow:
zsI= cs0Section.yCenterOfMass()
ratio4= abs(zsI-0.002948)/0.002948
### Initial bending moment at mid-span.
zp= eTendonMidspan-zsI
Mp= Np*zp
ratio5= abs(Mp+1435.91e3)/1435.91e3
### Stress at the top of the mid-span section in case I.
yTopI= concreteSection.h/2.0+cs0Section.yCenterOfMass()
sigma_cbI= -Np/cs0Section.A()+Mp/cs0Section.Iz()*yTopI
ratio6= abs(sigma_cbI+12.47e6)/12.47e6

# Case II: Prestress and self-weight at construction stage section 0 (P+G cs0)
MyII= Mg+Mp
ratio7= abs(MyII+185.91e3)/185.91e3
### Stress at the top of the mid-span section in case II.
yTopII= concreteSection.h/2.0+cs0Section.yCenterOfMass()
sigma_cbII= -Np/cs0Section.A()+MyII/cs0Section.Iz()*yTopII
ratio8= abs(sigma_cbII+4.82e6)/4.82e6

## Construction stage 1 section:
sectionList[1][1]= tendonEquivalentSection
cs1Section= sp.CompoundSection('cs1Section',sectionList, Iw= 0.0)

# Case III: Prestress and at construction stage section 1 (P cs1)
zsIII= cs1Section.yCenterOfMass()
ratio9= abs(zsIII+0.00518)/0.00518
### Bending moment at mid-span.
zpIII= eTendonMidspan-zsIII
MpIII= Np*zpIII
ratio10= abs(MpIII+1406.11e3)/1406.11e3
### Stress at the top of the mid-span section in case III.
yTopIII= concreteSection.h/2.0+cs1Section.yCenterOfMass()
sigma_cbIII=  -Np/cs1Section.A()+MpIII/cs1Section.Iz()*yTopIII
ratio11= abs(sigma_cbIII+11.76e6)/11.76e6

# Case IV: Prestress and self-weight at construction stage section 1 (P+G cs1)
MyIV= Mg+MpIII
ratio12= abs(MyIV+156.11e3)/156.11e3
### Stress at the top of the mid-span section in case IV.
yTopIV= concreteSection.h/2.0+cs1Section.yCenterOfMass()
sigma_cbIV= -Np/cs0Section.A()+MyIV/cs1Section.Iz()*yTopIV
ratio13= abs(sigma_cbIV+4.51e6)/4.51e6

'''
print('ratioAlphaEp= '+'{:.2f}'.format(ratioAlphaEp))
print('Net concrete area: Ac,net= '+'{:.4f}'.format(cs0Section.A())+' m2')
print('Ideal concrete area: Ac,ideal= '+'{:.4f}'.format(Ac_ideal)+' m2')

print('\nMaximum prestressing stress: sigma_p_max= '+'{:.1f}'.format(sigma_p_max/1e6)+' MPa')
print('Maximum prestressing force: Pmax= '+'{:.1f}'.format(Pmax/1e3)+' kN', 'ratio1= ', ratio1)
print('Maximum initial stress: sigma_p_m_0= '+'{:.1f}'.format(sigma_p_m_0/1e6)+' MPa')
print('Maximum initial force: Pm0= '+'{:.1f}'.format(Pm0/1e3)+' kN', 'ratio2= ', ratio2)
print('Initial stress at mid-span: sigma_p_x10= '+'{:.1f}'.format(midSpanInitialStress/1e6)+' kN')
print('Initial prestressing force at mid-span: Np= '+'{:.1f}'.format(Np/1e3)+' kN', 'ratio3= ', ratio3)
print('\nCase I')
print('  Prestressing force at mid-span: Np= '+'{:.1f}'.format(Np/1e3)+' kN')
print('  Bending moment at mid-span: Mp= '+'{:.1f}'.format(Mp/1e3)+' kN m', 'ratio5= ', ratio5)
print('  Stress at the top of mid-span section: sigma_cb= '+'{:.2f}'.format(sigma_cbI/1e6)+' MPa', 'ratio6= ', ratio6)
print('\nCase II')
print('  Prestressing force at mid-span: Np= '+'{:.1f}'.format(Np/1e3)+' kN')
print('  Bending moment at mid-span: My= '+'{:.1f}'.format(MyII/1e3)+' kN m', 'ratio7= ', ratio7)
print('  Stress at the top of mid-span section: sigma_cb= '+'{:.2f}'.format(sigma_cbII/1e6)+' MPa', 'ratio8= ', ratio8)
print('\nCase III')
print('  Prestressing force at mid-span: Np= '+'{:.1f}'.format(Np/1e3)+' kN')
print('  Bending moment at mid-span: My= '+'{:.1f}'.format(MpIII/1e3)+' kN m', 'ratio10= ', ratio10)
print('  Stress at the top of mid-span section: sigma_cb= '+'{:.2f}'.format(sigma_cbIII/1e6)+' MPa', 'ratio11= ', ratio11)
print('\nCase IV')
print('  Prestressing force at mid-span: Np= '+'{:.1f}'.format(Np/1e3)+' kN')
print('  Bending moment at mid-span: My= '+'{:.1f}'.format(MyIV/1e3)+' kN m', 'ratio12= ', ratio12)
print('  Stress at the top of mid-span section: sigma_cb= '+'{:.2f}'.format(sigma_cbIV/1e6)+' MPa', 'ratio13= ', ratio13)
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-4) and (ratio3<1e-5) and (ratio5<1e-3) and (ratio6<1e-3) and (ratio7<1e-3) and (ratio8<1e-3) and (ratio9<5e-2) and (ratio10<1e-3)) and (ratio11<1e-2) and (ratio12<1e-2) and (ratio13<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# ### FIGURES
# import matplotlib.pyplot as plt
# plt.plot(xi, initialStresses*(1/sigma_p_max))
# plt.show()
