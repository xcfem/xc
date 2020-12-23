# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
'''Test on applying short-term prestress losses to a tendon.
'''
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"


import numpy as np
import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials as tm
from materials.prestressing import prestressed_concrete as presconc

#Geometry
lBeam=20   #beam span [m]
#Parabola
eEnds=0         #eccentricity of cables at both ends of the beam
eMidspan=-0.558   #eccentricity of cables at midspan [m]
angl_Parab_XZ=math.pi/4 #angle between the vertical plane that contains the
                        #parabola and the plane XZ
#Material
Ep=195e9       #elastic modulus of prestressing steel [Pa]
#Prestressing process
mu=0.25        #coefficient of friction between the cables and their sheating
k=0.0017  #wobble coefficient per meter length of cable [1/m]
fpi=1239e6 #Initial stress of cable [Pa]
Aps=2850e-6        #Area of cable [m2]

# Interpolation
n_points_rough=5    #number of points provided to the interpolation algorithm
n_points_fine=101   #number of points interpolated

#Anchorage slip
deltaL=5e-3          #anchorage draw-in (provided by manufacturer) [m]

# End data

# XC model
# Problem type
FEcase= xc.FEProblem()
prep=  FEcase.getPreprocessor
nodes= prep.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#Exact parabola
from model.geometry import geom_utils
a,b,c=geom_utils.fit_parabola(x=np.array([0,lBeam/2.0,lBeam]), y=np.array([eEnds,eMidspan,eEnds]))
x_parab_rough,y_parab_rough,z_parab_rough=geom_utils.eq_points_parabola(0,lBeam,n_points_rough,a,b,c,angl_Parab_XZ)

#Tendon definition, layout and friction losses
tendon=presconc.PrestressTendon([])
tendon.roughCoordMtr=np.array([x_parab_rough,y_parab_rough,z_parab_rough])
#Interpolated 3D spline 
tendon.pntsInterpTendon(n_points_fine,smoothness=1,kgrade=3)

#Material
prestressingSteel= tm.defCableMaterial(preprocessor=prep, name="prestressingSteel",E=Ep,prestress=0.0,rho=0.0)

tendon.creaTendonElements(preprocessor=prep,materialName="prestressingSteel",elemTypeName='Truss',crdTransfName=None,areaTendon=Aps,setName='tendonSet')

# Losses of prestressing due to friction
lssFrict=tendon.getLossFriction(coefFric=mu,k=k,sigmaP0_extr1=fpi,sigmaP0_extr2=0.0)
tendon.applyStressToElems(stressMtr=tendon.stressAfterLossFriction)
# Losses of prestressing due to anchorage slip (loss due to friction must be
# previously calculated
lssAnch=tendon.getLossAnchor(Ep=Ep,anc_slip_extr1=deltaL,anc_slip_extr2=0.0)
tendon.applyStressLossToElems(stressLossMtr=lssAnch)
stressAfterLossAnch=tendon.stressAfterLossFriction-lssAnch #loss in nodes of tendon

# check
lstMat=[e.getMaterial() for e in tendon.lstOrderedElems]
lstStress=[m.getStress() for m in lstMat]
stressAppliedToElems=np.array(lstStress)
stressAfterLossAnchElems=(stressAfterLossAnch[1:]+stressAfterLossAnch[:-1])/2.0
ratio=abs(stressAppliedToElems.mean()-stressAfterLossAnchElems.mean())/stressAppliedToElems.mean()
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-7) :
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
