# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

''' Test distribution of wind pressures in the cylindrical body of a tank'''

import os
from actions.wind import base_wind as bw
import math

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com l.pereztato@gmail.com"

#units
kmh2ms=1/3.6

#   Data
#Wind data
v=155*kmh2ms #basic speed wind [m/s]
Kd=0.95      #wind directionality factor
Kzt=1.0      #topographic factor
I=1.15       #importance factor
alpha=9.5    #terrain exposure constant (exposure C)
zg=275       #terrain exposure constant (exposure C)

#Tanks data
diam=3.18      # diameter
height=20.6    # height of the cylindrical tank

#Tank1 data
windComp1=[1,0] # components [x,y] of a vector in wind direction 
zGround1=0      # ground level
xCent1=0        # x-coordinate of the cylinder axis
yCent1=0        # y-coordinate of the cylinder axis

#Tank2 data
windComp2=[1,-1] # components [x,y] of a vector in wind direction 
zGround2=2      # ground level
xCent2=0        # x-coordinate of the cylinder axis
yCent2=0        # y-coordinate of the cylinder axis

# Comparison values
# Velocity wind pressures at z=2 and z=10
qzConst=0.613*Kzt*Kd*v**2*I   #constant value 
qz_2=2.01*(5/zg)**(2/alpha)*qzConst
qz_10=2.01*(10/zg)**(2/alpha)*qzConst
# External wind pressure coefficients,
# circunferential distribution for theta=0, t
theta=math.radians(0)
Cp1_0=-0.5+0.4*math.cos(theta)+0.8*math.cos(2*theta)+0.3*math.cos(3*theta)-0.1*math.cos(4*theta)-0.05*math.cos(5*theta)
kc_0=1.0
Cp_0=Cp1_0*kc_0

theta=math.radians(45)
Cp1_45=-0.5+0.4*math.cos(theta)+0.8*math.cos(2*theta)+0.3*math.cos(3*theta)-0.1*math.cos(4*theta)-0.05*math.cos(5*theta)
kc_45=1.0-0.55*(Cp1_45+0.15)*math.log10(height/diam)
Cp_45=Cp1_45*kc_45

theta=math.radians(90)
Cp1_90=-0.5+0.4*math.cos(theta)+0.8*math.cos(2*theta)+0.3*math.cos(3*theta)-0.1*math.cos(4*theta)-0.05*math.cos(5*theta)
kc_90=1.0-0.55*(Cp1_90+0.15)*math.log10(height/diam)
Cp_90=Cp1_90*kc_90

theta=math.radians(180)
Cp1_180=-0.5+0.4*math.cos(theta)+0.8*math.cos(2*theta)+0.3*math.cos(3*theta)-0.1*math.cos(4*theta)-0.05*math.cos(5*theta)
kc_180=1.0-0.55*(Cp1_180+0.15)*math.log10(height/diam)
Cp_180=Cp1_180*kc_180
#    End data

R=diam/2.

windParams=bw.windParams(v,Kd,Kzt,I,alpha,zg)
tank1Wind=bw.cylindrWind(diam,height,windParams,windComp1,zGround1,xCent1,yCent1)
tank2Wind=bw.cylindrWind(diam,height,windParams,windComp2,zGround2,xCent2,yCent2)

pr1_2_0=tank1Wind.getWindPress(-R,0,2+zGround1)
pr2_2_0=tank2Wind.getWindPress(-R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),2+zGround2)
pr_comp=qz_2*Cp_0
ratio1=pr1_2_0-pr_comp
ratio2=pr2_2_0-pr_comp

pr1_2_45=tank1Wind.getWindPress(-R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),2+zGround1)
pr2_2_45=tank2Wind.getWindPress(0,R,2+zGround2)
pr_comp=qz_2*Cp_45
ratio3=pr1_2_45-pr_comp
ratio4=pr2_2_45-pr_comp

pr1_2_90=tank1Wind.getWindPress(0,R,2+zGround1)
pr2_2_90=tank2Wind.getWindPress(R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),2+zGround2)
pr_comp=qz_2*Cp_90
ratio5=pr1_2_90-pr_comp
ratio6=pr2_2_90-pr_comp

pr1_2_180=tank1Wind.getWindPress(R,0,2+zGround1)
pr2_2_180=tank2Wind.getWindPress(R*math.cos(math.radians(45)),-R*math.sin(math.radians(45)),2+zGround2)
pr_comp=qz_2*Cp_180
ratio7=pr1_2_180-pr_comp
ratio8=pr2_2_180-pr_comp


pr1_10_0=tank1Wind.getWindPress(-R,0,10+zGround1)
pr2_10_0=tank2Wind.getWindPress(-R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),10+zGround2)
pr_comp=qz_10*Cp_0
ratio9=pr1_10_0-pr_comp
ratio10=pr2_10_0-pr_comp

pr1_10_45=tank1Wind.getWindPress(-R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),10+zGround1)
pr2_10_45=tank2Wind.getWindPress(0,R,10+zGround2)
pr_comp=qz_10*Cp_45
ratio11=pr1_10_45-pr_comp
ratio12=pr2_10_45-pr_comp

pr1_10_90=tank1Wind.getWindPress(0,R,10+zGround1)
pr2_10_90=tank2Wind.getWindPress(R*math.cos(math.radians(45)),R*math.sin(math.radians(45)),10+zGround2)
pr_comp=qz_10*Cp_90
ratio13=pr1_10_90-pr_comp
ratio14=pr2_10_90-pr_comp

pr1_10_180=tank1Wind.getWindPress(R,0,10+zGround1)
pr2_10_180=tank2Wind.getWindPress(R*math.cos(math.radians(45)),-R*math.sin(math.radians(45)),10+zGround2)
pr_comp=qz_10*Cp_180
ratio15=pr1_10_180-pr_comp
ratio16=pr2_10_180-pr_comp

OK=abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6 and abs(ratio4)<1e-6 and abs(ratio5)<1e-6 and abs(ratio6)<1e-6 and abs(ratio7)<1e-6 and abs(ratio8)<1e-6 and abs(ratio9)<1e-6 and abs(ratio10)<1e-6 and abs(ratio11)<1e-6 and abs(ratio12)<1e-6 and abs(ratio13)<1e-6 and abs(ratio14)<1e-6 and abs(ratio15)<1e-6 and abs(ratio16)<1e-6

'''
print(OK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (OK):
  print ("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

