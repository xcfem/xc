# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "


''' Test of material concrete02 constitutive model. 
In the tensile Branch, the post-cracking range is initially approximated with a exponential 
decay curve, according to the model proposed by Stramandinoli and La Rovere, to take into account 
the tension-stiffening effect. Since the concrete02 implementation requires a straight line, a 
linear regression of this curve is used for this purpose.
'''

import os
import xc_base
import geom
import xc
from materials import concrete_base
from materials import typical_materials
from materials.ehe import EHE_materials
from materials import concrete_base
import matplotlib.pyplot as plt
import math
import numpy as np

width=0.3     #width (cross-section coordinate Y)
depth=0.5     #depth (cross-section coordinate Z)
ro_exp=0.67*1e-2  #longitudinal steel ratio 
fct_exp=1.17*1e6  #
Ec_exp=10*1e9
fy_exp=526e6
Es_exp=197*1e9

As_calc=ro_exp*width*depth   #reinforcement area in the cross-section

prueba= xc.ProblemaEF()
preprocessor= prueba.getPreprocessor

# Materials definition
concrAux= EHE_materials.HA25           #parameters only for the compression branche 

#Reinforcing steel.
rfSteel=concrete_base.ReinforcingSteel(steelName='rfSteel', fyk=fy_exp, emax=0.08, gammaS=1.15,k=1.05)
rfSteel.Es=Es_exp
steelDiagram= rfSteel.defDiagK(preprocessor) #Definition of steel stress-strain diagram in XC. 

#Parameters for tension stiffening of concrete
paramTS=concrete_base.paramTensStiffness(concrMat=concrAux,reinfMat=rfSteel,reinfRatio=ro_exp,diagType='K')
paramTS.E_c=Ec_exp  #concrete elastic modulus
paramTS.f_ct=fct_exp  #concrete tensile strength 
paramTS.E_ct=Ec_exp #concrete elastic modulus in the tensile linear-elastic range
paramTS.E_s=Es_exp 
paramTS.eps_y=fy_exp/Es_exp


#regression line type 1
ftdiag=paramTS.pointOnsetCracking()['ft']
ectdiag=paramTS.pointOnsetCracking()['eps_ct']
eydiag=paramTS.eps_y
######Etsdiag=ftdiag/(eydiag-ectdiag)
Etsdiag=paramTS.regresLine()['slope']
#Material for making concrete fibers: concrete02 with tension stiffening
concr= typical_materials.defConcrete02(preprocessor=preprocessor,name='concr',epsc0=concrAux.epsilon0(),fpc=concrAux.fmaxK(),fpcu=0.85*concrAux.fmaxK(),epscu=concrAux.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=abs(Etsdiag))

#regression line passing through point (optional approximation)
# ftdiag=paramTS.f_ct
# Etsdiag=-paramTS.slopeRegresLineFixedPoint()
# concr= typical_materials.defConcrete02(preprocessor=preprocessor,name='concr',epsc0=concrAux.epsilon0(),fpc=concrAux.fmaxK(),fpcu=0.85*concrAux.fmaxK(),epscu=concrAux.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)


# 
epsMin=0.0
epsMax=0.0028
incEps=(epsMax-epsMin)/25.0
strains=np.arange(epsMin,epsMax,incEps)

stress=list()
for eps in strains:
    concr.setTrialStrain(eps, 0.0)
    stress.append(concr.getStress())


#Test comparison values

stressComp=[0.0, 1029185.6272346211, 997696.6777423947, 966207.7282501684, 934718.778757942, 903229.8292657157, 871740.8797734893, 840251.930281263, 808762.9807890366, 777274.0312968101, 745785.0818045839, 714296.1323123574, 682807.1828201312, 651318.2333279047, 619829.2838356785, 588340.334343452, 556851.3848512258, 525362.4353589994, 493873.48586677294, 462384.5363745466, 430895.58688232035, 399406.6373900939, 367917.6878978675, 336428.73840564117, 304939.78891341493]

residStresses= (np.array(stress) - np.array(stressComp))

ratio1= np.linalg.norm(residStresses)/concrAux.fmaxK()

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


# ###   FIGURES & REPORTS
# plt.plot(strains,stress)

# #exponential curve 
# strainsCurve=paramTS.ptosExpCurvPostCracking()['strainPts']
# stressCurve=paramTS.ptosExpCurvPostCracking()['stressPts']
# plt.plot(strainsCurve,stressCurve)
# plt.show()

# #report concrete material
# from postprocess.reports import report_material
# report_material.report_concrete02(concrDiag=concr,paramTensStiffening=paramTS,grTitle='concrete $\sigma-\epsilon$ curve',grFileName='conc2',texFileName='conc2.tex')
