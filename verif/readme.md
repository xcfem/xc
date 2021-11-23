XC verification tests.
======================

The file «run_verif.sh» is a shell script that launchs the execution of
some (about 800) verification tests.

To execute these test you may assign execute permission to the file:

chmod a+x run_verif.sh

and run it:

./run_verif.sh

If you prefer to get rid of the 'ok' messages you can type:

./run_verif.sh | grep -v ok

Of course you can also learn how to work with XC by examining those tests.


## References

- [Minimal working example](https://portwooddigital.com/2021/07/01/minimal-working-example/)
- [Verifying Ain’t Easy](https://portwooddigital.com/2021/07/11/verifying-aint-easy/)
