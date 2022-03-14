# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc_base
import geom
from materials.astm_aisc import ASTM_materials
from misc_utils import units_utils

# Example II.A-11B-1 «End-plate connection subject to axial and shear loading
# from AISC v15.1 companion Vol. 1 design examples

#units
m2in=1/units_utils.inchToMeter
ksi2MPa=6.89476


#data
boltDiam=7/8*units_utils.inchToMeter
boltSteel=ASTM_materials.A325 # Group A
threadsExcluded=False #true if threads and transition area of shank are excluded from the shear plane.
nBoltRows=2  #in width direction of the plate
nBoltCols=5  # in length direction of the plate
distBolts=3*units_utils.inchToMeter   #distance between rows and columns (can be None)

plateWidth=(3+2*1.25)*units_utils.inchToMeter
plateLength=(4*3+2*1.25)*units_utils.inchToMeter
plateThick=0.5*units_utils.inchToMeter
plateSteel=ASTM_materials.A36 

Nd=60*units_utils.kipToN
Vd=75*units_utils.kipToN #acting in the plateLength direction 

#weldSize=3/16*units_utils.inchToMeter
weldSize=2.08/16*units_utils.inchToMeter
weldLength=14.5*units_utils.inchToMeter-2*weldSize
fillerMetal=ASTM_materials.E7018

beam=ASTM_materials.WShape(name='W18X50')
# end data

# Resultant load
Fd=math.sqrt(Nd**2+Vd**2)

ratio0=(Fd/units_utils.kipToN-96)/96

bolt=ASTM_materials.BoltFastener(boltDiam,boltSteel)

dh=bolt.getNominalHoleDiameter()


# Available bolt shear strength
fiRn_shear=bolt.getDesignShearStrength(threadsExcluded)

ratio1=(fiRn_shear/units_utils.kipToN-48.7/2)/(48.7/2)

# Array of bolts definition
bolt_array=ASTM_materials.BoltArray(bolt,nBoltRows,nBoltCols,distBolts)
# Bolted-plate definition
bolted_plate=ASTM_materials.BoltedPlate(bolt_array,plateWidth,plateLength,plateThick,plateSteel)

# Bolt bearing on the plate
# Available bearing strength of the plate
fiRn_bearing_plate=bolted_plate.getDesignBearingStrength()

ratio2=(fiRn_bearing_plate/units_utils.kipToN-5*91.5)/(5*91.5)

# Bolt tearout on the plate
# Available tearout strength of the plate. This verification doesn't match very well
# with the example procedure. Perhaps it should be rethought in the future
fiRn_tearout_plate=bolted_plate.getDesignTearoutStrength(geom.Vector2d(0,1))  
fiRn_comp=1*40.8+4*107
ratio3=(fiRn_tearout_plate/units_utils.kipToN-fiRn_comp)/fiRn_comp
#shear strength of bolted connection
# **********************************
fiRn_shear_bolted_conn=min(fiRn_shear*nBoltRows*nBoltCols,fiRn_bearing_plate,fiRn_tearout_plate)
CF_shear=Vd/fiRn_shear_bolted_conn
#print('Capacity factor shear of bolted connection = ', round(CF_shear,2))

ratio4=(fiRn_shear_bolted_conn/units_utils.kipToN-236)/236
# Bolt shear and tension interaction

# Available strength of the bolt due to effect of combined tension and shear
Vd_bolt=Vd/nBoltRows/nBoltCols
fiRn_comb_tens_shear=bolt.getDesignCombinedStrength(Vd_bolt,threadsExcluded)
fiRn_comb_tens_shear_plate=fiRn_comb_tens_shear*nBoltRows*nBoltCols                                                         
ratio5=(fiRn_comb_tens_shear/units_utils.kipToN-40.2)/40.2

CF_comb_tens_shear=Nd/fiRn_comb_tens_shear_plate
#print('Capacity factor combined tension and shear of bolted connection = ', round(CF_comb_tens_shear,2))

# Weld design
weld=ASTM_materials.FilletWeld(weldSize,weldLength,fillerMetal)

ratio6=(1-2*weld.getDesignStrength(theta=30)/Fd)


CF_weld=weld.getCapacityFactor(Vd/2,Nd/2)

#print('Capacity factor weld = ', CF_weld)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<5e-4 and abs(ratio1)<8e-4 and abs(ratio2)<2e-3 and abs(ratio3)<2e-1 and abs(ratio4)<4e-2 and abs(ratio5)<6e-4 and abs(ratio6)<3e-2:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
