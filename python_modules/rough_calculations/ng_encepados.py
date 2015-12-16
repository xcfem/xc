#----------------------------------------------------------------------------
#  programa XC; cálculo mediante el método de los elementos finitos orientado
#  a la solución de problemas estructurales.
#
#  Copyright (C)  Luis Claudio Pérez Tato
#
#  El programa deriva del denominado OpenSees <http:#opensees.berkeley.edu>
#  desarrollado por el «Pacific earthquake engineering research center».
#
#  Salvo las restricciones que puedan derivarse del copyright del
#  programa original (ver archivo copyright_opensees.txt) este
#  software es libre: usted puede redistribuirlo y/o modificarlo 
#  bajo los términos de la Licencia Pública General GNU publicada 
#  por la Fundación para el Software Libre, ya sea la versión 3 
#  de la Licencia, o (a su elección) cualquier versión posterior.
#
#  Este software se distribuye con la esperanza de que sea útil, pero 
#  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
#  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
#  Consulte los detalles de la Licencia Pública General GNU para obtener 
#  una información más detallada. 
#
# Debería haber recibido una copia de la Licencia Pública General GNU 
# junto a este programa. 
# En caso contrario, consulte <http:#www.gnu.org/licenses/>.
#----------------------------------------------------------------------------
#ng_encepados.py

def getTracArmaduraInfEncepado(alpha,Nd):
    '''
    Devuelve la traccion en la armadura inferior del encepado
    alpha: Ángulo de la biela a compresión con la horizontal.
    Nd: Axil de cálculo en el pilote.
    '''
    return Nd/math.tan(alpha)
  

def getAreaNecArmaduraInfEncepado(alpha,Nd,fyd):
    '''
    Devuelve el área necesaria para la armadura inferior del encepado
    alpha: Ángulo de la biela a compresión con la horizontal.
    Nd: Axil de cálculo en el pilote.
    '''
    return getTracArmaduraInfEncepado(alpha,Nd)/fyd
  

def getTracArmaduraInfEncepado2Pilotes(v,d,Nd):
    '''
    Devuelve la traccion en la armadura inferior de un encepado
    de DOS pilotes (ver números gordos HC.9 página 32).
    v: Distancia entre la pila y el pilote.
    d: Canto útil del encepado.
    Nd: Axil de cálculo en la pila.
    '''
    return Nd*v/2/d
  

def getCantoMinimoEncepado(diam,D):
    '''
    Devuelve el depth mínimo del encepado
    diam: Diámetro de la armadura del pilar o del pilote (la que sea mayor).
    D: Diámetro del pilote.
    '''
    return max((15*diam*diam)*100,max(D,0.4))
  

def getDistMinEjesPilotes(D):
    '''
    Devuelve la distancia mínima entre ejes de pilotes
    D: Diámetro del pilote.
    '''
    if D<0.5:
        return 2*D
    else:
        return 3*D
  

def getAreaArmaduraCercosVertEncepado2Pilotes(b,h,L):
    '''
    Devuelve el área de armadura necesaria para los cercos
    verticales de un encepado de DOS pilotes (ver números gordos
    HC.9 página 33).
    b: Ancho del encepado.
    h: Canto del encepado.
    L: Longitud del encepado.
    '''
    return 4*min(b,h/2)*L/1000
  

def getAreaArmaduraCercosHorizEncepado2Pilotes(b,h):
    '''
    Devuelve el área de armadura necesaria para los cercos
    horizontales de un encepado de DOS pilotes (ver números gordos
    HC.9 página 33).
    b: Ancho del encepado.
    L: Longitud del encepado.
    '''
    return 4*h*min(b,h/2)/1000
  
