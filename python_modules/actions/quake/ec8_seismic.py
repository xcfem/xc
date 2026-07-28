# -*- coding: utf-8 -*-
'''Functions to compute seismic loads according to EN 1998-1:2004 y AN/UNE-EN 1998-1:2022.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import numpy as np
from misc_utils import log_messages as lmsg

importance_class={
    'I':{'descr':'importancia moderada',
         'factor':0.8,
         },
    'II':{'descr':'importancia normal',
         'factor':1,
         },
    'III':{'descr':'mayor importancia',
         'factor':1.3,
         },
    'IV':{'descr':'importancia especial',
         'factor':1.4,
         },
    }

# Parameters S, TB , TC y TD recommended for response spectrum type 2 [UNE-EN 1998-1,Table 3.3]
param_spectrum_type2={
    'A':{'S':1.0, 'T_B':0.05, 'T_C':0.25,'T_D':1.2},
    'B':{'S':1.35, 'T_B':0.05, 'T_C':0.25,'T_D':1.2},
    'C':{'S':1.5, 'T_B':0.10, 'T_C':0.25,'T_D':1.2},
    'D':{'S':1.8, 'T_B':0.10, 'T_C':0.30,'T_D':1.2},
    'E':{'S':1.6, 'T_B':0.05, 'T_C':0.25,'T_D':1.2},
    }

def get_a_g(a_gR,impClass):
    '''
    Return the design horizontal acceleration (g)

    :param a_gR: horizontal acceleration (g) in soil type A
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    '''
    if impClass not in ['I', 'II', 'III', 'IV']:
        lmsg.error("importance class "+impClass+ " unknown. Only classes 'I', 'II', 'III' or 'IV' are considered"); exit(1)
    else:
        gamma_f=importance_class[impClass]['factor']
        a_g=gamma_f*a_gR
    return a_g

def get_param_horiz_spectrum_type1(a_gR,K,soilTyp,v_s30,impClass):
    ''' Return parameters S, TB , TC y TD that define the shape of the horizontal elastic response spectrum [AN/UNE-EN 1998-1, table AN/2 (Table 3.2)
    
    :param a_gR: horizontal acceleration (g) in soil type A
    :param K: coefficient of contribution Azores-Gibraltar
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    '''
    a_g=get_a_g(a_gR,impClass)
    C=round((800/v_s30)**0.465,3)
    T_D=2
    if soilTyp=='A':
        S=1; T_D=2
        T_C=round(K/4,3)
    elif soilTyp in ['B','C']:
        C=round((800/v_s30)**0.465,3)
        T_C=round(K*C/4,3)
        if a_g <=0.1:
            S=C
        elif a_g <=0.4:
            S=round(C+3.33*(a_g-0.1)*(1-C),3)
        else:
            S=1
    elif soilTyp=='D':
        T_C=K/2
        if a_g <=0.1:
            S=2
        elif a_g <=0.4:
            S=round(2.33+3.33*a_g,3)
        else:
            S=1
    else:
        lmsg.error("soil type "+soilTyp+"; unknown. Only types 'A', 'B', 'C' or 'D' are considered"); exit(1)
    T_B=round(T_C/5,3)
    return S, T_B , T_C, T_D
                   
def get_param_horiz_spectrum_type2(soilTyp):
    ''' Return parameters S, TB , TC y TD that define the shape of the horizontal elastic response spectrum type 2 [UNE-EN 1998-1,Table 3.3]

    :param soilTyp: soil type ('A', 'B', 'C', 'D' or 'E') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    '''
    if soilTyp not in ['A', 'B', 'C', 'D', 'E']:
        lmsg.error("soil type "+soilTyp+"; unknown. Only types 'A', 'B', 'C' , 'D' or 'E' are considered"); exit(1)
    else:
        param=param_spectrum_type2[soilTyp]
        S=param['S']
        T_B=param['T_B']
        T_C=param['T_C']
        T_D=param['T_D']
    return S, T_B , T_C, T_D
    
    
def get_coef_shape_horiz_spectra(T,T_B , T_C, T_D,eta):
    ''' Return the coefficent that multiplies a_g*S in obtaining the spectrums

    :param T: period (s)
    :param T_B , T_C, T_D: parameteters that define the shape of the horizontal elastic response spectrum
    :param eta: damping coefficient (= 1 for 5%) 
    '''
    if 0 <=T and T<=4:
        if T <= T_B:
            coef=(1+T/T_B*(eta*2.5-1))
        elif T <= T_C:
            coef=eta*2.5
        elif T <= T_D:
            coef=eta*2.5*T_C/T
        else:
            coef=eta*2.5*T_C*T_D/T**2
        return coef
    
        

def ec8_like_target_horiz_spectrum_type1(periods, a_gR,K,soilTyp,v_s30,impClass,eta=1):
    """
    Simple EC8 - style design 5%-damped horizontal response spectrum shape according to
    UNE-EN 1998-1:2018 §3.2.2.2
    
    :param periods: list of periods
    :param a_gR: horizontal acceleration (g) in soil type A
    :param K: coefficient of contribution Azores-Gibraltar
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    :param eta: damping coefficient (= 1 for 5%)
    """
    a_g=get_a_g(a_gR,impClass)
    S, T_B , T_C, T_D=get_param_horiz_spectrum_type1(a_gR,K,soilTyp,v_s30,impClass)
    Sa = np.zeros_like(periods)
    for i, T in enumerate(periods):
        coef=get_coef_shape_horiz_spectra(T,T_B , T_C, T_D,eta)
        Sa[i]=a_g*S*coef
    return Sa

def ec8_like_target_horiz_spectrum_type2(periods, a_gR,soilTyp,impClass,eta=1):
    """
    Simple EC8 - style design 5%-damped horizontal response spectrum shape according to
    UNE-EN 1998-1:2018 §3.2.2.2
    
    :param periods: list of periods
    :param a_gR: horizontal acceleration (g) in soil type A
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    :param eta: damping coefficient (= 1 for 5%) (defaults to 1)
    """
    a_g=get_a_g(a_gR,impClass)
    S, T_B , T_C, T_D=get_param_horiz_spectrum_type2(soilTyp)
    Sa = np.zeros_like(periods)
    for i, T in enumerate(periods):
        coef=get_coef_shape_horiz_spectra(T,T_B , T_C, T_D,eta)
        Sa[i]=a_g*S*coef
    return Sa


def get_coef_shape_vertical_spectra(T,T_vB , T_vC, T_vD,eta=1):
    ''' Return the coefficent that multiplies a_g*S in obtaining the spectrums

    :param T: period (s)
    :param T_vB , T_vC, T_vD: parameteters that define the shape of the vertical elastic response spectrum
    :param eta: damping coefficient (= 1 for 5%) 
    '''
    beta0=3
    if 0 <=T and T<=4:
        if T <= T_vB:
            coef=1+T/T_vB*(eta*beta0-1)
        elif T <= T_vC:
            coef=eta*beta0
        elif T <= T_vD:
            coef=eta*beta0*T_vC/T
        else:
            coef=eta*beta0*T_vC*T_vD/T**2
    return coef

def get_param_vertical_spectrum_type1(a_gR,K,soilTyp,v_s30,impClass):
    ''' Return parameters TvB , TvC y TvD that define the shape of the vertical elastic response spectrum [AN/UNE-EN 1998-1, table AN/2 (Table 3.2)
    
    :param a_gR: horizontal acceleration (g) in soil type A
    :param K: coefficient of contribution Azores-Gibraltar
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    '''
    S, T_B , T_C, T_D=get_param_horiz_spectrum_type1(a_gR,K,soilTyp,v_s30,impClass)
    T_vB=1.0*T_B
    T_vC=0.75*T_C
    T_vD=1.0*T_D
    return T_vB,T_vC,T_vD

def get_param_vertical_spectrum_type2(soilTyp):
    ''' Return parameters TvB , TvC y TvD that define the shape of the vertical elastic response spectrum type 2 [UNE-EN 1998-1,Table 3.3]

    :param soilTyp: soil type ('A', 'B', 'C', 'D' or 'E') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    '''
    S, T_B , T_C, T_D=get_param_horiz_spectrum_type2(soilTyp)
    T_vB=1.0*T_B
    T_vC=0.75*T_C
    T_vD=1.0*T_D
    return T_vB,T_vC,T_vD

def get_a_vg(a_gR,impClass):
    '''
    Return the design horizontal acceleration (g)

    :param a_gR: horizontal acceleration (g) in soil type A
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    '''
    a_g=get_a_g(a_gR,impClass)
    a_vg=0.7*a_g
    return a_vg
    

def ec8_like_target_vertical_spectrum_type1(periods, a_gR,K,soilTyp,v_s30,impClass,eta=1):
    """
    Simple EC8 - style design 5%-damped response vertical spectrum shape according to
    UNE-EN 1998-1:2018 §3.2.2.2
    
    :param periods: list of periods
    :param a_gR: horizontal acceleration (g) in soil type A
    :param K: coefficient of contribution Azores-Gibraltar
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    :param eta: damping coefficient (= 1 for 5%)
    """
    a_vg=get_a_vg(a_gR,impClass)
    
    T_vB , T_vC, T_vD=get_param_vertical_spectrum_type1(a_gR,K,soilTyp,v_s30,impClass)
    Sa = np.zeros_like(periods)
    for i, T in enumerate(periods):
        coef=get_coef_shape_vertical_spectra(T,T_vB , T_vC, T_vD,eta)
        Sa[i]=a_vg*coef
    return Sa
    
def ec8_like_target_vertical_spectrum_type2(periods, a_gR,soilTyp,impClass,eta=1):
    """
    Simple EC8 - style design 5%-damped response vertical spectrum shape according to
    UNE-EN 1998-1:2018 §3.2.2.2
    
    :param periods: list of periods
    :param a_gR: horizontal acceleration (g) in soil type A
    :param soilTyp: soil type ('A', 'B', 'C' or 'D') according to AN/UNE-EN 1998-1, table AN/1 (Table 3.1)
    :param v_s30: average speed (m/s) of the shear waves in the first 30 m of the ground [AN/UNE-EN 1998-1, tabla AN/1 (Tabla 3.1)]
    :param impClass: importancd class of the structure ('I', 'II', 'III', 'IV')
    :param eta: damping coefficient (= 1 for 5%) (defaults to 1)
    """
    a_vg=get_a_vg(a_gR,impClass)
    T_vB , T_vC, T_vD=get_param_vertical_spectrum_type2(soilTyp)
    Sa = np.zeros_like(periods)
    for i, T in enumerate(periods):
        coef=get_coef_shape_vertical_spectra(T,T_vB , T_vC, T_vD,eta)
        Sa[i]=a_vg*coef
    return Sa
