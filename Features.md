OSCATS currently supports the following features:

  * Object-oriented, modular, extensible
  * IRT Models:
    * 1PL (Rasch), 2PL, 3PL
    * Nominal Response
    * Partial Credit, Generalized Partial Credit
    * Graded Response (Homogenous and Heterogenous Logistic)
  * Classification (Cognitive Diagnosis) Models:
    * DINA
    * NIDA
  * Item Selection Algorithms:
    * Random
    * Optimize an arbitrary criterion
    * Pick item with closest difficulty
    * Maximize Fisher Information
    * Maximize Kullback-Leibler Information
    * a-Stratified
  * Stopping Criteria:
    * Fixed length
  * Statistics:
    * Estimate latent ability by MLE or EAP
    * Item exposure
    * Classification rates
  * Support for examinee covariates
  * Support for tagging item characteristics

The following features are planned for version 1.0 (features in brackets have
been implemented in the Mercurial repository):

  * Models:
    * Generalized DINA
    * Fusion, RUM
  * Item Selection Algorithms:
    * Dual-purpose item selection (IRT and Classification)
    * Test blueprints
    * Non-statistical selection constraints
  * Exposure control algorithms:
    * Sympson-Hetter
    * Exposure-control threshold
    * Multiple Item Pools
  * Stopping Criteria:
    * Estimation error
  * R bindings
  * Matlab bindings
  * Support writing new models/algorithms in bound languages
  * Example of integration with [Concerto](http://code.google.com/p/concerto-platform/)

Ideas for features to be implemented at some point in the future:

  * Classification Models:
    * Generalized NIDA
    * Reduced Reparameterized Unifed Model
    * Additive General Diagnostic Model
  * XML format for describing tests
  * GUI front-end for building simulations
  * Item parameter estimation utility (MMLE, MCMC, MHRM)
  * Online item calibration
  * Bindings for Octave