Monte Carlo Methods applied to the Black-Scholes financial market model
============
### Table of contents
[toc]


## Overview
### Black-Scholes Model
The [Black-Scholes model][Black-Scholes Model], which was first published by Fischer Black and Myron Scholes in 1973, is a famous and basic mathematical model describing the behaviour of investment instruments in financial markets. This model focuses on comparing the Return On Investment for one risky asset, whose price is subject to [geometric Brownian motion][geometric Brownian motion] and one riskless asset with a fixed interest rate. 

The geometric Brownian behaviour of the price of the risky asset is described by this stochastic differential equation:

![$$dS=rSdt+\sigma SdW_t$$]

where S is the price of the risky asset (usually called stock price), r is the fixed interest rate of the riskless asset, ![$\sigma$] is the volatility of the stock and ![$W_t$] is a [Wiener process][Wiener process].


According to Ito's Lemma, the analytical solution of this stochastic  differential equation is as follows:


![$$ S_{t+\Delta t}=e^{(r-\frac{1}{2}\sigma^2)\Delta t+\sigma\epsilon\sqrt{\Delta t} } $$]

where ![$\epsilon\sim N(0,1)$],  (the standard normal distribution).

### Call/Put Option
Entering more specifically into the financial sector operations, two styles of stock transaction [options][option] are considered in this project, namely the European vanilla option and Asian option (which is one of the [exotic options][exotic options]). 
[Call options][Call options] and [put options][put options] are defined reciprocally. Given the basic parameters for an option, namely expiration date and strike price, the call/put payoff price could be estimated as follows. 
For the European vanilla option, we have:

![$$P_{Call}=max\{S-K,0\}\\P_{put}=max\{K-S,0\}$$]

where S is the stock price at the expiration date (estimated by the model above) and K is the strike price.
For the Asian option, we have:

![$$P_{Call}=max\{\frac{1}{T}\int_0^TSdt-K,0\}\\P_{put}=max\{K-\frac{1}{T}\int_0^TSdt,0\}$$]

where T is the time period (between now and the option expiration date) , S is the stock price at the expiration date, and K is the strike price.

[option]: https://en.wikipedia.org/wiki/Option_style
[exotic option]: https://en.wikipedia.org/wiki/Exotic_option

### The Monte Carlo Method
The [Monte Carlo Method][Monte Carlo] is one of the most widely used approaches to simulate stochastic processes, like a stock price modeled with Black-Scholes. This is especially true for exotic options, which are usually not solvable analytically. In this project, the Monte Carlo Method is used to estimate the payoff price of a given instrument using the Black Scholes model. 

The given time period has to be partitioned into M steps according to the style of the option. M=1 for a European option, since the payoff price is independent of the price before the expiration date. At each time point of a Monte Carlo simulation of this kind, the stock price is determined by the stock price at the previous time point and by a normally distributed random number. The expectation of the payoff price can thus be estimated by N parallel independent simulations.

The convergence of the result produced by the Monte Carlo method is ensured in this case by running a very large number of simulation steps, namely ![$C=M \cdot N$], (which should be a very large number, e.g. ![$10^9$]). 
Other convergence criteria (e.g. checking the difference between successive iterations) could be added.


### Normally Distributed Random Number Generation
A key aspect of the quality of the results of the Monte Carlo method is the quality of the random numbers that it uses. The normally distributed random numbers that are used in this project are generated by the Mersenne-Twister algorithm followed by the Box-Muller transformation. 

#### Mersenne-Twister

The [Mersenne Twister][Mersenne Twister] is an algorithm to generate uniformly distributed pseudo random numbers. Its very long periodicity ![$2^{19937}-1$] makes it a suitable algorithm for our application, since as discussed above the Monte Carlo method requires millions of random numbers.


#### Box-Muller transform
The [Box Muller transformation][Box Muller transformation] transforms a pair of independent, uniformly distributed random numbers in the interval (0,1) into a pair of independent normally distributed random numbers, which are required for simulating the Black Scholes model.
Given two independent ![$U_1$,$U_2 \sim U(0,1)$], 

![$$Z_1=\sqrt{-2ln(U_1)}cos(2\pi U_2)\\Z_2=\sqrt{-2ln(U_1)}sin(2\pi U_2)$$]

then ![$Z_1$,$Z_2\sim N(0,1)$], also independent.





## Getting Started 

The repository contains two directories, "blackEuro" and "blackAsian", implementing the European option and the Asian option respectively. They are written using C++, rather than OpenCL, because there is no workgroup-level memory that is worth sharing. All Monte Carlo simulations are independent.
In this implementation, since the complexity of the random number generation process is simpler than the complexity of the Monte Carlo simulation step, 1 random number generator feeds a group of NUM_SIMS simulations, by utilizing BRAMs storing the intermediate results. The iterations over NUM_RNGS are fully unrolled (as if they were executed by an independent work item in a work group) and the iterations over NUM_SIMS and NUM_SIMGROUPS are pipelined. 
These two parameters should be chosen to fully utilize the resources on an FPGA.
In order to ensure that enough simulations of a given stock are performed, NUM_SIMGROUPS can then be tuned.

The total number of simulations is ![$N=NUM\_SIMS \cdot NUM\_RNG \cdot NUM\_SIMGROUPS$] and each simulation group assigned to a given RNG runs NUM_SIMS simulations. 
The best value for NUM_SIMS can be chosen by maximizing the use of BRAM blocks.  By default it is 512 (which fills one BRAM block with 512 float values).

### File Tree

```
blackScholes
│   README.md
│
└── common
│   │   defTypes.h
│   │   RNG.h
│   │   RNG.cpp
│   │   stockData.h
│   │   stockData.cpp
│   └─  ML_cl.h
│
└── blackEuro
│   │   solution.tcl
│   │   blackEuro.h
│   │   blackEuro.cpp
│   │   main.cpp
│   │   blackScholes.h
│   └─  blackScholes.cpp
│
└── blackAsian
    │   solution.tcl
    │   blackAsian.h
    │   blackAsian.cpp
    │   testBench.h
    │   main.cpp
    │   blackScholes.h
    └─  blackScholes.cpp
```

File/Dir name  |Information
-------------- | ---
blackEuro.cpp  | Top function of the European option kernel
blackAsian.cpp | Top function of the Asian option kernel
solution.tcl   | Script to run sdaccel
blackScholes.h | It declares the blackScholes object instantiated in the top functions (same methods for European and Asian option).
blackScholes.cpp | It defines the blackScholes object instantiated in the top functions. Note that the definitions of the object methods are different between the European And Asian options.
stockData.cpp	 | Basic stock datasets. It defines an object instantiated in the top functions
RNG.cpp   | Random Number Generator class. It defines an object instantiated in the blackSholes objects.
main.cpp  |  Host code calling the kernels
testBench.h | Input parameters for the kernels
ML_cl.h | CL/cl.hpp for OpenCL 1.2.6

Note that in the repository we had to include the OpenCL header for version 1.2.6, instead of the version 1.1 installed by sdaccel, because the latter causes compile-time errors. SDAccel and Vivado HLS work perfectly well with this header. See figure ![alt text][clerror]

[clerror]: /figures/header_failure.PNG

### Parameters
The values of the parameters for a given stock and option are listed in ***"testBench.h"***. 

Parameter |  information
:-------- | :---
T	       |  time period
rate       |  interest rate of riskless asset
volatility |  volatility of the risky asset (stock)
S0		   |  initial price of the stock
K          |  strike price for the option
kernel_name | the kernel name, to be passed to the OpenCL runtime

The number of simulations N, and the number of time partitions M, as well as all the other parameters related to the simulation, are listed in ***"blackScholes.cpp"***

Parameter |  information
:-------- | :---
NUM_STEPS    | number of time steps (M)
NUM_RNGS | number of RNGs running in parallel, proportional to the area cost
NUM_SIMGROUPS  | number of simulation groups (each with ![$NUM\_RNG \cdot NUM\_SIMS$] simulations) running in pipeline, proportional to the execution time
NUM_SIMS   | number of simulations running in parallel for a given RNG (512 optimizes BRAM usage)

The area cost is proposrtional to NUM_RNG.
### How to run an example
In each sub-directory, there is a script file called "solution.tcl". It can be used as follows:

`  sdaccel solution.tcl

The result of the call/put payoff price estimation will be printed on standard IO.

Due to bugs in SDAccel, the kernel (written in C++) cannot be emulated on the CPU right now (see the figure below). Only RTL simulation is available. However, note that RTL simulation takes a very long time. In order to obtain (imprecise) results quickly, the computation cost C can be reduced. For instance, NUM_SIMGROUPS has been set to 2 for the Asian option.

![alt text](/figures/CPU_emulation.PNG)

### Sample Output
For the European option:

Input parameter |  value
:-------- | :---
T| 1
S0  | 100
K 	| 110
rate    |  5%
volatility | 20%
NUM_RNGS | 8
NUM_SIMS  | 512
NUM_SIMGROUPS  | 4
NUM_STEPS | 1

Output |  value
:-------- | :--- 
call price| 6.048
put price | 10.65

For the Asian option,

Input parameter |  value
:-------- | :---
T| 10
S0  | 100
K 	| 105
rate    |  1%
volatility | 15%
NUM_RNGS | 2
NUM_SIMS  | 64 (to keep RTL simulation under control; it should ideally be 512)
NUM_SIMGROUPS  | 2
NUM_STEPS	| 128

Output |  value
:-------- | :--- 
call price| 24.86
put price | 0.33

## Performance Metrics


As discussed above, the computational cost ![$C=M \cdot N$] is a key factor that affects both the performance of the simulation and the quality of the result. The time complexity of the algorithm is O(C), so that we analyze the performance of our implementation as the total simulation time (number of clock cycles times clock period) per step: ![$t=T_s/C$]


The time taken by the algorithm is ![$$T=\alpha M \cdot N+\beta N+\gamma M+\theta$$] so for each step, ![$$t=T/C\approx\alpha$$] 

**Basic Simulation procedure:** 

>-  **Outer loop** (N iterations in total)

>>- **Inner loop** (M iterations)

>>> - Generate random numbers (unrolled loop)
>>> - Estimate stock price at each time partition point 

>> - Calculate the payoff price 
> - Count the sum of payoff prices
> 
- Estimate the average 

We can see that ![$\alpha$] is related to the latency of the inner loop. Since each iteration of the inner loop requires random numbers, one of factors that limit the latency is the latency of generating a random number. The other factor are the mathematical operations of one Black Sholes step. 

At frequencies below 100MHz on modern FPGAs, two random numbers are produced every two clock cycles (pipeline with Initiation Interval 2). By considering also the unrolling factor NUM_RNGS, the time for each step on the FPGA ![$t\approx\frac{clock\ period}{NUM\_RNGS}$]. For instance, at the frequency of 100MHz with NUM_RNGS=8, ![$t\approx1.25ns$]

[Black-Scholes Model]: https://en.wikipedia.org/wiki/Black%E2%80%93Scholes_model 
[geometric Brownian motion]: https://en.wikipedia.org/wiki/Geometric_Brownian_motion	
[Wiener process]: https://en.wikipedia.org/wiki/Wiener_process
[Call options]: https://en.wikipedia.org/wiki/Call_option
[put options]: https://en.wikipedia.org/wiki/Put_option
[Mersenne Twister]: https://en.wikipedia.org/wiki/Mersenne_Twister
[Monte Carlo]: https://en.wikipedia.org/wiki/Monte_Carlo_method  
[Box Muller transformation]: https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform


[$\alpha$]:/figures/alpha.PNG
[$N=NUM\_SIMS \cdot NUM\_RNG \cdot NUM\_SIMGROUPS$]:figures/N.PNG
[$\sigma$]:figures/equ_bs.PNG
[$W_t$]:figures/wt.PNG
[$C=M \cdot N$]:figures/cmn.PNG
[$10^9$]:figures/109.PNG
[$2^{19937}-1$]:figures/19937.PNG
[$U_1$,$U_2 \sim U(0,1)$]:figures/u12.PNG
[$Z_1$,$Z_2\sim N(0,1)$]:figures/z12.PNG
[$t\approx1.25ns$]:figures/t125.PNG
[$NUM\_RNG \cdot NUM\_SIMS$]:figures/nn.PNG
[$t=T_s/C$]:figures/tstep.PNG
[$$Z_1=\sqrt{-2ln(U_1)}cos(2\pi U_2)\\Z_2=\sqrt{-2ln(U_1)}sin(2\pi U_2)$$]:/figures/boxm.PNG
[$t\approx\frac{clock\ period}{NUM\_RNGS}$]:/figures/tpro.PNG
[$$t=T/C\approx\alpha$$]:/figures/tmall.PNG
[$$dS=rSdt+\sigma SdW_t$$]:/figures/equ_bs.PNG
[$$ S_{t+\Delta t}=e^{(r-\frac{1}{2}\sigma^2)\Delta t+\sigma\epsilon\sqrt{\Delta t} } $$]:/figures/ito.PNG
[$\epsilon\sim N(0,1)$]:/figures/eps.PNG
[$$P_{Call}=max\{S-K,0\}\\P_{put}=max\{K-S,0\}$$]:/figures/euro.PNG
[$$P_{Call}=max\{\frac{1}{T}\int_0^TSdt-K,0\}\\P_{put}=max\{K-\frac{1}{T}\int_0^TSdt,0\}$$]:/figures/asian.PNG
[$$T=\alpha M \cdot N+\beta N+\gamma M+\theta$$]:/figures/tall.PNG