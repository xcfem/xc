# -*- coding: utf-8 -*-
from __future__ import division
'''Test for checking that the results of prestressing losses are independent of
the unit system chosen.
'''
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"


import numpy as np
from scipy import interpolate
from scipy.spatial import distance
import math
from materials.prestressing import prestressed_concrete as presconc

#Geometry 
lBeam_mm=20e3    #beam span [mm]
lBeam_m=20   #beam span [m]
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan_mm=-558   #eccentricity of cables at midspan [mm]
eMidspan_m=-0.558   #eccentricity of cables at midspan [m]
angl_Parab_XZ=math.pi/4 #angle between the vertical plane that contains the
                        #parabola and the plane XZ
#Material
Ep_mm=195e3       #elastic modulus of prestressing steel [MPa]
#Ep_m=0.195       #elastic modulus of prestressing steel [Pa]
Ep_m=195e9       #elastic modulus of prestressing steel [Pa]
#Prestressing process
mu=0.25        #coefficient of friction between the cables and their sheating
k_mm=0.0017*1e-3  #wobble coefficient per millimeter length of cable [1/mm]
k_m=0.0017  #wobble coefficient per meter length of cable [1/m]
sigmap0max_mm=1239 #Initial stress of cable [MPa]
sigmap0max_m=1239e6 #Initial stress of cable [Pa]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Anchorage slip
deltaL_mm=5            #anchorage draw-in (provided by manufacturer) [mm]
deltaL_m=5e-3          #anchorage draw-in (provided by manufacturer) [m]


#Exact parabola
from model.geometry import geom_utils
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam_mm/2.0,lBeam_mm]), y=np.array([eEnds,eMidspan_mm,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam_mm,n_points_rough,a,b,c,angl_Parab_XZ)

#Tendon [mm] definition, layout and friction losses
tendon_mm=presconc.PrestressTendon([])
tendon_mm.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon_mm.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
tendon_mm.calcLossFriction(coefFric=mu,k=k_mm,sigmaP0_extr1=sigmap0max_mm,sigmaP0_extr2=0.0)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
tendon_mm.calcLossAnchor(Ep=Ep_mm,anc_slip_extr1=deltaL_mm,anc_slip_extr2=0.0)

Laffected_mm=tendon_mm.projXYcoordZeroAnchLoss[0] # effective length of tendon affected by
                           #the anchorage slip in extremity 1 [mm]
#Tendon [m] definition, layout and friction losses
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam_m/2.0,lBeam_m]), y=np.array([eEnds,eMidspan_m,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam_m,n_points_rough,a,b,c,angl_Parab_XZ)

tendon_m=presconc.PrestressTendon([])
tendon_m.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon_m.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)
# Losses of prestressing due to friction
tendon_m.calcLossFriction(coefFric=mu,k=k_m,sigmaP0_extr1=sigmap0max_m,sigmaP0_extr2=0.0)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
tendon_m.calcLossAnchor(Ep=Ep_m,anc_slip_extr1=deltaL_m,anc_slip_extr2=0.0)

Laffected_m=tendon_m.projXYcoordZeroAnchLoss[0] # effective length of tendon affected by
                           #the anchorage slip in extremity 1 [m]


#Geometric calculations
ratio1=(tendon_mm.fineCoordMtr-tendon_m.fineCoordMtr*1000).mean()
ratio2=(tendon_mm.fineDerivMtr-tendon_m.fineDerivMtr*1000).mean()
ratio3=(tendon_mm.fineScoord-tendon_m.fineScoord*1000).mean()
ratio4=(np.array(tendon_mm.getAngleSequence())-np.array(tendon_m.getAngleSequence())).mean()
ratio5=(tendon_mm.getCumAngle()-tendon_m.getCumAngle()).mean()
ratio6=(tendon_mm.getCumLength()-tendon_m.getCumLength()*1000).mean()
ratio7=(np.array(tendon_mm.getLengthSequence())-np.array(tendon_m.getLengthSequence())*1000).mean()
ratio8=(tendon_mm.getReverseCumAngle()-tendon_m.getReverseCumAngle()).mean()
ratio9=(tendon_mm.getReverseCumLength()-tendon_m.getReverseCumLength()*1000).mean()
#Loss results
ratio10=(np.array(tendon_mm.lossAnch)-np.array(tendon_m.lossAnch)*1e-6).mean()
ratio11=(np.array(tendon_mm.lossFriction)-np.array(tendon_m.lossFriction)*1e-6).mean()
ratio12=tendon_mm.slip1-tendon_m.slip1*1e-3
ratio13=(np.array(tendon_mm.stressAfterLossAnch)-np.array(tendon_m.stressAfterLossAnch)*1e-6).mean()
ratio14=(np.array(tendon_mm.stressAfterLossFriction)-np.array(tendon_m.stressAfterLossFriction)*1e-6).mean()
ratio15=Laffected_mm-Laffected_m*1000

'''
print 'ratio1= ', ratio1
print 'ratio2= ', ratio2
print 'ratio3= ', ratio3
print 'ratio4= ', ratio4
print 'ratio5= ', ratio5
print 'ratio6= ', ratio6
print 'ratio7= ', ratio7
print 'ratio8= ', ratio8
print 'ratio9= ', ratio9
print 'ratio10= ', ratio10
print 'ratio11= ', ratio11
print 'ratio12= ', ratio12
print 'ratio13= ', ratio13
print 'ratio14= ', ratio14
print 'ratio15= ', ratio15
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1.e-10 and abs(ratio2)<1.e-10  and abs(ratio3)<1.e-10  and abs(ratio4)<1.e-10  and abs(ratio5)<1.e-10  and abs(ratio6)<1.e-10  and abs(ratio7)<1.e-10  and abs(ratio8)<1.e-10  and abs(ratio9)<1.e-10  and abs(ratio10)<1.e-4   and abs(ratio11)<1.e-10  and abs(ratio12)<1.e-9  and abs(ratio13)<1.e-4  and abs(ratio14)<1.e-10  and abs(ratio15)<1e-3) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
