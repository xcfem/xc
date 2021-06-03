# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

import math
import xc_base
import geom
from materials.astm_aisc import ASTM_materials

#Example 5.1 AISC Design guide 24 Hollow structural connections.
#units
in2m=0.0254
m2in=1/in2m
ksi2MPa=6.89476
kip2N=4448.2
N2kip=1/kip2N
#data
bolt=ASTM_materials.M24 
bltPlate_thck=0.75*in2m
bltPlate_steel=ASTM_materials.A36
diag_steel=ASTM_materials.A500
diag_shape=ASTM_materials.HSSShape(diag_steel,'HSS8X6X5/16')
capplat_thck=1*in2m  #thickness of the cap plate
weldMetal= ASTM_materials.E7018
w_stem_cap=5/16*in2m #weld Tee stem to Tee flange
w_HSS_cap= 5/16*in2m #weld diagonal to Tee flange
Nd=125*kip2N

# Bolted plate
boltArray= ASTM_materials.BoltArray(bolt, nRows= 2, nCols= 2)
boltedPlate= ASTM_materials.BoltedPlate(boltArray, thickness= bltPlate_thck, steelType= bltPlate_steel, doublePlate= None)

# nominal strenght of the weld metal per unit area
Fw=0.6*weldMetal.fu 
ratio0=(Fw*1e-6/ksi2MPa-42)/42
# Welds connecting the tee flange (cap plate) to the HSS

# Check of the thickness of the capMinimum thickness of the cap-plate
# for allowing all four HSS walls welds to be fully effective
# It's assumed that the force from the stemspreads on both sides
# through the the cap plate at 2.5:1 slope
w_min=boltedPlate.getFilletMinimumLeg(capplat_thck)
w_max=boltedPlate.getFilletMaximumLeg(capplat_thck)
#w_stem_cap=round(w_min+weldFactor*(w_max-w_min),3) #not used in the test
spreadL=boltedPlate.thickness+2*w_stem_cap+2*2.5*capplat_thck
'''
if spreadL < diag_shape.b():
    lmsg.error('Thickness of cap plane is insufficient to spread axial force to all four sides of the HSS diagonal')
else:
    print('Cap-plate thickness: ', round(capplat_thck*1e3,0), 'mm')
'''
ratio1=spreadL*m2in-6.38
    
# Available strenght of the welds connecting the cap-plate to the HSS
w_min=ASTM_materials.getFilletWeldMinimumLegSheets(capplat_thck,diag_shape.get('t'))
w_max=ASTM_materials.getFilletWeldMaximumLegSheets(capplat_thck,diag_shape.get('t'))
#w_HSS_cap=round(w_min+weldFactor*(w_max-w_min),3) #not used in the test
l=2*(diag_shape.b()+diag_shape.h()) # lenght of weld
Aw=l*w_HSS_cap/math.sqrt(2) # area of weld
Rn=Fw*Aw   # weld strenght
fi=0.75
CF_weld_HSS_cap=Nd/(fi*Rn)   #capacity factor of the weld
#print('Weld cap-plate to HSS: leg size w= ',round(w_HSS_cap*1e3,0), ' mm, CF=',round(CF_weld_HSS_cap,2))
ratio2=(fi*Rn*N2kip-195)/195

# Dimensions of the cap plate (minimum 5 mm around the weld to the HSS)
# capplat_B=(diag_shape.b()+2*w_stem_cap+2*5e-3)
# capplat_B=0.005 * math.ceil(capplat_B/0.005) # cap plate width
# capplat_H=(diag_shape.h()+2*w_stem_cap+2*5e-3)
# capplat_H=0.005 * math.ceil(capplat_H/0.005) # cap plata height
capplat_B=7*in2m
capplat_H=9*in2m
#print('Cap-plate dimensions: ', capplat_H*1e3, 'x',capplat_B*1e3,'x',capplat_thck*1e3, '[L/W/T, mm]')
# Welds connecting the stem to the tee flange

l=2*capplat_H
Aw=l*w_stem_cap/math.sqrt(2) # area of weld
Rn=Fw*Aw   # weld strenght
fi=0.75
CF_weld_stem_cap=Nd/(fi*Rn)   #capacity factor of the weld

#print ('Weld tee flange (cap plate) to tee stem: leg size w= ',round(w_stem_cap*1e3,0), ' mm, CF=',round(CF_weld_stem_cap,2))
ratio3=(fi*Rn*N2kip-125)/125

# Local yielding of the HSS wall
if spreadL < diag_shape.b():
    lmsg.error('Thickness of cap plane is insufficient to spread axial force to all four sides of the HSS diagonal')
else:  # contribution of all the four walls of the HSS
    fi=1
    Rn=diag_steel.fy*diag_shape.A() #available strenght of the SHH
    CF_yield_HSS=Nd/(fi*Rn)
#    print('Local yielding of the HSS wall: CF=',round(CF_yield_HSS,2))
    ratio4=(fi*Rn*N2kip-349)/349

# Shear yielding through the cap-plate thickness
Ag=2*capplat_H*capplat_thck  #shear area
Rn=0.6*boltedPlate.steelType.fy*Ag
fi=1
CF_shear_yield_cap=Nd/(fi*Rn)
#print('Shear yielding through the cap-plate thickness: CF=',round(CF_shear_yield_cap,2))
ratio5=(fi*Rn*N2kip-389)/389

#Shear rupture through the cap-plate thickness
fi=0.75
Rn=0.6*boltedPlate.steelType.fu*Ag
fi=0.75
CF_shear_rupt_cap=Nd/(fi*Rn)
#print('Shear rupture through the cap-plate thickness: CF=',round(CF_shear_rupt_cap,2))
ratio6=(fi*Rn*N2kip-470)/470

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<2e-2 and  abs(ratio1)<5e-3 and abs(ratio2)<2e-2 and abs(ratio3)<2e-2 and abs(ratio4)<6e-3 and abs(ratio5)<7e-3 and abs(ratio6)<2e-4:
   print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

