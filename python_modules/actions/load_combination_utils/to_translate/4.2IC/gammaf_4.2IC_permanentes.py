# -*- coding: utf-8 -*-
# Coeficientes de ponderación según 4.2IC para acciones permanentes en situaciones de CONSTRUCCIÓN.
\permanentes
  {
    \gammaf
      {
        # ver tabla 1.1 de la colección de pequeñas obras de paso 4.2IC
        \gammaf_elu
          {
           \favorable{0.9} \desfavorable{1.6}# En situaciones persitentes o transitorias:
           \favorable_accidental{1} \desfavorable_accidental{1}# En situaciones accidentales:
          }
        \gammaf_els{ \favorable{1} \desfavorable{1} }
      }
  }
