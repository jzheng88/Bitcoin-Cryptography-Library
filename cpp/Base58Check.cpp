/* 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#include <cassert>
#include <cstring>
#include "Base58Check.hpp"
#include "Sha256.hpp"
#include "Sha256Hash.hpp"
#include "Utils.hpp"



/*---- Public and private functions for bytes-to-Base58 conversion ----*/

void Base58Check::pubkeyHashToBase58Check(const uint8_t pubkeyHash[RIPEMD160_HASH_LEN], char outStr[35]) {
	assert(pubkeyHash != nullptr && outStr != nullptr);
	uint8_t toEncode[1 + RIPEMD160_HASH_LEN + 4] = {};
	toEncode[0] = 0x00;  // Version byte
	memcpy(&toEncode[1], pubkeyHash, RIPEMD160_HASH_LEN);
	bytesToBase58Check(toEncode, sizeof(toEncode) - 4, outStr);
}


void Base58Check::privateKeyToBase58Check(const Uint256 &privKey, char outStr[53]) {
	assert(outStr != nullptr);
	uint8_t toEncode[1 + 32 + 1 + 4] = {};
	toEncode[0] = 0x80;  // Version byte
	privKey.getBigEndianBytes(&toEncode[1]);
	toEncode[33] = 0x01;  // Compressed marker
	bytesToBase58Check(toEncode, sizeof(toEncode) - 4, outStr);
}


void Base58Check::bytesToBase58Check(uint8_t *data, size_t dataLen, char *outStr) {
	// Append 4-byte hash
	#define MAX_TOTAL_BYTES 38  // Including the 4-byte hash
	assert(data != nullptr && dataLen <= MAX_TOTAL_BYTES - 4 && outStr != nullptr);
	const Sha256Hash sha256Hash = Sha256::getDoubleHash(data, dataLen);
	for (int i = 0; i < 4; i++, dataLen++)
		data[dataLen] = sha256Hash.value[i];
	
	// Count leading zero bytes
	size_t leadingZeros = 0;
	while (leadingZeros < dataLen && data[leadingZeros] == 0)
		leadingZeros++;
	
	// Encode to Base 58
	size_t outLen = 0;
	while (!isZero(data, dataLen)) {  // Extract digits in little-endian
		outStr[outLen] = ALPHABET[mod58(data, dataLen)];
		outLen++;
		uint8_t quotient[MAX_TOTAL_BYTES] = {};
		divide58(data, quotient, dataLen);  // quotient = floor(data / 58)
		Utils::copyBytes(data, quotient, dataLen);  // data = quotient
	}
	for (size_t i = 0; i < leadingZeros; i++) {  // Append leading zeros
		outStr[outLen] = ALPHABET[0];
		outLen++;
	}
	outStr[outLen] = '\0';
	
	// Reverse the string
	if (outLen == 0)
		return;  // Exit early to ensure that j does not overflow
	for (size_t i = 0, j = outLen - 1; i < j; i++, j--) {
		char temp = outStr[i];
		outStr[i] = outStr[j];
		outStr[j] = temp;
	}
	#undef MAX_TOTAL_BYTES
}


bool Base58Check::isZero(const uint8_t *x, size_t len) {
	assert(len == 0 || x != nullptr);
	for (size_t i = 0; i < len; i++) {
		if (x[i] != 0)
			return false;
	}
	return true;
}


uint8_t Base58Check::mod58(const uint8_t *x, size_t len) {
	assert(len == 0 || x != nullptr);
	uint_fast16_t sum = 0;
	for (size_t i = 0; i < len; i++)
		sum = ((sum * 24) + x[i]) % 58;  // Note: 256 % 58 = 24
	return static_cast<uint8_t>(sum);
}


void Base58Check::divide58(const uint8_t *x, uint8_t *y, size_t len) {
	assert(x != nullptr && y != nullptr);
	memset(y, 0, len);
	uint_fast16_t dividend = 0;
	for (size_t i = 0; i < len * 8; i++) {  // For each output bit
		dividend = (dividend << 1) | ((x[i >> 3] >> (7 - (i & 7))) & 1);  // Shift next input bit into right side
		if (dividend >= 58) {
			dividend -= 58;
			y[i >> 3] |= 1 << (7 - (i & 7));
		}
	}
}



/*---- Public and private functions for Base58-to-bytes conversion ----*/

bool Base58Check::pubkeyHashFromBase58Check(const char *addrStr, uint8_t outPubkeyHash[RIPEMD160_HASH_LEN]) {
	// Preliminary checks
	assert(addrStr != nullptr && outPubkeyHash != nullptr);
	if (strlen(addrStr) < 1 || strlen(addrStr) > 34 || addrStr[0] != '1')
		return false;
	
	// Perform Base58 decoding
	uint8_t decoded[1 + RIPEMD160_HASH_LEN + 4];
	if (!base58CheckToBytes(addrStr, decoded, sizeof(decoded) / sizeof(decoded[0])))
		return false;
	
	// Check format byte
	if (decoded[0] != 0x00)
		return false;
	
	// Successfully set the output
	memcpy(outPubkeyHash, &decoded[1], RIPEMD160_HASH_LEN * sizeof(uint8_t));
	return true;
}


bool Base58Check::privateKeyFromBase58Check(const char wifStr[53], Uint256 &outPrivKey) {
	// Preliminary checks
	assert(wifStr != nullptr);
	if (strlen(wifStr) != 52 || (wifStr[0] != 'L' && wifStr[0] != 'K'))
		return false;
	
	// Perform Base58 decoding
	uint8_t decoded[1 + 32 + 1 + 4];
	if (!base58CheckToBytes(wifStr, decoded, sizeof(decoded) / sizeof(decoded[0])))
		return false;
	
	// Check format bytes
	if (decoded[0] != 0x80 || decoded[33] != 0x01)
		return false;
	
	// Successfully set the value
	outPrivKey = Uint256(&decoded[1]);
	return true;
}


bool Base58Check::base58CheckToBytes(const char *inStr, uint8_t *outData, size_t outDataLen) {
	assert(inStr != nullptr && outData != nullptr && outDataLen >= 4);
	
	// Convert from Base 58 to base 256
	memset(outData, 0, outDataLen * sizeof(outData[0]));
	for (size_t i = 0; inStr[i] != '\0'; i++) {
		if (multiply58(outData, outDataLen))
			return false;
		const char *p = strchr(ALPHABET, inStr[i]);
		if (p == nullptr)
			return false;
		if (addUint8(outData, p - &ALPHABET[0], outDataLen))
			return false;
	}
	
	// Verify number of leading zeros
	for (size_t i = 0; ; i++) {
		if (inStr[i] != '1' && (i >= outDataLen || outData[i] != 0))
			break;  // Success
		else if (inStr[i] == '1' && i < outDataLen && outData[i] == 0)
			continue;  // Keep scanning
		else
			return false;  // Mismatch
	}
	
	// Compute and check hash
	const Sha256Hash sha256Hash = Sha256::getDoubleHash(outData, outDataLen - 4);
	for (int i = 0; i < 4; i++) {
		if (outData[outDataLen - 4 + i] != sha256Hash.value[i])
			return false;
	}
	return true;
}


bool Base58Check::addUint8(uint8_t *x, uint8_t y, size_t len) {
	assert(len >= 1 && x != nullptr);
	uint_fast16_t carry = 0;
	for (size_t i = len - 1; ; i--) {
		uint_fast16_t sum = static_cast<uint_fast16_t>(x[i]) + carry;
		assert(0 <= sum && sum < 512);
		if (i == len - 1)
			sum += y;
		x[i] = static_cast<uint8_t>(sum);
		carry = sum >> 8;
		assert((carry >> 1) == 0);
		if (i == 0)
			break;
	}
	return carry > 0;
}


bool Base58Check::multiply58(uint8_t *x, size_t len) {
	assert(len >= 1 && x != nullptr);
	uint_fast16_t carry = 0;
	for (size_t i = len - 1; ; i--) {
		uint_fast16_t temp = static_cast<uint_fast16_t>(x[i]) * 58 + carry;
		x[i] = static_cast<uint8_t>(temp);
		carry = temp >> 8;
		assert(0 <= carry && carry < 58);
		if (i == 0)
			break;
	}
	return carry > 0;
}



/*---- Miscellaneous definitions ----*/

Base58Check::Base58Check() {}


// Static initializers
const char *Base58Check::ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
