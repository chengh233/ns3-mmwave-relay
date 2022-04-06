# Out of band backhaul feature for the ns-3 mmWave module #
Out of band mmWave backhaul for 5G cellular network is a potential solution to the severe backhaul challenge introduced by the dense small-cell deployment in urban environment. 

In the mmWave out-of-band backhaul, the backhaul and access tiers of the network use different frequency bands to transmit their data. The whole time resource could be allocated to both backhaul and access.

In this module, we modify and extend the current IAB module to enable the simulation of out-of-band backhaul. It can currently simulate some simple scenarios as depicted in the scripts. We are still working on some other features of this module.

Note that, we will develop a dedicated out-of-band backhaul module based on the 5G NR module developed by CTTC later.

If you have any questions related to this module, please email us at qianghu@gatech.edu or hqfrank@gmail.com.

# Usage

```
./waf configure --disable-python --enable-examples && ./waf build

# redirect output to the file "output.log" and redirect error message to stander output
./waf --run scratch/mmwave-replay-path > output.log 2>&1

```

# IAB feature for the ns-3 mmWave module #
Integrated Access and Backhaul (IAB) is a 3GPP Study Item on the possibility of using the same network equipment and resources for both access and backhaul.

This extension of the [ns-3 mmWave module](https://github.com/nyuwireless-unipd/ns3-mmwave "ns-3 mmWave repo") adds wireless relaying capabilities to an ns-3 NetDevice, and the possibility of simulating in-band relaying at mmWave frequencies. This framework has been developed by the University of Padova as part of a project with InterDigital Communications. It is still a _work in progress_, and more functionalities will be added soon.

Please refer to [our paper](/ "IAB") for a complete description of the model.

# Relevant papers on IAB #
- [M. Polese, M. Giordani, A. Roy, D. Castor, M. Zorzi, "Distributed Path Selection Strategies for Integrated Access and Backhaul at mmWaves", IEEE Global Communications Conference (GLOBECOM), 2018](https://arxiv.org/abs/1805.04351 "globecom paper")
- [M. Polese, M. Giordani, A. Roy, S. Goyal, D. Castor, M. Zorzi, "End-to-End Simulation of Integrated Access and Backhaul at mmWaves", IEEE CAMAD, 2018](https://arxiv.org/abs/1808.00376 "camad paper")

-----------------

## mmWave ns-3 module ##

This repository builds on the [ns-3](https://www.nsnam.org "ns-3 Website") mmWave module for the simulation of 5G mmWave cellular networks. A description of this module can be found on [IEEExplore (open access)](https://ieeexplore.ieee.org/document/8344116/ "mmwave paper").

## Documentation and examples for ns-3 mmWave ##

For a complete description and examples, refer to:

- [End-to-End Simulation of 5G mmWave Networks](https://ieeexplore.ieee.org/document/8344116/ "comst paper")
- [A Framework for End-to-End Evaluation of 5G mmWave Cellular Networks in ns-3](https://arxiv.org/abs/1602.06932 "wns3 paper")
- [ Performance Comparison of Dual Connectivity and Hard Handover for LTE-5G Tight Integration](https://arxiv.org/abs/1607.05425 "simutools paper")
- [ns-3 manual](https://www.nsnam.org/docs/manual/html "ns-3 Manual")

## License ##

This software is licensed under the terms of the GNU GPLv2, as like as ns-3. See the LICENSE file for more details.
