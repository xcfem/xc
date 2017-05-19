# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com"

def getPerdidaTensionPenetracionCunaTendonRectoCorto(a, L, Ep):
  '''
  Pérdidas por penetración de cuñas en tendones rectos cortos postesos
   o tendones pretesos según el apartado 20.2.2.1.2 de la EHE-08.

   a: Penetración de la cuña (lo usual: 5 mm)
   L: Longitud del tendón.
   Ep: Módulo de deformación longitudinal de la reinforcement activa (190e9).
  '''
  return a/L*Ep

def getPerdidaTensionAcortElasticoTendonPreteso(sigma_cp, Ep, Ecj):
  '''
  Pérdidas por acortamiento elástico del hormigón en tendones pretensados.

   sigma_cp: Tensión de compresión en el hormigón, a nivel del centro de gravedad de
             las reinforcement activas, producida por el pretensado una
             vez descontadas la pérdida por penetración de cuñas
   Ep: Módulo de deformación longitudinal de la reinforcement activa (190e9).
   Ecj: Módulo de deformación longitudinal del hormigón en el momento
        de la transferencia. 
  '''
  return sigma_cp*Ep/Ecj
