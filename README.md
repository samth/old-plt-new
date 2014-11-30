This repository contains an archive of Racket history from inital use
of CVS in 1996 to splitting the repository in 2014.

In order to preserve SHA references the master branch has the history
of `plt/racket`. Therefore, to see the full history, use the following
graft.

e41b2fb359d69543359c625e130f01456974a8cb a580d75deae2a0d2f3d8a93bc3c4f8f1f619b5b7

Furthermore, the resulting repository has two commits without parents,
due to issues with cvs2git. Therefore, we create an empty commit to
serve as a parent for both.

Running `graft.sh` in this directory will add these grafts.

The resulting history was used for splitting the Racket repository.
