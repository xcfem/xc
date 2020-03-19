Explanation for the contents of this directory has been taken from
this [post](https://portwooddigital.com/2019/12/08/an-update-of-the-imk-models/).

The degrading bilinear, pinching, and peak-oriented models that would become the IMK (Ibarra-Medina-Krawinkler) were implemented by Ibarra and Krawinkler in a structural analysis code known as SNAP (see Chapter 3 of this [report](https://stacks.stanford.edu/file/druid:dj885ym2486/TR152_Ibarra.pdf)). In the early 2000s, the SNAP models were recoded in C++ for OpenSees and still reside in the src/material/uniaxial/snap directory. You can use these models, collectively known as the IMK models, via the uniaxialMaterial commands Bilinear, Pinching, and Clough.
