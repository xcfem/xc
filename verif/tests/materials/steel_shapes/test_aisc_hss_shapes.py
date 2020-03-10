# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
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

hssSection= aisc_metric_shapes.HSS['HSS7X7X3/8']

meter= units.m
meter2= units.m*units.m
meter3= meter2*units.m
meter4= meter3*units.m

inch= units.inch
inch2= inch*inch
inch3= inch2*inch
inch4= inch3*inch

def convert(value,unitA,unitB):
  return value*(unitA/unitB)

ratio1= abs((convert(hssSection['A'],meter2,inch2)-8.97452)/8.97452)
ratio2= abs((convert(hssSection['h'],meter,inch)-7.00787)/7.00787)
ratio3= abs((convert(hssSection['t'],meter,inch)-0.375197)/0.375197)
ratio4= abs((convert(hssSection['b'],meter,inch)-7.00787)/7.00787)
ratio5= abs((convert(hssSection['Iz'],meter4,inch4)-65.108)/65.108)
ratio6= abs((convert(hssSection['Wzel'],meter3,inch3)-18.6122)/18.6122)
ratio7= abs((convert(hssSection['Wzpl'],meter3,inch3)-22.0906)/22.0906)
ratio8= abs((convert(hssSection['iz'],meter,inch)-2.68898)/2.68898)
ratio9= abs((convert(hssSection['Iy'],meter4,inch4)-65.108)/65.108)
ratio10= abs((convert(hssSection['Wyel'],meter3,inch3)-18.6122)/18.6122)
ratio11= abs((convert(hssSection['Wypl'],meter3,inch3)-22.0906)/22.0906)
ratio12= abs((convert(hssSection['iy'],meter,inch)-2.68898)/2.68898)
ratio13= abs((convert(hssSection['It'],meter4,inch4)-104.99)/104.99)

# print "A= ",convert(hssSection['A'],meter2,inch2)
# print "ratio1= ",ratio1
# print "h= ",convert(hssSection['h'],meter,inch)
# print "ratio2= ",ratio2
# print "e= ",convert(hssSection['e'],meter,inch)
# print "ratio3= ",ratio3
# print "b= ",convert(hssSection['b'],meter,inch)
# print "ratio4= ",ratio4
# print "Iz= ",convert(hssSection['Iz'],meter4,inch4)
# print "ratio5= ",ratio5
# print "Wzel= ",convert(hssSection['Wzel'],meter3,inch3)
# print "ratio6= ",ratio6
# print "Wzpl= ",convert(hssSection['Wzpl'],meter3,inch3)
# print "ratio7= ",ratio7
# print "iz= ",convert(hssSection['iz'],meter,inch)
# print "ratio8= ",ratio8
# print "Iy= ",convert(hssSection['Iy'],meter4,inch4)
# print "ratio9= ",ratio9
# print "Wyel= ",convert(hssSection['Wyel'],meter3,inch3)
# print "ratio10= ",ratio10
# print "Wypl= ",convert(hssSection['Wypl'],meter3,inch3)
# print "ratio11= ",ratio11
# print "iy= ",convert(hssSection['iy'],meter,inch)
# print "ratio12= ",ratio12
# print "It= ",convert(hssSection['It'],meter4,inch4)
# print "ratio13= ",ratio13



import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-6) & (ratio2<1e-6) & (ratio3<1e-6) & (ratio4<1e-6) & (ratio5<1e-6) & (ratio6<1e-5) & (ratio7<1e-6) & (ratio8<1e-5) & (ratio9<1e-6) & (ratio10<1e-5) & (ratio11<1e-5) & (ratio12<1e-5) & (ratio13<1e-5)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

