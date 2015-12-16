# -*- coding: utf-8 -*-


# Hormigón
'''
XXX: Crear una clase base en Python para los hormigones de EHE y del código ACI.

   nmbHorm: Nombre del hormigón.
   fck: Resistencia característica a compresión (Pa).
   fcd: Resistencia de cálculo a compresión (Pa).
  
# Define las propiedades "derivadas" del material.
def setupHormigon(nmbHormigon, gammaC):
    \nmbHormigon
      {
        nmbMaterial= nmbHormigon #Nombre del material.
        nmbDiagK= "dgK"+nmbHormigon #Nombre para el diagrama característico.
        matTagK= -1 #Tag del material uniaxial con el diagrama característico del material.
        nmbDiagD= "dgD"+nmbHormigon #Nombre para el diagrama de cálculo.
        matTagD= -1 #Tag del material uniaxial con el diagrama de cálculo del material.
        gmmC= gammaC #Minoración de la resistencia del hormigón.
        fcd= fck/gmmC #Resistencia de cálculo a compresión del hormigón (en Pa).
        fmaxK= 0.85*fck
        fmaxD= 0.85*fcd
        fctm= fctMedEHE08(fck) #Resistencia media a tracción (en Pa).
        fctd= fctkEHE08(fck)/gammaC #Resistencia de cálculo a tracción del hormigón (en Pa).
        epsilon0= -2e-3 #Deformación para tensión máxima (de compresión).
        epsilonU= -3.5e-3 #Deformación para tensión última (de compresión).
        nuc= 0.2 #Coeficiente de Poisson del hormigón.
        Ecm= getEcm(fck) #Modulo de deformación longitudinal secante a 28 días.
        Gcm= Ecm/(2*(1+nuc)) #Modulo de deformación transversal.
      

  }

def defDiagKHormigon(nmbMat):
    \nmbMat
      {
        \concrete01_material[nmbDiagK]
          {
            \epsc0{epsilon0} # Deformación para tensión máxima (de compresión).
            \fpc{fmaxK} \fpcu{fmaxK}
            \epscu{epsilonU} # Deformación para tensión última (de compresión).
            matTagK= tag
          }
        return matTagK
      }
  }

def defDiagDHormigon(nmbMat):
    \nmbMat
      {
        \concrete01_material[nmbDiagD]
          {
            \epsc0{epsilon0} # Deformación para tensión máxima (de compresión).
            \fpc{fmaxD} \fpcu{fmaxD}
            \epscu{epsilonU} # Deformación para tensión última (de compresión).
            matTagD= tag
          }
        return matTagD
  }

# Diagrama tensión deformación del hormigón según EHE.
def sigmac(eps,fcd):
    if(eps<0):
        \then
          {
            if(eps>-2e-3):
                \then
                  { return (-850*fcd*eps*(250*eps+1))} 
                else:
                  { return (0.85*fcd)} 
              }
          }
        else:  return 0.0} 
      }
  }

# Módulo de deformación tangente del hormigón según EHE.
def tangc(eps,fcd):
    if(eps<0):
        \then
          {
            if(eps>-2e-3):
                \then
                  { return (-850*fcd*(500*eps+1))} 
                else:
                  { return 0.0} 
              }
          }
        else:  return 0.0} 
      }
  }

# Diagrama característico tensión deformación del hormigón según EHE.
def sigmaKHormigon(eps,record mat):
    return sigmac(eps,mat.fck)

# Diagrama característico tensión deformación del hormigón según EHE.
def sigmaDHormigon(eps,record mat):
    return sigmac(eps,mat.fcd)

# Diagrama característico tensión deformación del hormigón según EHE.
def tangDHormigon(eps,record mat):
    return tangc(eps,mat.fcd)

# Comprueba la corrección de los resultados del diagrama de cálculo del modelo de elementos finitos.
def testDiagDHormigon(record mat):
    tag= defDiagDHormigon(mat.nmbMaterial)
    nmbDiagHormigon= mat.nmbDiagD
    incr= mat.epsilonU/20
    errMax= 0.0
    \nmbDiagHormigon
      {
        errMax= 0.0
           for      \inicio{e= -0.1e-8}} \continua{e>=mat.epsilonU} \incremento{\expr{e=(e+incr)}
            \bucle
              {
                \set_trial_strain{e} \commit
                sg= sigmaDHormigon(e,mat)
                err= abs((sg-stress)/sg)
                errMax= max(err,errMax)
              }
          }
      }
    return errMax
# Comprueba la corrección de los resultados de la tangente al diagrama de cálculo del modelo de elementos finitos.
def testTangDHormigon(record mat):
    tag= defDiagDHormigon(mat.nmbMaterial)
    nmbDiagHormigon= mat.nmbDiagD
    incr= mat.epsilonU/20
    errMax= 0.0
    \nmbDiagHormigon
      {
        incr= mat.epsilonU/30
           for      \inicio{e= -0.1e-8}} \continua{e>=mat.epsilonU} \incremento{\expr{e=(e+incr)}
            \bucle
              {
                \set_trial_strain{e};
                \commit
                tg= tangDHormigon(e,mat)
                err= abs(tg-getTangent)
                errMax= max(err,errMax)
              }
          }
      }
    return errMax

A36M= 
\A36M
  {
    fck= -24.5e6 #Resistencia característica a compresión del hormigón (en Pa).
  
'''
