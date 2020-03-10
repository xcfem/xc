# -*- coding: utf-8 -*-
from materials import typical_materials
from materials.sections.structural_shapes import aisc_metric_shapes
from sympy.physics import units

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

ratio1= 10
ratio2= 10
ratio3= 10
ratio4= 10
ratio5= 10
ratio6= 10
ratio7= 10
ratio8= 10
ratio9= 10
ratio10= 10
ratio11= 10
ratio12= 10
ratio13= 10
ratio14= 10
ratio15= 10
ratio16= 10
ratio17= 10

wSection= aisc_metric_shapes.W['W8X21']
meter= units.m
meter2= units.m*units.m
meter3= meter2*units.m
meter4= meter3*units.m

inch= units.inch
inch2= inch*inch
inch3= inch2*inch
inch4= inch3*inch

def convert(value,unitA,unitB):
  factor= unitA.evalf()/unitB.evalf()
  return value*factor

ratio1= abs((convert(wSection['A'],meter2,inch2)-6.15351)/6.15351)
ratio2= abs((convert(wSection['h'],meter,inch)-8.26772)/8.26772)
ratio3= abs((convert(wSection['tw'],meter,inch)-0.25)/0.25)
ratio4= abs((convert(wSection['b'],meter,inch)-5.27559)/5.27559)
ratio5= abs((convert(wSection['tf'],meter,inch)-0.401575)/0.401575)
ratio6= abs((convert(wSection['kDes'],meter,inch)-0.700787)/0.700787)
ratio7= abs((convert(wSection['kDet'],meter,inch)-0.874016)/0.874016)
ratio8= abs((convert(wSection['k1'],meter,inch)-0.562992)/0.562992)
ratio9= abs((convert(wSection['Iz'],meter4,inch4)-75.1986)/75.1986)
ratio10= abs((convert(wSection['Wzel'],meter3,inch3)-18.1851)/18.1851)
ratio11= abs((convert(wSection['Wzpl'],meter3,inch3)-20.3819)/20.3819)
ratio12= abs((convert(wSection['iz'],meter,inch)-3.48819)/3.48819)
ratio13= abs((convert(wSection['Iy'],meter4,inch4)-9.77821)/9.77821)
ratio14= abs((convert(wSection['Wyel'],meter3,inch3)-3.71024)/3.71024)
ratio15= abs((convert(wSection['Wypl'],meter3,inch3)-5.68741)/5.68741)
ratio16= abs((convert(wSection['iy'],meter,inch)-1.25984)/1.25984)
ratio17= abs((convert(wSection['It'],meter4,inch4)-0.281094)/0.281094)

# print "A= ",convert(wSection['A'],meter2,inch2)
# print "ratio1= ",@ratio1
# print "h= ",convert(wSection['h'],meter,inch)
# print "ratio2= ",@ratio2
# print "tw= ",convert(wSection['tw'],meter,inch)
# print "ratio3= ",@ratio3
# print "b= ",convert(wSection['b'],meter,inch)
# print "ratio4= ",@ratio4
# print "tf= ",convert(wSection['tf'],meter,inch)
# print "ratio5= ",@ratio5
# print "k_des= ",convert(wSection['kDes'],meter,inch)
# print "ratio6= ",@ratio6
# print "k_det= ",convert(wSection['kDet'],meter,inch)
# print "ratio7= ",@ratio7
# print "k1= ",convert(@k1,meter,inch)
# print "ratio8= ",@ratio8
# print "Iz= ",convert(wSection['Iz'],meter4,inch4)
# print "ratio9= ",@ratio9
# print "Wzel= ",convert(wSection['Wzel'],meter3,inch3)
# print "ratio10= ",@ratio10
# print "Wzpl= ",convert(wSection['Wzpl'],meter3,inch3)
# print "ratio11= ",@ratio11
# print "iz= ",convert(wSection['iz'],meter,inch)
# print "ratio12= ",@ratio12
# print "Iy= ",convert(wSection['Iy'],meter4,inch4)
# print "ratio13= ",@ratio13
# print "Wyel= ",convert(wSection['Wyel'],meter3,inch3)
# print "ratio14= ",@ratio14
# print "Wypl= ",convert(wSection['Wypl'],meter3,inch3)
# print "ratio15= ",@ratio15
# print "iy= ",convert(wSection['iy'],meter,inch)
# print "ratio16= ",@ratio16
# print "It= ",convert(wSection['It'],meter4,inch4)
# print "ratio17= ",@ratio17

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-6) & (ratio2<1e-6) & (ratio3<1e-6) & (ratio4<1e-6) & (ratio5<1e-6) & (ratio6<1e-6) & (ratio7<1e-6) & (ratio8<1e-6) & (ratio9<1e-6) & (ratio10<1e-5) & (ratio11<1e-5) & (ratio12<1e-5) & (ratio13<1e-5) & (ratio14<1e-5) & (ratio15<1e-5) & (ratio16<1e-5) & (ratio17<1e-5)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
