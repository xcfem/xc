# Actions due to railway traffic on bridges.
Imposed loads (models and representative values) associated with rail traffic which include dynamic effects and centrifugal, braking and acceleration actions and actions for accidental design situations.

![Railway traffic load distribution](https://github.com/xcfem/xc/blob/master/doc/screenshots/railway_traffic_load_distribution.png)

### Dynamic load factors.
Dynamic load factors (ϕ) relate the magnitude of vertical wheel-to-rail loads in operation (dynamic loads) to static loads resulting from the weight of the vehicle and its contents, as a function of train speed.

### Classification factor
Load factor used normally to introduce future variations in the loads introduced by the trains.

### Slipstream
A slipstream is a region behind a moving object in which a wake of fluid (typically air or water) is moving at velocities comparable to that of the moving object, relative to the ambient fluid through which the object is moving.

### Centrifugal forces
The values of the centrifugal forces depend on the code you are using, so they are defined in the derived classes inside the module for the specific code (for the time being, EC1_rail_load_models). The methods that create the corresponding nodal loads in the generic classes (TrackAxis, LocomotiveLoad) receive the values of the centrifugal forces already computed according to the code.

### Nosing load
A single nominal load, acting horizontally in either direction at right angles to the track at the top of the rail is taken to provide for the lateral effect of the nosing of vehicles, such as locomotives.

## References

- [Eurocode 1: Actions on structures - Part 2: Traffic loads on bridges.](https://www.en.une.org/encuentra-tu-norma/busca-tu-norma/norma/?c=norma-une-en-1991-2-2019-n0061460)
- [Eurocode 1: Actions on structures](https://en.wikipedia.org/wiki/Eurocode_1:_Actions_on_structures)
- [Slipstream](https://en.wikipedia.org/wiki/Slipstream)
- [Track geometry](https://en.wikipedia.org/wiki/Track_geometry)
- [1435 mm track gauge](https://www.wikidata.org/wiki/Q1999572)
- [Hunting oscillation](https://en.wikipedia.org/wiki/Hunting_oscillation)
- [Actions on bridges](https://fgg-web.fgg.uni-lj.si/~/pmoze/esdep/master/wg15b/l0200.htm)
