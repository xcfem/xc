      subroutine jcg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *                iparm,rparm,ier)
c
c     itpackv 2d main routine  jcg  (jacobi conjugate gradient)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          jcg drives the jacobi conjugate gradient algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  jacobi conjugate
c                 gradient needs this to be in length at least
c                 4*n + 4*itmax.  here itmax = iparm(1) is the
c                 maximum allowable number of iterations.
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... jcg module references --
c
c         from itpackv    chgcon, determ, dfault, echall,
c                         eigvns, eigvss, eqrt1s, iterm ,
c                         itjcg , parcon, permat, perror,
c                         pervec, pjac  , pmult , prbndx, pstop ,
c                         sbelm , scal  , sum3  , unscal,
c                         vout  , zbrent
c          system         abs, alog10, amax0, amax1, mod, sqrt
c
c ... local itpackv references --
c
c          echall, itjcg , permat,
c          perror, pervec, pjac  , prbndx, sbelm , scal  ,
c          unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      j c g      ')
      if (iparm(1).le.0) go to 370
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *      '    in itpackv routine jcg'/1x,
     *      '    rparm(1) =',e10.3,' (zeta)'/1x,
     *      '    a value this small may hinder convergence '/1x,
     *      '    since machine precision srelpr =',e10.3/1x,
     *      '    zeta reset to ',e10.3) 
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 11
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *        '    called from itpackv routine jcg '/1x,
     *        '    invalid matrix dimension, n =',i8)
      go to 370
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jcg '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 370
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
      ib4 = ib3+n
      ib5 = ib4+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jcg  '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 350
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 14
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jcg      '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 350
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... check for sufficient workspace.
c
  170 iparm(8) = 4*n+4*itmax
      if (nw.ge.iparm(8)) go to 190
      ier = 12
      if (level.ge.0) write (nout,180) nw,iparm(8)
  180 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jcg '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 330
c
  190 continue
      if (level.le.2) go to 220
      write (nout,200)
  200 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
      if (adapt) write (nout,210)
  210 format (1x,'cme is the estimate of the largest eigenvalue of',
     *   ' the jacobi matrix')
  220 if (iparm(11).eq.0) timi1 = timer(0.0)
c
c ... compute initial pseudo-residual
c
      do 230 i = 1,nw
         wksp(i) = 0.0
  230 continue
      call scopy (n,rhs,1,wksp(ib2),1)
      call pjac (n,ndim,maxnz,jcoef,coef,u,wksp(ib2))
      do 240 i = 1,n
         wksp(n+i) = wksp(n+i)-u(i)
  240 continue
c
c ... iteration sequence
c
      itmax1 = itmax+1
      do 260 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 250
c
c ... code for the even iterations.
c
c     u           = u(in)             wksp(ib2) = del(in)
c     wksp(ib1)   = u(in-1)           wksp(ib3) = del(in-1) 
c
         call itjcg (n,ndim,maxnz,jcoef,coef,u,wksp(ib1),wksp(ib2),
     *      wksp(ib3),wksp(ib4),wksp(ib5))
c
         if (halt) go to 290
         go to 260
c
c ... code for the odd iterations.
c
c     u           = u(in-1)           wksp(ib2) = del(in-1) 
c     wksp(ib1)   = u(in)             wksp(ib3) = del(in)
c
  250    call itjcg (n,ndim,maxnz,jcoef,coef,wksp(ib1),u,wksp(ib3),
     *      wksp(ib2),wksp(ib4),wksp(ib5))
c
         if (halt) go to 290
  260 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 270
      timi2 = timer(0.0)
      time1 = timi2-timi1
  270 ier = 13
      if (level.ge.1) write (nout,280) itmax
  280 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine jcg'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 320
c
c ... method has converged
c
  290 if (iparm(11).ne.0) go to 300
      timi2 = timer(0.0)
      time1 = timi2-timi1
  300 if (level.ge.1) write (nout,310) in
  310 format (/1x,'jcg  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  320 continue
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
c
c ... un-permute matrix,rhs, and solution
c
  330 if (iparm(9).ne.-1) go to 340
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp(ib4),
     *   iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib4))
      call pervec (n,iwksp(ib2),u,wksp(ib4))
      if (ier.eq.12) go to 350
c
c ... optional error analysis 
c
  340 idgts = iparm(12)
      if (idgts.lt.0) go to 350
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  350 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      iparm(8) = iparm(8)-4*(itmax-in)
      if (iparm(11).ne.0) go to 360
      timj2 = timer(0.0)
      time2 = timj2-timj1
  360 if (iparm(3).ne.0) go to 370
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  370 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,380)
  380 format (/1x,'execution successful')
c
      return
      end 
      subroutine jsi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *                iparm,rparm,ier)
c
c     itpackv 2d main routine  jsi  (jacobi semi-iterative)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          jsi drives the jacobi semi-iteration algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  jacobi si 
c                 needs this to be in length at least 2*n.
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... jsi module references --
c
c         from itpackv    cheby , chgsi , chgsme, dfault, echall,
c                         iterm , itjsi ,
c                         parsi , permat, perror, pervec, pjac  ,
c                         pmult , prbndx, pstop , pvtbv ,
c                         sbelm , scal  , sum3  , 
c                         tstchg, unscal,
c                         vout
c          system         abs, alog10, amax0, amax1, float, mod, sqrt 
c
c ... local itpackv references --
c
c          echall, itjsi , permat,
c          perror, pervec, prbndx, sbelm , scal  ,
c          unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      j s i      ')
      if (iparm(1).le.0) go to 350
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine jsi'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 21
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jsi '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 350
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jsi '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 350
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jsi  '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 330
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 24
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jsi      '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 330
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... check for sufficient workspace.
c
  170 iparm(8) = 2*n
      if (nw.ge.iparm(8)) go to 190
      ier = 22
      if (level.ge.0) write (nout,180) nw,iparm(8)
  180 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine jsi '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 310
c
  190 continue
      if (level.le.2) go to 210
      write (nout,200)
  200 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
  210 if (iparm(11).eq.0) timi1 = timer(0.0)
      do 220 i = 1,nw
         wksp(i) = 0.0
  220 continue
c
c ... iteration sequence
c
      itmax1 = itmax+1
      do 240 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 230
c
c ... code for the even iterations.
c
c     u           = u(in)
c     wksp(ib1)   = u(in-1)
c
         call itjsi (n,ndim,maxnz,jcoef,coef,rhs,u,wksp(ib1),wksp(ib2),
     *      icnt)
c
         if (halt) go to 270
         go to 240
c
c ... code for the odd iterations.
c
c     u           = u(in-1)
c     wksp(ib1)   = u(in)
c
  230    call itjsi (n,ndim,maxnz,jcoef,coef,rhs,wksp(ib1),u,wksp(ib2),
     *      icnt)
c
         if (halt) go to 270
  240 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 250
      timi2 = timer(0.0)
      time1 = timi2-timi1
  250 ier = 23
      if (level.ge.1) write (nout,260) itmax
  260 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine jsi'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 300
c
c ... method has converged
c
  270 if (iparm(11).ne.0) go to 280
      timi2 = timer(0.0)
      time1 = timi2-timi1
  280 if (level.ge.1) write (nout,290) in
  290 format (/1x,'jsi  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  300 continue
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
c
c ... un-permute matrix,rhs, and solution
c
  310 if (iparm(9).ne.-1) go to 320
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp(ib2),
     *   iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.22) go to 330
c
c ... optional error analysis 
c
  320 idgts = iparm(12)
      if (idgts.lt.0) go to 330
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  330 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      if (iparm(11).ne.0) go to 340
      timj2 = timer(0.0)
      time2 = timj2-timj1
  340 if (iparm(3).ne.0) go to 350
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  350 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,360)
  360 format (/1x,'execution successful')
c
      return
      end 
      subroutine sor (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *                iparm,rparm,ier)
c
c     itpackv 2d main routine  sor  (successive overrelaxation)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          sor drives the successive overrelaxation algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  sor
c                 needs this to be in length at least 2*n.
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... sor module references --
c
c         from itpackv    dfault, echall, ipstr , iterm ,
c                         itsor , move  , permat, 
c                         perror, pervec, pfsor1, pmult , prbndx,
c                         pstop , sbelm , scal  , 
c                         tau   , unscal,
c                         vout
c          system         abs, alog10, amax0, amax1, float, iabs, mod,
c                         sqrt
c
c ... local itpackv references --
c
c          echall, itsor , move  ,
c          permat, perror, pervec, prbndx, sbelm , scal  ,
c          unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c ... specifications for local variables
c
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      s o r      ')
      if (iparm(1).le.0) go to 360
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine sor'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 31
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine sor '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 360
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine sor '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 360
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine sor  '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 340
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 34
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine sor      '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 340
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... re-arrange data structure if natural ordering is used.
c
  170 if (nb.ge.0) go to 180
      call move (n,ndim,maxnz,nlow,nup,jcoef,coef,level,nout,moved,
     *   iwksp)
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,1)
c
c ... check for sufficient workspace.
c
  180 iparm(8) = 2*n
      if (nw.ge.iparm(8)) go to 200
      ier = 32
      if (level.ge.0) write (nout,190) nw,iparm(8)
  190 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine sor '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 310
c
  200 continue
      if (level.le.2) go to 230
      if (adapt) write (nout,210)
  210 format (///1x,'cme is the estimate of the largest eigenvalue of',
     *   ' the jacobi matrix')
      write (nout,220)
  220 format (1x,'omega is the relaxation factor')
  230 if (iparm(11).eq.0) timi1 = timer(0.0)
      do 240 i = 1,nw
         wksp(i) = 0.0
  240 continue
c
c ... iteration sequence
c
      itmax1 = itmax+1
      do 250 loop = 1,itmax1
         in = loop-1
c
c ... code for one iteration. 
c
c         u          = u(in)
c
         call itsor (n,nb,ndim,maxnz,jcoef,coef,rhs,u,nup,moved,
     *      wksp,iwksp(ib3))
c
         if (halt) go to 280
  250 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 260
      timi2 = timer(0.0)
      time1 = timi2-timi1
  260 ier = 33
      if (level.ge.1) write (nout,270) itmax
  270 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine sor'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 310
c
c ... method has converged
c
  280 if (iparm(11).ne.0) go to 290
      timi2 = timer(0.0)
      time1 = timi2-timi1
  290 if (level.ge.1) write (nout,300) in
  300 format (/1x,'sor  has converged in ',i5,' iterations')
c
c ... un-permute matrix,rhs, and solution
c
  310 if (iparm(9).ne.-1) go to 320
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp,iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.32) go to 340
c
c ... undo wave front reordering.
c
  320 if (nb.ge.0) go to 330
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,2)
c
c ... optional error analysis 
c
  330 idgts = iparm(12)
      if (idgts.lt.0) go to 340
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  340 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      if (iparm(11).ne.0) go to 350
      timj2 = timer(0.0)
      time2 = timj2-timj1
  350 if (iparm(3).ne.0) go to 360
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(5) = omega
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  360 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,370)
  370 format (/1x,'execution successful')
c
      return
      end 
      subroutine ssorcg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp, 
     *                   iparm,rparm,ier)
c
c     itpackv 2d main routine  ssorcg  (symmetric successive over-
c                                     relaxation conjugate gradient)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          ssorcg drives the symmetric sor-cg algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  ssorcg needs
c                 this to be in length at least 6*n + 4*itmax.
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... ssorcg module references --
c
c         from itpackv    chgcon, determ, dfault, echall,
c                         eigvns, eigvss, eqrt1s, iterm ,
c                         itsrcg, move  , omeg  , omgchg,
c                         omgstr, parcon, pbeta , pbsor , permat,
c                         perror, pervec, pfsor , pjac  , pmult ,
c                         prbndx, pstop , pvtbv , sbelm , scal  ,
c                         sum3  , unscal,
c                           vout  ,
c                         zbrent
c          system         abs, alog, alog10, amax0, amax1, amin1,
c                         mod, sqrt
c
c ... local itpackv references --
c
c          echall, itsrcg, move  ,
c          omeg  , pbeta , permat, perror, pervec, pfsor ,
c          prbndx, sbelm , scal  , unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c ... specifications for local variables
c
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (iparm(9).ge.-1) iparm(6) = 2
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      s s o r c g')
      if (iparm(1).le.0) go to 420
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine ssorcg'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 41
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorcg '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 420
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorcg '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 420
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
      ib4 = ib3+n
      ib5 = ib4+n
      ib6 = ib5+n
      ib7 = ib6+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorcg  '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 400
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 44
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorcg   '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 400
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... re-arrange data structure if natural ordering is used.
c
  170 if (nb.ge.0) go to 180
      call move (n,ndim,maxnz,nlow,nup,jcoef,coef,level,nout,moved,
     *   iwksp)
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,1)
c
c ... check for sufficient workspace.
c
  180 iparm(8) = 6*n+4*itmax
      if (nw.ge.iparm(8)) go to 200
      ier = 42
      if (level.ge.0) write (nout,190) nw,iparm(8)
  190 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorcg '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 370
c
  200 continue
      if (level.le.2) go to 230
      write (nout,210)
  210 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
      write (nout,220)
  220 format (1x,'s-prime is an initial estimate for new cme')
  230 if (iparm(11).eq.0) timi1 = timer(0.0)
c
c ... special procedure for fully adaptive case.
c
      do 240 i = 1,nw
         wksp(i) = 0.0
  240 continue
      if (.not.adapt) go to 270
      if (.not.betadt) go to 260
      do 250 i = 1,n
         wksp(i) = 1.0
  250 continue
      betnew = pbeta(n,nb,ndim,maxnz,jcoef,coef,wksp(ib1),nup,nlow,moved
     *   ,wksp(ib2),wksp(ib3))/float(n) 
      betab = amax1(betab,0.25,betnew)
  260 call omeg (0.0,1)
      is = 0
c
c ... initialize forward pseudo-residual
c
  270 call sorscl (n,ndim,maxnz,coef,rhs,omega)
      call scopy (n,rhs,1,wksp,1)
      call scopy (n,u,1,wksp(ib2),1)
      call pfsor (n,nb,ndim,maxnz,jcoef,coef,wksp(ib2),wksp(ib1),nup, 
     *   moved,iwksp(ib3))
      do 280 i = 1,n
         wksp(n+i) = wksp(n+i)-u(i)
  280 continue
c
c ... iteration sequence
c
      itmax1 = itmax+1
      do 300 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 290
c
c ... code for the even iterations.
c
c     u           = u(in)       wksp(ib2) = c(in) 
c     wksp(ib1)   = u(in-1)     wksp(ib3) = c(in-1)
c
         call itsrcg (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,u,
     *      wksp(ib1),wksp(ib2),wksp(ib3),wksp(ib4),wksp(ib5),moved,
     *      wksp(ib6),wksp(ib7),iwksp(ib3))
c
         if (halt) go to 330
         go to 300
c
c ... code for the odd iterations.
c
c     u           = u(in-1)     wksp(ib2) = c(in-1)
c     wksp(ib1)   = u(in)       wksp(ib3) = c(in) 
c
  290    call itsrcg (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,wksp(ib1),
     *      u,wksp(ib3),wksp(ib2),wksp(ib4),wksp(ib5),moved,wksp(ib6),
     *      wksp(ib7),iwksp(ib3))
c
         if (halt) go to 330
  300 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 310
      timi2 = timer(0.0)
      time1 = timi2-timi1
  310 ier = 43
      if (level.ge.1) write (nout,320) itmax
  320 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine ssorcg'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 360
c
c ... method has converged
c
  330 if (iparm(11).ne.0) go to 340
      timi2 = timer(0.0)
      time1 = timi2-timi1
  340 if (level.ge.1) write (nout,350) in
  350 format (/1x,'ssorcg  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  360 factor = 1.0/omega
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
c
c ... un-permute matrix,rhs, and solution
c
  370 if (iparm(9).ne.-1) go to 380
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp,iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.42) go to 400
c
c ... undo wave front reordering.
c
  380 if (nb.ge.0) go to 390
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,2)
c
c ... optional error analysis 
c
  390 idgts = iparm(12)
      if (idgts.lt.0) go to 400
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  400 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      if (iparm(11).ne.0) go to 410
      timj2 = timer(0.0)
      time2 = timj2-timj1
  410 iparm(8) = iparm(8)-4*(itmax-in)
      if (iparm(3).ne.0) go to 420
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(5) = omega
      rparm(6) = specr
      rparm(7) = betab
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  420 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,430)
  430 format (/1x,'execution successful')
c
      return
      end 
      subroutine ssorsi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp, 
     *                   iparm,rparm,ier)
c
c     itpackv 2d main routine  ssorsi  (symmetric successive over-
c                                     relaxation semi-iteration)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          ssorsi drives the symmetric sor-si algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  ssorsi
c                 needs this to be in length at least 5*n
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... ssorsi module references --
c
c         from itpackv    cheby , chgsi , dfault, echall,
c                         iterm , itsrsi, move  , 
c                         omeg  , omgstr, parsi , pbeta , pbsor ,
c                         permat, perror, pervec, pfsor , pjac  ,
c                         pmult , prbndx, pstop , pvtbv , sbelm ,
c                         scal  , sum3  , tstchg, 
c                         unscal,
c                         vout
c          system         abs, alog, alog10, amax0, amax1, float,
c                         mod, sqrt
c
c ... local itpackv references --
c
c          echall, itsrsi, move  ,
c          omeg  , pbeta , permat, perror, pervec, prbndx,
c          sbelm , scal  , unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c ... specifications for local variables
c
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      if (iparm(9).ge.-1) iparm(6) = 2
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      s s o r s i')
      if (iparm(1).le.0) go to 400
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine ssorsi'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 51
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorsi '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 400
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorsi '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 400
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
      ib4 = ib3+n
      ib5 = ib4+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorsi  '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 380
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 54
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorsi   '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 380
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... re-arrange data structure if natural ordering is used.
c
  170 if (nb.ge.0) go to 180
      call move (n,ndim,maxnz,nlow,nup,jcoef,coef,level,nout,moved,
     *   iwksp)
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,1)
c
c ... check for sufficient workspace.
c
  180 iparm(8) = 5*n
      if (nw.ge.iparm(8)) go to 200
      ier = 52
      if (level.ge.0) write (nout,190) nw,iparm(8)
  190 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine ssorsi '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 350
c
  200 continue
      if (level.le.2) go to 220
      write (nout,210)
  210 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
  220 if (iparm(11).eq.0) timi1 = timer(0.0)
c
c ... special procedure for fully adaptive case.
c
      do 230 i = 1,nw
         wksp(i) = 0.0
  230 continue
      if (.not.adapt) go to 260
      if (.not.betadt) go to 250
      do 240 i = 1,n
         wksp(i) = 1.0
  240 continue
      betnew = pbeta(n,nb,ndim,maxnz,jcoef,coef,wksp(ib1),nup,nlow,moved
     *   ,wksp(ib2),wksp(ib3))/float(n) 
      betab = amax1(betab,0.25,betnew)
  250 call omeg (0.0,1)
      is = 0
c
c ... iteration sequence
c
  260 call sorscl (n,ndim,maxnz,coef,rhs,omega)
      itmax1 = itmax+1
      do 280 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 270
c
c ... code for the even iterations.
c
c     u           = u(in)
c     wksp(ib1)   = u(in-1)
c
         call itsrsi (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,u,
     *      wksp(ib1),wksp(ib2),wksp(ib3),wksp(ib4),moved,wksp(ib5),
     *      iwksp(ib3))
c
         if (halt) go to 310
         go to 280
c
c ... code for the odd iterations.
c
c     u           = u(in-1)
c     wksp(ib1)   = u(in)
c
  270    call itsrsi (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,wksp(ib1),
     *      u,wksp(ib2),wksp(ib3),wksp(ib4),moved,wksp(ib5),iwksp(ib3))
c
         if (halt) go to 310
  280 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 290
      timi2 = timer(0.0)
      time1 = timi2-timi1
  290 ier = 53
      if (level.ge.1) write (nout,300) itmax
  300 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine ssorsi'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 340
c
c ... method has converged
c
  310 if (iparm(11).ne.0) go to 320
      timi2 = timer(0.0)
      time1 = timi2-timi1
  320 if (level.ge.1) write (nout,330) in
  330 format (/1x,'ssorsi  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  340 factor = 1.0/omega
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
c
c ... un-permute matrix,rhs, and solution
c
  350 if (iparm(9).ne.-1) go to 360
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp,iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.52) go to 380
c
c ... undo wave front reordering.
c
  360 if (nb.ge.0) go to 370
      call sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,iwksp(ib2),
     *   iwksp(ib3),wksp,numwav,2)
c
c ... optional error analysis 
c
  370 idgts = iparm(12)
      if (idgts.lt.0) go to 380
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  380 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      if (iparm(11).ne.0) go to 390
      timj2 = timer(0.0)
      time2 = timj2-timj1
  390 if (iparm(3).ne.0) go to 400
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(5) = omega
      rparm(6) = specr
      rparm(7) = betab
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  400 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,410)
  410 format (/1x,'execution successful')
c
      return
      end 
      subroutine rscg (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *                 iparm,rparm,ier) 
c
c     itpackv 2d main routine  rscg  (reduced system conjugate
c                                    gradient)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          rscg drives the reduced system cg algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  rscg needs this
c                 to be in length at least n+3*nb+4*itmax.  here
c                 itmax = iparm(1) and nb is the order of the black
c                 subsystem.  itmax is the maximum allowable number of
c                 iterations. 
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... rscg module references --
c
c         from itpackv    chgcon, determ, dfault, echall,
c                         eigvns, eigvss, eqrt1s, iterm ,
c                         itrscg, parcon, permat, perror,
c                         pervec, pmult , prbndx, prsblk, prsred,
c                         pstop , sbelm , scal  , 
c                         sum3  , unscal,
c                         vout  , zbrent
c          system         abs, alog10, amax0, amax1, mod, sqrt
c
c ... local itpackv references --
c
c          echall, itrscg, permat,
c          perror, pervec, prbndx, prsblk, prsred, sbelm ,
c          scal  , unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      r s c g    ')
      if (iparm(1).le.0) go to 380
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine rscg'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 61
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rscg '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 380
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rscg '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 380
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rscg '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 360
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 64
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rscg     '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 360
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... finish workspace base addresses
c
  170 jb2 = ib1+nb
      jb3 = jb2+nb
      jb4 = jb3+nb
      ib5 = n+3*nb+1
      nr = n-nb
      nrp1 = nr+1
c
c ... check for sufficient workspace.
c
      iparm(8) = n+3*nb+4*itmax
      if (nw.ge.iparm(8)) go to 190
      ier = 62
      if (level.ge.0) write (nout,180) nw,iparm(8)
  180 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rscg '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 340
c
  190 continue
      if (level.le.2) go to 220
      write (nout,200)
  200 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
      if (adapt) write (nout,210)
  210 format (1x,'cme is the estimate of the largest eigenvalue of',
     *   ' the jacobi matrix')
  220 if (iparm(11).eq.0) timi1 = timer(0.0)
c
c ... initialize forward pseudo-residual
c
      if (n.gt.1) go to 230
      u(1) = rhs(1) 
      go to 300
  230 do 240 i = 1,nw
         wksp(i) = 0.0
  240 continue
      call scopy (nr,rhs,1,wksp,1)
      call prsred (nr,ndim,maxnz,jcoef,coef,u,wksp)
      call scopy (nb,rhs(nrp1),1,wksp(ib2),1)
      call prsblk (nb,nr,ndim,maxnz,jcoef,coef,wksp,wksp(jb2))
      do 250 i = 1,nb
         wksp(n+i) = wksp(n+i)-u(nr+i)
  250 continue
c
c ... iteration sequence
c
      itmax1 = itmax+1
      do 270 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 260
c
c ... code for the even iterations.
c
c     u           = u(in)             wksp(jb2+nr) = d(in)
c     wksp(ib1)   = u(in-1)           wksp(jb3+nr) = d(in-1)
c
         call itrscg (n,nb,ndim,maxnz,jcoef,coef,u,wksp,wksp(jb2),
     *      wksp(jb3),wksp(jb4),wksp(ib5))
c
         if (halt) go to 300
         go to 270
c
c ... code for the odd iterations.
c
c     u           = u(in-1)           wksp(jb2+nr) = d(in-1)
c     wksp(ib1)   = u(in)             wksp(jb3+nr) = d(in)
c
  260    call itrscg (n,nb,ndim,maxnz,jcoef,coef,wksp,u,wksp(jb3),
     *      wksp(jb2),wksp(jb4),wksp(ib5))
c
         if (halt) go to 300
  270 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 280
      timi2 = timer(0.0)
      time1 = timi2-timi1
  280 ier = 63
      if (level.ge.1) write (nout,290) itmax
  290 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine rscg'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 330
c
c ... method has converged
c
  300 if (iparm(11).ne.0) go to 310
      timi2 = timer(0.0)
      time1 = timi2-timi1
  310 if (level.ge.1) write (nout,320) in
  320 format (/1x,'rscg  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  330 continue
      if (n.eq.1) go to 340
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
      call scopy (nr,rhs,1,u,1)
      call prsred (nr,ndim,maxnz,jcoef,coef,u,u)
c
c ... un-permute matrix,rhs, and solution
c
  340 if (iparm(9).ne.-1) go to 350
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp,iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.62) go to 360
c
c ... optional error analysis 
c
  350 idgts = iparm(12)
      if (idgts.lt.0) go to 360
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  360 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      iparm(8) = iparm(8)-4*(itmax-in)
      if (iparm(11).ne.0) go to 370
      timj2 = timer(0.0)
      time2 = timj2-timj1
  370 if (iparm(3).ne.0) go to 380
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  380 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,390)
  390 format (/1x,'execution successful')
c
      return
      end 
      subroutine rssi (n,ndim,maxnz,jcoef,coef,rhs,u,iwksp,nw,wksp,
     *                 iparm,rparm,ier) 
c
c     itpackv 2d main routine  rssi  (reduced system semi-iterative)
c     each of the main routines --
c           jcg, jsi, sor, ssorcg, ssorsi, rscg, rssi
c     can be used independently of the others
c
c ... function --
c
c          rssi drives the reduced system si algorithm.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for sparse matrix representation.
c          coef   array for sparse matrix representation.
c                 jcoef and coef use the ellpack data structure.
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 initial guess to the solution. on output, it contains
c                 the latest estimate to the solution.
c          iwksp  integer vector workspace of length 3*n
c          nw     input integer.  length of available wksp.  on output,
c                 iparm(8) is amount used.
c          wksp   vector used for working space.  rssi
c                 needs this to be in length at least  n + nb
c                 nb is the order of the black subsystem
c          iparm  integer vector of length 12.  allows user to specify
c                 some integer parameters which affect the method.
c          rparm  vector of length 12. allows user to specify some
c                 parameters which affect the method.
c          ier    output integer.  error flag.
c
c ... rssi module references --
c
c         from itpackv    cheby , chgsi , dfault, echall,
c                         iterm , itrssi, parsi , 
c                         permat, perror, pervec, pmult , prbndx,
c                         prsblk, prsred, pstop , sbelm ,
c                         scal  , sum3  , tstchg, 
c                         unscal,
c                         vout
c          system         abs, alog10, amax0, amax1, float, mod,
c                         sqrt
c
c ... local itpackv references --
c
c          echall, itrssi, permat,
c          perror, pervec, prbndx, prsred, sbelm , scal  ,
c          unscal
c
c     version -  itpackv 2d (january 1990) 
c
c     code written by - david kincaid, roger grimes, john respess
c                       center for numerical analysis
c                       university of texas
c                       austin, tx  78712
c                       (512) 471-1242
c
c     for additional details on the
c          (a) routine    see toms article 1982
c          (b) algorithm  see cna report 150
c
c     based on theory by - david young, david kincaid, lou hageman
c
c     reference the book - applied iterative methods
c                          l. hageman, d. young
c                          academic press, 1981
c
c     **************************************************
c     *               important note                   *
c     *                                                *
c     *      when installing itpackv routines on a     *
c     *  different computer, reset some of the values  *
c     *  in  subroutne dfault.   most important are    *
c     *                                                *
c     *   srelpr      machine relative precision       *
c     *   rparm(1)    stopping criterion               *
c     *                                                *
c     *   also change system-dependent routine         *
c     *   second used in timer                         *
c     *                                                *
c     **************************************************
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1),iparm(12)
      dimension coef(ndim,1),rhs(n),u(n),wksp(nw),rparm(12) 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... variables in common block - itcom1
c
c     in     - iteration number
c     is     - iteration number when parameters last changed
c     isym   - symmetric/nonsymmetric case switch 
c     itmax  - maximum number of iterations allowed
c     level  - level of output control switch
c     nout   - output unit number
c
c ... variables in common block - itcom2
c
c     adapt  - fully adaptive procedure switch
c     betadt - switch for adaptive determination of beta
c     caseii - adaptive procedure case switch
c     halt   - stopping test switch
c     partad - partially adaptive procedure switch
c
c ... variables in common block - itcom3
c
c     bdelnm - two norm of b times delta-super-n
c     betab  - estimate for the spectral radius of lu matrix
c     cme    - estimate of largest eigenvalue
c     delnnm - inner product of pseudo-residual at iteration n
c     delsnm - inner product of pseudo-residual at iteration s
c     ff     - adaptive procedure damping factor
c     gamma  - acceleration parameter
c     omega  - overrelaxation parameter for sor and ssor
c     qa     - pseudo-residual ratio
c     qt     - virtual spectral radius
c     rho    - acceleration parameter
c     rrr    - adaptive parameter
c     sige   - parameter sigma-sub-e
c     sme    - estimate of smallest eigenvalue
c     specr  - spectral radius estimate for ssor
c     srelpr - machine relative precision
c     stptst - stopping parameter
c     udnm   - two norm of u
c     zeta   - stopping criterion
c
c ... initialize common blocks
c
      ier = 0
      level = iparm(2)
      nout = iparm(4)
      if (level.ge.1) write (nout,10)
   10 format (///1x,'i t p a c k      r s s i    ')
      if (iparm(1).le.0) go to 360
      if (iparm(11).eq.0) timj1 = timer(0.0)
      call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,1)
      temp = 500.0*srelpr
      if (zeta.ge.temp) go to 30
      if (level.ge.1) write (nout,20) zeta,srelpr,temp
   20 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine rssi'/1x,
     *   '    rparm(1) =',e10.3,' (zeta)'/1x,
     *   '    a value this small may hinder convergence '/1x,
     *   '    since machine precision srelpr =',e10.3/1x,
     *   '    zeta reset to ',e10.3)
      zeta = temp
   30 continue
      time1 = rparm(9)
      time2 = rparm(10)
      digit1 = rparm(11)
      digit2 = rparm(12)
c
c ... verify n
c
      if (n.gt.0) go to 50
      ier = 71
      if (level.ge.0) write (nout,40) n 
   40 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rssi '/1x,
     *   '    invalid matrix dimension, n =',i8)
      go to 360
c
c ... scale linear system, u, and rhs by the square root of the
c ... diagonal elements.
c
   50 continue
      call scal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,ier)
      if (ier.eq.0) go to 70
      if (level.ge.0) write (nout,60) ier
   60 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rssi '/1x,
     *   '    error detected in routine  scal  '/1x,
     *   '    which scales the system   '/1x,
     *   '    ier = ',i5)
      go to 360
c
c ... remove rows and columns if requested
c
   70 continue
      if (iparm(10).eq.0) go to 80
      tol = rparm(8)
      call sbelm (n,ndim,maxnz,jcoef,coef,rhs,wksp,tol)
c
c ... initialize wksp base addresses.
c
   80 ib1 = 1
      ib2 = ib1+n
      ib3 = ib2+n
c
c ... permute to  red-black system if requested
c
      nb = iparm(9) 
      if (nb.ge.0) go to 110
      if (nb.le.-2) go to 170 
      n3 = n*3
      do 90 i = 1,n3
         iwksp(i) = 0
   90 continue
      call prbndx (n,ndim,maxnz,jcoef,iwksp,iwksp(ib2),nb,level,nout, 
     *   ier)
      if (ier.eq.0) go to 110 
      if (level.ge.0) write (nout,100) ier,nb
  100 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rssi '/1x,
     *   '    error detected in routine  prbndx'/1x,
     *   '    which computes the red-black indexing'/1x,
     *   '    ier = ',i5,' iparm(9) = ',i5,' (nb)')
      go to 340
  110 if (nb.ge.0.and.nb.le.n) go to 130
      ier = 74
      if (level.ge.0) write (nout,120) ier,nb
  120 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rssi     '/1x,
     *   '    error detected in red-black subsystem index'/1x,
     *   '    ier = ',i5,' iparm(9) =',i5,' (nb)')
      go to 340
  130 if (nb.ne.0.and.nb.ne.n) go to 150
      nbo = nb
      nb = n/2
      if (level.ge.2) write (nout,140) nbo,nb
  140 format (/10x,' nb = ',i5,' implies matrix is diagonal'/10x,
     *   ' nb reset to ',i5)
c
c ... permute matrix and rhs
c
  150 if (level.ge.2) write (nout,160) nb
  160 format (/10x,'order of black subsystem = ',i5,' (nb)')
      if (iparm(9).ge.0) go to 170
      call permat (n,ndim,maxnz,jcoef,coef,iwksp,wksp,iwksp(ib3))
      call pervec (n,iwksp,rhs,wksp)
      call pervec (n,iwksp,u,wksp)
c
c ... finish workspace base addresses
c
  170 jb2 = ib1+nb
      nr = n-nb
c
c ... check for sufficient workspace.
c
      iparm(8) = n+nb
      if (nw.ge.iparm(8)) go to 190
      ier = 72
      if (level.ge.0) write (nout,180) nw,iparm(8)
  180 format (/1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    called from itpackv routine rssi '/1x,
     *   '    not enough workspace at ',i10/1x,
     *   '    set iparm(8) =',i10,' (nw)')
      go to 320
c
  190 continue
      if (level.le.2) go to 210
      write (nout,200)
  200 format (///1x,'in the following, rho and gamma are',
     *   ' acceleration parameters')
  210 if (iparm(11).eq.0) timi1 = timer(0.0)
      do 220 i = 1,nw
         wksp(i) = 0.0
  220 continue
c
c ... iteration sequence
c
      if (n.gt.1) go to 230
      u(1) = rhs(1) 
      go to 280
c
  230 itmax1 = itmax+1
      do 250 loop = 1,itmax1
         in = loop-1
         if (mod(in,2).eq.1) go to 240
c
c ... code for the even iterations.
c
c     u           = u(in)
c     wksp(ib1)   = u(in-1)
c
         call itrssi (n,nb,ndim,maxnz,jcoef,coef,rhs,u,wksp,wksp(jb2))
c
         if (halt) go to 280
         go to 250
c
c ... code for the odd iterations.
c
c     u           = u(in-1)
c     wksp(ib1)   = u(in)
c
  240    call itrssi (n,nb,ndim,maxnz,jcoef,coef,rhs,wksp,u,wksp(jb2))
c
         if (halt) go to 280
  250 continue
c
c ... itmax has been reached
c
      if (iparm(11).ne.0) go to 260
      timi2 = timer(0.0)
      time1 = timi2-timi1
  260 ier = 73
      if (level.ge.1) write (nout,270) itmax
  270 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine rssi'/1x,
     *   '    failure to converge in',i5,' iterations')
      if (iparm(3).eq.0) rparm(1) = stptst
      go to 310
c
c ... method has converged
c
  280 if (iparm(11).ne.0) go to 290
      timi2 = timer(0.0)
      time1 = timi2-timi1
  290 if (level.ge.1) write (nout,300) in
  300 format (/1x,'rssi  has converged in ',i5,' iterations')
c
c ... put solution into u if not already there.
c
  310 continue
      if (n.eq.1) go to 320
      if (mod(in,2).eq.1) call scopy (n,wksp,1,u,1)
      call scopy (nr,rhs,1,u,1)
      call prsred (nr,ndim,maxnz,jcoef,coef,u,u)
c
c ... un-permute matrix,rhs, and solution
c
  320 if (iparm(9).ne.-1) go to 330
      call permat (n,ndim,maxnz,jcoef,coef,iwksp(ib2),wksp,iwksp(ib3))
      call pervec (n,iwksp(ib2),rhs,wksp(ib2))
      call pervec (n,iwksp(ib2),u,wksp(ib2))
      if (ier.eq.72) go to 340
c
c ... optional error analysis 
c
  330 idgts = iparm(12)
      if (idgts.lt.0) go to 340
      if (iparm(2).le.0) idgts = 0
      call perror (n,ndim,maxnz,jcoef,coef,rhs,u,wksp,digit1,digit2,
     *   idgts)
c
c ... unscale the matrix, solution, and rhs vectors.
c
  340 continue
      call unscal (n,ndim,maxnz,jcoef,coef,rhs,u,wksp)
c
c ... set return parameters in iparm and rparm
c
      if (iparm(11).ne.0) go to 350
      timj2 = timer(0.0)
      time2 = timj2-timj1
  350 if (iparm(3).ne.0) go to 360
      iparm(1) = in 
      iparm(9) = nb 
      rparm(2) = cme
      rparm(3) = sme
      rparm(9) = time1
      rparm(10) = time2
      rparm(11) = digit1
      rparm(12) = digit2
c
  360 continue
      if (level.ge.3) call echall (n,ndim,maxnz,jcoef,coef,rhs,iparm, 
     *   rparm,2)
      if (ier.eq.0.and.level.ge.1) write (nout,370)
  370 format (/1x,'execution successful')
c
      return
      end 
      subroutine itjcg (n,ndim,maxnz,jcoef,coef,u,u1,d,d1,dtwd,tri)
c
c ... itjcg performs one iteration of the jacobi conjugate gradient
c     algorithm.  it is called by jcg.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer sparse matrix representation
c          coef   sparse matrix representation
c          u      input vector.  contains the value of the
c                 solution vector at the end of in iterations.
c          u1     input/output vector.  on input, it contains
c                 the value of the solution at the end of the in-1
c                 iteration.  on output, it will contain the newest
c                 estimate for the solution vector.
c          d      input vector.  contains the pseudo-residual
c                 vector after in iterations.
c          d1     input/output vector.  on input, d1 contains
c                 the pseudo-residual vector after in-1 iterations.  on
c                 output, it will contain the newest pseudo-residual
c                 vector.
c          dtwd   array.  used in the computations of the
c                 acceleration parameter gamma and the new pseudo-
c                 residual.
c          tri    array.  stores the tridiagonal matrix associated
c                 with the eigenvalues of the conjugate gradient
c                 polynomial. 
c
c ... local itpackv references --
c
c          chgcon, iterm , parcon, pjac  , pstop ,
c          sum3
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension u(n),u1(n),d(n),d1(n),dtwd(n),tri(2,1),coef(ndim,1)
c
c ... specifications for local variables
c
      logical q1
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine jcg
c
c ... compute new estimate for cme if adapt = .true.
c
      save
      if (adapt) call chgcon (itmax,tri,gamold,rhoold,1)
c
c ... test for stopping
c
      delnnm = sdot(n,d,1,d,1)
      dnrm = delnnm 
      con = cme
      call pstop (n,u,dnrm,con,1,q1)
      if (halt) go to 50
c
c ... compute rho and gamma - acceleration parameters
c
      do 10 i = 1,n 
         dtwd(i) = 0.0
   10 continue
      call pjac (n,ndim,maxnz,jcoef,coef,d,dtwd)
      dtnrm = sdot(n,d,1,dtwd,1)
      if (isym.eq.0) go to 20 
      rhotmp = sdot(n,dtwd,1,d1,1)
      call parcon (dtnrm,c1,c2,c3,c4,gamold,rhotmp,1)
      rhoold = rhotmp
      go to 30
   20 call parcon (dtnrm,c1,c2,c3,c4,gamold,rhoold,1)
c
c ... compute u(in+1) and d(in+1)
c
   30 do 40 i = 1,n 
         u1(i) = c1*d(i)+c2*u(i)+c3*u1(i)
         d1(i) = c1*dtwd(i)+c4*d(i)+c3*d1(i)
   40 continue
c
c ... output intermediate information
c
   50 call iterm (n,coef,u,dtwd,1)
c
      return
      end 
      subroutine itjsi (n,ndim,maxnz,jcoef,coef,rhs,u,u1,d,icnt)
c
c ... function --
c
c          itjsi performs one iteration of the
c          jacobi semi-iterative algorithm.  it is called by jsi.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                  routine
c          maxnz  maximum number of nonzero entries per row 
c          jcoef  integer array for sparse matrix representation
c          coef   array for sparse matrix representation
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input vector.  contains the estimate for the
c                 solution vector after in iterations.
c          u1     input/output vector.  on input, u1 contains the
c                 solution vector after in-1 iterations.  on output,
c                 it will contain the newest estimate for the solution
c                 vector.
c          d      array.  d is used for the computation of the
c                 pseudo-residual array for the current iteration.
c          icnt   number of iterations since last change of sme
c
c ... local itpackv references --
c
c          chgsi , chgsme, iterm , parsi , pjac  , pstop , pvtbv ,
c          sum3  , tstchg
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),u(1),u1(1),d(1)
c
c ... specifications for local variables
c
      logical q1
c
c ... specifications for function subprograms
c
      logical tstchg,chgsme
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine jsi
c
      if (in.eq.0) icnt = 0
c
c ... compute pseudo-residuals
c
      call scopy (n,rhs,1,d,1)
      call pjac (n,ndim,maxnz,jcoef,coef,u,d)
      do 10 i = 1,n 
         d(i) = d(i)-u(i)
   10 continue
c
c ... stopping and adaptive change tests
c
      oldnrm = delnnm
      delnnm = sdot(n,d,1,d,1)
      dnrm = delnnm 
      con = cme
      call pstop (n,u,dnrm,con,1,q1)
      if (halt) go to 70
      if (.not.adapt) go to 50
      if (.not.tstchg(1)) go to 20
c
c ... change iterative parameters (cme) 
c
      dtnrm = pvtbv(n,ndim,maxnz,jcoef,coef,d)
      call chgsi (dtnrm,1)
      if (.not.adapt) go to 50
      go to 30
c
c ... test if sme needs to be changed and change if necessary.
c
   20 continue
      if (caseii) go to 50
      if (.not.chgsme(oldnrm,icnt)) go to 50
      icnt = 0
c
c ... compute u(in+1) after change of parameters
c
   30 do 40 i = 1,n 
         u1(i) = u(i)+gamma*d(i)
   40 continue
      go to 70
c
c ... compute u(in+1) without change of parameters
c
   50 call parsi (c1,c2,c3,1) 
      do 60 i = 1,n 
         u1(i) = c1*d(i)+c2*u(i)+c3*u1(i)
   60 continue
c
c ... output intermediate information
c
   70 call iterm (n,coef,u,d,2)
c
      return
      end 
      subroutine itsor (n,nb,ndim,maxnz,jcoef,coef,rhs,u,nup,
     *                  moved,wk,iwk)
c
c          itsor performs one iteration of the successive
c          overrelaxation algorithm.  it is called by sor.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          nb     dimension of the black subsystem
c          ndim   row dimension of coef and jcoef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array of coefficient columns
c          coef   array of coefficients 
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input/output vector.  on input, u contains the
c                 solution vector after in iterations.  on output,
c                 it will contain the newest estimate for the solution
c                 vector.
c          nup    maximum number of nonzeros per row in upper
c                  triangle u (for natural ordering only)
c          moved  logical variable indicating whether or not matrix
c                  was reshuffled
c          wk     array.  work vector of length 2*n.
c          iwk    integer array giving wave populations.
c
c ... local itpackv references --
c
c          ipstr , iterm , pfsor1, pstop , tau
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwk(1)
      dimension coef(ndim,1),rhs(1),u(1),wk(1)
      logical moved 
c
c ... specifications for local variables
c
      logical change,q1
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine sor
c
c ... set initial parameters not already set
c
      save
      ib2 = n+1
      if (in.ne.0) go to 20
      call pstop (n,u,0.0,0.0,0,q1)
      if (adapt) go to 10
      change = .false.
      ip = 0
      iphat = 2
      iss = 0
      call sorscl (n,ndim,maxnz,coef,rhs,omega)
      go to 30
c
   10 change = .true.
      ip = 0
      omegap = omega
      omega = 1.0
      iss = 0
      iphat = 2
      ipstar = 4
      if (omegap.le.1.0) change = .false.
c
c ... reset omega, iphat, and ipstar (circle a in flowchart)
c
   20 if (.not.change) go to 30
      change = .false.
      is = is+1
      ip = 0
      iss = 0
      omgsav = omega
      omega = amin1(omegap,tau(is))
      iphat = max0(3,int((omega-1.0)/(2.0-omega)))
      ipstar = ipstr(omega)
      factor = omega/omgsav
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
c
c ... compute u (in + 1) and norm of del(s,p) - circle b in flow chart
c
   30 continue
      delsnm = delnnm
      spcrm1 = specr
      call scopy (n,rhs,1,wk,1)
      call pfsor1 (n,nb,ndim,maxnz,jcoef,coef,u,wk,nup,moved,
     *   wk(ib2),iwk)
      if (delnnm.eq.0.0) go to 40
      if (in.ne.0) specr = delnnm/delsnm
      if (ip.lt.iphat) go to 80
c
c ... stopping test, set h
c
      if (specr.ge.1.0) go to 80
      if (.not.(specr.gt.(omega-1.0))) go to 40
      h = specr
      go to 50
   40 iss = iss+1
      h = omega-1.0 
c
c ... perform stopping test.
c
   50 continue
      dnrm = delnnm**2
      call pstop (n,u,dnrm,h,1,q1)
      if (.not.halt) go to 60 
      factor = 1.0/omega
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
      go to 80
c
c ... method has not converged yet, test for changing omega 
c
   60 if (.not.adapt) go to 80
      if (ip.lt.ipstar) go to 80
      if (omega.gt.1.0) go to 70
      cme = sqrt(abs(specr))
      omegap = 2.0/(1.0+sqrt(abs(1.0-specr)))
      change = .true.
      go to 80
   70 if (iss.ne.0) go to 80
      if (specr.le.(omega-1.0)**ff) go to 80
      if ((specr+0.00005).le.spcrm1) go to 80
c
c ... change parameters
c
      cme = (specr+omega-1.0)/(sqrt(abs(specr))*omega)
      omegap = 2.0/(1.0+sqrt(abs(1.0-cme*cme)))
      change = .true.
c
c ... output intermediate information
c
   80 call iterm (n,coef,u,wk,3)
      ip = ip+1
c
      return
      end 
      subroutine itsrcg (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,u,
     *                   u1,c,c1,d,dl,moved,wk,tri,iwk)
c
c ... function --
c
c          itsrcg performs one iteration of the symmetric sor
c          conjugate gradient algorithm.  it is called by ssorcg.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          nb     order of the black subsystem
c          nup    maximum number of nonzeros per row in the upper
c                  triangle u  (input for natural ordering only)
c          nlow   maximum number of nonzeros per row in the lower
c                  triangle l  (input for natural ordering only)
c          ndim   row dimension of jcoef and coef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row in the entire
c                  array
c          jcoef  integer array for coefficient columns
c          coef   array for coefficients
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input vector.  contains the estimate of the
c                 solution vector after in iterations.
c          u1     input/output vector.  on input, u1 contains the
c                 the estimate for the solution after in-1 iterations.
c                 on output, u1 contains the updated estimate.
c          c      input vector.  contains the forward residual
c                 after in iterations.
c          c1     input/output vector.  on input, c1 contains
c                 the forward residual after in-1 iterations.  on
c                 output, c1 contains the updated forward residual.
c          d      vector.  is used to compute the backward pseudo-
c                 residual vector for the current iteration.
c          dl     vector.  is used in the computations of the
c                 acceleration parameters.
c          moved  logical constant indicating whether or not matrix
c                  has been reshuffled
c          wk     workspace vector of length n.
c          tri    vector. stores the tridiagonal matrix associated
c                 with the conjugate gradient acceleration. 
c          iwk    integer vector giving wave population counts
c
c ... local itpackv references --
c
c        chgcon, iterm , omeg  , omgchg, omgstr, parcon, pbeta ,
c        pbsor , pfsor , pjac  , pstop , pvtbv ,
c        sum3
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwk(1)
      dimension coef(ndim,1),rhs(1),u(1),u1(1),c(1),c1(1),d(1),dl(1), 
     *   wk(1),tri(2,1)
c
c ... specifications for local variables
c
      logical q1,moved
c
c ... specifications for function subprograms
c
      logical omgchg,omgstr
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine ssorcg
c
c ... calculate s-prime for adaptive procedure.
c
      save
      if (adapt.or.partad) call chgcon (itmax,tri,gamold,rhoold,3)
c
c ... compute backward residual
c
      call scopy (n,rhs,1,wk,1)
      do 10 i = 1,n 
         d(i) = c(i)+u(i)
   10 continue
      call pbsor (n,nb,ndim,maxnz,jcoef,coef,d,wk,nup,nlow,moved,iwk) 
      do 20 i = 1,n 
         d(i) = d(i)-u(i)
   20 continue
c
c ... compute acceleration parameters and then u(in+1) (in u1)
c
      call scopy (n,d,1,dl,1) 
      do 30 i = 1,n 
         wk(i) = 0.0
   30 continue
      call pfsor (n,nb,ndim,maxnz,jcoef,coef,dl,wk,nup,moved,iwk)
      do 40 i = 1,n 
         dl(i) = d(i)-dl(i)
   40 continue
      delnnm = sdot(n,c,1,c,1)
      if (delnnm.eq.0.0) go to 80
      dnrm = sdot(n,c,1,dl,1) 
      if (dnrm.eq.0.0) go to 80
      if (isym.eq.0) go to 50 
      rhotmp = sdot(n,c,1,c1,1)-sdot(n,dl,1,c1,1) 
      call parcon (dnrm,t1,t2,t3,t4,gamold,rhotmp,3)
      rhoold = rhotmp
      go to 60
   50 call parcon (dnrm,t1,t2,t3,t4,gamold,rhoold,3)
   60 do 70 i = 1,n 
         u1(i) = t1*d(i)+t2*u(i)+t3*u1(i)
   70 continue
c
c ... test for stopping
c
   80 bdelnm = sdot(n,d,1,d,1)
      dnrm = bdelnm 
      con = specr
      call pstop (n,u,dnrm,con,1,q1)
      if (halt) go to 190
c
c ... if non- or partially-adaptive, compute c(in+1) and exit.
c
      if (adapt) go to 100
      do 90 i = 1,n 
         c1(i) = -t1*dl(i)+t2*c(i)+t3*c1(i)
   90 continue
      go to 190
c
c ... fully adaptive procedure
c
  100 continue
      omgsav = omega
      if (omgstr(1)) go to 170
      if (omgchg(1)) go to 120
c
c ... parameters have been unchanged.  compute c(in+1) and exit.
c
      do 110 i = 1,n
         c1(i) = -t1*dl(i)+t2*c(i)+t3*c1(i)
  110 continue
      go to 190
c
c ... it has been decided to change parameters
c        (1) compute new betab if betadt = .true. 
c
  120 continue
      if (.not.betadt) go to 130
      betnew = pbeta(n,nb,ndim,maxnz,jcoef,coef,d,nup,nlow,moved,wk,c1)/
     *   (bdelnm*omega**2)
      betab = amax1(betab,0.25,betnew)
c
c ...    (2) compute new cme, omega, and specr
c
  130 continue
      if (caseii) go to 140
      dnrm = pvtbv(n,ndim,maxnz,jcoef,coef,d)/omega
      go to 160
  140 do 150 i = 1,n
         wk(i) = 0.0
  150 continue
      call pjac (n,ndim,maxnz,jcoef,coef,d,wk)
      dnrm = sdot(n,wk,1,wk,1)/(omega**2)
  160 call omeg (dnrm,3)
c
c ...    (3) compute new forward residual since omega has been changed.
c
  170 continue
      factor = omega/omgsav
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
      call scopy (n,rhs,1,wk,1)
      call scopy (n,u1,1,c1,1)
      call pfsor (n,nb,ndim,maxnz,jcoef,coef,c1,wk,nup,moved,iwk)
      do 180 i = 1,n
         c1(i) = c1(i)-u1(i)
  180 continue
c
c ... output intermediate results.
c
  190 call iterm (n,coef,u,wk,4)
c
      return
      end 
      subroutine itsrsi (n,nb,nup,nlow,ndim,maxnz,jcoef,coef,rhs,u,
     *                   u1,c,d,ctwd,moved,wk,iwk)
c
c ... function --
c
c          itsrsi performs one iteration of the symmetric sor
c          semi-iteration algorithm.  it is called by ssorsi.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          nb     order of the black subsystem (for red-black order-
c                   ing only) 
c          nup    maximum number of nonzeros per row in the upper
c                   triangle u (for natural ordering only)
c          nlow   maximum number of nonzeros per row in the lower
c                   triangle l (for natural ordering only)
c          ndim   row dimension of coef and jcoef arrays in calling
c                   routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for coefficient columns
c          coef   array for coefficients
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          u      input vector.  contains the estimate of the
c                 solution vector after in iterations.
c          u1     input/output vector.  on input, u1 contains the
c                 the estimate for the solution after in-1 iterations.
c                 on output, u1 contains the updated estimate.
c          c      vector.  is used to compute the forward pseudo-
c                 residual vector for the current iteration.
c          d      vector.  is used to compute the backward pseudo-
c                 residual vector for the current iteration.
c          ctwd   vector.  is used in the computations of the
c                 acceleration parameters.
c          moved  logical constant indicating whether or not matrix
c                  has been reshuffled
c          wk     workspace vector of length n.
c          iwk    integer vector giving wave population counts
c
c ... local itpackv references --
c
c         chgsi , iterm , omeg  , omgstr, parsi , pbeta , pbsor ,
c         pfsor , pjac  , pstop , pvtbv , sum3  , 
c         tstchg
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwk(1)
      dimension coef(ndim,1),rhs(1),u(1),u1(1),c(1),d(1),ctwd(1),wk(1)
c
c ... specifications for local variables
c
      logical q1,moved
c
c ... specifications for function subprograms
c
      logical omgstr,tstchg
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine ssorsi
c
c ... compute pseudo-residuals (forward and backward)
c
      call scopy (n,rhs,1,wk,1)
      call scopy (n,u,1,ctwd,1)
      call pfsor (n,nb,ndim,maxnz,jcoef,coef,ctwd,wk,nup,moved,iwk)
      do 10 i = 1,n 
         c(i) = ctwd(i)-u(i)
   10 continue
      call scopy (n,rhs,1,wk,1)
      call pbsor (n,nb,ndim,maxnz,jcoef,coef,ctwd,wk,nup,nlow,moved,iwk)
      do 20 i = 1,n 
         d(i) = ctwd(i)-u(i)
   20 continue
c
c ... compute u(in+1) -- contained in the vector u1.
c
      call parsi (c1,c2,c3,3) 
      do 30 i = 1,n 
         u1(i) = c1*d(i)+c2*u(i)+c3*u1(i)
   30 continue
c
c ... test for stopping
c
      bdelnm = sdot(n,d,1,d,1)
      dnrm = bdelnm 
      con = specr
      call pstop (n,u,dnrm,con,1,q1)
      if (halt.or..not.(adapt.or.partad)) go to 100
c
c ... adaptive procedure
c
      omgsav = omega
      if (omgstr(1)) go to 100
      delnnm = sdot(n,c,1,c,1)
      if (in.eq.is) delsnm = delnnm
      if (in.eq.0.or..not.tstchg(1)) go to 100
c
c ... it has been decided to change parameters.
c ...    (1) compute ctwd
c
      call scopy (n,d,1,ctwd,1)
      do 40 i = 1,n 
         wk(i) = 0.0
   40 continue
      call pfsor (n,nb,ndim,maxnz,jcoef,coef,ctwd,wk,nup,moved,iwk)
      do 50 i = 1,n 
         ctwd(i) = ctwd(i)+c(i)-d(i)
   50 continue
c
c ...    (2) compute new spectral radius for current omega. 
c
      dnrm = sdot(n,c,1,ctwd,1)
      call chgsi (dnrm,3)
      if (.not.adapt) go to 100
c
c ...    (3) compute new betab if betadt = .true. 
c
      if (.not.betadt) go to 60
      betnew = pbeta(n,nb,ndim,maxnz,jcoef,coef,d,nup,nlow,moved,wk,
     *   ctwd)/(bdelnm*omega**2)
      betab = amax1(betab,0.25,betnew)
c
c ...    (4) compute new cme, omega, and specr.
c
   60 continue
      if (caseii) go to 70
      dnrm = pvtbv(n,ndim,maxnz,jcoef,coef,d)/omega
      go to 90
   70 do 80 i = 1,n 
         wk(i) = 0.0
   80 continue
      call pjac (n,ndim,maxnz,jcoef,coef,d,wk)
      dnrm = sdot(n,wk,1,wk,1)/(omega**2)
   90 call omeg (dnrm,3)
      factor = omega/omgsav
      call sorscl (n,ndim,maxnz,coef,rhs,factor)
c
c ... output intermediate information
c
  100 call iterm (n,coef,u,wk,5)
c
      return
      end 
      subroutine itrscg (n,nb,ndim,maxnz,jcoef,coef,ub,ub1, 
     *                   db,db1,wb,tri) 
c
c ... itrscg performs one iteration of the reduced system conjugate
c ... gradient algorithm.  it is called by rscg.
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          nb     input integer.  contains the number of black points 
c                 in the red-black matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                 routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer data structure for coefficient columns
c          coef   data structure for matrix coefficients
c          ub     input vector.  contains the estimate for the
c                 solution on the black points after in iterations.
c          ub1    input/output vector.  on input, ub1 contains
c                 the solution vector after in-1 iterations.  on
c                 output, it will contain the newest estimate for
c                 the solution vector.  this is only for the black
c                 points.
c          db     input array.  db(nrp1) contains the value of
c                 the current pseudo-residual on the black points.
c          db1    input/output array.  db1(nrp1) contains the
c                 pseudo-residual on the black points for the in-1
c                 iteration.
c                 on input.  on output, it is for the in+1 iteration. 
c          wb     array.  wb is used for computations involving
c                 black vectors.
c          tri    array.  stores the tridiagonal matrix associated
c                 with conjugate gradient acceleration.
c
c ... local itpackv references --
c
c          chgcon, iterm , parcon, prsblk, prsred, pstop ,
c          sum3
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),ub(1),ub1(1),db(1),db1(1),wb(1),tri(2,1) 
c
c ... specifications for local variables
c
      logical q1
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine rscg
c
c ... compute new estimate for cme if adapt = .true.
c
      save
      nr = n-nb
      nrp1 = nr+1
      if (adapt) call chgcon (itmax,tri,gamold,rhoold,2)
c
c ... test for stopping
c
      delnnm = sdot(nb,db(nrp1),1,db(nrp1),1)
      dnrm = delnnm 
      con = cme
      call pstop (nb,ub(nrp1),dnrm,con,2,q1)
      if (halt) go to 70
c
c ... compute acceleration parameters
c
      do 10 i = 1,nr
         ub1(i) = 0.0
   10 continue
      call prsred (nr,ndim,maxnz,jcoef,coef,db,ub1)
      do 20 i = nrp1,n
         wb(i) = 0.0
   20 continue
      call prsblk (nb,nr,ndim,maxnz,jcoef,coef,ub1,wb)
      dnrm = sdot(nb,db(nrp1),1,wb(nrp1),1)
      if (isym.eq.0) go to 30 
      rhotmp = sdot(nb,wb(nrp1),1,db1(nrp1),1)
      call parcon (dnrm,c1,c2,c3,c4,gamold,rhotmp,2)
      rhoold = rhotmp
      go to 40
   30 call parcon (dnrm,c1,c2,c3,c4,gamold,rhoold,2)
c
c ... compute ub(in+1) and db(in+1)
c
   40 do 50 i = nrp1,n
         ub1(i) = c1*db(i)+c2*ub(i)+c3*ub1(i)
   50 continue
      do 60 i = nrp1,n
         db1(i) = c1*wb(i)+c4*db(i)+c3*db1(i)
   60 continue
c
c ... output intermediate information
c
   70 call iterm (nb,coef(nrp1,1),ub(nrp1),wb(nrp1),6)
      return
c
      end 
      subroutine itrssi (n,nb,ndim,maxnz,jcoef,coef,rhs,ub,ub1,db)
c
c ... itrssi performs one iteration of the reduced system semi-
c ... iteration algorithm.  it is called by rssi. 
c
c ... parameter list --
c
c          n      input integer.  dimension of the matrix.
c          nb     input integer.  contains the number of black points 
c                 in the red-black matrix.
c          ndim   row dimension of jcoef and coef arrays in calling
c                 routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer data structure for coefficient columns
c          coef   data structure for matrix coefficients
c          rhs    input vector.  contains the right hand side
c                 of the matrix problem.
c          ub     input vector.  contains the estimate for the
c                 solution on the black points after in iterations.
c          ub1    input/output vector.  on input, ub1 contains
c                 the solution vector after in-1 iterations.  on
c                 output, it will contain the newest estimate for
c                 the solution vector.  this is only for the black
c                 points.
c          db     input array.  db(nrp1) contains the value of
c                 the current pseudo-residual on the black points.
c
c ... local itpackv references --
c
c          chgsi , iterm , parsi , prsblk, prsred, pstop ,
c          sum3  , tstchg
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),ub(1),ub1(1),db(1)
c
c ... specifications for local variables
c
      logical q1
c
c ... specifications for function subprograms
c
      logical tstchg
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in routine rssi
c
c ... compute ur(in) into ub
c
      nr = n-nb
      nrp1 = nr+1
      call scopy (nr,rhs,1,ub,1)
      call prsred (nr,ndim,maxnz,jcoef,coef,ub,ub)
c
c ... compute pseudo-residual, db(in)
c
      call scopy (nb,rhs(nrp1),1,db(nrp1),1)
      call prsblk (nb,nr,ndim,maxnz,jcoef,coef,ub,db)
      do 10 i = nrp1,n
         db(i) = db(i)-ub(i)
   10 continue
c
c ... test for stopping
c
      delnnm = sdot(nb,db(nrp1),1,db(nrp1),1)
      dnrm = delnnm 
      const = cme
      call pstop (nb,ub(nrp1),dnrm,const,2,q1)
      if (halt) go to 60
      if (.not.adapt) go to 40
c
c ... test to change parameters
c
      if (.not.tstchg(2)) go to 40
c
c ... change parameters
c
      do 20 i = 1,nr
         ub1(i) = 0.0
   20 continue
      call prsred (nr,ndim,maxnz,jcoef,coef,db,ub1)
      dnrm = sdot(nr,ub1,1,ub1,1)
      call chgsi (dnrm,2)
      if (.not.adapt) go to 40
c
c ... compute ub(n+1) after changing parameters
c
      do 30 i = nrp1,n
         ub1(i) = ub(i)+gamma*db(i)
   30 continue
      go to 60
c
c ... compute ub(n+1) without change of parameters
c
   40 call parsi (c1,c2,c3,2) 
      do 50 i = nrp1,n
         ub1(i) = c1*db(i)+c2*ub(i)+c3*ub1(i)
   50 continue
c
c ... output intermediate information
c
   60 call iterm (nb,coef(nrp1,1),ub(nrp1),db(nrp1),7)
      return
c
      end 
      function cheby (qa,qt,rrr,ip,cme,sme)
c
c ... cheby computes the solution to the chebyshev equation 
c
c ... parameter list --
c
c          qa     ratio of pseudo-residuals
c          qt     virtual spectral radius
c          rrr    adaptive parameter
c          ip     number of iterations since last change of 
c                     parameters
c          cme,   estimates for the largest and smallest eigen-
c          sme      values of the iteration matrix
c
      z = 0.5*(qa+sqrt(abs(qa**2-qt**2)))*(1.0+rrr**ip)
      x = z**(1.0/float(ip))
      y = (x+rrr/x)/(1.0+rrr) 
c
      cheby = 0.5*(cme+sme+y*(2.0-cme-sme))
c
      return
      end 
      subroutine chgcon (ldt,tri,gamold,rhoold,ibmth)
c
c ... chgcon computes the new estimate for the largest eigenvalue
c     for conjugate gradient acceleration.
c
c ... parameter list --
c
c          ldt    leading dimension of tri
c          tri    tridiagonal matrix associated with the eigenvalues
c                    of the conjugate gradient polynomial
c          gamold
c            and
c          rhoold previous values of acceleration parameters
c          ibmth  indicator of basic method being accelerated by cg
c                      ibmth = 1,  jacobi
c                            = 2,  reduced system 
c                            = 3,  ssor 
c
c ... local itpackv references --
c
c          eigvns, eigvss
c
c ... specifications for arguments
c
      dimension tri(ldt,4)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      save
      go to (10,20,30), ibmth 
c
c ... jacobi conjugate gradient
c
   10 start = cme
      ip = in
      go to 40
c
c ... reduced system cg
c
   20 start = cme**2
      ip = in
      go to 40
c
c ... ssor cg
c
   30 if (adapt) start = spr
      if (.not.adapt) start = specr
      ip = in-is
c
c ... define the matrix
c
   40 if (ip.ge.2) go to 60
      if (ip.eq.1) go to 50
c
c ... ip = 0
c
      end = 0.0
      cmold = 0.0
      go to 110
c
c ... ip = 1
c
   50 end = 1.0-1.0/gamma
      tri(1,1) = end
      tri(1,2) = 0.0
      go to 110
c
c ... ip > 1
c
   60 if (abs(start-cmold).le.zeta*start) go to 120
      cmold = start 
c
c ... compute the largest eigenvalue
c
      tri(ip,1) = 1.0-1.0/gamma
      tri(ip,2) = (1.0-rho)/(rho*rhoold*gamma*gamold)
      if (isym.ne.0) go to 80 
      end = eigvss(ip,tri,start,zeta,itmax,ier)
      if (ier.eq.0) go to 100 
      if (level.ge.2) write (nout,70) ier
   70 format (/10x,'difficulty in computation of maximum eigenvalue'/ 
     *         15x,'of iteration matrix'/
     *         10x,'routine zbrent returned ier =',i5)
      go to 100
   80 continue
      end = eigvns(ldt,ip,tri,tri(1,3),tri(1,4),ier)
      if (ier.eq.0) go to 100 
      if (level.ge.2) write (nout,90) ier
   90 format (/10x,'difficulty in computation of maximum eigenvalue'/ 
     *         15x,'of iteration matrix'/
     *         10x,'routine eqrt1s returned ier =',i5)
  100 continue
      if (ier.ne.0) go to 130 
c
c ... set spectral radius for the various methods 
c
  110 if (ibmth.eq.1) cme = end
      if (ibmth.eq.2) cme = sqrt(abs(end))
      if (ibmth.eq.3.and.adapt) spr = end
      if (ibmth.eq.3.and..not.adapt) specr = end
      return
c
c ... relative change in cme is less than zeta.  therefore stop
c     changing.
c
  120 adapt = .false.
      partad = .false.
      return
c
c ... estimate for cme > one.  therefore need to stop adaptive
c     procedure and keep old value of cme.
c
  130 adapt = .false.
      partad = .false.
      if (level.ge.2) write (nout,140) in,start
  140 format (/10x,'estimate of maximum eigenvalue of jacobi   '/15x, 
     *   'matrix (cme) not accurate'/10x,
     *   'adaptive procedure turned off at iteration ',i5/10x,
     *   'final estimate of maximum eigenvalue =',e15.7/)
c
      return
      end 
      subroutine chgsi (dtnrm,ibmth)
c
c ... chgsi computes new chebyshev acceleration parameters adaptively.
c
c ... parameter list --
c
c          dtnrm  numerator of rayleigh quotient
c          ibmth  indicator of basic method being accelerated by si
c                      ibmth = 1,   jacobi
c                            = 2,   reduced system
c                            = 3,   symmetric sor 
c
c ... local itpackv references --
c
c          cheby
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      go to (10,30,50), ibmth 
c
c     ---------------------
c ... jacobi semi-iterative
c     ---------------------
c
c ... chebyshev equation
c
   10 continue
      if (in.eq.0) zm1 = cme
      if (in.ne.0) zm1 = cheby(qa,qt,rrr,in-is,cme,sme)
c
c ... rayleigh quotient
c
      zm2 = dtnrm/delnnm
c
c ... computation of iterative parameters
c
      cmold = cme
      cme = amax1(zm1,zm2,cmold)
      if (cme.ge.1.0) go to 20
      if (caseii) sme = -cme
      sige = (cme-sme)/(2.0-cme-sme)
      gamma = 2.0/(2.0-cme-sme)
      rrr = (1.0-sqrt(abs(1.0-sige*sige)))/(1.0+sqrt(abs(1.0-sige*sige))
     *   )
      is = in
      delsnm = delnnm
      rho = 1.0
      if (level.ge.2) write (nout,90) in,zm1,zm2,cme,gamma,cme
      return
c
c ... adaptive procedure failed for jacobi si
c
   20 cme = cmold
      adapt = .false.
      if (level.ge.2) write (nout,110) in,cme
      return
c
c     -----------------------------
c ... reduced system semi-iterative
c     -----------------------------
c
c ... chebyshev equation
c
   30 continue
      if (in.eq.0) zm1 = cme
      if (in.ne.0) zm1 = cheby(qa,qt,rrr,2*(in-is),0.0,0.0) 
c
c ... rayleigh quotient
c
      zm2 = sqrt(abs(dtnrm/delnnm))
c
c ... computation of new iterative parameters
c
      cmold = cme
      cme = amax1(zm1,zm2,cmold)
      if (cme.ge.1.0) go to 40
      sige = cme*cme/(2.0-cme*cme)
      gamma = 2.0/(2.0-cme*cme)
      rrr = (1.0-sqrt(abs(1.0-cme*cme)))/(1.0+sqrt(abs(1.0-cme*cme))) 
      is = in
      delsnm = delnnm
      rho = 1.0
      if (level.ge.2) write (nout,90) in,zm1,zm2,cme,gamma,cme
      return
c
c ... adaptive procedure failed for reduced system si
c
   40 cme = cmold
      adapt = .false.
      if (level.ge.2) write (nout,110) in,cme
      return
c
c     -----------------------------
c ... symmetric sor semi-iterative
c     ----------------------------
c
   50 continue
      if (specr.eq.0.0) specr = .171572875
      if (in.eq.0) go to 60
      zm1 = cheby(qa,qt,rrr,in-is,specr,0.0)
      go to 70
   60 zm1 = specr
      spr = specr
c
c ... rayleigh quotient
c
   70 zm2 = dtnrm/delnnm
c
c ... computation of new estimate for spectral radius
c
      if (adapt) go to 80
c
c ... partially adaptive ssor si
c
      specr = amax1(zm1,zm2,specr)
      is = in+1
      delsnm = delnnm
      if (level.ge.2) write (nout,100) in,zm1,zm2,cme,specr 
      return
c
c ... fully adaptive ssor si
c
   80 spr = amax1(zm1,zm2,spr)
      return
c
c ... format statements
c
   90 format (/5x,'parameters were changed at iteration no.',i5/10x,
     *   'solution to chebyshev eqn.       =',e15.7/10x,
     *   'solution to rayleigh quotient    =',e15.7/10x,
     *   'new estimate for cme             =',e15.7/10x,
     *   'new estimate for gamma           =',e15.7/10x,
     *   'new estimate for spectral radius =',e15.7/)
c
  100 format (/5x,'parameters were changed at iteration no.',i5/10x,
     *   'solution to chebyshev eqn.       =',e15.7/10x,
     *   'solution to rayleigh quotient    =',e15.7/10x,
     *   'new estimate for cme             =',e15.7/10x,
     *   'new estimate for spectral radius =',e15.7/)
c
  110 format (/10x,'estimate of maximum eigenvalue of jacobi   '/10x, 
     *   'matrix (cme) too large'/10x,
     *   'adaptive procedure turned off at iteration ',i5/10x,
     *   'final estimate of maximum eigenvalue =',e15.7/)
c
      end 
      logical function chgsme (oldnrm,icnt)
c
c ... chgsme tests for jacobi si whether sme should be changed
c ... when caseii = .false..  if the test is positive the new value
c ... of sme is computed.
c
c ... parameter list --
c
c          oldnrm square of the norm of the pseudo-residual 
c                    at the last iteration
c          icnt   number of iterations since last change of 
c                    parameters
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      chgsme = .false.
      rn = sqrt(delnnm/oldnrm)
      if (.not.(qa.gt.1.0.and.rn.gt.1.0)) return
      if (in.le.is+2) return
c
      icnt = icnt+1 
      if (icnt.lt.3) return
c
c ... change sme in j-si adaptive procedure
c
      chgsme = .true.
      sm1 = 0.0
      sm2 = 0.0
      if (sme.ge.cme) go to 10
c
c ... compute sm1
c
      ip = in-is
      q = qa*(1.0+rrr**ip)/(2.0*sqrt(rrr**ip))
      z = (q+sqrt(q**2-1.0))**(1.0/float(ip))
      wp = (z**2+1.0)/(2.0*z) 
      sm1 = 0.5*(cme+sme-wp*(cme-sme))
c
c ... compute sm2
c
      q = rn*(1.0+rrr**ip)/((1.0+rrr**(ip-1))*sqrt(rrr))
      wp = (q**2+1.0)/(2.0*q) 
      sm2 = 0.5*(cme+sme-wp*(cme-sme))
c
   10 sme = amin1(1.25*sm1,1.25*sm2,sme,-1.0)
      sige = (cme-sme)/(2.0-cme-sme)
      gamma = 2.0/(2.0-cme-sme)
      rrr = (1.0-sqrt(1.0-sige**2))/(1.0+sqrt(1.0-sige**2)) 
      is = in
      delsnm = delnnm
      rho = 1.0
c
      if (level.ge.2) write (nout,20) in,sm1,sm2,sme
c
   20 format (/5x,'estimate of smallest eigenvalue of jacobi'/10x,
     *   'matrix (sme) changed at iteration ',i5/10x,
     *   'first estimate of sme            =',e15.7/10x,
     *   'second estimate of sme           =',e15.7/10x,
     *   'new estimate of sme              =',e15.7/)
c
      return
      end 
      function determ (ldt,n,tri,wk1,wk2,xlmda)
c
c     determ computes the determinant of a symmetric
c     tridiagonal matrix given by tri. det(tri - xlmda*i) = 0
c
c ... parameter list --
c
c          ldt    leading dimension of array tri
c          n      order of tridiagonal system
c          tri    symmetric tridiagonal matrix of order n
c          wk1,   workspace vectors of length n
c           wk2
c          xlmda  argument for characteristic equation
c
c ... specifications for arguments
c
      dimension tri(ldt,2),wk1(1),wk2(1)
c
      do 10 i = 1,n 
         wk1(i) = tri(i,1)-xlmda
   10 continue
      wk2(n) = wk1(n)
      wk2(n-1) = wk1(n-1)*wk2(n)+tri(n,2)
      if (n.eq.2) go to 30
c
c ... beginning of loop
c
      do 20 l = n-2,1,-1
         wk2(l) = wk1(l)*wk2(l+1)+tri(l+1,2)*wk2(l+2)
   20 continue
c
c     wk2(1) = solrn (n,wk1(-1),-1,tri(0,2),-1,wk2,-1)
c
c ... determinant computed
c
   30 determ = wk2(1)
c
      return
      end 
      subroutine dfault (iparm,rparm)
c
c ... dfault sets the default values of iparm and rparm.
c
c ... parameter list --
c
c          iparm
c           and
c          rparm  arrays specifying options and tolerances
c
c ... specifications for arguments
c
      integer iparm(12)
      dimension rparm(12)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
c     srelpr  - computer precision (approx.)
c     if installer of package does not know srelpr value,
c     an approximate value can be determined from a simple
c     fortran program such as 
c
c     srelpr = 1.0
c   2 srelpr = 0.5*srelpr
c     temp = srelpr + 1.0
c     if (temp .gt. 1.0)  go to 2
c     srelpr = 2.0*srelpr
c     write (6,3) srelpr
c   3 format (5x,e15.8)
c     stop
c     end 
c
c     some values are-
c
c     srelpr = 7.1e-15   for cray x-mp  (approx.) 2**-47
c          = 1.49e-8   for dec 10  (approx.) 2**-26
c          = 1.192e-7  for vax 11/780 (approx) 2**-23
c          = 4.768e-7  for ibm 370/158
c
c             *** should be changed for other machines ***
c
c     to facilitate convergence, rparm(1) should be set to
c          500.*srelpr or larger
c
      srelpr = 2.0**(-47)
c
      iparm(1) = 100
      iparm(2) = 0
      iparm(3) = 0
      iparm(4) = 6
      iparm(5) = 0
      iparm(6) = 1
      iparm(7) = 1
      iparm(8) = 0
      iparm(9) = -2 
      iparm(10) = 0 
      iparm(11) = 0 
      iparm(12) = 0 
c
      rparm(1) = 0.5e-5
      rparm(2) = 0.0
      rparm(3) = 0.0
      rparm(4) = .75
      rparm(5) = 1.0
      rparm(6) = 0.0
      rparm(7) = .25
      rparm(8) = 100.0*srelpr 
      rparm(9) = 0.0
      rparm(10) = 0.0
      rparm(11) = 0.0
      rparm(12) = 0.0
c
      return
      end 
      subroutine echall (n,ndim,maxnz,jcoef,coef,rhs,iparm,rparm,icall)
c
c ... echall initializes the itpackv common blocks from the
c ... information contained in iparm and rparm. echall also prints the
c ... values of all the parameters in iparm and rparm.
c
c ... parameter list --
c
c          iparm
c           and
c          rparm  arrays of parameters specifying options and
c                    tolerances
c          icall  indicator of which parameters are being printed
c                    icall = 1,  initial parameters
c                    icall = 2,  final parameters 
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iparm(12)
      dimension coef(ndim,1),rhs(n),rparm(12)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      if (icall.ne.1) go to 120
c
c ... initialize itpackv common
c
      zeta = rparm(1)
      cme = rparm(2)
      sme = rparm(3)
      ff = rparm(4) 
      omega = rparm(5)
      specr = rparm(6)
      betab = rparm(7)
      itmax = iparm(1)
      level = iparm(2)
      isym = iparm(5)
c
      adapt = .false.
      partad = .false.
      betadt = .false.
      if (iparm(6).eq.1.or.iparm(6).eq.3) adapt = .true.
      if (iparm(6).eq.1) betadt = .true.
      if (iparm(6).eq.2) partad = .true.
c
      caseii = .false.
      if (iparm(7).eq.2) caseii = .true.
      if (caseii) sme = -cme
      if (.not.caseii.and.sme.eq.0.0) sme = -1.0
      spr = sme
c
c ... set rest of common variables to zero
c
      in = 0
      is = 0
      halt = .false.
      bdelnm = 0.0
      delnnm = 0.0
      delsnm = 0.0
      gamma = 0.0
      qa = 0.0
      qt = 0.0
      rho = 0.0
      rrr = 0.0
      sige = 0.0
      stptst = 0.0
      udnm = 0.0
c
      if (level.le.4) go to 100
c
c     this section of echall causes printing of the linear system and 
c     the iterative parameters
c
      write (nout,10)
   10 format (///5x,'the linear system is as follows')
      write (nout,20)
   20 format (/2x,'jcoef array')
      do 30 i = 1,n 
         write (nout,40) (jcoef(i,j),j=1,maxnz)
   30 continue
   40 format (1x,8(1x,i8))
      write (nout,50)
   50 format (/2x,'coef array')
      do 60 i = 1,n 
         write (nout,70) (coef(i,j),j=1,maxnz)
   60 continue
   70 format (1x,5(2x,g14.6)) 
      write (nout,80)
   80 format (/2x,'rhs array')
      write (nout,90) (rhs(i),i=1,n)
   90 format (1x,5g16.6)
  100 if (level.le.2) return
      write (nout,110)
  110 format (///5x,'initial iterative parameters'/)
      go to 140
  120 write (nout,130)
  130 format (///5x,'final iterative parameters'/)
  140 write (nout,150) iparm(1),level,iparm(3),nout,isym,iparm(6)
  150 format (10x,'iparm(1)  =',i15,4x,'(itmax)'/ 
     *        10x,'iparm(2)  =',i15,4x,'(level)'/ 
     *        10x,'iparm(3)  =',i15,4x,'(ireset)'/
     *        10x,'iparm(4)  =',i15,4x,'(nout)'/
     *        10x,'iparm(5)  =',i15,4x,'(isym)'/
     *        10x,'iparm(6)  =',i15,4x,'(iadapt)')
      write (nout,160) iparm(7),iparm(8),iparm(9),iparm(10),iparm(11),
     *   iparm(12)
  160 format (10x,'iparm(7)  =',i15,4x,'(icase)'/ 
     *        10x,'iparm(8)  =',i15,4x,'(nwksp)'/ 
     *        10x,'iparm(9)  =',i15,4x,'(nb)'/
     *        10x,'iparm(10) =',i15,4x,'(iremove)'/
     *        10x,'iparm(11) =',i15,4x,'(itime)'/ 
     *        10x,'iparm(12) =',i15,4x,'(idgts)') 
      write (nout,170) zeta,cme,sme,ff,omega,specr
  170 format (10x,'rparm(1)  =',e15.8,4x,'(zeta)'/
     *        10x,'rparm(2)  =',e15.8,4x,'(cme)'/ 
     *        10x,'rparm(3)  =',e15.8,4x,'(sme)'/ 
     *        10x,'rparm(4)  =',e15.8,4x,'(ff)'/
     *        10x,'rparm(5)  =',e15.8,4x,'(omega)'/
     *        10x,'rparm(6)  =',e15.8,4x,'(specr)')
      write (nout,180) betab,rparm(8),rparm(9),rparm(10),rparm(11),
     *   rparm(12)
  180 format (10x,'rparm(7)  =',e15.8,4x,'(betab)'/
     *        10x,'rparm(8)  =',e15.8,4x,'(tol)'/ 
     *        10x,'rparm(9)  =',e15.8,4x,'(time1)'/
     *        10x,'rparm(10) =',e15.8,4x,'(time2)'/
     *        10x,'rparm(11) =',e15.8,4x,'(digit1)'/
     *        10x,'rparm(12) =',e15.8,4x,'(digit2)')
c
      return
      end 
      function eigvns (ldt,n,tri,d,e2,ier)
c
c ... eigvns computes the largest eigenvalue of a symmetric 
c     tridiagonal matrix for conjugate gradient acceleration.
c
c ... parameter list --
c
c          ldt    leading dimension of tri
c          n      order of tridiagonal system
c          tri    symmetric tridiagonal matrix of order n
c          d      array for eqrt1s (negative diagonal elements)
c          e2     array for eqrt1s (super diagonal elements)
c          ier    error flag -- on return, ier=0 indicates that
c                    the largest eigenvalue of tri was found.
c
c ... local itpackv references --
c
c          eqrt1s
c
c ... specifications for arguments
c
      dimension tri(ldt,2),d(n),e2(n)
c
      eigvns = 0.0
c
      d(1) = -tri(1,1)
      do 10 i = 2,n 
         d(i) = -tri(i,1)
         e2(i) = abs(tri(i,2))
   10 continue
c
      call eqrt1s (d,e2,n,1,0,ier)
      eigvns = -d(1)
c
      return
      end 
      function eigvss (n,tri,start,zeta,itmax,ier)
c
c ... eigvss computes the largest eigenvalue of a symmetric 
c     tridiagonal matrix for conjugate gradient acceleration.
c     modified imsl routine zbrent used.
c
c ... parameter list --
c
c          n      order of tridiagonal system
c          tri    symmetric tridiagonal matrix of order n
c          start  initial lower bound of interval containing root
c          zeta   stopping criteria
c          ier    error flag -- on return, ier = 0 indicates that
c                    the largest eigenvalue of tri was found.
c
c ... local itpackv references --
c
c          zbrent
c
c ... specifications for arguments
c
      dimension tri(1)
c
      eigvss = 0.0
      itmp = int(-alog10(abs(zeta)))
      nsig = max0(itmp,4)
      maxfn = max0(itmax,50)
      eps = 0.0
      a = start
      b = 1.0
      call zbrent (n,tri,eps,nsig,a,b,maxfn,ier)
      eigvss = b
c
      return
      end 
      subroutine eqrt1s (d,e2,n,m,isw,ier)
c
c   modified imsl routine name   - eqrt1s
c
c-----------------------------------------------------------------------
c
c   computer            - cdc/single
c
c   latest revision     - june 1, 1980
c
c   purpose             - smallest or largest m eigenvalues of a
c                           symmetric tridiagonal matrix
c
c   usage               - call eqrt1s (d,e2,n,m,isw,ier)
c
c   arguments    d      - input vector of length n containing
c                           the diagonal elements of the matrix.  the 
c                           computed eigenvalues replace the first m
c                           components of the vector d in non-
c                           decreasing sequence, while the remaining
c                           components are lost.
c                e2     - input vector of length n containing
c                           the squares of the off-diagonal elements
c                           of the matrix.  input e2 is destroyed.
c                n      - input scalar containing the order of the
c                           matrix.
c                m      - input scalar containing the number of
c                           smallest eigenvalues desired (m is
c                           less than or equal to n).
c                isw    - input scalar meaning as follows - 
c                           isw=1 means that the matrix is known to be
c                             positive definite.
c                           isw=0 means that the matrix is not known
c                             to be positive definite.
c                ier    - error parameter. (output)
c                           warning error
c                             ier = 601 indicates that successive
c                               iterates to the k-th eigenvalue were not
c                               monotone increasing. the value k is
c                               stored in e2(1).
c                           terminal error
c                             ier = 602 indicates that isw=1 but matrix
c                               is not positive definite
c
c   precision/hardware  - single and double/h32
c                       - single/h36,h48,h60
c
c   notation            - information on special notation and
c                           conventions is available in the manual
c                           introduction or through imsl routine uhelp
c
c   remarks      as written, the routine computes the m smallest
c                eigenvalues. to compute the m largest eigenvalues,
c                reverse the sign of each element of d before and
c                after calling the routine. in this case, isw must
c                equal zero.
c
c   copyright           - 1980 by imsl, inc. all rights reserved.
c
c   warranty            - imsl warrants only that imsl testing has been
c                           applied to this code. no other warranty,
c                           expressed or implied, is applicable.
c
c-----------------------------------------------------------------------
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
c ... specifications for arguments
c
      dimension d(n),e2(n)
c
c                                  srelpr = machine precision
c                                  first executable statement
c
      ier = 0
      dlam = 0.0
      err = 0.0
      s = 0.0
c
c                                  look for small sub-diagonal entries
c                                  define initial shift from lower
c                                  gerschgorin bound.
c
      tot = d(1)
      q = 0.0
      j = 0
      do 30 i = 1,n 
         p = q
         if (i.eq.1) go to 10 
         if (p.gt.srelpr*(abs(d(i))+abs(d(i-1)))) go to 20
   10    e2(i) = 0.0
c
c                                  count if e2(i) has underflowed
c
   20    if (e2(i).eq.0.0) j = j+1
         q = 0.0
         if (i.ne.n) q = sqrt(abs(e2(i+1)))
         tot = amin1(d(i)-p-q,tot)
   30 continue
      if (isw.eq.1.and.tot.lt.0.0) go to 50
      do 40 i = 1,n 
         d(i) = d(i)-tot
   40 continue
      go to 60
   50 tot = 0.0
   60 do 190 k = 1,m
c
c                                  next qr transformation
c
   70    tot = tot+s
         delta = d(n)-s
         i = n
         f = abs(srelpr*tot)
         if (dlam.lt.f) dlam = f
         if (delta.gt.dlam) go to 90
         if (delta.ge.(-dlam)) go to 160
         ier = 602
         if (level.ge.1) write (nout,80)
   80    format (/1x,'*** w a r n i n g ************'/1x,
     *      '    in itpackv routine eqrt1s'/1x,
     *      '    parameter isw = 1 but matrix'/1x,
     *      '    not positive definite')
         go to 200
c
c                                  replace small sub-diagonal squares 
c                                  by zero to reduce the incidence of 
c                                  underflows
c
   90    if (k.eq.n) go to 110
         k1 = k+1
         do 100 j = k1,n
            if (e2(j).le.(srelpr*(d(j)+d(j-1)))**2) e2(j) = 0.0
  100    continue
  110    f = e2(n)/delta
         qp = delta+f
         p = 1.0
         if (k.eq.n) go to 140
         k1 = n-k
         do 130 ii = 1,k1
            i = n-ii
            q = d(i)-s-f
            r = q/qp
            p = p*r+1.0
            ep = f*r
            d(i+1) = qp+ep
            delta = q-ep
            if (delta.gt.dlam) go to 120
            if (delta.ge.(-dlam)) go to 160
            ier = 602
            if (level.ge.1) write (nout,80)
            go to 200
  120       f = e2(i)/q
            qp = delta+f
            e2(i+1) = qp*ep
  130    continue
  140    d(k) = qp
         s = qp/p
         if (tot+s.gt.tot) go to 70
         ier = 601
         e2(1) = k
         if (level.ge.1) write (nout,150) k
  150    format (/1x,'*** w a r n i n g ************'//1x,
     *      '    in itpackv routine eqrt1s'/1x,
     *      '    successive iterates to the',i10/1x,
     *      '    eigenvalue were not monotone increasing')
c
c                                  set error -- irregular end
c                                  deflate minimum diagonal element
c
         s = 0.0
         i = ismin(n-k+1,d(k),1)
         delta = amin1(qp,d(i))
c
c                                  convergence
c
  160    if (i.lt.n) e2(i+1) = e2(i)*f/qp
         if (i.eq.k) go to 180
         do 170 j = i-1,k,-1
            d(j+1) = d(j)-s
            e2(j+1) = e2(j)
  170    continue
  180    d(k) = tot 
         err = err+abs(delta) 
         e2(k) = err
  190 continue
      if (ier.eq.0) go to 210 
  200 continue
  210 return
      end 
      integer function ipstr (omega)
c
c     ipstr finds the smallest integer, ipstr, greater than 5 such
c     that    ipstr * (omega-1)**(ipstr-1) .le. 0.50. ipstr will be
c          set in loop.
c
c ... parameter list --
c
c          omega  relaxation factor for sor method
c
      wm1 = omega-1.0
      factor = wm1**5
c
      do 10 ip = 6,940
         if (float(ip)*factor.le.0.5) go to 20
         factor = factor*wm1
   10 continue
      ip = 940
   20 ipstr = ip
      return
c
      end 
      subroutine iterm (n,coef,u,wk,imthd)
c
c     iterm produces the iteration summary line at the end
c     of each iteration. if level .ge. 4, the latest approximation
c     to the solution will be printed.
c
c ... parameter list --
c
c          n      order of system or, for reduced system
c                    routines, order of black subsystem
c          coef   iteration matrix
c          u      solution estimate
c          wk     work array of length n
c          imthd  indicator of method
c                    imthd = 1,  jcg
c                    imthd = 2,  jsi
c                    imthd = 3,  sor
c                    imthd = 4,  ssorcg 
c                    imthd = 5,  ssorsi 
c                    imthd = 6,  rscg
c                    imthd = 7,  rssi
c
c ... specifications for arguments
c
      dimension coef(n,1),u(n),wk(n)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c ... print various parameters after each iteration
c
      if (level.lt.2) return
      go to (10,100,140,170,50,10,100), imthd
   10 if (in.gt.0) go to 30
c
c ... print header for jcg and rscg
c
      write (nout,20)
   20 format (////5x,'intermediate output after each iteration'//
     *   ' number of',3x,'convergence',5x,'cme ',10x,'rho',8x,'gamma'/
     *   ' iterations',5x,'test '//)
c
c ... print summary line
c
   30 write (nout,40) in,stptst,cme,rho,gamma
   40 format (3x,i5,3x,5e13.5)
      if (level.ge.4) go to 200
c
      return
c
   50 if (in.gt.0) go to 70
c
c ... print header for ssor-si
c
      write (nout,60)
   60 format (////5x,'intermediate output after each iteration'//
     *   ' number of',3x,'convergence',3x,'parameter change test',8x, 
     *   'rho',8x,'gamma'/' iterations',5x,'test ',6x,'lhs(qa)',4x,
     *   'rhs(qt**ff)'//)
c
c ... print summary line
c
   70 ip = in-is
      if (imthd.eq.7) ip = 2*ip
      if (ip.lt.3) go to 80
      qtff = qt**ff 
      write (nout,40) in,stptst,qa,qtff,rho,gamma 
      if (level.ge.4) go to 200
      return
c
   80 write (nout,90) in,stptst,rho,gamma
   90 format (3x,i5,3x,e13.5,26x,2e13.5)
      if (level.ge.4) go to 200
      return
c
  100 if (in.gt.0) go to 120
c
c ... print header for j-si and rs-si
c
      write (nout,110)
  110 format (////5x,'intermediate output after each iteration'//
     *   ' number of',3x,'convergence',3x,'parameter change test',8x, 
     *   'rho'/' iterations',5x,'test ',6x,'lhs(qa)',4x,'rhs(qt**ff)'//)
c
c ... print summary line
c
  120 ip = in-is
      if (imthd.eq.7) ip = 2*ip
      if (ip.lt.3) go to 130
      qtff = qt**ff 
      write (nout,40) in,stptst,qa,qtff,rho
      if (level.ge.4) go to 200
      return
c
  130 write (nout,90) in,stptst,rho
      if (level.ge.4) go to 200
      return
c
c ... print various parameters after each iteration for sor.
c
  140 if (in.gt.0) go to 160
c
c ... print header for sor
c
      write (nout,150)
  150 format (////5x,'intermediate output after each iteration'//
     *   ' number of',3x,'convergence',5x,'cme ',8x,'omega',7x,
     *   'spectral'/' iterations',5x,'test',34x,'radius'//) 
c
c ... print summary line for sor
c
  160 continue
      write (nout,40) in,stptst,cme,omega,specr
      if (level.ge.4) go to 200
c
      return
c
c ... print various parameters after each iteration for ssor-cg.
c
  170 if (in.gt.0) go to 190
c
c ... print header for ssor-cg
c
      write (nout,180)
  180 format (////5x,'intermediate output after each iteration'//
     *   ' number of',3x,'convergence',2x,' spectral',5x,'s-prime',9x,
     *   'rho',8x,'gamma'/' iterations',5x,'test ',7x,'radius'//)
c
c ... print summary line for ssor-cg
c
  190 continue
      write (nout,40) in,stptst,specr,spr,rho,gamma
      if (level.ge.4) go to 200
      return
c
  200 if (imthd.gt.5) go to 220
      write (nout,210) in
  210 format (/1x,2x,'estimate of solution at iteration ',i5)
      go to 240
  220 write (nout,230) in
  230 format (/1x,2x,'estimate of solution at black points ',
     *   'at iteration ',i5)
  240 do 250 i = 1,n
         wk(i) = u(i)*coef(i,1)
  250 continue
      write (nout,260) (wk(i),i=1,n)
  260 format (1x,5g16.7)
      write (nout,270)
  270 format (//)
c
      return
      end 
      subroutine move (n,ndim,maxnz,nlow,nup,jcoef,coef,
     *                 level,nout,moved,iwksp)
c
c ... move sets up the i-u-l data structure for the sor and ssor
c ... methods when natural ordering is used.
c
c ... parameter list --
c
c         n        order of matrix
c         ndim     row dimension of coef and jcoef arrays in calling
c                   routine
c         maxnz    maximum number of nonzeros per row
c         nlow     maximum number of nonzeros per row in lower
c                   triangle l  (output)
c         nup      maximum number of nonzeros per row in upper
c                   triangle u  (output)
c         jcoef    integer data structure for coefficient columns
c         coef     data structure for coefficients
c         level    level of output
c         nout     output device number 
c         moved    logical constant indicating whether or not data
c                   structure has been reshuffled 
c         iwksp    integer workspace
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),iwksp(1)
      dimension coef(ndim,1)
      logical moved 
c
      moved = .false.
c
c ... determine maximum number of nonzeros per row in l and u.
c
      nup = 0
      nlow = 0
      if (maxnz.lt.2) return
      do 10 i = 1,n 
         iwksp(i) = 0
   10 continue
      do 30 j = 1,maxnz
         do 20 i = 1,n
            if (jcoef(i,j).lt.i) iwksp(i) = iwksp(i)+1
   20    continue
   30 continue
      do 40 i = 1,n 
         nlow = max0(nlow,iwksp(i))
   40 continue
      do 50 i = 1,n 
         iwksp(i) = 0
   50 continue
      do 70 j = 1,maxnz
         do 60 i = 1,n
            if (jcoef(i,j).gt.i) iwksp(i) = iwksp(i)+1
   60    continue
   70 continue
      do 80 i = 1,n 
         nup = max0(nup,iwksp(i))
   80 continue
c
c ... shuffle matrix so that u is first.
c
      nlpu = nup+nlow+1
      if (nlpu.le.maxnz) go to 100
c
c ... note -- maxnz is too small.
c
      if (level.ge.5) write (nout,90) nlpu
   90 format (//1x,'*** p l e a s e   n o t e ***'//1x,
     *   'maxnz should be increased to ',i5/1x,
     *   'for increased vectorization'/)
      return
c
c ... permute elements of each row.
c
  100 moved = .true.
      if (nup.le.0) return
      nu2 = nup+1
      nl1 = nu2+1
      do 130 j1 = 2,nu2
         do 120 j2 = nl1,maxnz
            do 110 i = 1,n
               if (jcoef(i,j1).gt.i.or.jcoef(i,j2).lt.i.or.jcoef(i,j1)
     *            .eq.jcoef(i,j2)) go to 110
               itemp = jcoef(i,j1)
               jcoef(i,j1) = jcoef(i,j2)
               jcoef(i,j2) = itemp
               temp = coef(i,j1)
               coef(i,j1) = coef(i,j2)
               coef(i,j2) = temp
  110       continue
  120    continue
  130 continue
      return
      end 
      subroutine omeg (dnrm,iflag)
c
c     omeg computes new values for  cme, omega, and specr for
c     fully adaptive ssor methods.
c
c ... parameter list --
c
c          dnrm   numerator of rayleigh quotient
c          iflag  indicator of appropriate entry point
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      zm1 = 0.0
      zm2 = 0.0
      if (iflag.eq.1) go to 10
c
c ... iflag .ne. 1, compute new estimate for cme
c
      zm1 = ((1.0-spr)*(1.0+betab*omega**2)-omega*(2.0-omega))/(omega*
     *   (omega-1.0-spr))
c
      if (.not.caseii) zm2 = dnrm/bdelnm
      if (caseii) zm2 = sqrt(abs(dnrm/bdelnm))
      cme = amax1(cme,zm1,zm2)
c
c ... iflag = 1, or continuation of iflag .ne. 1
c
c        compute new values of omega and specr based on cme and betab 
c
   10 is = in+1
      delsnm = delnnm
      if (cme.ge.(4.0*betab)) go to 30
c
c ... cme .lt. 4.*betab
c
      temp = sqrt(abs(1.0-2.0*cme+4.0*betab))
      omega = amax1((2.0/(1.0+temp)),1.0)
      temp = (1.0-cme)/temp
      specr = (1.0-temp)/(1.0+temp)
      if (abs(omega-1.0).lt.srelpr) specr = 0.0
      if (level.ge.2) write (nout,20) in,betab,zm1,zm2,cme,omega,specr
   20 format (/5x,'parameters were changed at iteration no.',i5/10x,
     *   'new estimate of betab            =',e15.7/10x,
     *   'solution to chebyshev eqn.       =',e15.7/10x,
     *   'solution to rayleigh quotient    =',e15.7/10x,
     *   'new estimate for cme             =',e15.7/10x,
     *   'new estimate for omega           =',e15.7/10x,
     *   'new estimate for spectral radius =',e15.7/)
c
      return
c
c ... cme .ge. 4.*betab
c
c ... omega-star will be chosen
c
   30 cme = 2.0*sqrt(abs(betab))
      omega = 2.0/(1.0+sqrt(abs(1.0-4.0*betab)))
      specr = omega-1.0
      adapt = .false.
      partad = .false.
      if (level.ge.2) write (nout,20) in,betab,zm1,zm2,cme,omega,specr
c
      return
      end 
      logical function omgchg (ndummy)
c
c ... omgchg tests to see whether omega should be changed
c ... for ssor cg method.
c
c ... parameter list --
c
c          ndummy arbitrary integer parameter
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
c ... statement function phi(x)
c
      phi(x) = (1.0-sqrt(abs(1.0-x)))/(1.0+sqrt(abs(1.0-x)))
c
      omgchg = .false.
      if (in-is.lt.3) return
      if (specr.eq.0.0) go to 10
      if (specr.gt.spr) return
      del1 = -alog(abs(phi(specr)/phi(specr/spr)))
      del2 = -alog(abs(phi(spr)))
      if ((del1/del2).ge.ff) return
c
   10 omgchg = .true.
c
      return
      end 
      logical function omgstr (ndummy)
c
c     omgstr tests for fully adaptive ssor methods whether
c     omega-star should be used for omega and the adaptive process
c     turned off.
c
c ... parameter list --
c
c          ndummy arbitrary integer parameter
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
c ... statement function phi(x)
c
      phi(x) = (1.0-sqrt(abs(1.0-x)))/(1.0+sqrt(abs(1.0-x)))
c
      omgstr = .false.
      if (betab.ge.0.25.or..not.adapt) return
      omstar = 2.0/(1.0+sqrt(abs(1.0-4.0*betab))) 
c
c ... test to choose omega-star
c
      if ((omstar.le.1.0).or.(specr.le.0.0)) go to 10
      temp = alog(abs(phi(omstar-1.0))) 
      temp1 = alog(abs(phi(specr)))
      if ((temp/temp1).lt.ff) return
c
c ... omega-star was chosen
c
   10 omega = omstar
      specr = omega-1.0
      omgstr = .true.
      adapt = .false.
      partad = .false.
      cme = 2.0*sqrt(abs(betab))
      rrr = phi(1.0-specr)**2 
      gamma = 2.0/(2.0-specr) 
      sige = specr/(2.0-specr)
      rho = 1.0
      is = in+1
      delsnm = delnnm
      if (level.ge.2) write (nout,20) in,cme,omega,specr
   20 format (/5x,'omega-star, an alternate estimate of',
     *   ' omega, was chosen at iteration',i5/10x,
     *   'new estimate for cme             =',e15.7/10x,
     *   'new estimate for omega           =',e15.7/10x,
     *   'new estimate for spectral radius =',e15.7/)
c
      return
      end 
      subroutine parcon (dtnrm,c1,c2,c3,c4,gamold,rhotmp,ibmth)
c
c ... parcon computes acceleration parameters for conjugate gradient
c     acceleration methods.
c
c ... parameter list --
c
c          dtnrm  inner product of residuals
c          c1     output -- rho*gamma
c          c2     output -- rho
c          c3     output -- 1-rho
c          c4     output -- rho*(1-gamma)
c          gamold output -- value of gamma at preceding iteration
c          rhotmp last estimate for value of rho
c          ibmth  indicator of basic method being accelerated by cg
c                      ibmth = 1,   jacobi
c                            = 2,   reduced system
c                            = 3,   ssor
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      ip = in-is
c
c ... set rhoold and gamold
c
      rhoold = rho
      gamold = gamma
c
c ... compute gamma (in+1)
c
c ... for jacobi or reduced system cg
c
      if (ibmth.le.2) gamma = 1.0/(1.0-dtnrm/delnnm)
c
c ... for ssor cg
c
      if (ibmth.eq.3) gamma = delnnm/dtnrm
c
c ... compute rho (in+1)
c
      rho = 1.0
      if (ip.eq.0) go to 20
      if (isym.eq.0) go to 10 
      rho = 1.0/(1.0-gamma*rhotmp/delsnm)
      go to 20
   10 rho = 1.0/(1.0-gamma*delnnm/(gamold*delsnm*rhoold))
c
c ... compute constants c1, c2, c3, and c4
c
   20 delsnm = delnnm
      rhotmp = rhoold
      c1 = rho*gamma
      c2 = rho
      c3 = 1.0-rho
      c4 = rho*(1.0-gamma)
c
      return
      end 
      subroutine parsi (c1,c2,c3,ibmth) 
c
c     parsi computes acceleration parameters for semi-iterative
c     accelerated methods.
c
c ... parameter list --
c
c          c1,c2
c           and
c           c3    output acceleration parameters
c          ibmth  indicator of basic method being accelerated by si
c                      ibmth = 1, jacobi
c                            = 2, reduced system
c                            = 3, ssor
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      ip = in-is
      if (ip.eq.0) go to 30
      if (ip.eq.1) go to 10
      rho = 1.0/(1.0-sige*sige*rho*0.25)
      go to 20
   10 rho = 1.0/(1.0-sige*sige*0.5)
c
   20 c1 = rho*gamma
      c2 = rho
      c3 = 1.0-rho
c
      return
c
c ... nonadaptive initialization for semi-iterative methods 
c
   30 continue
      go to (40,50,60), ibmth 
c
c ... jsi 
c
   40 if (caseii) sme = -cme
      gamma = 2.0/(2.0-cme-sme)
      sige = (cme-sme)/(2.0-cme-sme)
      go to 70
c
c ... reduced system si
c
   50 gamma = 2.0/(2.0-cme*cme)
      sige = cme*cme/(2.0-cme*cme)
      rrr = (1.0-sqrt(abs(1.0-cme*cme)))/(1.0+sqrt(abs(1.0-cme*cme))) 
      go to 70
c
c ... ssorsi
c
   60 gamma = 2.0/(2.0-specr) 
      sige = specr/(2.0-specr)
      rrr = (1.0-sqrt(abs(1.0-sige*sige)))/(1.0+sqrt(abs(1.0-sige*sige))
     *   )
c
   70 rho = 1.0
      c1 = gamma
      c2 = 1.0
      c3 = 0.0
c
      return
      end 
      function pbeta (n,nb,ndim,maxnz,jcoef,coef,v,nup,nlow,moved,wl,wu)
c
c     ... pbeta computes the numerator for the computation of betab
c     ...  in the ssor methods.  pbeta = (v,lu*v).
c
c ... parameter list --
c
c          n      dimension of matrix
c          nb     order of black subsystem
c          ndim   row dimension of jcoef and coef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array for coefficient columns
c          coef   array for coefficients
c          v      input array of length n
c          nup    maximum number of nonzeros per row for upper
c                  triangle u  (input for natural ordering only)
c          nlow   maximum number of nonzeros per row for lower
c                  triangle l  (input for natural ordering only)
c          moved  logical constant indicating whether the data
c                  structure has been reshuffled. 
c          wl,wu  workspace vectors of length n each
c
c ... specifications for arguments
c
      integer jcoef(ndim,2)
      dimension coef(ndim,2),v(1),wl(1),wu(1)
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      if (nb.ge.0) go to 120
c
c ... natural ordering.
c
      if (.not.moved) go to 40
      do 10 i = 1,n 
         wu(i) = 0.0
   10 continue
      call ypasx2 (ndim,n,nup,coef(1,2),jcoef(1,2),wu,v)
      if (isym.ne.0) go to 20 
c
c ... symmetric matrix so  (v,lu*v) = (u*v,u*v)
c
      pbeta = sdot(n,wu,1,wu,1)
      return
c
c ... nonsymmetric section
c
   20 nup2 = nup+2
      do 30 i = 1,n 
         wl(i) = 0.0
   30 continue
      call ypasx2 (ndim,n,nlow,coef(1,nup2),jcoef(1,nup2),wl,wu)
      pbeta = sdot(n,v,1,wl,1)
      return
c
c ... natural ordering -- unshuffled data structure
c
   40 continue
      do 50 i = 1,n 
         wu(i) = 0.0
   50 continue
      do 70 j = 2,maxnz
         do 60 i = 1,n
            jcol = jcoef(i,j) 
            if (jcol.gt.i) wu(i) = wu(i)+coef(i,j)*v(jcol)
   60    continue
   70 continue
      if (isym.ne.0) go to 80 
      pbeta = sdot(n,wu,1,wu,1)
      return
c
c ... nonsymmetric case -- unshuffled data structure
c
   80 do 90 i = 1,n 
         wl(i) = 0.0
   90 continue
      do 110 j = 2,maxnz
         do 100 i = 1,n
            jcol = jcoef(i,j) 
            if (jcol.lt.i) wl(i) = wl(i)+coef(i,j)*wu(jcol) 
  100    continue
  110 continue
      pbeta = sdot(n,v,1,wl,1)
      return
c
c ... red/black ordering.
c
  120 nr = n-nb
      nrp1 = nr+1
      maxm1 = maxnz-1
      do 130 i = 1,n
         wl(i) = 0.0
  130 continue
      call ypasx2 (ndim,nr,maxm1,coef(1,2),jcoef(1,2),wl,v) 
      if (isym.ne.0) go to 140
c
c ... symmetric case so  (v,lu*v) = (fr*vb,fr*vb) .
c
      pbeta = sdot(nr,wl,1,wl,1)
      return
c
c ... nonsymmetric section
c
  140 call ypasx2 (ndim,nb,maxm1,coef(nrp1,2),jcoef(nrp1,2),wl(nrp1),wl)
      pbeta = sdot(nb,v(nrp1),1,wl(nrp1),1)
      return
      end 
      subroutine pbsor (n,nb,ndim,maxnz,jcoef,coef,u,rhs,
     *                  nup,nlow,moved,ipop)
c
c         pbsor computes a backward sor sweep on u.
c
c ... parameter list --
c
c          n      order of system
c          nb     order of black subsystem
c          ndim   row dimension of coef and jcoef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array of coefficient columns
c          coef   array of coefficients 
c          u      latest estimate of solution
c          rhs    right hand side of matrix problem
c          nup    maximum number of nonzeros per row in lower
c                  triangle l (input for natural order only)
c          nlow   maximum number of nonzeros per row in upper triangle
c                  u (input for natural order only)
c          moved  logical variable indicating whether or not matrix
c                  has been reshuffled
c          ipop   integer vector giving the wave populations
c
c ... specifications for arguments
c
      integer jcoef(ndim,2),ipop(1)
      dimension coef(ndim,2),u(1),rhs(1)
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      fac = 1.0-omega
      if (nb.ge.0) go to 60
c
c ... natural ordering
c
      if (.not.moved) go to 30
c
c ... compute   omega * (l*un + c) + (1-w) * un
c
      call ymasx2 (ndim,n,nlow,coef(1,nup+2),jcoef(1,nup+2),rhs,u)
      do 10 i = 1,n 
         u(i) = rhs(i)+fac*u(i)
   10 continue
c
c ... compute  u(n+1) = inv(i - omega*u) * rhs
c
      ist = n+1
      do 20 k = numwav,1,-1
         nlen = ipop(k)
         ied = ist-1
         ist = ist-nlen
         do 18 j = 2,nup+1
cdir$ ivdep
            do 15 i = ist,ied 
               u(i) = u(i) - coef(i,j)*u(jcoef(i,j))
   15       continue
   18    continue
   20 continue
      return
c
c ... natural ordering -- unshuffled matrix
c
   30 continue
      ist = n+1
      do 50 k = numwav,1,-1
         nlen = ipop(k)
         ied = ist-1
         ist = ist-nlen
         do 38 j = 2,maxnz
            do 35 i = ist,ied 
               rhs(i) = rhs(i) - coef(i,j)*u(jcoef(i,j))
   35       continue
   38    continue
         do 40 i = ist,ied
            u(i) = rhs(i)+fac*u(i)
   40    continue
   50 continue
      return
c
c ... red-black ordering
c
   60 nr = n-nb
      nrp1 = nr+1
      maxm1 = maxnz-1
      call ymasx2 (ndim,nb,maxm1,coef(nrp1,2),jcoef(nrp1,2),rhs(nrp1),u)
      do 70 i = nrp1,n
         u(i) = rhs(i)+fac*u(i)
   70 continue
      call ymasx2 (ndim,nr,maxm1,coef(1,2),jcoef(1,2),rhs,u)
      do 80 i = 1,nr
         u(i) = rhs(i)+fac*u(i)
   80 continue
c
      return
      end 
      subroutine permat (n,ndim,maxnz,jcoef,coef,p,work,iwork)
c
c ... permat takes the sparse matrix representation
c     of the matrix stored in the arrays jcoef and coef and 
c     permutes both rows and columns, overwriting the previous
c     structure.
c
c ... parameter list --
c
c          n         order of system
c          ndim      row dimension of arrays jcoef and coef in
c                       the calling routine
c          maxnz     maximum number of nonzero entries per row
c          jcoef     integer array for data
c          coef      array for data structure coefficients
c          p         permutation vector 
c          work      workspace of length n
c          iwork     integer workspace of length n
c
c ... it is assumed that the i-th entry of the permutation vector
c     p indicates the row the i-th row gets mapped into.  (i.e.
c     if ( p(i) = j ) row i gets mapped into row j)
c
c     *** note ***  this routine is to be called after routine scal.
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),p(1),iwork(1)
      dimension coef(ndim,1),work(1)
c
      if (n.le.0) return
      do 50 j = 1,maxnz
         call scopy (n,coef(1,j),1,work,1)
         do 10 i = 1,n
            iwork(i) = jcoef(i,j)
   10    continue
         do 20 i = 1,n
            coef(p(i),j) = work(i)
   20    continue
         do 30 i = 1,n
            jcoef(p(i),j) = iwork(i)
   30    continue
         do 40 i = 1,n
            jcoef(i,j) = p(jcoef(i,j))
   40    continue
   50 continue
      return
      end 
      subroutine perror (n,ndim,maxnz,jcoef,coef,rhs,u,work,
     *                   digit1,digit2,idgts)
c
c     perror computes the residual, r = rhs - a*u.  the user
c     also has the option of printing the residual and/or the
c     unknown vector depending on idgts.
c
c ... parameter list --
c
c          n      dimension of matrix
c          ndim   row dimension of jcoef and coef in calling routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array of sparse matrix representation
c          coef   array of sparse matrix representation
c          rhs    right hand side of matrix problem
c          u      latest estimate of solution
c          work   workspace vector of length 2*n
c          digit1 output - measure of accuracy of stopping test
c          digit2 output - measure of accuracy of solution
c          idgts   parameter controlling level of output
c                    if idgts < 1 or idgts > 4, then no output.
c                            = 1, then number of digits is printed, pro-
c                                 vided level .ge. 1
c                            = 2, then solution vector is printed, pro-
c                                 vided level .ge. 1
c                            = 3, then residual vector is printed, pro-
c                                 vided level .ge. 1
c                            = 4, then both vectors are printed, pro- 
c                                 vided level .ge. 1
c
c ... local itpackv references --
c
c          pmult , vout
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),u(1),work(1)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      digit1 = 0.0
      digit2 = 0.0
      if (n.le.0) go to 70
c
      digit1 = -alog10(abs(srelpr))
      if (stptst.gt.0.0) digit1 = -alog10(abs(stptst))
      do 10 i = 1,n 
         work(i) = rhs(i)/coef(i,1)
   10 continue
      bnrm = sdot(n,work,1,work,1)
      if (bnrm.eq.0.0) go to 30
      call pmult (n,ndim,maxnz,jcoef,coef,u,work) 
      do 20 i = 1,n 
         work(i) = (rhs(i)-work(i))/coef(i,1)
   20 continue
      rnrm = sdot(n,work,1,work,1)
      temp = rnrm/bnrm
      if (temp.eq.0.0) go to 30
      digit2 = -alog10(abs(temp))/2.0
      go to 40
c
   30 digit2 = -alog10(abs(srelpr))
c
   40 if ((idgts.lt.1).or.(level.le.0)) go to 70
      write (nout,50) digit1,digit2
   50 format (/10x,'approx. no. of digits in stopping test =',
     *                f5.1,'  (digit1)' 
     *        /10x,'approx. no. of digits in ratio test    =',
     *                f5.1,'  (digit2)')
c
      if (idgts.le.1.or.idgts.gt.4) go to 70
      if (idgts.ge.3) call vout (n,work,1,nout)
      do 60 i = 1,n 
         work(i) = u(i)*coef(i,1)
   60 continue
      if (idgts.ne.3) call vout (n,work,2,nout)
c
   70 continue
      return
      end 
      subroutine pervec (n,p,v,work)
c
c ... pervec permutes a vector as dictated by the permutation
c ... vector p.  if p(i) = j, then v(j) gets v(i).
c
c ... parameter list --
c
c          n       length of vectors p, v, and work
c          p       integer permutation vector
c          v       vector to be permuted
c          work    workspace vector of length n
c
c ... specifications for arguments
c
      integer p(1)
      dimension v(1),work(1)
c
      call scopy (n,v,1,work,1)
      do 10 i = 1,n 
         v(p(i)) = work(i)
   10 continue
      return
      end 
      subroutine pfsor (n,nb,ndim,maxnz,jcoef,coef,u,rhs,
     *                  nup,moved,ipop) 
c
c         pfsor computes a forward sor sweep on u.
c
c ... parameter list --
c
c          n      order of system
c          nb     order of black subsystem
c          ndim   row dimension of coef and jcoef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array of coefficient columns
c          coef   array of coefficients 
c          u      latest estimate of solution
c          rhs    right hand side of matrix problem
c          nup    maximum number of nonzeros per row in lower
c                  triangle l (input for natural order only)
c          moved  logical variable indicating whether or not matrix
c                  has been reshuffled
c          ipop   integer vector giving the wave populations
c
c ... specifications for arguments
c
      integer jcoef(ndim,2),ipop(1)
      dimension coef(ndim,2),u(1),rhs(1)
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      fac = 1.0-omega
      if (nb.ge.0) go to 60
c
c ... natural ordering
c
      if (.not.moved) go to 30
c
c ... compute   omega * (u*un + c) + (1-w) * un
c
      call ymasx2 (ndim,n,nup,coef(1,2),jcoef(1,2),rhs,u)
      do 10 i = 1,n 
         u(i) = rhs(i)+fac*u(i)
   10 continue
c
c ... compute  u(n+1) = inv(i - omega*l) * rhs
c
      nup2 = nup+2
      ied = 0
      do 20 k = 1,numwav
         nlen = ipop(k)
         ist = ied+1
         ied = ied+nlen
         do 18 j = nup2,maxnz 
cdir$ ivdep
            do 15 i = ist,ied 
               u(i) = u(i) - coef(i,j)*u(jcoef(i,j))
   15       continue
   18    continue
   20 continue
      return
c
c ... natural ordering -- unshuffled matrix.
c
   30 ied = 0
      do 50 k = 1,numwav
         nlen = ipop(k)
         ist = ied+1
         ied = ied+nlen
         do 38 j = 2,maxnz
            do 35 i = ist,ied 
               rhs(i) = rhs(i) - coef(i,j)*u(jcoef(i,j))
   35       continue
   38    continue
         do 40 i = ist,ied
            u(i) = rhs(i)+fac*u(i)
   40    continue
   50 continue
      return
c
c ... red-black ordering
c
   60 nr = n-nb
      nrp1 = nr+1
      maxm1 = maxnz-1
      call ymasx2 (ndim,nr,maxm1,coef(1,2),jcoef(1,2),rhs,u)
      do 70 i = 1,nr
         u(i) = rhs(i)+fac*u(i)
   70 continue
      call ymasx2 (ndim,nb,maxm1,coef(nrp1,2),jcoef(nrp1,2),rhs(nrp1),u)
      do 80 i = nrp1,n
         u(i) = rhs(i)+fac*u(i)
   80 continue
c
      return
      end 
      subroutine pfsor1 (n,nb,ndim,maxnz,jcoef,coef,u,rhs,
     *                   nup,moved,work,ipop)
c
c         pfsor1 computes a forward sor sweep on u and
c         computes the norm of the pseudo-residual vector.
c
c ... parameter list --
c
c          n      order of system
c          nb     order of black subsystem
c          ndim   row dimension of coef and jcoef arrays in calling
c                  routine
c          maxnz  maximum number of nonzeros per row
c          jcoef  integer array of coefficient columns
c          coef   array of coefficients 
c          u      latest estimate of solution
c          rhs    right hand side of matrix problem
c          nup    maximum number of nonzeros per row in lower
c                  triangle l (input for natural order only)
c          moved  logical variable indicating whether or not matrix
c                  has been reshuffled
c          work   workspace vector of length n
c          ipop   integer vector giving the wave populations
c
c ... specifications for arguments
c
      integer jcoef(ndim,2),ipop(1)
      dimension coef(ndim,2),u(1),rhs(1),work(1)
      logical moved 
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      fac = 1.0-omega
      if (nb.ge.0) go to 70
c
c ... natural ordering
c
      if (.not.moved) go to 30
c
c ... compute   omega * (u*un + c) + (1-w) * un
c
      call ymasx2 (ndim,n,nup,coef(1,2),jcoef(1,2),rhs,u)
      call saxpy (n,fac,u,1,rhs,1)
c
c ... compute  u(n+1) = inv(i - omega*l) * rhs
c
      nup2 = nup+2
      ied = 0
      do 10 k = 1,numwav
         nlen = ipop(k)
         ist = ied+1
         ied = ied+nlen
         do 8 j = nup2,maxnz
cdir$ ivdep
            do 5 i = ist,ied
               rhs(i) = rhs(i) - coef(i,j)*rhs(jcoef(i,j))
    5       continue
    8    continue
   10 continue
      do 20 i = 1,n 
         u(i) = rhs(i)-u(i)
   20 continue
      delnnm = snrm2(n,u,1)
      call scopy (n,rhs,1,u,1)
      return
c
c ... natural ordering -- unshuffled matrix.
c
   30 continue
      call scopy (n,u,1,work,1)
      ied = 0
      do 50 k = 1,numwav
         nlen = ipop(k)
         ist = ied+1
         ied = ied+nlen
         do 38 j = 2,maxnz
            do 35 i = ist,ied 
               rhs(i) = rhs(i) - coef(i,j)*work(jcoef(i,j)) 
   35       continue
   38    continue
         do 40 i = ist,ied
            work(i) = rhs(i)+fac*u(i)
   40    continue
   50 continue
      do 60 i = 1,n 
         u(i) = work(i)-u(i)
   60 continue
      delnnm = snrm2(n,u,1)
      call scopy (n,work,1,u,1)
      return
c
c ... red-black ordering
c
   70 nr = n-nb
      nrp1 = nr+1
      maxm1 = maxnz-1
      call ymasx2 (ndim,nr,maxm1,coef(1,2),jcoef(1,2),rhs,u)
      do 80 i = 1,nr
         work(i) = rhs(i)+fac*u(i)
   80 continue
      call ymasx2 (ndim,nb,maxm1,coef(nrp1,2),jcoef(nrp1,2),rhs(nrp1),
     *   work)
      do 90 i = nrp1,n
         work(i) = rhs(i)+fac*u(i)
   90 continue
      do 100 i = 1,n
         u(i) = work(i)-u(i)
  100 continue
      delnnm = snrm2(n,u,1)
      call scopy (n,work,1,u,1)
c
      return
      end 
      subroutine pjac (n,ndim,maxnz,jcoef,coef,u,rhs)
c
c ... pjac performs one jacobi iteration.
c
c ... parameter list --
c
c         n       dimension of matrix
c         ndim    row dimension of jcoef and coef arrays in calling
c                   routine
c         maxnz   maximum number of nonzeros per row
c         jcoef   integer data structure for coefficient columns
c         coef    data structure for array coefficients
c         u       estimate of solution of a matrix problem
c         rhs     on input -- contains the right hand side of the
c                             matrix problem
c                 on output -- contains b*u + rhs  where b = i - a
c                              and a has been scaled to have a unit
c                              diagonal 
c
c ... specifications for arguments
c
      integer jcoef(ndim,2)
      dimension coef(ndim,2),u(1),rhs(1)
c
      maxm1 = maxnz-1
      call ymasx2 (ndim,n,maxm1,coef(1,2),jcoef(1,2),rhs,u) 
      return
      end 
      subroutine pmult (n,ndim,maxnz,jcoef,coef,b,c)
c
c ... pmult computes c = a*b, a matrix-vector product.  matrix
c     a is assumed to be stored in the coef, jcoef ellpack
c     data structure and all entries in the column array jcoef
c     are assumed to be between 1 and n, inclusive.
c     a is assumed to have a unit diagonal.
c
c ... parameter list --
c
c          n        dimension of matrix 
c          ndim     row dimension of coef and jcoef in calling routine
c          maxnz    maximum number of nonzeros per row
c          jcoef    integer array for coefficient columns
c          coef     array for coefficients
c          b        multiplying vector of length n
c          c        product vector of length n
c
c ... specifications for arguments
c
      integer jcoef(ndim,2)
      dimension coef(ndim,2),b(1),c(1)
c
      call scopy (n,b,1,c,1)
      maxm1 = maxnz-1
      call ypasx2 (ndim,n,maxm1,coef(1,2),jcoef(1,2),c,b)
      return
      end 
      subroutine prbndx (n,ndim,maxnz,jcoef,p,ip,nblack,level,nout,ier)
c
c**************************************************************
c
c     prbndx computes the red-black permutation
c     vectors p ( and its inverse ip ) if possible.
c
c     the algorithm is to mark the first node as red (arbitrary).
c     all of its adjacent nodes are marked black and placed in
c     a stack.  the remainder of the code pulls the first node
c     off the top of the stack and tries to type its adjacent nodes.
c     the typing of the adjacent point is a five way case statement
c     which is well commented below (see do loop 100).
c
c     the array p is used both to keep track of the color of a node
c     (red node is positive, black is negative) but also the father
c     node that caused the color marking of that point.  since
c     complete information on the adjacency structure is hard to come 
c     by this forms a link to enable the color change of a partial
c     tree when a recoverable color conflict occurs.
c
c     the array ip is used as a stack to point to the set of nodes
c     left to be typed that are known to be adjacent to the current
c     father node.
c
c     *** note ***  this routine is to be called after routine scal.
c
c*********************************************************************
c
c     input parameters --
c
c        n      number of nodes.  (integer, scalar)
c
c        ndim   row dimension of jcoef in calling routine.
c
c        maxnz  maximum number of nonzeros per row
c
c        jcoef  array of column indices.  it is assumed
c               that for every row where only one element is
c               stored that element corresponds to the diagonal
c               entry.  the diagonal must be the first entry stored.
c                 (integer, arrays)
c
c        level  switch for printing
c
c        nout   output tape number
c
c     output parameters --
c
c        nblack number of black nodes.  number of red nodes is
c               n - nblack.  (integer, scalar)
c
c        p, ip  permutation and inverse permutation vectors.
c               (integer, arrays each of length n)
c
c        ier    error flag. (integer, scalar)
c
c               ier = 0, normal return.  indexing performed 
c                        successfully
c               ier = 201, red-black indexing not possible. 
c
c******************************************************************** 
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),p(1),ip(1)
c
c ... specifications for local variables
c
      integer first,old,young,curtyp,type
c
c-----------------------------------------------------------------------
c
      ier = 0
      if (n.le.0) return
      do 10 i = 1,n 
         p(i) = 0
         ip(i) = 0
   10 continue
c
c ... handle the first set of points until some adjacent points
c ... are found
c
      first = 1
c
   20 p(first) = first
      if (maxnz.gt.1) go to 40
c
c ... search for next entry that has not been marked
c
      if (first.eq.n) go to 120
      ibgn = first+1
      do 30 i = ibgn,n
         if (p(i).ne.0) go to 30
         first = i
         go to 20
   30 continue
      go to 120
c
c ... first set of adjacent points found
c
   40 next = 1
      last = 1
      ip(1) = first 
c
c ... loop over labeled points indicated in the stack stored in
c ... the array ip
c
   50 k = ip(next)
      curtyp = p(k) 
      nxttyp = -curtyp
      do 100 j = 2,maxnz
         jcol = jcoef(k,j)
         if (jcol.eq.k) go to 100
         type = p(jcol)
c
c==================================================================
c
c     the following is a five way case statement dealing with the
c     labeling of the adjacent node.
c
c ... case i.  if the adjacent node has already been labeled with
c              label equal to nxttyp, then skip to the next adjacent
c              node.
c
         if (type.eq.nxttyp) go to 100
c
c ... case ii.  if the adjacent node has not been labeled yet label
c               it with nxttyp and enter it in the stack
c
         if (type.ne.0) go to 60
         last = last+1
         ip(last) = jcol
         p(jcol) = nxttyp
         go to 100
c
c ... case iii.  if the adjacent node has already been labeled with
c                opposite color and the same father seed, then there
c                is an irrecoverable color conflict.
c
   60    if (type.eq.curtyp) go to 140
c
c ... case iv.  if the adjacent node has the right color and a different
c               father node, then change all nodes of the youngest fathe
c               node to point to the oldest father seed and retain the
c               same colors.
c
         if (type*nxttyp.lt.1) go to 80 
         old = min0(iabs(type),iabs(nxttyp))
         young = max0(iabs(type),iabs(nxttyp))
         do 70 l = young,n
            if (iabs(p(l)).eq.young) p(l) = isign(old,p(l)) 
   70    continue
         curtyp = p(k)
         nxttyp = -curtyp
         go to 100
c
c ... case v.  if the adjacent node has the wrong color and a different
c              father node, then change all nodes of the youngest father
c              node to point to the oldest father node along with
c              changing their colors.  since until this time the
c              youngest father node tree has been independent no other
c              color conflicts will arise from this change. 
c
   80    old = min0(iabs(type),iabs(nxttyp))
         young = max0(iabs(type),iabs(nxttyp))
         do 90 l = young,n
            if (iabs(p(l)).eq.young) p(l) = isign(old,-p(l))
   90    continue
         curtyp = p(k)
         nxttyp = -curtyp
c
c ... end of case statement
c
c==================================================================
c
  100 continue
c
c ... advance to next node in the stack 
c
      next = next+1 
      if (next.le.last) go to 50
c
c ... all nodes in the stack have been removed
c
c ... check for nodes not labeled.  if any are found
c ... start the labeling process again at the first
c ... node found that is not labeled.
c
      ibgn = first+1
      do 110 i = ibgn,n
         if (p(i).ne.0) go to 110
         first = i
         go to 20
  110 continue
c
c===================================================================
c
c ... all nodes are now typed either red or black 
c
c ... generate permutation vectors
c
  120 call whenige (n,p,1,0,ip,nred)
      call whenilt (n,p,1,0,ip(nred+1),nblack)
      do 130 i = 1,n
         p(ip(i)) = i
  130 continue
c
c ... successful red-black ordering completed
c
      return
c
c ...... type conflict
c
  140 ier = 201
      if (level.ge.0) write (nout,150)
  150 format (//1x,'*** f a t a l     e r r o r ************'//1x,
     *   '    in itpackv routine prbndx  '/1x,
     *   '    red-black indexing not possible')
      return
      end 
      subroutine prsblk (nb,nr,ndim,maxnz,jcoef,coef,u,v)
c
c ... prsblk computes a black-rs sweep on a red vector into a black
c ... vector.
c
c ... parameter list --
c
c          nb       number of black points
c          nr       number of red points
c          ndim     row dimension of jcoef and coef arrays in calling 
c                    routine
c          maxnz    maximum number of nonzeros per row
c          jcoef    integer data structure for coefficient columns
c          coef     data structure for array coefficients
c          u        latest estimate of the solution
c          v        on input -- contains the right hand side
c                   on output -- v(nr+1,...n) contains (fr**t)*ur+cb
c
c ... specifications for arguments
c
      integer jcoef(ndim,2)
      dimension coef(ndim,2),u(1),v(1)
c
      if (nb.le.0.or.maxnz.le.1) return 
      nrp1 = nr+1
      maxm1 = maxnz-1
      call ymasx2 (ndim,nb,maxm1,coef(nrp1,2),jcoef(nrp1,2),v(nrp1),u)
      return
      end 
      subroutine prsred (nr,ndim,maxnz,jcoef,coef,u,vr)
c
c ... prsred computes a red-rs sweep on a black vector into a red
c ... vector.
c
c ... parameter list --
c
c         nr       number of red points 
c         ndim     row dimension of jcoef and coef arrays in calling
c                   routine
c         maxnz    maximum number of nonzeros per row
c         jcoef    integer data structure for coefficient columns
c         coef     data structure for array coefficients
c         u        latest estimate of solution
c         vr       on input -- contains the right hand side 
c                  on output -- contains fr*ub + cr
c
c ... specifications for arguments
c
      integer jcoef(ndim,2)
      dimension coef(ndim,2),u(1),vr(1) 
c
      if (nr.le.0.or.maxnz.le.1) return 
      maxm1 = maxnz-1
      call ymasx2 (ndim,nr,maxm1,coef(1,2),jcoef(1,2),vr,u) 
      return
      end 
      subroutine pstop (n,u,dnrm,ccon,iflag,q1)
c
c     pstop performs a test to see if the iterative
c     method has converged to a solution inside the error
c     tolerance, zeta.
c
c ... parameter list --
c
c          n      order of system
c          u      present solution estimate
c          dnrm   inner product of pseudo-residuals at preceding
c                    iteration
c          con    stopping test parameter (= ccon)
c          iflag  stopping test integer flag
c                    iflag = 0,  sor iteration zero
c                    iflag = 1,  non-rs method
c                    iflag = 2,  rs method
c          q1     stopping test logical flag
c
c ... specifications for arguments
c
      dimension u(n)
      logical q1
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
      con = ccon
      halt = .false.
c
c     special procedure for zeroth iteration
c
      if (in.ge.1) go to 10
      q1 = .false.
      udnm = 1.0
      stptst = 1000.0
      if (iflag.le.0) return
c
c ... test if udnm needs to be recomputed
c
   10 continue
      if (q1) go to 20
      if ((in.gt.5).and.(mod(in,5).ne.0)) go to 20
      uold = udnm
      udnm = sdot(n,u,1,u,1)
      if (udnm.eq.0.0) udnm = 1.0
      if ((in.gt.5).and.(abs(udnm-uold).le.udnm*zeta)) q1 = .true.
c
c ... compute stopping test
c
   20 tr = sqrt(udnm)
      tl = 1.0
      if (con.eq.1.0) go to 40
      if (iflag.eq.2) go to 30
      tl = sqrt(dnrm)
      tr = tr*(1.0-con)
      go to 40
   30 tl = sqrt(2.0*dnrm)
      tr = tr*(1.0-con*con)
   40 stptst = tl/tr
      if (tl.ge.tr*zeta) return
      halt = .true. 
c
      return
      end 
      function pvtbv (n,ndim,maxnz,jcoef,coef,v)
c
c ... pvtbv computes  (v**t)*b*v  where  b = i - a.
c
c ... parameter list --
c
c        n      dimension of matrix
c        ndim   row dimension of jcoef and coef in calling routine
c        maxnz  maximum number of nonzeros per row
c        jcoef  integer data structure for coefficient columns
c        coef   data structure for equation coefficients
c        v      array of length n
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),v(1)
c
      pvtbv = 0.0
      if (maxnz.le.1) return
      do 20 j = 2,maxnz
         do 10 i = 1,n
            pvtbv = pvtbv-v(i)*coef(i,j)*v(jcoef(i,j))
   10    continue
   20 continue
      return
      end 
      subroutine sbelm (n,ndim,maxnz,jcoef,coef,rhs,work,tol)
c
c ... sbelm is designed to remove rows of the matrix for which
c ... all off-diagonal elements are very small (less than tol).
c ... this is to take care of matrices arising from finite
c ... element discretizations of partial differential equations
c ... with dirichlet boundary conditions.  any such rows and
c ... corresponding columns are then eliminated (set to the 
c ... identity after correcting the rhs).
c ... *** note ***  this routine is to be called after routine scal.
c
c ... parameter list --
c
c         n       dimension of matrix
c         ndim    row dimension of arrays jcoef and coef in the
c                    calling program
c         maxnz   maximum number of nonzero entries per row 
c         jcoef   integer array of matrix representation
c         coef    array of sparse matrix representation
c         rhs     right hand side of matrix problem
c         work    work array of length n
c         tol     tolerance factor
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),work(1)
c
      if (n.le.0.or.maxnz.lt.2) return
c
c ... find maximum off-diagonal elements in absolute value. 
c
      do 10 i = 1,n 
         work(i) = 0.0
   10 continue
      do 30 j = 2,maxnz
         do 20 i = 1,n
            work(i) = amax1(work(i),abs(coef(i,j)))
   20    continue
   30 continue
c
c ... eliminate desired rows and columns.
c
      do 60 j = 2,maxnz
         do 50 i = 1,n
            if (work(i).lt.tol) go to 40
            jcol = jcoef(i,j) 
            if (work(jcol).ge.tol) go to 50
            rhs(i) = rhs(i)-coef(i,j)*rhs(jcol)
   40       coef(i,j) = 0.0
            jcoef(i,j) = i
   50    continue
   60 continue
      return
      end 
      subroutine scal (n,ndim,maxnz,jcoef,coef,rhs,u,work,ier)
c
c ... scal scales original matrix to a unit diagonal matrix.  rhs
c ... and u vectors are scaled accordingly.  the data
c ... structure is adjusted to have diagonal entries in
c ... column 1.  zero entries in jcoef array are changed to 
c ... positive integers between 1 and n.
c
c ... parameter list --
c
c         n       dimension of matrix
c         ndim    row dimension of arrays jcoef and coef in the
c                    calling program
c         maxnz   maximum number of nonzero entries per row 
c         jcoef   integer array of matrix representation
c         coef    array of sparse matrix representation
c         rhs     right hand side of matrix problem
c         u       latest estimate of solution
c         work    work array of length n
c         ier     error flag -- on return, nonzero values mean
c                    401 -- zero diagonal element 
c                    402 -- nonexistent diagonal element
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),u(1),work(1)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
c ... check for positive diagonal entries for each row.
c ... put diagonal entries in column 1.  replace zeros in
c ... row i of jcoef with i.
c
      ier = 0
      nsgncg = 0
      if (n.le.0) return
      do 110 i = 1,n
         if (jcoef(i,1).eq.i) go to 50
         if (maxnz.lt.2) go to 20
         do 10 j = 2,maxnz
            if (jcoef(i,j).eq.i) go to 40
   10    continue
c
c ... fatal error -- no diagonal entry for row i. 
c
   20    ier = 402
         if (level.ge.0) write (nout,30) i
   30    format (//1x,'*** f a t a l     e r r o r ************'//1x, 
     *      '    in itpackv routine scal    '/1x,
     *      '    no diagonal entry in row',i10)
         return
c
c ... shift row i so that diagonal element is in column 1.
c
   40    save = coef(i,j)
         coef(i,j) = coef(i,1)
         jcoef(i,j) = jcoef(i,1)
         coef(i,1) = save
         jcoef(i,1) = i
c
c ... check sign of diagonal entry.  if negative, change signs of
c ... all row coefficients and corresponding rhs element.
c
   50    if (coef(i,1)) 60 , 90 , 110
   60    do 70 j = 1,maxnz
            coef(i,j) = -coef(i,j)
   70    continue
         rhs(i) = -rhs(i)
         nsgncg = nsgncg+1
         if (level.ge.5) write (nout,80) i
   80    format (//1x,'*** n o t e ***'//1x,
     *      '    in itpackv routine scal'/1x,
     *      '    equation ',i10,' has been negated')
         go to 110
c
c ... fatal error -- zero diagonal element for row i.
c
   90    ier = 401
         if (level.ge.0) write (nout,100) i
  100    format (//1x,'*** f a t a l     e r r o r ************'//1x, 
     *      '    in itpackv routine scal    '/1x,
     *      '    diagonal entry in row ',i10,' is zero')
         return
  110 continue
c
c ... change zero elements of jcoef array.
c
      if (maxnz.lt.2) go to 140
      do 130 j = 2,maxnz
         do 120 i = 1,n
            if (jcoef(i,j).le.0) jcoef(i,j) = i
  120    continue
  130 continue
c
c ... scale rhs and u arrays.  store reciprocal square roots
c ... of diagonal entries in column 1 of coef.
c
  140 do 150 i = 1,n
         work(i) = sqrt(coef(i,1))
  150 continue
      do 160 i = 1,n
         u(i) = u(i)*work(i)
  160 continue
      do 170 i = 1,n
         work(i) = 1.0/work(i)
  170 continue
      call scopy (n,work,1,coef,1)
      do 180 i = 1,n
         rhs(i) = rhs(i)*work(i)
  180 continue
c
c ... scale matrix. 
c
      if (maxnz.lt.2) return
      do 200 j = 2,maxnz
         do 190 i = 1,n
            coef(i,j) = coef(i,j)*work(i)*work(jcoef(i,j))
  190    continue
  200 continue
c
c ... adjust isym if the  0 .lt. nsgncg .lt. n
c
      if (nsgncg.gt.0.and.nsgncg.lt.n) isym = 1
c
      return
      end 
      subroutine sorscl (n,ndim,maxnz,coef,rhs,factor)
c
c ... sorscl scales the matrix and rhs vector by factor.
c
c ... parameter list --
c
c         n       dimension of matrix
c         ndim    row dimension of array coef
c         maxnz   maximum number of nonzero entries per row 
c         coef    array of sparse matrix representation
c         rhs     right hand side of matrix problem
c         factor  scaling factor
c
c ... specifications for arguments
c
      dimension coef(ndim,1),rhs(1)
c
c ... scale matrix and rhs.
c
      do 10 j = 2,maxnz
         call sscal (n,factor,coef(1,j),1)
   10 continue
      call sscal (n,factor,rhs,1)
      return
      end 
      subroutine sorwav (n,ndim,maxnz,jcoef,coef,rhs,u,p,ip,
     *                   iwork,work,numwav,icall) 
c
c ... sorwav determines the wave front reordering for a forward
c     and back sor pass.  the permutation vector corresponding to
c     a wave front ordering is constructed.  the matrix and rhs
c     are permuted accordingly.
c
c ... parameter list --
c
c          n         order of system
c          ndim      row dimension of arrays jcoef and coef in
c                       the calling routine
c          maxnz     maximum number of nonzero entries per row
c          jcoef     integer array for data
c          coef      array for data structure coefficients
c          rhs       right-hand-side
c          u         solution vector
c          p         permutation vector 
c          ip        inverse permutation vector
c          iwork     integer workspace of length n
c          work      real workspace of length n
c          numwav    number of wave fronts
c          icall     switch for calling 
c                     = 1   for initial call
c                     = 2   for second call
c
c ... specifications for arguments
c
      integer jcoef(ndim,1),p(1),ip(1),iwork(1)
      dimension coef(ndim,1),rhs(1),u(1),work(1)
c
      if (maxnz.le.1) return
      if (icall.eq.2) go to 90
      do 10 i = 1,n 
         p(i) = 1
         iwork(i) = 0
   10 continue
      do 40 i = 1,n 
         do 20 j = 2,maxnz
            jcol = jcoef(i,j) 
            if (jcol.ge.i) go to 20
            if (coef(i,j).eq.0.0) go to 20
            p(i) = max0(p(i),p(jcol)+1) 
   20    continue
         do 30 j = 2,maxnz
            jcol = jcoef(i,j) 
            if (jcol.le.i) go to 30
            if (coef(i,j).eq.0.0) go to 30
            p(jcol) = max0(p(jcol),p(i)+1)
   30    continue
   40 continue
c
c ... determine number of wave fronts and populations.
c
      numwav = 0
      do 50 i = 1,n 
         numwav = max0(numwav,p(i))
   50 continue
      do 60 i = 1,n 
         iw = p(i)
         iwork(iw) = iwork(iw)+1
   60 continue
c
c ... construct permutation vector.
c
      ip(1) = 1
      do 70 i = 2,numwav
         ip(i) = ip(i-1)+iwork(i-1)
   70 continue
      do 80 i = 1,n 
         iw = p(i)
         p(i) = ip(iw)
         ip(iw) = ip(iw)+1
   80 continue
c
c ... permute matrix, rhs, and u.
c
      call permat (n,ndim,maxnz,jcoef,coef,p,work,ip)
      call pervec (n,p,rhs,work)
      call pervec (n,p,u,work)
      return
c
c ... construct inverse permutation vector.
c
   90 do 100 i = 1,n
         ip(p(i)) = i
  100 continue
c
c ... unpermute matrix, rhs, and u.
c
      call permat (n,ndim,maxnz,jcoef,coef,ip,work,p)
      call pervec (n,ip,rhs,work)
      call pervec (n,ip,u,work)
      return
      end 
      function tau (i)
c
c ... tau sets tau(i) for the sor method.
c
c ... parameter list --
c
c          i     number of times parameters have been changed
c
c ... specifications for arguments
c
c ... specifications for local variables
c
      dimension t(8)
c
      data t(1),t(2),t(3),t(4),t(5),t(6),t(7),t(8) / 1.5,1.8,1.85,1.9,
     *   1.94,1.96,1.975,1.985 /
c
      tau = 1.992
      if (i.le.8) tau = t(i)
c
      return
      end 
      real function timer (timdmy)
c
c ... timer is a routine to return the execution time in
c     seconds.
c
c ... parameter list --
c
c          timdmy   dummy argument
c
      timer = second()
c
c     real tarray(2)
c     call etime (tarray)
c     timer = tarray(1)
c
      return
      end 
      logical function tstchg (ibmth)
c
c     tstchg performs a test to determine if parameters
c     should be changed for semi-iteration accelerated methods.
c
c ... parameter list --
c
c          ibmth  indicator of basic method being accelerated by si
c                      ibmth = 1,   jacobi
c                            = 2,   reduced system
c                            = 3,   ssor
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common blocks in main routine
c
      ip = in-is
      if (ibmth.eq.2) ip = 2*ip
c
      if (in.eq.0) go to 10
      if (ip.lt.3) go to 20
c
      qa = sqrt(abs(delnnm/delsnm))
      qt = 2.0*sqrt(abs(rrr**ip))/(1.0+rrr**ip)
      if ((qa.ge.1.0).or.(qa.lt.qt**ff)) go to 20 
c
c ... test passes -- change parameters
c
   10 tstchg = .true.
      return
c
c ... test fails -- do not change parameters
c
   20 tstchg = .false.
      return
c
      end 
      subroutine unscal (n,ndim,maxnz,jcoef,coef,rhs,u,work)
c
c ... unscal reverses the scaling done in routine scal.
c
c ... parameter list --
c
c         n       dimension of matrix
c         ndim    row dimension of arrays jcoef and coef in the
c                    calling program
c         maxnz   maximum number of nonzero entries per row 
c         jcoef   integer array of matrix representation
c         coef    array of sparse matrix representation
c         rhs     right hand side of matrix problem
c         u       latest estimate of solution
c         work    work array of length n
c
c ... specifications for arguments
c
      integer jcoef(ndim,1)
      dimension coef(ndim,1),rhs(1),u(1),work(1)
c
c ... unscale u and rhs arrays.
c
      call scopy (n,coef,1,work,1)
      do 10 i = 1,n 
         u(i) = u(i)*work(i)
   10 continue
      do 20 i = 1,n 
         work(i) = 1.0/work(i)
   20 continue
      do 30 i = 1,n 
         rhs(i) = rhs(i)*work(i)
   30 continue
c
c ... unscale matrix.
c
      if (maxnz.lt.2) go to 80
      do 50 j = 2,maxnz
         do 40 i = 1,n
            coef(i,j) = coef(i,j)*work(i)*work(jcoef(i,j))
   40    continue
   50 continue
c
c ... put original zeros back in icoef array and restore unscaled
c ... diagonal entries to column one.
c
      do 70 j = 2,maxnz
         do 60 i = 1,n
            if (jcoef(i,j).eq.i) jcoef(i,j) = 0
   60    continue
   70 continue
   80 do 90 i = 1,n 
         coef(i,1) = work(i)**2
   90 continue
      return
      end 
      subroutine ypasx2 (ndim,n,m,a,ja,y,x)
c
c ... ypasx2 does the loop
c
c           do 20 j = 1,m
c              do 10 i = 1,n
c                 y(i) = y(i) + a(i,j)*x(ja(i,j)) 
c       10     continue
c       20  continue
c
c ... parameters -- 
c
c       ndim      row dimension of a and ja arrays
c       n         order of system
c       m         number of columns in a and ja arrays
c       a         real array of active size n by m
c       ja        integer array of active size n by m
c       y         accumulation vector
c       x         right-hand-side vector
c
c ... specifications for parameters
c
      dimension a(ndim,3),ja(ndim,3),x(1),y(1)
c
      if (n.le.0 .or. m.le.0) return
      l = mod(m,4)
      if (l.eq.0) go to 80
c
c ... initial short computations
c
      go to (10,30,50), l
   10 do 20 i = 1,n 
         y(i) = y(i)+a(i,1)*x(ja(i,1))
   20 continue
      go to 70
   30 do 40 i = 1,n 
         y(i) = y(i)+a(i,1)*x(ja(i,1))+a(i,2)*x(ja(i,2))
   40 continue
      go to 70
   50 do 60 i = 1,n 
         y(i) = y(i)+a(i,1)*x(ja(i,1))+a(i,2)*x(ja(i,2))+a(i,3)*x(ja(i,
     *      3))
   60 continue
   70 if (m.le.4) return
c
c ... loop unrolling to a level of 4.
c
   80 lp1 = l+1
      do 100 j = lp1,m,4
         do 90 i = 1,n
            y(i) = y(i)+a(i,j)*x(ja(i,j))+a(i,j+1)*x(ja(i,j+1))+a(i,j+2)
     *         *x(ja(i,j+2))+a(i,j+3)*x(ja(i,j+3))
   90    continue
  100 continue
      return
      end 
      subroutine ymasx2 (ndim,n,m,a,ja,y,x)
c
c ... ymasx2 does the loop
c
c           do 20 j = 1,m
c              do 10 i = 1,n
c                 y(i) = y(i) - a(i,j)*x(ja(i,j)) 
c       10     continue
c       20  continue
c
c ... parameters -- 
c
c       ndim      row dimension of a and ja arrays
c       n         order of system
c       m         number of columns in a and ja arrays
c       a         real array of active size n by m
c       ja        integer array of active size n by m
c       y         accumulation vector
c       x         right-hand-side vector
c
c ... specifications for parameters
c
      dimension a(ndim,3),ja(ndim,3),x(1),y(1)
c
      if (n.le.0 .or. m.le.0) return
      l = mod(m,4)
      if (l.eq.0) go to 80
c
c ... initial short computations
c
      go to (10,30,50), l
   10 do 20 i = 1,n 
         y(i) = y(i)-a(i,1)*x(ja(i,1))
   20 continue
      go to 70
   30 do 40 i = 1,n 
         y(i) = y(i)-a(i,1)*x(ja(i,1))-a(i,2)*x(ja(i,2))
   40 continue
      go to 70
   50 do 60 i = 1,n 
         y(i) = y(i)-a(i,1)*x(ja(i,1))-a(i,2)*x(ja(i,2))-a(i,3)*x(ja(i,
     *      3))
   60 continue
   70 if (m.le.4) return
c
c ... loop unrolling to a level of 4.
c
   80 lp1 = l+1
      do 100 j = lp1,m,4
         do 90 i = 1,n
            y(i) = y(i)-a(i,j)*x(ja(i,j))-a(i,j+1)*x(ja(i,j+1))-a(i,j+2)
     *         *x(ja(i,j+2))-a(i,j+3)*x(ja(i,j+3))
   90    continue
  100 continue
      return
      end 
      subroutine vfill (n,v,val)
c
c     vfill fills a vector, v, with a constant value, val.
c
c ... parameter list --
c
c          n      integer length of vector v
c          v      vector
c          val    constant that fills first n locations of v
c
c ... specifications for arguments
c
      dimension v(n)
c
      if (n.le.0) return
      do 10 i = 1,n 
         v(i) = val 
   10 continue
      return
      end 
      subroutine vout (n,v,iswt,nout)
c
c     vout effects printing of residual and solution
c     vectors - called from perror
c
c ... parameter list --
c
c          v      vector of length n
c          iswt   labelling information 
c          nout   output device number
c
c ... specifications for arguments
c
      dimension v(n)
c
c        if (n .le. 0) return 
c
      kupper = min0(n,4)
      if (iswt.eq.1) write (nout,10)
   10 format (//5x,'residual vector')
      if (iswt.eq.2) write (nout,20)
   20 format (//5x,'solution vector')
      write (nout,30) (i,i=1,kupper)
   30 format (10x,4i15)
      write (nout,40)
   40 format (10x,65('-')/)
c
      do 60 j = 1,n,4
         kupper = min0(j+3,n) 
         jm1 = j-1
         write (nout,50) jm1,(v(k),k=j,kupper)
   50    format (4x,i5,'+  ',4e15.5)
   60 continue
c
      return
      end 
      subroutine zbrent (n,tri,eps,nsig,a,b,maxfn,ier)
c
c   modified imsl routine name   - zbrent
c
c-----------------------------------------------------------------------
c
c   computer            - cdc/single
c
c   latest revision     - january 1, 1978
c
c   purpose             - zero of a function which changes sign in a
c                           given interval (brent algorithm)
c
c   usage               - call zbrent (f,eps,nsig,a,b,maxfn,ier)
c
c   arguments    tri    - a tridiagonal matrix of order n
c                eps    - first convergence criterion (input).  a root,
c                           b, is accepted if abs(f(b)) is less than or
c                           equal to eps.  eps may be set to zero.
c                nsig   - second convergence criterion (input).  a root,
c                           b, is accepted if the current approximation
c                           agrees with the true solution to nsig
c                           significant digits.
c                a,b    - on input, the user must supply two points, a
c                           and b, such that f(a) and f(b) are opposite
c                           in sign.
c                           on output, both a and b are altered.  b
c                           will contain the best approximation to the
c                           root of f. see remark 1.
c                maxfn  - on input, maxfn should contain an upper bound
c                           on the number of function evaluations
c                           required for convergence.  on output, maxfn
c                           will contain the actual number of function
c                           evaluations used.
c                ier    - error parameter. (output)
c                         terminal error
c                           ier = 501 indicates the algorithm failed to
c                             converge in maxfn evaluations.
c                           ier = 502 indicates f(a) and f(b) have the
c                             same sign.
c
c   precision/hardware  - single and double/h32
c                       - single/h36,h48,h60
c
c   notation            - information on special notation and
c                           conventions is available in the manual
c                           introduction or through imsl routine uhelp
c
c   remarks  1.  let f(x) be the characteristic function of the matrix
c                tri evaluated at x. function determ evaluates f(x).
c                on exit from zbrent, when ier=0, a and b satisfy the 
c                following,
c                f(a)*f(b) .le. 0,
c                abs(f(b)) .le. abs(f(a)), and
c                either abs(f(b)) .le. eps or
c                abs(a-b) .le. max(abs(b),0.1)*10.0**(-nsig).
c                the presence of 0.1 in this error criterion causes
c                leading zeroes to the right of the decimal point to be
c                counted as significant digits. scaling may be required
c                in order to accurately determine a zero of small
c                magnitude.
c            2.  zbrent is guaranteed to reach convergence within
c                k = (alog((b-a)/d)+1.0)**2 function evaluations where
c                  d=min(over x in (a,b) of
c                    max(abs(x),0.1)*10.0**(-nsig)).
c                this is an upper bound on the number of evaluations. 
c                rarely does the actual number of evaluations used by 
c                zbrent exceed sqrt(k). d can be computed as follows, 
c                  p = amin1(abs(a),abs(b))
c                  p = amax1 (0.1,p)
c                  if ((a-0.1)*(b-0.1).lt.0.0) p = 0.1
c                  d = p*10.0**(-nsig)
c
c   copyright           - 1977 by imsl, inc. all rights reserved.
c
c   warranty            - imsl warrants only that imsl testing has been
c                           applied to this code. no other warranty,
c                           expressed or implied, is applicable.
c
c-----------------------------------------------------------------------
c
c ... specifications for arguments
c
      dimension tri(1)
c
c *** begin -- itpackv common
c
      common /itcom1/ in,is,isym,itmax,level,nout,numwav
c
      logical adapt,betadt,caseii,halt,partad
      common /itcom2/ adapt,betadt,caseii,halt,partad
c
      common /itcom3/ bdelnm,betab,cme,delnnm,delsnm,ff,gamma,omega,qa,
     *   qt,rho,rrr,sige,sme,specr,spr,srelpr,stptst,udnm,zeta
c
c *** end   -- itpackv common
c
c     description of variables in common block in main routine
c
c ... local itpackv references --
c
c          determ
c
c                                  first executable statement
c
      ier = 0
      ib3 = 2*itmax+1
      ib4 = 3*itmax+1
      t = 10.0**(-nsig)
      ic = 2
      fa = determ(itmax,n,tri,tri(ib3),tri(ib4),a)
      fb = determ(itmax,n,tri,tri(ib3),tri(ib4),b)
      s = b
c
c                                  test for same sign
c
      if (fa*fb.gt.0.0) go to 110
   10 c = a
      fc = fa
      d = b-c
      e = d
   20 if (abs(fc).ge.abs(fb)) go to 30
      a = b
      b = c
      c = a
      fa = fb
      fb = fc
      fc = fa
   30 continue
      tol = t*amax1(abs(b),0.1)
      rm = (c-b)*0.5
c
c                                  test for first convergence criteria
c
      if (abs(fb).le.eps) go to 80
c
c                                  test for second convergence criteria
c
      if (abs(c-b).le.tol) go to 80
c
c                                  check evaluation counter 
c
      if (ic.ge.maxfn) go to 90
c
c                                  is bisection forced
c
      if (abs(e).lt.tol) go to 60
      if (abs(fa).le.abs(fb)) go to 60
      s = fb/fa
      if (a.ne.c) go to 40
c
c                                  linear interpolation
c
      p = (c-b)*s
      q = 1.0-s
      go to 50
c
c                                  inverse quadratic interpolation
c
   40 q = fa/fc
      r = fb/fc
      rone = r-1.0
      p = s*((c-b)*q*(q-r)-(b-a)*rone)
      q = (q-1.0)*rone*(s-1.0)
   50 if (p.gt.0.0) q = -q
      if (p.lt.0.0) p = -p
      s = e
      e = d
c
c                                  if abs(p/q).ge.75*abs(c-b) then
c                                     force bisection
c
      if (p+p.ge.3.0*rm*q) go to 60
c
c                                  if abs(p/q).ge..5*abs(s) then force
c                                     bisection. s = the value of p/q 
c                                     on the step before the last one 
c
      if (p+p.ge.abs(s*q)) go to 60
      d = p/q
      go to 70
c
c                                  bisection
c
   60 e = rm
      d = e
c
c                                  increment b
c
   70 a = b
      fa = fb
      temp = d
      if (abs(temp).le.0.5*tol) temp = sign(0.5*tol,rm)
      b = b+temp
      s = b
      fb = determ(itmax,n,tri,tri(ib3),tri(ib4),s)
      ic = ic+1
      if (fb*fc.le.0.0) go to 20
      go to 10
c
c                                  convergence of b
c
   80 a = c
      maxfn = ic
      go to 130
c
c                                  maxfn evaluations
c
   90 ier = 501
      a = c
      maxfn = ic
      if (level.ge.1) write (nout,100) maxfn
  100 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine zbrent'/1x,
     *   '    algorithm failed to converge'/1x,
     *   '    in',i6,' iterations ')
      go to 130
c
c                                  terminal error - f(a) and f(b) have
c                                  the same sign
c
  110 ier = 502
      maxfn = ic
      if (level.ge.1) write (nout,120)
  120 format (/1x,'*** w a r n i n g ************'//1x,
     *   '    in itpackv routine zbrent  '/1x,
     *   '    f(a) and f(b) have same sign   ')
  130 continue
      return
      end 
      integer function ismin (n,sx,incx)
c
c     find smallest index of minimum value of single precision sx.
c
      real sx(1),smin,xval
      ismin = 0
      if (n.le.0) return
      ismin = 1
      if (n.le.1) return
      if (incx.eq.1) go to 30 
c
c        code for increments not equal to 1.
c
      smin = sx(1)
      ns = n*incx
      ii = 1
      do 20 i = 1,ns,incx
         xval = sx(i)
         if (xval.ge.smin) go to 10
         ismin = ii 
         smin = xval
   10    ii = ii+1
   20 continue
      return
c
c        code for increments equal to 1.
c
   30 smin = sx(1)
      do 40 i = 2,n 
         xval = sx(i)
         if (xval.ge.smin) go to 40
         ismin = i
         smin = xval
   40 continue
      return
      end 
      subroutine whenige (n,p,inc,itarg,ip,npt)
      integer p(n), ip(n)
      npt = 0
      do 10 i = 1,n 
         if (p(i) .lt. itarg) go to 10
         npt = npt + 1
         ip(npt) = i
 10   continue
      return
      end 
      subroutine whenilt (n,p,inc,itarg,ip,npt)
      integer p(n), ip(n)
      npt = 0
      do 10 i = 1,n 
         if (p(i) .ge. itarg) go to 10
         npt = npt + 1
         ip(npt) = i
 10   continue
      return
      end 
      subroutine saxpy(n,sa,sx,incx,sy,incy)
c
c     overwrite single precision sy with single precision sa*sx +sy.
c
      real sx(1),sy(1),sa
      if(n.le.0.or.sa.eq.0.e0) return
      if(incx.eq.incy) if(incx-1) 5,20,60
    5 continue
c
c        code for nonequal or nonpositive increments.
c
      ix = 1
      iy = 1
      if(incx.lt.0)ix = (-n+1)*incx + 1 
      if(incy.lt.0)iy = (-n+1)*incy + 1 
      do 10 i = 1,n 
        sy(iy) = sy(iy) + sa*sx(ix)
        ix = ix + incx
        iy = iy + incy
   10 continue
      return
c
c        code for both increments equal to 1
c
c
c        clean-up loop so remaining vector length is a multiple of 4. 
c
   20 m = n - (n/4)*4
      if( m .eq. 0 ) go to 40 
      do 30 i = 1,m 
        sy(i) = sy(i) + sa*sx(i)
   30 continue
      if( n .lt. 4 ) return
   40 mp1 = m + 1
      do 50 i = mp1,n,4
        sy(i) = sy(i) + sa*sx(i)
        sy(i + 1) = sy(i + 1) + sa*sx(i + 1)
        sy(i + 2) = sy(i + 2) + sa*sx(i + 2)
        sy(i + 3) = sy(i + 3) + sa*sx(i + 3)
   50 continue
      return
c
c        code for equal, positive, nonunit increments.
c
   60 continue
      ns = n*incx
          do 70 i=1,ns,incx
          sy(i) = sa*sx(i) + sy(i)
   70     continue
      return
      end 
      subroutine  scopy(n,sx,incx,sy,incy)
c
c     copy single precision sx to single precision sy.
c
      real sx(1),sy(1)
      if(n.le.0)return
      if(incx.eq.incy) if(incx-1) 5,20,60
    5 continue
c
c        code for unequal or nonpositive increments.
c
      ix = 1
      iy = 1
      if(incx.lt.0)ix = (-n+1)*incx + 1 
      if(incy.lt.0)iy = (-n+1)*incy + 1 
      do 10 i = 1,n 
        sy(iy) = sx(ix)
        ix = ix + incx
        iy = iy + incy
   10 continue
      return
c
c        code for both increments equal to 1
c
c
c        clean-up loop so remaining vector length is a multiple of 7. 
c
   20 m = n - (n/7)*7
      if( m .eq. 0 ) go to 40 
      do 30 i = 1,m 
        sy(i) = sx(i)
   30 continue
      if( n .lt. 7 ) return
   40 mp1 = m + 1
      do 50 i = mp1,n,7
        sy(i) = sx(i)
        sy(i + 1) = sx(i + 1) 
        sy(i + 2) = sx(i + 2) 
        sy(i + 3) = sx(i + 3) 
        sy(i + 4) = sx(i + 4) 
        sy(i + 5) = sx(i + 5) 
        sy(i + 6) = sx(i + 6) 
   50 continue
      return
c
c        code for equal, positive, nonunit increments.
c
   60 continue
      ns = n*incx
          do 70 i=1,ns,incx
          sy(i) = sx(i)
   70     continue
      return
      end 
      real function sdot(n,sx,incx,sy,incy)
c
c     returns the dot product of single precision sx and sy.
c
      real sx(1),sy(1)
      sdot = 0.0e0
      if(n.le.0)return
      if(incx.eq.incy) if(incx-1)5,20,60
    5 continue
c
c        code for unequal increments or nonpositive increments.
c
      ix = 1
      iy = 1
      if(incx.lt.0)ix = (-n+1)*incx + 1 
      if(incy.lt.0)iy = (-n+1)*incy + 1 
      do 10 i = 1,n 
        sdot = sdot + sx(ix)*sy(iy)
        ix = ix + incx
        iy = iy + incy
   10 continue
      return
c
c        code for both increments equal to 1
c
c
c        clean-up loop so remaining vector length is a multiple of 5. 
c
   20 m = n - (n/5)*5
      if( m .eq. 0 ) go to 40 
      do 30 i = 1,m 
        sdot = sdot + sx(i)*sy(i)
   30 continue
      if( n .lt. 5 ) return
   40 mp1 = m + 1
      do 50 i = mp1,n,5
        sdot = sdot + sx(i)*sy(i) + sx(i + 1)*sy(i + 1) +
     $   sx(i + 2)*sy(i + 2) + sx(i + 3)*sy(i + 3) + sx(i + 4)*sy(i + 4)
   50 continue
      return
c
c        code for positive equal increments .ne.1.
c
   60 continue
      ns=n*incx
      do 70 i=1,ns,incx
        sdot = sdot + sx(i)*sy(i)
   70   continue
      return
      end 
      real function snrm2 ( n, sx, incx)
      integer          next
      real   sx(1),  cutlo, cuthi, hitest, sum, xmax, zero, one
      data   zero, one /0.0e0, 1.0e0/
c
c     euclidean norm of the n-vector stored in sx() with storage
c     increment incx .
c     if    n .le. 0 return with result = 0.
c     if n .ge. 1 then incx must be .ge. 1
c
c           c.l.lawson, 1978 jan 08
c
c     four phase method     using two built-in constants that are
c     hopefully applicable to all machines.
c         cutlo = maximum of  sqrt(u/eps)  over all known machines.
c         cuthi = minimum of  sqrt(v)      over all known machines.
c     where
c         eps = smallest no. such that eps + 1. .gt. 1.
c         u   = smallest positive no.   (underflow limit)
c         v   = largest  no.            (overflow  limit)
c
c     brief outline of algorithm..
c
c     phase 1    scans zero components. 
c     move to phase 2 when a component is nonzero and .le. cutlo
c     move to phase 3 when a component is .gt. cutlo
c     move to phase 4 when a component is .ge. cuthi/m
c     where m = n for x() real and m = 2*n for complex.
c
c     values for cutlo and cuthi..
c     from the environmental parameters listed in the imsl converter
c     document the limiting values are as follows..
c     cutlo, s.p.   u/eps = 2**(-102) for  honeywell.  close seconds are
c                   univac and dec at 2**(-103)
c                   thus cutlo = 2**(-51) = 4.44089e-16
c     cuthi, s.p.   v = 2**127 for univac, honeywell, and dec.
c                   thus cuthi = 2**(63.5) = 1.30438e19
c     cutlo, d.p.   u/eps = 2**(-67) for honeywell and dec. 
c                   thus cutlo = 2**(-33.5) = 8.23181d-11
c     cuthi, d.p.   same as s.p.  cuthi = 1.30438d19
c     data cutlo, cuthi / 8.232d-11,  1.304d19 /
c     data cutlo, cuthi / 4.441e-16,  1.304e19 /
      data cutlo, cuthi / 4.441e-16,  1.304e19 /
c
      if(n .gt. 0) go to 10
         snrm2  = zero
         go to 300
c
   10 assign 30 to next
      sum = zero
      nn = n * incx 
c                                                 begin main loop
      i = 1
   20    go to next,(30, 50, 70, 110)
   30 if( abs(sx(i)) .gt. cutlo) go to 85
      assign 50 to next
      xmax = zero
c
c                        phase 1.  sum is zero
c
   50 if( sx(i) .eq. zero) go to 200
      if( abs(sx(i)) .gt. cutlo) go to 85
c
c                                prepare for phase 2.
      assign 70 to next
      go to 105
c
c                                prepare for phase 4.
c
  100 i = j
      assign 110 to next
      sum = (sum / sx(i)) / sx(i)
  105 xmax = abs(sx(i))
      go to 115
c
c                   phase 2.  sum is small.
c                             scale to avoid destructive underflow.
c
   70 if( abs(sx(i)) .gt. cutlo ) go to 75
c
c                     common code for phases 2 and 4.
c                     in phase 4 sum is large.  scale to avoid overflow.
c
  110 if( abs(sx(i)) .le. xmax ) go to 115
         sum = one + sum * (xmax / sx(i))**2
         xmax = abs(sx(i))
         go to 200
c
  115 sum = sum + (sx(i)/xmax)**2
      go to 200
c
c
c                  prepare for phase 3. 
c
   75 sum = (sum * xmax) * xmax
c
c
c     for real or d.p. set hitest = cuthi/n
c     for complex      set hitest = cuthi/(2*n)
c
   85 hitest = cuthi/float( n )
c
c                   phase 3.  sum is mid-range.  no scaling.
c
      do 95 j =i,nn,incx
      if(abs(sx(j)) .ge. hitest) go to 100
   95    sum = sum + sx(j)**2 
      snrm2 = sqrt( sum )
      go to 300
c
  200 continue
      i = i + incx
      if ( i .le. nn ) go to 20
c
c              end of main loop.
c
c              compute square root and adjust for scaling.
c
      snrm2 = xmax * sqrt(sum)
  300 continue
      return
      end 
      subroutine  sscal(n,sa,sx,incx)
c
c     replace single precision sx by single precision sa*sx.
c
      real sa,sx(1) 
      if(n.le.0)return
      if(incx.eq.1)goto 20
c
c        code for increments not equal to 1.
c
      ns = n*incx
          do 10 i = 1,ns,incx 
          sx(i) = sa*sx(i)
   10     continue
      return
c
c        code for increments equal to 1.
c
c
c        clean-up loop so remaining vector length is a multiple of 5. 
c
   20 m = n - (n/5)*5
      if( m .eq. 0 ) go to 40 
      do 30 i = 1,m 
        sx(i) = sa*sx(i)
   30 continue
      if( n .lt. 5 ) return
   40 mp1 = m + 1
      do 50 i = mp1,n,5
        sx(i) = sa*sx(i)
        sx(i + 1) = sa*sx(i + 1)
        sx(i + 2) = sa*sx(i + 2)
        sx(i + 3) = sa*sx(i + 3)
        sx(i + 4) = sa*sx(i + 4)
   50 continue
      return
      end 
