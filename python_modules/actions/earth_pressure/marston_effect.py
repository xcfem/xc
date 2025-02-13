# -*- coding: utf-8 -*-
'''Calculation of vertical pressure increment due to negative friction effect 
   on the earth pressure according to clause 6.6.1 of the publication: 
        
   "Guia de cimentaciones en  obras de carretera". 
   ISBN 8449808626, 9788449808623
   https://books.google.es/books/about/Gu%C3%ADa_de_cimentaciones_en_obras_de_carre.html?hl=de&id=a0eoygAACAAJ&redir_esc=y
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import math

def marston_adimensional_factor(D, H, Er, Et):
    ''' Return the value of the adimensional factor for the calculation of the 
        negative friction effect on the earth pressure according to clause 
        6.6.1 of the publication "Guia de cimentaciones en obras de carretera". 
        ISBN 8449808626, 9788449808623
        https://books.google.es/books/about/Gu%C3%ADa_de_cimentaciones_en_obras_de_carre.html?hl=de&id=a0eoygAACAAJ&redir_esc=y

    :param D: overburden.
    :param H: height of the structure.
    :param Er: elastic modulus of the soil over the structure.
    :param Et: elastic modulus of the soil at both sides of the structure (transition wedge).
    '''
    return D/H*Et/Er+1

def marston_vertical_pressure_increment(gammaSoil, D, H, B, Er, Et):
    ''' Return the vertical pressure increment due to negative friction effect 
        on the earth pressure according to clause 6.6.1 of the publication
        "Guia de cimentaciones en  obras de carretera". 
        ISBN 8449808626, 9788449808623
        https://books.google.es/books/about/Gu%C3%ADa_de_cimentaciones_en_obras_de_carre.html?hl=de&id=a0eoygAACAAJ&redir_esc=y

    :param gammaSoil: unit weight of the backfill.
    :param D: overburden.
    :param H: height of the structure.
    :param B: structure width.
    :param Er: elastic modulus of the soil over the structure.
    :param Et: elastic modulus of the soil at both sides of the structure
               (transition wedge).
    '''
    r_max= 0.3*D**2/B
    f= marston_adimensional_factor(D= D, H= H, Er= Er, Et= Et)
    retval= min((D+H/2.0)/f, r_max)*gammaSoil
    return retval
