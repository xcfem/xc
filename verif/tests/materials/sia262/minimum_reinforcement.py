# -*- coding: utf-8 -*-
# Home made test.

from __future__ import division
import sys
from materials.sia262 import SIA262_materials
from materials.sia262 import minimal_reinforcement

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

fctm= SIA262_materials.c25_30.fctm()/1e6
ratio1= abs(fctm-2.56496392002)/2.56496392002

enrobage= 30e-3
ecartement= 150e-3
t= 0.30
sgAdmA= SIA262_materials.limitationContraintes("A",ecartement)
ratio2= abs(sgAdmA-435e6)/435e6
sgAdmB= SIA262_materials.limitationContraintes("B",ecartement)
ratio3= abs(sgAdmB-400e6)/400e6
sgAdmC= SIA262_materials.limitationContraintes("C",ecartement)
ratio4= abs(sgAdmC-230e6)/230e6
kt= SIA262_materials.reductionFactorKT(t)
AsTractionA= minimal_reinforcement.AsMinTraction(SIA262_materials.c25_30,"A",ecartement,t)
ratio5= abs(AsTractionA-1538.20924739e-6)/1538.20924739e-6
AsTractionB= minimal_reinforcement.AsMinTraction(SIA262_materials.c25_30,"B",ecartement,t)
ratio6= abs(AsTractionB-1672.80255653e-6)/1672.80255653e-6
AsTractionC= minimal_reinforcement.AsMinTraction(SIA262_materials.c25_30,"C",ecartement,t)
ratio7= abs(AsTractionC-2909.22183745e-6)/2909.22183745e-6
AsFlexionA= minimal_reinforcement.AsMinFlexion(SIA262_materials.c25_30,enrobage,"A",ecartement,t)
ratio8= abs(AsFlexionA-346.647194688e-6)/346.647194688e-6
AsFlexionB= minimal_reinforcement.AsMinFlexion(SIA262_materials.c25_30,enrobage,"B",ecartement,t)
ratio9= abs(AsFlexionB-376.978824223e-6)/376.978824223e-6
AsFlexionC= minimal_reinforcement.AsMinFlexion(SIA262_materials.c25_30,enrobage,"C",ecartement,t)
ratio10= abs(AsFlexionC-655.615346475e-6)/655.615346475e-6

'''
print "fctm= ", fctm, " MPa"
print "ratio1= ",ratio1
print "sgAdmA= ", sgAdmA, " MPa"
print "ratio2= ",ratio2
print "sgAdmB= ", sgAdmB, " MPa"
print "ratio3= ",ratio3
print "sgAdmC= ", sgAdmC, " MPa"
print "ratio4= ",ratio4
print "kt= ",kt
print "AstTractionA= ",AsTractionA*1e6," mm2"
print "ratio5= ",ratio5
print "AstTractionB= ",AsTractionB*1e6," mm2"
print "ratio6= ",ratio6
print "AstTractionC= ",AsTractionC*1e6," mm2"
print "ratio7= ",ratio7
print "AstFlexionA= ",AsFlexionA*1e6," mm2"
print "ratio8= ",ratio8
print "AstFlexionB= ",AsFlexionB*1e6," mm2"
print "ratio9= ",ratio9
print "AstFlexionC= ",AsFlexionC*1e6," mm2"
print "ratio10= ",ratio10
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-11 and abs(ratio2)<1e-11  and abs(ratio3)<1e-11 and abs(ratio4)<1e-11 and abs(ratio5)<1e-11  and abs(ratio6)<1e-11 and abs(ratio7)<1e-11 and abs(ratio8)<1e-11  and abs(ratio9)<1e-11 and abs(ratio10)<1e-11 ) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
