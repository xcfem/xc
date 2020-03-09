# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega.Ort@gmail.com"

import math
from rough_calculations import vaults_rough_calc as vault
import numpy as np

# Rios Warehouse in Mexico City, by Candela
# Example 1 from 'The art of structural engineering : Vaults' course,
# by María Garlock

#Data. Units: m
lx=7.625  #size of one tympan (one-fourth of the umbrella) in X direction
ly=5      #size of one tympan (one-fourth of the umbrella) in Y direction
zMax=2    #Z coordinate of the umbrella corner
zSupp=0   #Z coordinate of the umbrella central point
th=0.04   #thickness of the shell
pz=1.96   #vertical load per unit area of ground projection [kPa]
#


riosW=vault.hyparUmbrella(lx=lx,ly=ly,zMax=zMax,zSupp=zSupp,thickness=th)
k=riosW.k    #warping [m-1]
xCoor=[i*0.5 for i in range(15)]
xCoor.append(lx)
yCoor=[i*0.5 for i in range(11)]

zCoord=list()
for y in yCoor:
  zCoord.append([riosW.getZ(x,y) for x in xCoor])
zCoord=np.array(zCoord)


Nxy=riosW.getNxy(pz) #shearing force [kN/m]
fc=riosW.getCompStress(pz)  #maximum compressive stress [kPa]
ft=riosW.getTensStress(pz)  #maximum tensile stress [kPa]
Tmax=riosW.getTmax(pz)      #maximum tension force in edge of umbrella [kN]
Cmax=riosW.getCmax(pz)      #maximum compression force in edge of umbrella [kN]

k_comp=0.05245
zCoord_comp=np.array([[0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0],
 [0.0,0.013,0.026,0.039,0.052,0.066,0.079,0.092,0.105,0.118,0.131,0.144,0.157,0.17,0.184,0.2],
 [0.0,0.026,0.052,0.079,0.105,0.131,0.157,0.184,0.21,0.236,0.262,0.289,0.315,0.341,0.367,0.4],
 [0.0,0.039,0.079,0.118,0.157,0.197,0.236,0.275,0.315,0.354,0.393,0.433,0.472,0.511,0.551,0.6],
 [0.0,0.052,0.105,0.157,0.21,0.262,0.315,0.367,0.42,0.472,0.525,0.577,0.63,0.682,0.734,0.8],
 [0.0,0.066,0.131,0.197,0.262,0.328,0.393,0.459,0.525,0.59,0.656,0.721,0.787,0.852,0.918,1.0],
 [0.0,0.079,0.157,0.236,0.315,0.393,0.472,0.551,0.63,0.708,0.787,0.866,0.944,1.023,1.102,1.2],
 [0.0,0.092,0.184,0.275,0.367,0.459,0.551,0.643,0.734,0.826,0.918,1.01,1.102,1.193,1.285,1.4],
 [0.0,0.105,0.21,0.315,0.42,0.525,0.63,0.734,0.839,0.944,1.049,1.154,1.259,1.364,1.469,1.6],
 [0.0,0.118,0.236,0.354,0.472,0.59,0.708,0.826,0.944,1.062,1.18,1.298,1.416,1.534,1.652,1.8],
 [0.0,0.131,0.262,0.393,0.525,0.656,0.787,0.918,1.049,1.18,1.311,1.443,1.574,1.705,1.836,2.0]])
Nxy_comp=-18.68
fc_comp=-467.03
ft_comp=467.03
Tmax_comp=142.44
Cmax_comp=-294.53

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if max(abs(k-k_comp),abs((zCoord-zCoord_comp).max()),abs(Nxy-Nxy_comp),abs(fc-fc_comp),abs(ft-ft_comp),abs(Tmax-Tmax_comp),abs(Cmax-Cmax_comp))<0.01:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
