# -*- coding: utf-8 -*-
''' Check SDFResponse class.

Test inspired on the example «Elastic Response Spectra» by Amir Hossein 
Namadchi.

See https://github.com/AmirHosseinNamadchi/OpenSeesPy-Examples/blob/master/Elastic%20Response%20Spectra.ipynb

See also https://openseesdigital.com/2021/02/14/how-many-clicks-does-it-take/
'''

import math
import os
import json
import xc
import numpy as np
from scipy.constants import g
from actions.quake import response_spectrum as rs

silent= True

# Loading El Centro EQ data (North-south component)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
accelFilePath= pth+'/../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)
timeValues= el_centro_raw[:,0]
accelerationValues= list(el_centro_raw[:,1]*g)

# a list of damping ratios to be included
zeta_list= [0.02, 0.03, 0.05]
T_min= 0.00001
T_max= 5
dtA= timeValues[1]-timeValues[0] # time step for input data.
Fy= 1e16 # yielding strength (big value, so no yielding at all).
alpha= .01 # strain-hardening ratio (no yielding so does not matter).

data_frame= rs.compute_response_spectrum(accelerations= accelerationValues, dtA= dtA, dt= None, zLst= zeta_list, T_min= T_min, T_max= T_max, Fy= Fy, alpha= alpha, silent= silent)

refFilePath= pth+"/../../aux/reference_files/"
fname= os.path.basename(__file__)
jsonFileName= refFilePath+'/ref_'+fname.replace('.py', '.json')
# # Save output as reference.
# with open(jsonFileName, 'w') as f:
#     json.dump(data_frame, f)

# Check results.
## Load reference values.
with open(jsonFileName, 'r') as f:
     ref_data_frame= json.load(f)
## Compare results.
err= 0.0
tol= 1e-8
for key in ref_data_frame:
    ref_resp= ref_data_frame[key]
    ref_z= float(key)
    resp= data_frame[ref_z]
    for key in ref_resp:
        values= resp[key]
        ref_values= ref_resp[key]
        error= (len(values)-len(ref_values))**2
        if(error<tol):
            for v, rv in zip(values, ref_values):
                error+=(v-rv)**2
error= np.sqrt(error)

# print(error)

from misc_utils import log_messages as lmsg
if error<tol:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

if(not silent):
    import matplotlib.pyplot as plt
    ## Plot accelerogram.
    plt.figure(figsize=(15,3))
    plt.plot(el_centro_raw[:,0], el_centro_raw[:,1], color='k')

    plt.ylabel('$\\ddot{d_g} (g)$', {'size':14})
    plt.xlabel('Time (sec)', {'fontstyle':'italic','size':13})

    plt.grid()
    plt.yticks(fontsize= 14)
    plt.xticks(fontsize= 14)
    plt.xlim([0.0, el_centro_raw[-1,0]]);
    plt.show()

    ## Displacment -----------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SD'],
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative displacement (m)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (s)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative displacement response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    # Velocity ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], data_frame[z]['SV'],
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative velocity (m/s)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (s)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative veloctiy response spectrum', 
              {'fontstyle':'italic','size':18});
    plt.show()

    # Acceleration ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], np.array(data_frame[z]['SA'])/g,
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('Relative acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('Relative acceleration response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()
    
    # True acceleration ------------
    plt.figure(figsize=(14,5))

    [plt.plot(data_frame[z]['T'], np.array(data_frame[z]['STA'])/g,
              label=('$\\zeta$ = '+str(z))) for z in zeta_list]

    plt.ylabel('True acceleration (g)', {'fontstyle':'italic','size':14})
    plt.xlabel('Period (sec)', {'fontstyle':'italic','size':14})
    plt.legend()
    plt.grid()
    plt.yticks(fontsize = 14)
    plt.xticks(fontsize = 14)
    plt.title('True acceleration response spectrum',
              {'fontstyle':'italic','size':18});
    plt.show()

    
