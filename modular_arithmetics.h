#ifndef MODULAR_ARITHMETICS_H
#define MODULAR_ARITHMETICS_H



bignum inverseByMod(const bignum & a, const bignum & mod);

bignum alignByMod(bignum num, const bignum & mod);

bignum powByMod(const bignum & num, const bignum & pow, const bignum & mod);

bignum fastPowByMod(const bignum & num, const bignum & pow, const bignum & mod);

bignum alignByPow2FromMod(const bignum & num, unsigned int pow2FromMod);

bignum negInvMod_Montgomery(const bignum & mod, const bignum & modR);

bignum RPow2ByMod_Montgomery(const bignum & modR, const bignum & mod);

bignum reductionByMod_Montgomery(const bignum & num, const bignum & mod, const bignum & modR, const bignum & mu);

bignum pseudomultiplicationByMod_Montgomery(const bignum & num1, const bignum & num2, const bignum & mod, const bignum & modR, const bignum & mu);

bignum powByMod_Montgomery(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu);

bignum pseudomultiplicationByMod_Montgomery_base256(const bignum & num1, const bignum & num2, const bignum & mod, const bignum & modR, const bignum & mu);

bignum powByMod_Montgomery_experimental(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu);

bignum powByMod_Montgomery_experimental15(const bignum & prepNum, const bignum & exp, const bignum & prepR, const bignum & mod, const bignum & modR, const bignum & mu);

#endif