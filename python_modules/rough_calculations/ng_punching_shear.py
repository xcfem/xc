# -*- coding: utf-8 -*-
''' Rough design of concrete slabs under punching loads according to the
    publication: "Números gordos en el proyecto de estructuras" by
    Juan Carlos Arroyo et al. ISBN: 97884932270-4-3. Cinter 2009.

   https://books.google.es/books?id=tf5-tgAACAAJ
''' 
__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega (AO_O) "
__copyright__= "Copyright 2017, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math

def designPunchingLoad(qk,A):
    '''Rough estimation of the punching shear load over a column
      (HL.3 números gordos).
    
    :param qk: characteristic value of the total uniform load on the slab.
    :param A: column tributary area.
    '''
    return 1.6*qk*A

def maximum_punching_shear_load(concrete,d,a,b):
    ''' Estimate of the maximum punching shear resistance along the section 
        at the intersection of the support contour with the deck
        (HL.3 números gordos).

        (This is the maximum punching shear load you can transfer to the
         column through the contact surface. This is the same type of
         limitation that the maximum shear forde that the strut can
         support in a )

        If the shear load to resist is greater, then we need to.

          - increment the support dimensions (the cheaper solution)
          - increment the slab thickness (the better solution)
          - increase the concrete strength
   
        :param concrete: concrete material (N/m2)
        :param d: effective depth of the slab (m)
        :param a,b: dimensions of the column (m)

        Result is expressed in N
    '''
    fcd= -concrete.fcd()  # concrete design compressive strength
    nu= 0.6*(1-concrete.fckMPa()/250) # EC2:2004 eq. (6.6N) (recommended value).
    return 0.4*nu*fcd*2*d*(a+b)

def concrete_resisted_punching_load(concrete, d, a, b, criticalPerimeterLength= None):
    ''' Approximate design value of punching shear resistance of a slab without 
        punching shear reinforcement, computed at the critical perimeter
        defined to occur at d/2 from the column faces (HL.3 números gordos)

       :param concrete: concrete material.
       :param d: effective depth of the floor deck
       :param a,b: dimensions of the column
       :param criticalPerimeterLength: length of the critical perimeter.
    '''
    fck= concrete.fckMPa()
    k= min(1+math.sqrt(.2/d),2.0)
    # Expresion based on EC2:2004 clause 6.2.2 equation (6.2.b).
    tau= 0.035*pow(k,3/2.0)*math.sqrt(fck)*1e6 # About 0.5 MPa
    if(criticalPerimeterLength is None):
        criticalPerimeterLength= 2*(a+b+2*math.pi*d) # The correct formula is
                                                     # this. Ther is an error
                                                     # in the book.
    criticalArea= criticalPerimeterLength*d
    return tau*criticalArea
    

def punching_shear_reinforcement_area(concrete, steel, Vd, d, a, b, h, criticalPerimeterLength= None):
    ''' Estimate the needed punching shear reinforcement area
        computed at the critical perimeter defined to occur at 
        2d from the column faces (HL.3 números gordos)

    :param concrete: concrete material.
    :param steel: steel material.
    :param Vd: Desing value of the punching shear (N)
    :param d: effective depth of the floor deck (m)
    :param h: slab depth (m).
    :param a,b: dimensions of the column (m)
    :param criticalPerimeterLength: length of the critical perimeter.
    '''
    Vcu= concrete_resisted_punching_load(concrete= concrete, a= a, b= b, d= d, criticalPerimeterLength= criticalPerimeterLength)
    if(abs(Vd) < Vcu):
        return 0.0
    else:
        fyalfad=min(400,steel.fyd()/1e6)
        Aalfa=(abs(Vd)-0.5*Vcu)/0.8/h/fyalfad*10/1e7
        return Aalfa

