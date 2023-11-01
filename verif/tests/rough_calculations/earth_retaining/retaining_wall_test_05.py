# -*- coding: utf-8 -*-
''' Trivial checks for a retaining wall with two steps in its back face.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2023, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from scipy.constants import g
from rough_calculations import ng_retaining_wall
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from actions import load_cases
from actions import combinations
from geotechnics import earth_pressure as ep
from actions.earth_pressure import earth_pressure
from misc_utils import log_messages as lmsg

#     __      __    _ _                          _            
#     \ \    / /_ _| | |  __ _ ___ ___ _ __  ___| |_ _ _ _  _ 
#      \ \/\/ / _` | | | / _` / -_) _ \ '  \/ -_)  _| '_| || |
#       \_/\_/\__,_|_|_| \__, \___\___/_|_|_\___|\__|_|  \_, |
#                        |___/                           |__/ 
#                              +
#                             /|
#                           /  | V
#                         /    | i
#     Backfill slope -> /      | r            
#                     /        | t
#      zTopWall -- /           | u
#               | |            | a
#               | |            | l
#               | | step 1     |
#               | |_           | b
#               |  |           | a
#               |  | step 2    | c
#               |  |__         | k
#               |     |        | 
#               |     |        | 
#               |     |        | 
#               |     |        | 
#          +----      ---------+    <- zTopFooting
#          |Toe          Heel  |
#          +-------------------+
#

# Wall geometry
stemHeight= 11.48
zTopWall= stemHeight

## Stem
stemTopWidth= 0.50 # width of the wall at its top.
stemBottomWidth= 1.25 # width of the wall at its bottom.
stemBackSteps= [(4.48, 0.7-stemTopWidth), (4.48+3.50, stemBottomWidth-0.7)] # steps on the stem back.

## Foundation.
bToe= 1.75 # width of the toe.
footingWidth= 7.0
bHeel= footingWidth-stemBottomWidth-bToe
footingThickness= 1.75 # thickness of the footing.

#  __  __      _           _      _    
# |  \/  |__ _| |_ ___ _ _(_)__ _| |___
# | |\/| / _` |  _/ -_) '_| / _` | (_-<
# |_|  |_\__,_|\__\___|_| |_\__,_|_/__/

# Partial factors (M1)
gammaMPhiM1= 1.0

# Granular fill.
slopeOfBackfillSurface= 0.0
## Design approach 2 (A1+M1+R2).
granularFillM1= ep.RankineSoil(phi= math.radians(30), rho= 2000, gammaMPhi= gammaMPhiM1, beta= slopeOfBackfillSurface) 

# Wall materials.
concrete= EC2_materials.C30
steel= EC2_materials.S500B
cover= 45e-3
reinf_types= EC2_limit_state_checking.define_rebar_families(steel= steel, cover= cover)

#  __      __    _ _        _     _        _   
# \ \    / /_ _| | |   ___| |__ (_)___ __| |_ 
# \ \/\/ / _` | | |  / _ \ '_ \| / -_) _|  _|
# \_/\_/\__,_|_|_|  \___/_.__// \___\__|\__|
#                            |__/            

wall= ng_retaining_wall.RetainingWall(name= 'Retaining_wall_test_05', stemHeight= stemHeight, stemBottomWidth= stemBottomWidth, stemTopWidth= stemTopWidth, stemBackSlope= 0.0, footingThickness= footingThickness, bToe= bToe, bHeel= bHeel, concrete= concrete, steel= steel)
wall.setStemBackSteps(stemBackSteps)

#    ___     _       __                              _   
#   | _ \___(_)_ _  / _|___ _ _ __ ___ _ __  ___ _ _| |_ 
#   |   / -_) | ' \|  _/ _ \ '_/ _/ -_) '  \/ -_) ' \  _|
#   |_|_\___|_|_||_|_| \___/_| \__\___|_|_|_\___|_||_\__|
#                                                      
reinf_types= EC2_limit_state_checking.define_rebar_families(steel= steel, cover= cover)
wall.stemReinforcement.setReinforcement(1, reinf_types['A25_10'].getCopy()) # vert. trasdós (esperas)
wall.stemReinforcement.setReinforcement(2, reinf_types['A25_10'].getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(6, reinf_types['A12_10'].getCopy()) # coronación
wall.stemReinforcement.setReinforcement(11, reinf_types['A12_15'].getCopy()) # horiz. trasdós

wall.stemReinforcement.setReinforcement(4, reinf_types['A10_10'].getCopy()) # vert. intradós (esperas)
wall.stemReinforcement.setReinforcement(5, reinf_types['A10_10'].getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(12, reinf_types['A12_15'].getCopy()) # horiz. intradós

wall.stemReinforcement.setReinforcement(3, reinf_types['A32_20'].getCopy()) # tr. sup. zapata
wall.stemReinforcement.setReinforcement(9, reinf_types['A12_15'].getCopy()) # ln. sup. zapata
wall.stemReinforcement.setReinforcement(7, reinf_types['A10_15'].getCopy()) # tr. inf. zapata
wall.stemReinforcement.setReinforcement(8, reinf_types['A12_15'].getCopy()) # ln. inf. zapata

wall.stemReinforcement.setReinforcement(10, reinf_types['A10_15'].getCopy()) # lateral zapata

wall.stemReinforcement.setReinforcement(102, reinf_types['A20_10'].getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(105, reinf_types['A10_10'].getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(111, reinf_types['A12_15'].getCopy()) # horiz. trasdós
wall.stemReinforcement.setReinforcement(112, reinf_types['A12_15'].getCopy()) # horiz. intradós

wall.stemReinforcement.setReinforcement(202, reinf_types['A16_20'].getCopy()) # vert. trasdós (contacto terreno)
wall.stemReinforcement.setReinforcement(205, reinf_types['A12_20'].getCopy()) # vert. intradós (exterior)
wall.stemReinforcement.setReinforcement(211, reinf_types['A12_15'].getCopy()) # horiz. trasdós
wall.stemReinforcement.setReinforcement(212, reinf_types['A12_15'].getCopy()) # horiz. intradós

# Check area.
wallArea= wall.getArea()
footingArea= footingWidth*footingThickness
stemTopArea= stemTopWidth*stemBackSteps[0][0]
stemIntermediateArea= (stemTopWidth+stemBackSteps[0][1])*(stemBackSteps[1][0]-stemBackSteps[0][0])
stemBottomArea= stemBottomWidth*(stemHeight-stemBackSteps[1][0])
refWallArea= footingArea+stemTopArea+stemIntermediateArea+stemBottomArea
ratioWallArea= abs(wallArea-refWallArea)/refWallArea


# Check extracted sections.
sectionStemLongExt= wall.stemReinforcement.getSectionStemLongExt()
sectionStemLongInt= wall.stemReinforcement.getSectionStemLongInt()
sectionAtMiddle= wall.stemReinforcement.getSectionStem(y= stemHeight/2.0, reinforcementIndex= 102)
err= math.sqrt((sectionStemLongExt.h-stemBottomWidth)**2+(sectionStemLongInt.h-stemBottomWidth)**2+(sectionAtMiddle.h-(stemTopWidth+stemBackSteps[0][1]))**2)

'''
print('wall area: ', wallArea, 'm2')
print('reference wall area: ', refWallArea, 'm2')
print('section stem long ext h= ', sectionStemLongExt.h)
print('section stem long int h= ', sectionStemLongInt.h)
print('intermediate section h= ', sectionAtMiddle.h)
'''

fname= os.path.basename(__file__)
if(abs(ratioWallArea)<1e-6) and (err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic output.
# wall.draw()
    

