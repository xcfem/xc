      program test1
c
c     changes to be made for use on different computers --
c     1. remove or change program line above or open line below
c     2. change the value of srelpr below and in itpack routine dfault
c     3. changes in the itpack timing routine timer
c
c ... test1 is a program designed to test itpack 2c methods on
c ... matrices arising from the symmetric five point discretization
c ... of two dimensional elliptic partial differential equations on
c ... a rectangle with a rectangular mesh.  all seven methods from
c ... itpack 2c are tested and a summary is printed at the end.
c
c     the exact size of the test problems can be increased or
c     decreased by changing the array size in dimension statements
c     and the variables listed below under size of test problem.
c     also, the number of times through the test loops can be reduced 
c     by changing  icase and jcase  as follows.
c
c          icase = 1 for symmetric case 
c                = 2 for symmetric and nonsymmetric cases
c          jcase = 1 for natural ordering test
c                = 2 for natural and red-black ordering tests
c
c     array dimensioning
c
      parameter (nside=19)
      parameter (nlen=nside**2)
      parameter (itm=110)
      parameter (nww=6*nlen+4*itm)
      parameter (nside2=nside+2)
      parameter (nlen3=nlen*3)
      dimension coef(nlen,5),rhs(nlen),u(nlen),wksp(nww),rparm(12),
     *          gridx(nside2),gridy(nside2)
      integer   jcoef(nlen,5),iwksp(nlen3),iparm(12),bctype(4)
      integer   iter(7), iwrk(7)
      dimension digit1(7),digit2(7),tim1(7),tim2(7)
c
c ... sym5pt common blocks
c
      common /tstbk1/ ax,ay,bx,by,srelpr,hx,hy
      common /tstbk2/ bctype,ilevel,isym,nout,ngridx,ngridy,n
c
      open (unit=6,file='output')
c
c ... initialize integer constants which control output and define
c ... array dimensions. they are
c
c        srelpr  -  machine precision
c        nout    -  fortran output unit 
c        level   -  level of output from itpack 2c
c        ieran   -  error analysis switch
c        itmax   -  maximum number of iterations allowed
c        zeta    -  stopping criterion
c        nw      -  size of the array wksp
c        ilevel  -  0/1 level of output from sym7pt
c        ndim    -  row dimension of coef and jcoef arrays in array
c                   declarations
c        maxnz   -  maximum number of non-zero elements in any row
c        ngridx  -  number of horizontal mesh planes
c        ngridy  -  number of vertical mesh planes
c        ngrdxd  -  maximum number of vertical mesh planes including the
c                   vertical boundary planes
c        ngrdyd  -  maximum number of horizontal mesh planes including
c                   the horizontal boundary planes
c
c     srelpr = 7.1e-15   for cdc cyber 170/750  (approx.) 2**-47
c            = 1.49e-8   for dec 10  (approx.) 2**-26
c            = 1.192e-7  for vax 11/780 (approx.) 2**-23
c
      srelpr = 2.0**(-47)
c
c     set itpack switches
c
      nout = 6
      level = 0
      ieran = 0
      itmax = itm
      zeta = 5.0e-6 
      nw = nww
      ndim = nlen
c
c     set number of times through test loops
c
      icase = 2
      jcase = 2
c
c     set size of test problem
c
      ilevel = 0
      maxnz  = 5
      ngridx = nside2
      ngridy = nside2
c
      ngrdxd = ngridx
      ngrdyd = ngridy
      write (nout,10)
   10 format ('1'//15x,'itpack 2c  test program -- test1'/15x,
     *   'tests iterative modules'/15x, 
     *   'uses five point symmetric discretization')
c
c ... set up default values for bctype and initialize information arrays
c
c ... bctype defines the boundary conditions on the edges of the
c ... rectangle.  where
c
c        bctype(i) = 1 implies the ith side is dirichlet
c                  = 0 implies the ith side is neumann or mixed
c
c          and i = 1 implies the east  side defined by (bx, y)
c                = 2 implies the south side defined by ( x,ay)
c                = 3 implies the west  side defined by (ax, y)
c                = 4 implies the north side defined by ( x,by)
c
      do 20 i = 1,4 
         bctype(i) = 1
   20 continue
c
c ... define the discretization mesh
c
c       ax      -  minimum x value on the rectangle (west side)
c       bx      -  maximum x value on the rectangle (east side)
c       gridx   -  array containing the x-coordinate of the 
c                  horizontal mesh lines from west to east. 
c                  these are uniform but that is not required.
c       ay      -  minimum y value on the rectangle (south side)
c       by      -  maximum y value on the rectangle (north side)
c       gridy   -  array containing the y-coordinate of the 
c                  vertical mesh lines from south to north. 
c                  these are  uniform but that is not required.
c
      ax = 0.0
      bx = 1.0
      hx = (bx-ax)/float(ngridx-1)
      do 30 j = 1,ngridx
         gridx(j) = ax+float(j-1)*hx
   30 continue
      gridx(ngridx) = bx
c
      ay = 0.0
      by = 1.0
      hy = (by-ay)/float(ngridy-1)
      do 40 j = 1,ngridy
         gridy(j) = ay+float(j-1)*hy
   40 continue
      gridy(ngridy) = by
c
c ... discretize the elliptic pde
c
      do 60 loop1 = 1,icase
         isym = loop1-1
         if (loop1.eq.2) write (nout,50)
   50    format ('1'///)
         call sym5pt (gridx,ngrdxd,gridy,ngrdyd,rhs,ndim,maxnz,jcoef, 
     *      coef)
c
c ... solve the matrix problem
c
         do 60 loop2 = 1,jcase
            nb = loop2-3
            if (isym.eq.0) write (nout,70)
            if (isym.eq.1) write (nout,80)
            if (nb.eq.(-2)) write (nout,90)
            if (nb.eq.(-1)) write (nout,100)
c
c        test jcg
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = nb
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call jcg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,iparm
     *         ,rparm,ier)
            tim1(1) = rparm(9)
            tim2(1) = rparm(10)
            digit1(1) = rparm(11)
            digit2(1) = rparm(12)
            iter(1) = iparm(1)
            iwrk(1) = iparm(8)
c
c        test jsi
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = nb
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call jsi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,iparm
     *         ,rparm,ier)
            tim1(2) = rparm(9)
            tim2(2) = rparm(10)
            digit1(2) = rparm(11)
            digit2(2) = rparm(12)
            iter(2) = iparm(1)
            iwrk(2) = iparm(8)
c
c        test sor
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = nb
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call sor (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,iparm
     *         ,rparm,ier)
            tim1(3) = rparm(9)
            tim2(3) = rparm(10)
            digit1(3) = rparm(11)
            digit2(3) = rparm(12)
            iter(3) = iparm(1)
            iwrk(3) = iparm(8)
c
c        test ssorcg
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = nb
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call ssorcg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp, 
     *         iparm,rparm,ier)
            iter(4) = iparm(1)
            iwrk(4) = iparm(8)
            tim1(4) = rparm(9)
            tim2(4) = rparm(10)
            digit1(4) = rparm(11)
            digit2(4) = rparm(12)
c
c        test ssorsi
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = nb
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call ssorsi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp, 
     *         iparm,rparm,ier)
            tim1(5) = rparm(9)
            tim2(5) = rparm(10)
            digit1(5) = rparm(11)
            digit2(5) = rparm(12)
            iter(5) = iparm(1)
            iwrk(5) = iparm(8)
c
c        test rscg
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = -1
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call rscg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *         iparm,rparm,ier)
            tim1(6) = rparm(9)
            tim2(6) = rparm(10)
            digit1(6) = rparm(11)
            digit2(6) = rparm(12)
            iter(6) = iparm(1)
            iwrk(6) = iparm(8)
c
c        test rssi
c
            call dfault (iparm,rparm)
            iparm(1) = itmax
            iparm(2) = level
            iparm(5) = isym
            iparm(9) = -1
            iparm(12) = ieran 
            rparm(1) = zeta
            call vfill (n,u,0.0)
            call rssi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *         iparm,rparm,ier)
            tim1(7) = rparm(9)
            tim2(7) = rparm(10)
            digit1(7) = rparm(11)
            digit2(7) = rparm(12)
            iter(7) = iparm(1)
            iwrk(7) = iparm(8)
c
c     timing analysis
c
            call timanl (n,ndim,maxnz,jcoef,coef,wksp,wksp(n+1),iter, 
     *         tim1,tim2,digit1,digit2,iwrk,nout) 
   60 continue
c
   70 format (//15x,'symmetric case')
   80 format (//15x,'nonsymmetric case')
   90 format (15x,'natural ordering used')
  100 format (15x,'red-black ordering used')
      stop
      end 
      subroutine timanl (n,ndim,maxnz,jcoef,coef,v,w,iter,tim1,tim2,
     *   digit1,digit2,iwork,nout)
      integer iter(7),jcoef(ndim,maxnz),iwork(7)
      dimension v(1),w(1),rel(7),coef(ndim,maxnz),digit1(7),digit2(7),
     *   tim1(7),tim2(7)
      data maxlp / 20 /
c
      call vfill (n,v,1.0)
c
      do 20 j = 2,maxnz
         do 10 i = 1,n
            if (jcoef(i,j).eq.0) jcoef(i,j) = i
   10    continue
   20 continue
c
      timi1 = timer (timdmy)
      do 30 i = 1,maxlp
         call pmult (n,ndim,maxnz,jcoef,coef,v,w) 
   30 continue
      timi2 = timer (timdmy)
      timmat = (timi2-timi1)/float(maxlp)
c
      do 50 j = 2,maxnz
         do 40 i = 1,n
            if (jcoef(i,j).eq.i) jcoef(i,j) = 0
   40    continue
   50 continue
c
      write (nout,70)
      do 60 i = 1,7 
         rel(i) = 0.0
         temp = float(iter(i))
         if ((timi2.eq.timi1).or.(iter(i).eq.0)) go to 60
         rel(i) = tim1(i)/(temp*timmat) 
   60 continue
c
      write (nout,80) (tim1(i),tim2(i),iter(i),rel(i),digit1(i),
     *   digit2(i),iwork(i),i=1,7)
c
      return
c
   70 format (////15x,'timing analysis'/15x,15('-')//35x,'iteration',3x,
     *   'total',9x,'no. of',4x,'avg iter/',2x,'digits',4x,'digits',5x,
     *   'wksp'/15x,'method',14x,'time (sec)',2x,'time (sec)',1x,
     *   'iterations',1x,'mtx-vctr mlt',1x,'stp tst',3x,'res/rhs',4x, 
     *   'used'/15x,103('-')) 
   80 format (15x,'jacobi cg        ',2f10.3,i14,f10.1,1x,2f10.1,i10/15x
     *   ,'jacobi si        ',2f10.3,i14,f10.1,1x,2f10.1,i10/15x,
     *   'sor              ',2f10.3,i14,f10.1,1x,2f10.1,i10/15x,
     *   'symmetric sor cg ',2f10.3,i14,f10.1,1x,2f10.1,i10/15x,
     *   'symmetric sor si ',2f10.3,i14,f10.1,1x,2f10.1,i10/15x,
     *   'reduced system cg',2f10.3,i14,f10.1,1x,2f10.1,i10/15x,
     *   'reduced system si',2f10.3,i14,f10.1,1x,2f10.1,i10/)
c
      end 
      subroutine pde (x,y,cvalus)
c
c ... pde is a user supplied routine to specify the
c ... self-adjoint elliptic pde for sym5pt in the following form
c
c        (cvalus(1)*ux)x + (cvalus(3)*uy)y + cvalus(6)*u = cvalus(7)
c
c     note --  cvalus(i), for i = 2, 4, and 5 are not used. 
c
      dimension cvalus(7)
c
      cvalus(1) = 1.0
      cvalus(2) = 0.0
      cvalus(3) = 2.0
      cvalus(4) = 0.0
      cvalus(5) = 0.0
      cvalus(6) = 0.0
      cvalus(7) = 0.0
c
      return
      end 
      function bcond (iside,x,y,bvalus) 
c
c ... this function is a user supplied function to specify the
c ... boundary conditions of the elliptic pde depending on iside, x,
c ... and y.
c
c        if iside = 1, then x = bx (east side)
c                 = 2, then y = ay (south side)
c                 = 3, then x = ax (west side)
c                 = 4, then y = by (north side)
c
c ... the bvalus array is defined as follows
c
c        bvalus(1)*u + bvalus(2)*ux + bvalus(3)*uy = bvalus(4)
c
c        note --   bcond is set to bvalus(4) before returning.
c
      dimension bvalus(4)
c
      go to (10,20,30,40), iside
c
   10 bvalus(1) = 1.0
      bvalus(4) = 1.0+x*y
      bvalus(2) = 0.0
      bvalus(3) = 0.0
      go to 50
c
   20 bvalus(1) = 1.0
      bvalus(4) = 1.0+x*y
      bvalus(2) = 0.0
      bvalus(3) = 0.0
      go to 50
c
   30 bvalus(1) = 1.0
      bvalus(4) = 1.0+x*y
      bvalus(2) = 0.0
      bvalus(3) = 0.0
      go to 50
c
   40 bvalus(1) = 1.0
      bvalus(4) = 1.0+x*y
      bvalus(2) = 0.0
      bvalus(3) = 0.0
c
   50 bcond = bvalus(4)
      return
      end 
      subroutine sym5pt (gridx,ngrdxd,gridy,ngrdyd,rhs,ndim,maxnz,
     *   jcoef,coef)
c
      integer jcoef(ndim,maxnz),bctype(4)
      dimension gridx(ngrdxd),gridy(ngrdyd),rhs(1),coef(ndim,maxnz)
c
c ... sym5pt / sym7pt common blocks
c
      common /tstbk1/ ax,ay,bx,by,srelpr,hx,hy
      common /tstbk2/ bctype,ilevel,isym,nout,ngridx,ngridy,n
c
      if (ngridx.lt.3.or.ngridy.lt.3) go to 100
c
c     determine range of unknown grid points
c
      ix1 = 1
      ix2 = ngridx
      jy1 = 1
      jy2 = ngridy
      if (bctype(1).eq.1) ix2 = ngridx-1
      if (bctype(2).eq.1) jy1 = 2
      if (bctype(3).eq.1) ix1 = 2
      if (bctype(4).eq.1) jy2 = ngridy-1
      lngthx = ix2-ix1+1
      n = (jy2-jy1+1)*lngthx
      if (n.gt.ndim) go to 130
c
c     output initial grid information
c
      if (ilevel.eq.0) go to 60
      write (nout,10) ax,bx,ay,by
   10 format (/10x,'finite difference module',' ---- ',
     *   'symmetric five point'/10x,'domain = rectangle  (',e11.4,',',
     *   e11.4,') x (',e11.4,',',e11.4,')')
c
      write (nout,20)
   20 format (/10x,'coefficients of vertical mesh lines')
      write (nout,30) (gridx(i),i=1,ngridx)
   30 format (/8x,8(2x,e11.4))
      write (nout,40)
   40 format (/10x,'coefficients of horizontal mesh lines') 
      write (nout,30) (gridy(i),i=1,ngridy)
      write (nout,50) (bctype(i),i=1,4) 
   50 format (/10x,'boundary conditions on pieces 1,2,3,4 are ',3(1x,i1,
     *   ','),1x,i1,'.')
c
c     generate equations one mesh point at a time 
c
   60 call sbini (n,ndim,maxnz,jcoef,coef)
      ixadd = 0
      jyadd = 0
      if (bctype(3).eq.1) ixadd = 1
      if (bctype(2).eq.1) jyadd = 1
c
      do 70 ij = 1,n
         ixx = mod(ij-1,lngthx)+1
         jyy = (ij-ixx)/lngthx+1
         ixx = ixx+ixadd
         jyy = jyy+jyadd
         hn = 0.0
         hs = 0.0
         he = 0.0
         hw = 0.0
         px = gridx(ixx)
         py = gridy(jyy)
         if (ixx.ne.1) hw = px-gridx(ixx-1)
         if (ixx.ne.ngridx) he = gridx(ixx+1)-px
         if (jyy.ne.1) hs = py-gridy(jyy-1)
         if (jyy.ne.ngridy) hn = gridy(jyy+1)-py
c
         inij = ij
         call pnt2d (px,py,inij,ixx,jyy,lngthx,hn,hs,hw,he,b,ndim,maxnz
     *      ,jcoef,coef)
c
         rhs(ij) = b
   70 continue
c
c     normal exit
c
      if (ilevel.eq.0) return 
c
      write (nout,80)
   80 format (/10x,'sym5pt completed successfully.')
      nuu = 2*n*maxnz
      write (nout,90) n,nuu
   90 format (10x,'sparse matrix representation finished.'/15x,
     *   'no. of equations        =',i8/15x,'total matrix storage    =',
     *   i8/)
c
      return
c
c     error exits
c
  100 continue
      if (ngridx.lt.3) write (nout,110) 
  110 format (/10x,'sym5pt error -- ngridx .lt. 3 ')
      if (ngridy.lt.3) write (nout,120) 
  120 format (/10x,'sym5pt error -- ngridy .lt. 3 ')
c
      stop
c
  130 write (nout,140) n,ndim 
  140 format (/10x,'n .gt. ndim, n =',i10,' ndim =',i10)
      stop
c
      end 
      subroutine pnt2d (pcx,pcy,ij,ix,jy,lngthx,hn,hs,hw,he,b,
     *   ndim,maxnz,jcoef,coef)
c
      dimension cvalus(7),bvalus(4),coef(ndim,maxnz)
      integer jcoef(ndim,maxnz),bctype(4)
c
c ... sym5pt / sym7pt common blocks
c
      common /tstbk1/ ax,ay,bx,by,srelpr,hx,hy
      common /tstbk2/ bctype,ilevel,isym,nout,ngridx,ngridy,n
c
c ... initialize coefficients 
c
      ceast = 0.0
      cwest = 0.0
      cnorth = 0.0
      csouth = 0.0
      call pde (pcx,pcy,cvalus)
      center = -cvalus(6)*(hn+hs)*(he+hw)/4.0
      crhs = -cvalus(7)*(hn+hs)*(he+hw)/4.0
c
c     set east coefficient
c
      call pde (pcx+0.5*he,pcy,cvalus)
      if (ix.ge.ngridx-1) temp = bcond(1,bx,pcy,bvalus)
      if (ix.eq.ngridx) go to 20
      if ((ix+1.eq.ngridx).and.(bctype(1).eq.1)) go to 10
c
c     normal east point
c
      ceast = -cvalus(1)*(hn+hs)/(2.0*he)
      center = center-ceast
      go to 30
c
c     east point is a dirichlet point
c
   10 temp = cvalus(1)*(hn+hs)/(2.0*he) 
      crhs = crhs+bvalus(4)*temp/bvalus(1)
      center = center+temp
      go to 30
c
c     center point lies on the east boundary which is mixed 
c
   20 temp = cvalus(1)*(hn+hs)/(2.0*bvalus(2))
      center = center+bvalus(1)*temp
      crhs = crhs+bvalus(4)*temp
c
c     set west coefficient
c
   30 call pde (pcx-0.5*hw,pcy,cvalus)
      if (ix.le.2) temp = bcond(3,ax,pcy,bvalus)
      if (ix.eq.1) go to 50
      if (ix.eq.2.and.bctype(3).eq.1) go to 40
c
c     normal west point
c
      cwest = -cvalus(1)*(hn+hs)/(2.0*hw)
      center = center-cwest
      go to 60
c
c     west point is dirichlet 
c
   40 temp = cvalus(1)*(hn+hs)/(2.0*hw) 
      crhs = crhs+bvalus(4)*temp/bvalus(1)
      center = center+temp
      go to 60
c
c     center point lies on west boundary which is mixed.
c
   50 temp = cvalus(1)*(hn+hs)/(2.0*bvalus(2))
      center = center+bvalus(1)*temp
      crhs = crhs+bvalus(4)*temp
c
c     set north coefficients
c
   60 call pde (pcx,pcy+0.5*hn,cvalus)
      if (jy.ge.ngridy-1) temp = bcond(4,pcx,by,bvalus)
      if (jy.eq.ngridy) go to 80
      if ((jy.eq.ngridy-1).and.bctype(4).eq.1) go to 70
c
c     normal north point
c
      cnorth = -cvalus(3)*(he+hw)/(2.0*hn)
      center = center-cnorth
      go to 90
c
c     north point is dirichlet
c
   70 temp = cvalus(3)*(he+hw)/(2.0*hn) 
      crhs = crhs+bvalus(4)*temp/bvalus(1)
      center = center+temp
      go to 90
c
c     center point lies on northern mixed boundary
c
   80 temp = cvalus(3)*(he+hw)/(2.0*bvalus(3))
      center = center+bvalus(1)*temp
      crhs = crhs+bvalus(4)*temp
c
c     set south coefficients
c
   90 call pde (pcx,pcy-0.5*hs,cvalus)
      if (jy.le.2) temp = bcond(2,pcx,ay,bvalus)
      if (jy.eq.1) go to 110
      if (jy.eq.2.and.bctype(2).eq.1) go to 100
c
c     normal south point
c
      csouth = -cvalus(3)*(he+hw)/(2.0*hs)
      center = center-csouth
      go to 120
c
c     south point is dirichlet
c
  100 temp = cvalus(3)*(he+hw)/(2.0*hs) 
      crhs = crhs+bvalus(4)*temp/bvalus(1)
      center = center+temp
      go to 120
c
c     center point lies on southern mixed boundary
c
  110 temp = cvalus(3)*(he+hw)/(2.0*bvalus(3))
      center = center+bvalus(1)*temp
      crhs = crhs+bvalus(4)*temp
c
c     coefficient generation is completed.
c     now set b (rhs) and ia,ja, and a (matrix representation)
c
  120 b = crhs
c
      if (ilevel.eq.1) write (nout,130) ij,crhs
  130 format (/10x,'sym5pt -- equation',i8/20x,
     *   'right hand side =       ',e15.7)
c
      call setval (ndim,maxnz,jcoef,coef,ij,ij,center)
      call setval (ndim,maxnz,jcoef,coef,ij,ij+1,ceast)
      call setval (ndim,maxnz,jcoef,coef,ij,ij+lngthx,cnorth)
      call setval (ndim,maxnz,jcoef,coef,ij,ij-lngthx,csouth)
      call setval (ndim,maxnz,jcoef,coef,ij,ij-1,cwest)
c
      return
      end 
      subroutine setval (ndim,maxnz,jcoef,coef,i,j,val)
      integer jcoef(ndim,maxnz),bctype(4)
      dimension coef(ndim,maxnz)
c
      common /tstbk1/ ax,ay,bx,by,srelpr,hx,hy
      common /tstbk2/ bctype,ilevel,isym,nout,ngridx,ngridy,n
c
      if (j.gt.n.or.abs(val).lt.srelpr) return
      if (ilevel.eq.1) write (nout,10) j,val
   10 format (20x,'column',i8,'   value =',e15.7) 
c
      ier = 0
      call sbsij (n,ndim,maxnz,jcoef,coef,i,j,val,0,ilevel,nout,ier)
      if (ier.gt.700) stop
c
      return
      end 
      subroutine sbsij (n,ndim,maxnz,jcoef,coef,i,j,val,mode,
     *   level,nout,ier)
c
c     sbsij places the (i,j) entry val of the matrix in the sparse
c     representation given by jcoef,coef.  diagonal elements
c     are placed in the first column of coef.  all other
c     entries are packed from left to right.  the corresponding
c     entry of jcoef contains the column number of the entry.
c
      integer jcoef(ndim,maxnz)
      dimension coef(ndim,maxnz)
c
      ier = 0
c
c check subscript bounds
c
      if ((i.le.0).or.(i.gt.n)) go to 70
      if ((j.le.0).or.(j.gt.n)) go to 70
c
      do 10 k = 1,maxnz
         if (jcoef(i,k).eq.j) go to 40
   10 continue
      do 20 k = 1,maxnz
         if (jcoef(i,k).eq.0) go to 30
   20 continue
      go to 80
c
   30 coef(i,k) = val
      jcoef(i,k) = j
      go to 90
c
c entry already set.  value now depends on mode.
c
   40 ier = 700
      if (mode.gt.0) go to 50 
      if (mode.lt.0) go to 60 
c
c mode = 0.  use new value.
c
      if (level.ge.1) write (nout,120) ier,i,j,coef(i,k),val
      coef(i,k) = val
      go to 90
c
c mode.gt.0.  add values
c
   50 temp = coef(i,k)+val
      if (level.ge.1) write (nout,120) ier,i,j,coef(i,k),temp
      coef(i,k) = temp
      go to 90
c
c mode.lt.0.  use old value
c
   60 if (level.ge.1) write (nout,130) ier,i,j,coef(i,k)
      go to 90
c
c subscript out of bounds
c
   70 ier = 701
      if (level.ge.0) write (nout,100) ier,i,j
      go to 90
c
c array too small
c
   80 ier = 702
      if (level.ge.0) write (nout,110) ier
   90 continue
      return
c
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    in itpack routine sbsij   '/' ','    ier = ',i10/' ',
     *   '    ( ',i10,' , ',i10,' )'/' ',
     *   '    improper value for i or j ')
  110 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    in itpack routine sbsij   '/' ','    ier = ',i10/' ',
     *   '    maxnz too small - no room for new entry')
  120 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpack routine sbsij   '/' ','    ier = ',i10/' ',
     *   '    ( ',i10,' , ',i10,' )'/' ',
     *   '    entry already set - current value of',e15.8/' ',
     *   '                                reset to',e15.8)
  130 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpack routine sbsij   '/' ','    ier = ',i10/' ',
     *   '    ( ',i10,' , ',i10,' )'/' ',
     *   '    entry already set and is left as    ',e15.8)
      end 
      subroutine sbini (n,ndim,maxnz,jcoef,coef)
c
c     sbini initializes the arrays jcoef and coef 
c     for the sparse matrix representation.
c
      integer jcoef(ndim,maxnz)
      dimension coef(ndim,maxnz)
c
      do 20 j = 1,maxnz
         do 10 i = 1,n
            jcoef(i,j) = 0
            coef(i,j) = 0.0
   10    continue
   20 continue
      return
      end 
