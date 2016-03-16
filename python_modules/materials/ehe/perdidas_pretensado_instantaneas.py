'''
Pérdidas por penetración de cuñas en tendones rectos cortos postesos
   o tendones pretesos según el apartado 20.2.2.1.2 de la EHE-08.
   a: Penetración de la cuña (lo usual: 5 mm)
   L: Longitud del tendón.
   Ep: Módulo de deformación longitudinal de la reinforcement activa (190e9).
'''
def getPerdidaTensionPenetracionCunaTendonRectoCorto(a, L, Ep):
  return a/L*Ep

'''
Pérdidas por acortamiento elástico del hormigón en tendones
   pretensados.
   sigma_cp: Tensión de compresión en el hormigón, a nivel del centro de gravedad de
             las reinforcement activas, producida por el pretensado una
             vez descontadas la pérdida por penetración de cuñas
   Ep: Módulo de deformación longitudinal de la reinforcement activa (190e9).
   Ecj: Módulo de deformación longitudinal del hoirmigón en el momento
        de la transferencia. 
'''
def getPerdidaTensionAcortElasticoTendonPreteso(sigma_cp, Ep, Ecj):
  return sigma_cp*Ep/Ecj
