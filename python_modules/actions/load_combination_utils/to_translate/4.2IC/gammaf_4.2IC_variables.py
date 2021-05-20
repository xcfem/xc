# -*- coding: utf-8 -*-
'''Coeficientes de ponderación según 4.2IC para acciones
variables en situación de CONSTRUCCIÓN.'''

\variables
  {
    \gammaf
      {
        # ver tabla 1.1 de la colección de pequeñas obras de paso 4.2IC
        \gammaf_elu
           {
             \favorable{0} \desfavorable{1.6}
             \favorable_accidental{0} \desfavorable_accidental{1} # En situaciones accidentales.
           }
        \gammaf_els{ \favorable{0} \desfavorable{1} }
      }
  }
