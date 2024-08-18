# -*- coding: utf-8 -*-
''' Test calculation of stress relaxation of prestressing steel according to
    the following clauses of EC2:2004:

    - Properties (Section 3.3.2)
    - Annex D: Detailed calculation method for prestressing steel relaxation losses (Section D.1).
    - Strength (Section 3.3.3)

    Inspired on: "DCE-EN22 Stress Relaxation of Prestressing Steel - DIN EN 
    1992-1-1" from the SOFiSTiK verification manual.
    URL: https://docs.sofistik.com/2023/en/verification/_static/verification/pdf/dce-en22.pdf
'''

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.sections import section_properties as sp
from model.geometry import geom_utils
import numpy as np
from scipy.constants import g
from scipy.interpolate import interp1d
from materials.prestressing import prestressed_concrete
from misc_utils import log_messages as lmsg
from materials.ec2 import EC2_materials
import geom

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

## Prestressing axial load.
P0= midSpanInitialStress*Ap # initial prestressing force at mid-span.
sigma_p_0= midSpanInitialStress

#       ___ _           _                      
#      / __| |_ ___ ___| |                     
#      \__ \  _/ -_) -_) |                     
#      |___/\__\___\___|_|      _   _          
#      | _ \___| |__ ___ ____ _| |_(_)___ _ _  
#      |   / -_) / _` \ \ / _` |  _| / _ \ ' \ 
#      |_|_\___|_\__,_/_\_\__,_|\__|_\___/_||_|
# Calculating the prestressing losses due steel relaxation.
mu= prestressingSteel.getMu(sigma_pi= sigma_p_0)
delta_sigma_pr= prestressingSteel.getRelaxationLoss(sigma_pi= sigma_p_0, t= 1e3, ro1000= None)
ratio1= abs(delta_sigma_pr-15.388e6)/15.388e6

# Time dependent losses of prestress according to 5.10.6
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

# Creep and shrinkage is not taken into account therefore we have:
fi_t_t0= 0
sigma_c_qp= 1 # (irrelevant)
epsilon_cs= 0
delta_sigma_pr/=0.8 # Difference between formula in DIN 1045-1 8.7.3 (6) and
                    # equation (5.46) of EN 1992-1-1:2004
## Construction stage 0 section:
positions= [geom.Pos2d(0,0), geom.Pos2d(0,eDuctMidspan)]
sectionList= list()
for p, section in zip(positions, [concreteSection, ductHole]):
    sectionList.append([p, section])
cs0Section= sp.CompoundSection('cs0Section',sectionList, Iw= 0.0)
deltaStress= prestressingSteel.getTimeDependentStressLosses(Ap= Ap, concrete= concrete, Ac= cs0Section.A(), Ic= cs0Section.Iz(), z_cp= eTendonMidspan, epsilon_cs= epsilon_cs, fi_t_t0= fi_t_t0, delta_sigma_pr= delta_sigma_pr, sigma_c_qp= sigma_c_qp)
## Recompute sigma_p_0
sigma_p_0-= 0.3*deltaStress
## And iterate:
delta_sigma_pr= prestressingSteel.getRelaxationLoss(sigma_pi= sigma_p_0, t= 1e3, ro1000= None)
ratio2= abs(delta_sigma_pr-15.043e6)/15.043e6
## We ignore the tendon strain terms in the SOFiSTiK example.
deltaPpr= delta_sigma_pr*Ap
refDeltaPpr= 15.043e6*Ap
ratio3= abs(deltaPpr-refDeltaPpr)/refDeltaPpr
deltaPprRatio= deltaPpr/P0

'''
print("P0= ", P0/1e3)
print("sigma_p_0= ", sigma_p_0/1e6)
print("mu= ", mu)
print("delta_sigma_pr= ", delta_sigma_pr/1e6, ratio2)
print("deltaStress= ", deltaStress/1e6)
print("deltaPpr= ", deltaPpr/1e3, ratio3)
print("deltaPprRatio= ", deltaPprRatio*100)
'''

import os
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-2) and (ratio3<1e-2)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
