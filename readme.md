# My Thesis' Repository
### Abstract
Modern-day cloud solutions face big security challenges. Homomorphic Encryption (HE) schemes can alleviate some of the underlying security problems by preserving the
Integrity and the Confidentiality of the data and, thus, it is claimed that they can radically change the way that traditional cloud applications work. Microsoft SEAL is 
currently the most popular Homomorphic Encryption library, due to its ease of use and performance. The most important problem of all Fully Homomorphic Encryption schemes 
(FHEs) is their high processing demands and the corresponding high latency per involved computation. In this Thesis, the acceleration of one widely used such scheme is 
presented, included in the SEAL library, (i.e. CKKS) on a state-of-the-art Xilinx Data Accelerator Card. The presented module achieves a speedup of almost two orders of 
magnitude when compared with the execution of the same scheme in a modern CPU, while achieving notable results in comparison with existing GPU-based implementations in 
terms of both performance and/or power efficiency. This can be considered as a significant step towards porting, and thus accelerating, the most computationally 
intensive parts of the SEAL library to reconfigurable devices. In this Thesis, a small but rather costly part of the SEAL library is accelerated using the Alveo U200
Data Accelerator Card. To specify which function/operation should be accelerated, Intel® VTune™ Profiler was used for the analysis of the CKKS example that Microsoft 
SEAL provides, using an i5-9600k system. This methodlogy, determined the most costly function, which is divide_uint128_uint64_inplace_generic. The entire library is run at the host (PC) side, 
except for the costly operation of division, which is executed at the source (accelerator) side. By doing this, a X92.66 speedup was achieved, thus accelerating this costly 
operation. 
