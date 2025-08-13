# -*- coding: utf-8 -*-
'''Functions to compute thermal loads according to Spanish IAPF-10 code.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2023,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import sys
import math
import scipy.interpolate
from misc_utils import log_messages as lmsg

# Data from table 2.11 of IAPF-10.
table_2_11_data= dict()
table_2_11_data['solid_slab']= {'K': 23.89, 'a':0.292, 'b':-0.139, 'c':0, 'h_min':0.30, 'h_max':1.20}
table_2_11_data['voided_slab_bridge']= {'K': 24.91, 'a':0.292, 'b':-0.172, 'c':0, 'h_min':0.60, 'h_max':1.50}
table_2_11_data['concrete_box_girder']= {'K': 29.13, 'a':0.301, 'b':-0.148, 'c':0, 'h_min':1.70, 'h_max':4.00}
table_2_11_data['concrete_beams']= {'K': 26.90, 'a':0.300, 'b':-0.088, 'c':0.057, 'h_min':0.60, 'h_max':2.50}
table_2_11_data['steel_box_girder']= {'K': 39.77, 'a':0.234, 'b':-0.069, 'c':0, 'h_min':1.50, 'h_max':4.50}
table_2_11_data['steel_beams']= {'K': 40.11, 'a':0.228, 'b':-0.072, 'c':0, 'h_min':2.00, 'h_max':6.00}

def get_annual_temp_variation(deck_type:str, z:float, h:float, s:float):
    ''' Return the annual variation of temperature according to clause 
        2.3.9.1.1. of IAPF-10.

    :param deck_type: deck type: solid_slab or voided_slab_bridge or concrete_box_girder or concrete_beams or steel_box_girder or steel_beams.
    :param z: arabic number corresponding to the climate zone.
    :param h: depth of the bridge deck.
    :param s: beam spacing (if deck_type is concrete_beams or steel_beams).
    '''
    retval= None
    if(deck_type!='steel_beams' and deck_type!='concrete_beams'): # no beams.
        s= 1.0
    if(deck_type in table_2_11_data.keys()):
        data= table_2_11_data[deck_type]
        K= data['K']
        a= data['a']
        b= data['b']
        c= data['c']
        h_min= data['h_min']
        h_max= data['h_max']
        h= min(h_max, h)
        h= max(h, h_min)
        retval= K*math.pow(z,a)*math.pow(h,b)
        if(s!=1.0):
            retval*= math.pow(s,c)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+"; deck type: '"+str(deck_type)+"' unknown.")
    return retval
        
# K1 factor interpolation according to figure 2.29 of IAPF-07.
fig_2_29_hi= [1.0, 1.5, 2.0, 2.5]
fig_2_29_k1_d_3_5= [1.2, 1.06, 0.96, 0.96]
fig_2_29_k1_d_3_0= [1.16, 1.00, 0.92, 0.92]
fig_2_29_k1_d_2_5= [1.12, 0.96, 0.86, 0.86]
fig_2_29_k1_d_2_0= [1.06, 0.91, 0.80, 0.77]
f_2_29_k1_d_3_5= scipy.interpolate.interp1d(fig_2_29_hi, fig_2_29_k1_d_3_5, kind='linear')
f_2_29_k1_d_3_0= scipy.interpolate.interp1d(fig_2_29_hi, fig_2_29_k1_d_3_0, kind='linear')
f_2_29_k1_d_2_5= scipy.interpolate.interp1d(fig_2_29_hi, fig_2_29_k1_d_2_5, kind='linear')
f_2_29_k1_d_2_0= scipy.interpolate.interp1d(fig_2_29_hi, fig_2_29_k1_d_2_0, kind='linear')

def get_k1_according_to_figure_2_29(h:float, d:float):
    ''' Return the value of the K1 factor according to figure 2.29 of IAPF-07.

    :param h: depth of the deck beams (m).
    :param d: distance between beams (m).
    '''
    retval= 1.2
    if (h<2.5) and (d< 3.5):
        if(d>=3.0):
            v35= float(f_2_29_k1_d_3_5(h))
            v30= float(f_2_29_k1_d_3_0(h))
            retval= (v35-v30)/0.5*(h-3.0)+v30
        elif(d>=2.5):
            v30= float(f_2_29_k1_d_3_0(h))
            v25= float(f_2_29_k1_d_2_5(h))
            retval= (v30-v25)/0.5*(h-2.5)+v25            
        elif(d>=2.0):
            v25= float(f_2_29_k1_d_2_5(h))
            v20= float(f_2_29_k1_d_2_0(h))
            retval= (v25-v20)/0.5*(h-2.0)+v20            
    else:
        methodName= sys._getframe(0).f_code.co_name
        errMsg= '; '
        if(h>2.5):
            errMsg+= 'beam depth: '+str(h)+ 'out of range (1, 2.5).\n'
        if(d>3.5):
            errMsg+= 'beam spacing: '+str(d)+ 'out of range (2, 3.5).\n'
        lmsg.error(methodName+errMsg)
    return retval

