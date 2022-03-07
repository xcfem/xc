# -*- coding: utf-8 -*-
''' Simply supported beam capacity check example E1.2a
    taken from the document NDS Structural Wood Design
    Examples 2015/2018 Edition.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from materials.sections import section_properties
from materials.awc_nds import AWCNDS_materials
from materials.awc_nds import AWCNDS_limit_state_checking
from materials.awc_nds import dimensional_lumber

inchToMeter= 2.54/100.0
footToMeter= .3048
psiToPa= 6894.76

# Materials
# Douglas Fir-Larch Select structural.
wood= dimensional_lumber.DouglasFirLarchWood(grade= 'structural')
beamSection= AWCNDS_materials.CustomLumberSection("DouglasFirLarch",b=3.5*inchToMeter,h=15.25*inchToMeter, woodMaterial= wood)

CD= 1.0 # Duration factor.
Cfu= 1.0
CM= 1.0
Cr= 1.0
CT= 1.0
Ct= 1.0
Ci= 1.0
CF= 1.0 # (Table 4A 14" and wider)


length= 20.0*footToMeter
loadingCondition= AWCNDS_materials.MemberLoadingCondition(numberOfConcentratedLoads= 1, lateralSupport= False, cantilever= False)
beam= AWCNDS_limit_state_checking.BeamMember(unbracedLength= length, section= beamSection, memberLoadingCondition= loadingCondition, Cr= Cr)
Fb= beam.crossSection.wood.Fb
E= beam.crossSection.wood.E
Emin= beam.crossSection.wood.Emin
Fct= beam.crossSection.wood.Fct
Fv= beam.crossSection.wood.Fv

Ep= E*CM*Ct*Ci
Epmin= Emin*CM*Ct*Ci*CT

# Member dimensions and properties
w_bearing= 3.5*inchToMeter
Ag= beam.crossSection.A()
S= beam.crossSection.getElasticSectionModulusZ() 
I= beam.crossSection.Iz()

# Beam stability factor
Fbp_aster= Fb*CD*CM*Ct*CF*Ci*Cr  # Fbp_aster is adjusted bending design value
                                 # with all adjustment factors except the
                                 # beam stability factor CL and flat use factor
                                 # Cfu applied.
FbAdj= beam.crossSection.getFbAdj()
ratio0= abs(FbAdj-Fbp_aster)/Fbp_aster

lu= beam.unbracedLengthX
le= beam.getEffectiveLength()
RB= beam.getBendingSlendernessRatio()
FbE= beam.getFbECriticalBucklingDesignValue()
CL= beam.getBeamStabilityFactor()
Fb_aster= Fb*CL
    
ratio1= abs(Fb-1500*psiToPa)/1500
ratio2= abs(E-1.9e6*psiToPa)/1.9e6
ratio3= abs(Emin-690e3*psiToPa)/690e3
ratio4= abs(Fct-625*psiToPa)/625
ratio5= abs(Fv-180*psiToPa)/180
ratio6= abs(Ag-53.38*inchToMeter**2)/53.38
ratio7= abs(S-135.66*inchToMeter**3)/135.66
ratio8= abs(I-1034*inchToMeter**4)/1034.0
ratio9= abs(Fbp_aster-1500*psiToPa)/1500.0
ratio10= abs(lu-240*inchToMeter)/240.0
ratio11= abs(le-375*inchToMeter)/375.0
ratio12= abs(RB-21.6)/21.6
ratio13= abs(FbE-12.24350981e6)/12.24350981e6
ratio14= abs(CL-.875623337112)/.875623337112
ratio15= abs(Fb_aster-9.05581913968e6)/(9.05581913968e6)

check= (ratio0<1e-15) and (ratio1<1e-15) and (ratio2<1e-15) and (ratio3<1e-15) and (ratio4<1e-15) and (ratio5<1e-15) and (ratio6<1e-7) and (ratio8<1e-9) and (ratio9<1e-15) and (ratio10<1e-15) and (ratio11<1e-4) and (ratio12<1e-3) and (ratio13<1e-9) and (ratio14<1e-12) and (ratio15<1e-12)

'''
print('Fb_aster= ', Fb_aster/1e6, ' MPa theor. Fb_aster= ',  1313*psiToPa/1e6)
print('FbAdj= ', FbAdj/1e6, ' MPa theor. FbAdj= ',  1313*psiToPa/1e6)
print('ratio0= ',ratio0)
print('ratio1= ',ratio1)
print('ratio2= ',ratio2)
print('ratio3= ',ratio3)
print('ratio4= ',ratio4)
print('ratio5= ',ratio5)
print('Ag= ',Ag)
print('ratio6= ',ratio6)
print('ratio7= ',ratio7)
print('ratio8= ',ratio8)
print('ratio9= ',ratio9)
print('lu= ', lu, ' theor. lu= ', 240*inchToMeter)
print('ratio10= ',ratio10)
print('le= ', le, ' theor. lu= ', 375*inchToMeter)
print('ratio11= ',ratio11)
print('ratio12= ',ratio12)
print('FbE= ', FbE/1e6, ' MPa theor. FbE= ', 1776*psiToPa/1e6)
print('ratio13= ',ratio13)
print('CL= ', CL, ' theor. CL= ', .876)
print('ratio14= ',ratio14)
print('Fb_aster= ', Fb_aster/1e6, ' MPa theor. Fb_aster= ',  1313*psiToPa/1e6)
print('ratio15= ',ratio15)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(check):
    print("test "+fname+": ok.")
else:
    lmsg.error('test: '+fname+' ERROR.')
