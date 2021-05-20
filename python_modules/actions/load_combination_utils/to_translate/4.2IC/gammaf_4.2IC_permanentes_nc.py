# -*- coding: utf-8 -*-
'''Coeficientes de ponderación según 4.2IC para acciones
permanentes de valor no constante en situación de CONSTRUCCIÓN.'''

\permanentes_nc
  {
    \defFamiliaAcciones["Terr"]{}
    \Terr
      {
        '''Coeficientes de ponderación para acciones permanentes
           de valor no constante debidas a acciones provocadas por empujes o movimientos del terreno.'''
        \gammaf
          {
            # ver tabla 1.1 de la colección de pequeñas obras de paso 4.2IC
            \gammaf_elu
               {
                 \favorable{0.9} \desfavorable{0.75*1.6}
                 \favorable_accidental{0} \desfavorable_accidental{1} # En situaciones accidentales.
               }
            \gammaf_els{ \favorable{1} \desfavorable{1} }
          }
      }
   }
