## BDT reweighting

based on:

- https://github.com/arogozhnikov/hep_ml
- https://github.com/dguest/mlbnn

reading material:

- https://arxiv.org/abs/1608.05806
- http://arogozhnikov.github.io/2015/10/09/gradient-boosted-reweighter.html


### How to use this

1. Generate TRUTH1 DxAODs for the desired signal points: one for the original signal point and one for the target signal point. Take care that a sufficient number of events is generated (at least 200.000) per point.

2. Use the `dump-xaod` utility to write the truth particle information to a HDF5 file. This is done in c++ to achieve good performance. This part of the code lives in `atlas-sw` and is based on dguest's code.

3. Use the `reweight.py` script to perform the gradient boosted reweighting. The idea is to use privately generated samples for the training and then to run over the information of the simulated + reconstructed official sample to append the weights to the DxAOD file.


### General remark

It is always a good idea to reweight from a densely populated part of the phase space of one signal point to a less densely or equally densely populated phase space and not the other way around. In other words: if you are reweighting using events from the tail of a distribution to model the bulk of another distribution you are going to have a bad time. Or put simply: always reweight from the high MET sample to the low MET sample.
