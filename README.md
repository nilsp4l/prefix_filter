# prefix_filter
Prefix Filter for DSE practical at TUM
## required
linux, gcc, avx and avx-2 support
## tests
Run tests with ```make all test``` in your build directory
## benchmark
In the benchmark directory of your build directory are all the executables you need
## references
```bibtex
@misc{prefix,
      title={Prefix Filter: Practically and Theoretically Better Than Bloom}, 
      author={Tomer Even and Guy Even and Adam Morrison},
      year={2022},
      eprint={2203.17139},
      archivePrefix={arXiv},
      primaryClass={cs.DS},
      url={https://arxiv.org/abs/2203.17139}, 
}
@InProceedings{dietz,
author="Dietzfelbinger, Martin",
editor="Puech, Claude
and Reischuk, R{\"u}diger",
title="Universal hashing and k-wise independent random variables via integer arithmetic without primes",
booktitle="STACS 96",
year="1996",
publisher="Springer Berlin Heidelberg",
address="Berlin, Heidelberg",
pages="567--580",
abstract="Let u, m≥1 be arbitrary integers and let k≥u. The central result of this paper is that the multiset H={\{}itha,b{\textbrokenbar}0≤a, b<km{\}} of functions from U=0,..., u {\}}-1 to M={\{}0,..., m −1, where ha,b(x)=((ax+b) mod km) div k, for x∈ U, is a (c, 2)-universal class of hash functions from U to M in the sense of Carter and Wegman [7, 25], with c=5/4. More precisely, we show that if x1, x2 are distinct elements of U and i1,i2∈ M are arbitrary, and if h is chosen at random from H, then {\textbrokenbar}Prob (h(x1)=i1 ∧ h(x2)=i2-1/m2{\textbrokenbar}≤(1/2km)2≤1/4m2. Among the many known constructions of (c, 2)-universal classes there was none that would get by with such a small number of pure integer arithmetic operations without the assumption that a prime number of size the order of{\textbrokenbar}U{\textbrokenbar} or at least {\textbrokenbar}M{\textbrokenbar} was available. --- Varying this result, we obtain: (a) two-independent sequences of random variables; (b) universal hash classes of higher degree (``(c, l)-universal'' classes) and l-wise independent random variables, for l ≥ 2; (c) algorithms for static and dynamic perfect hashing with an optimal number of random bits; all using pure integer arithmetic without the need for providing prime numbers (arbitrary or random) of a certain size. It should be noted that the focus here is not on minimizing the size of the probability space used, as in much of the recent work on ``almost k-independent random variables'', but on the realization of such variables or hash classes using the most natural and most widely available operations, viz., integer arithmetic.",
isbn="978-3-540-49723-3"
}
```
