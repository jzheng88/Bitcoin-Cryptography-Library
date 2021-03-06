/* 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#pragma once

#include "CurvePoint.hpp"
#include "Sha256Hash.hpp"
#include "Uint256.hpp"


/* 
 * Performs ECDSA signature generation and verification.
 * Provides just three static methods.
 */
class Ecdsa final {
	
public:
	
	// Computes the signature (deterministically) when given the private key, message hash, and random nonce.
	// Returns true if signing was successful (overwhelming probability), or false if a new nonce must be chosen
	// (vanishing probability). Both privateKey and nonce must be in the range [1, CurvePoint::ORDER).
	// outR and outS will be in the same range too; their values are assigned iff signing is successful.
	// Note: The nonce must be unique, unpredictable, and secret. Otherwise the signature may leak the private key.
	// All successful executions are constant-time with respect to the input values; in order words
	// one successful execution is indistinguishable from another one based on side channel information.
	static bool sign(const Uint256 &privateKey, const Sha256Hash &msgHash, const Uint256 &nonce, Uint256 &outR, Uint256 &outS);
	
	
	// Computes a deterministic nonce based on the HMAC-SHA-256 of the message hash with the private key,
	// and then performs ECDSA signing. Returns true iff signing is successful (with overwhelming probability).
	static bool signWithHmacNonce(const Uint256 &privateKey, const Sha256Hash &msgHash, Uint256 &outR, Uint256 &outS);
	
	
	// Checks whether the given signature, message, and public key are valid together. The public key point must be normalized.
	static bool verify(const CurvePoint &publicKey, const Sha256Hash &msgHash, const Uint256 &r, const Uint256 &s);
	
	
private:
	
	// Computes x = (x * y) % CurvePoint::ORDER. Requires x < CurvePoint::ORDER, but y is unrestricted.
	static void multiplyModOrder(Uint256 &x, const Uint256 &y);
	
	
	Ecdsa();  // Not instantiable
	
};
