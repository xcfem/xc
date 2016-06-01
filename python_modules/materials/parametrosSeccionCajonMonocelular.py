# -*- coding: utf-8 -*-

from __future__ import division

import math

##   Devuelve el valor de la inercia a torsión de un cajón
##   monocelular según el libro Puentes (apuntes para su diseño
##   y construcción) de Javier Manterola Armisén (apartado 5.2.3 página 251)
##
##                        bs
##             |----------------------|
##
##                        ts
##    -    ---------------------------------
##    |         \                    /
##    |h         \td                /
##    |           \       ti       /
##    -            ----------------
##
##                        bi
##                 |--------------|
##
##   siendo:
##     -bs: Ancho de la losa superior del cajón (esto es, descontando los voladizos)
##     -bi: Ancho de la losa inferior del cajón.
##     -ts: Espesor de la losa superior del cajón.
##     -ti: Espesor de la losa inferior del cajón.
##     -td: Espesor de las almas.
##     -h: Canto del cajón (entre planos medios).

def getInerciaTorsionCajonMonocelular(bs,bi,h,ts,ti,td):
    '''
    Devuelve el valor de la inercia a torsión de un cajón monocelular

    :param bs: Ancho de la losa superior del cajón (esto es, descontando los voladizos)
    :param bi: Ancho de la losa inferior del cajón.
    :param ts: Espesor de la losa superior del cajón.
    :param ti: Espesor de la losa inferior del cajón.
    :param td: Espesor de las almas.
    :param h: Canto del cajón (entre planos medios).
    '''
    longAlma=math.sqrt(h**2+((bs-bi)/2)**2)
    return (bs+bi)**2*h**2/(bs/ts+2*longAlma/td+bi/ti)
