// Copyright (c) 2017  Phil Leblanc  -- see LICENSE file
// ---------------------------------------------------------------------

/*

luanacha

This is a Lua library wrapping the Monocypher library by Loup Vaillant.
http://loup-vaillant.fr/projects/monocypher/

The functions keep as much as possible the same name as in  Monocypher 
(without the "crypto_" prefix)

luanachaAPI:

randombytes(n)
	return a string containing n random bytes
	
--- Authenticated encryption

ae_lock
	authenticated encryption
	with an optional prefix prepended to the encrypted text

ae_unlock
	authenticated decryption
	with an optional offset for the start of the encrypted text

--- Curve25519-based key exchange

x25519_keypair
	generates a pair of curve25519 keys (secret key, public key)

x25519_public_key
	return the public key associated to a secret key

lock_key
	DH key exchange. Return a session key

--- Blake2b cryptographic hash

blake2b_init
	initialize and return a blake2b context object

blake2b_update
	update the hash with a new text fragment

blake2b_final
	return the final value of the hash

blake2b
	compute the hash of a string (convenience function)



--- Ed25519 signature

ed25519_keypair
	generates a pair of ed25519 signature keys (secret key, public key)

ed25519_public_key
	return the public key associated to a secret key

ed25519_sign
	sign a text with a secret key

ed25519_check
	check a text signature with a public key

---

Links:
https://en.wikipedia.org/wiki/BLAKE_%28hash_function%29

*/

#define LUANACHA_VERSION "luanacha-0.1"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "monocypher.h"

//----------------------------------------------------------------------
// compatibility with Lua 5.2  --and lua 5.3, added 150621
// (from roberto's lpeg 0.10.1 dated 101203)
//
#if (LUA_VERSION_NUM >= 502)

#undef lua_equal
#define lua_equal(L,idx1,idx2)  lua_compare(L,(idx1),(idx2),LUA_OPEQ)

#undef lua_getfenv
#define lua_getfenv	lua_getuservalue
#undef lua_setfenv
#define lua_setfenv	lua_setuservalue

#undef lua_objlen
#define lua_objlen	lua_rawlen

#undef luaL_register
#define luaL_register(L,n,f) \
	{ if ((n) == NULL) luaL_setfuncs(L,f,0); else luaL_newlib(L,f); }

#endif

//----------------------------------------------------------------------
// lua binding   (all LuaNacha exposed functions are prefixed with "ln_")

# define LERR(msg) return luaL_error(L, msg)



extern void randombytes(unsigned char *x,unsigned long long xlen); 

static int ln_randombytes(lua_State *L) {
	
    size_t bufln; 
	lua_Integer li = luaL_checkinteger(L, 1);  // 1st arg
	bufln = (size_t) li;
    unsigned char *buf = malloc(bufln); 
	randombytes(buf, li);
    lua_pushlstring (L, buf, bufln); 
    free(buf);
	return 1;
}//randombytes()

//----------------------------------------------------------------------
// authenticated encryption

static int ln_ae_lock(lua_State *L) {
	// Lua API: ae_lock(k, n, m [, pfx])
	//  k: key string (32 bytes)
	//  n: nonce string (24 bytes)
	//	m: message (plain text) string 
	//  pfx: optional prefix string - prepended to the encrypted text
	//     - pfx length should be a multiple of 8 for alignment
	//  return encrypted text string
	int r;
	size_t mln, nln, kln, pfxln, bufln;
	const char *k = luaL_checklstring(L,1,&kln);
	const char *n = luaL_checklstring(L,2,&nln);	
	const char *m = luaL_checklstring(L,3,&mln);	
	const char *pfx = luaL_optlstring(L,4,"",&pfxln);
	if (nln != 24) LERR("bad nonce size");
	if (kln != 32) LERR("bad key size");
	if ((pfxln % 8) != 0) LERR("bad prefix size");
	bufln = mln + 16 + pfxln;
	unsigned char * buf = malloc(bufln);
	crypto_ae_lock(buf+pfxln, k, n, m, mln);
	if (pfxln > 0) {
		memcpy(buf, pfx, pfxln);
	}
	lua_pushlstring (L, buf, bufln); 
	free(buf);
	return 1;
} // ae_lock()

static int ln_ae_unlock(lua_State *L) {
	// Lua API: ae_unlock(k, n, c [, i])
	//  k: key string (32 bytes)
	//  n: nonce string (24 bytes)
	//	c: encrypted message string 
	//  i: optional offset of the start of the encrypted text in c
	//     default value is 0 - useful if c starts with a prefix
	//  return plain text string or (nil, error msg if MAC is not valid)
	int r = 0;
	size_t cln, nln, kln, mln;
	const char *k = luaL_checklstring(L,1,&kln);
	const char *n = luaL_checklstring(L,2,&nln);	
	const char *c = luaL_checklstring(L,3,&cln);	
	int i = luaL_optinteger(L,4, 0);	
	if (nln != 24) LERR("bad nonce size");
	if (kln != 32) LERR("bad key size");
	
	unsigned char * buf = malloc(cln);
	mln = cln - i - 16;
	r = crypto_ae_unlock(buf, k, n, c+i, mln);
	if (r != 0) { 
		free(buf); 
		lua_pushnil (L);
		lua_pushliteral(L, "unlock error");
		return 2;         
	} 
	lua_pushlstring (L, buf, mln); 
	free(buf);
	return 1;
} // ln_ae_unlock()

//----------------------------------------------------------------------
// curve25519 functions

static int ln_x25519_keypair(lua_State *L) {
	// generate and return a random key pair (publickey, secretkey)
	// lua api: x25519_keypair()
	// return (sk, pk)
	unsigned char pk[32];
	unsigned char sk[32];
	// sk is a random string. Then, compute the matching public key
	randombytes(sk, 32);
	crypto_x25519_public_key(pk, sk);
	lua_pushlstring (L, pk, 32); 
	lua_pushlstring (L, sk, 32); 
	return 2;
}//ln_x25519_keypair()

static int ln_x25519_public_key(lua_State *L) {
	// return the public key associated to a secret key
	// lua api:  x25519_public_key(sk) return pk
	// sk: a secret key (can be any random value)
	// pk: the matching public key
	size_t skln;
	unsigned char pk[32];
	const char *sk = luaL_checklstring(L,1,&skln); // secret key
	if (skln != 32) LERR("bad sk size");
	crypto_x25519_public_key(pk, sk);
	lua_pushlstring (L, pk, 32); 
	return 1;
}//ln_x25519_public_key()

static int ln_lock_key(lua_State *L) {
	// DH key exchange: compute a session key
	// lua api:  lock_key(sk, pk) => k
	// !! beware, reversed order compared to nacl box_beforenm() !!
	// sk: "your" secret key
	// pk: "their" public key
	// return the session key k
	size_t pkln, skln;
	unsigned char k[32];
	const char *sk = luaL_checklstring(L,1,&skln); // your secret key
	const char *pk = luaL_checklstring(L,2,&pkln); // their public key
	if (pkln != 32) LERR("bad pk size");
	if (skln != 32) LERR("bad sk size");
	crypto_lock_key(k, sk, pk);
	lua_pushlstring(L, k, 32); 
	return 1;   
}// ln_lock_key()


//----------------------------------------------------------------------
// blake2b hash functions

static int ln_blake2b(lua_State *L) {
	// compute the hash of a string (convenience function)
	// with default parameters (64-byte digest, no key)
	// lua api:  blake2b(m) return digest
	// m: the string to be hashed
	// digest: the blake2b hash (a 64-byte string)
    size_t mln; 
    const char *m = luaL_checklstring (L, 1, &mln);
    char digest[64];
    crypto_blake2b_general(digest, 64, 0, 0, m, mln);
    lua_pushlstring (L, digest, 64); 
    return 1;
}// ln_blake2b

static int ln_blake2b_init(lua_State *L) {
	// create and initialize a blake2b context
	// lua api:  blake2b_init([digln [, key]]) return ctx
	// digln: the optional length of the digest to be computed 
	// (between 1 and 64) - default value is 64
	// key: an optional secret key, allowing blake2b to work as a MAC 
	//    (if provided, key length must be between 1 and 64)
	//    default is no key
	// return ctx, a pointer to the blake2b context as a light userdata
	// 
	// NOTE: the caller must ensure that blake2b_final() will be called to
	// free the context, and that the ctx varible will NOT be used after
	// the call to blake2b_final() 
	//
    size_t keyln = 0; 
    int digln = luaL_optinteger(L, 1, 64);
    const char *key = luaL_optlstring(L, 2, NULL, &keyln);
	if ((keyln < 0)||(keyln > 64)) LERR("bad key size");
	if ((digln < 1)||(digln > 64)) LERR("bad digest size");
    size_t ctxln = sizeof(crypto_blake2b_ctx);
	crypto_blake2b_ctx *ctx = (crypto_blake2b_ctx *) malloc(ctxln);
    crypto_blake2b_general_init(ctx, digln, key, keyln);
	lua_pushlightuserdata(L, (void *)ctx);
    return 1;
}// ln_blake2b_init

static int ln_blake2b_update(lua_State *L) {
	// update the hash with a new text fragment
	// lua api:  blake2b_update(ctx, t)
	// ctx, a pointer to the blake2b context as a light userdata
	//    (created by blake2b_init())
	// t: a text fragment as a string
	//
	size_t tln; 
	crypto_blake2b_ctx *ctx = (crypto_blake2b_ctx *) lua_touserdata(L, 1);
    const char *t = luaL_checklstring (L, 2, &tln);
	if (ctx == NULL) LERR("invalid ctx");	
    crypto_blake2b_update(ctx, t, tln);
    return 0;
}// ln_blake2b_update


static int ln_blake2b_final(lua_State *L) {
	// return the final value of the hash (and free the context)
	// lua api:  blake2b_final(ctx) return dig
	// ctx, a pointer to the blake2b context as a light userdata
	//    (created by blake2b_init())
	// dig: the digest value as a string (string length depends on 
	// the digln parameter used for blake2b_init() - default is 64
	//
	crypto_blake2b_ctx *ctx = (crypto_blake2b_ctx *) lua_touserdata(L, 1);
	if (ctx == NULL) LERR("invalid ctx");	
	int digln = ctx->output_size;
	unsigned char dig[64];
    crypto_blake2b_final(ctx, dig);
	free(ctx);
    lua_pushlstring (L, dig, digln); 
    return 1;
}// ln_blake2b_final


//----------------------------------------------------------------------
// ed25519 signature functions

static int ln_ed25519_keypair(lua_State *L) {
	// generates and return a pair of ed25519 signature keys 
	// lua api: ed25519_keypair()
	// return (sk, pk)
	unsigned char pk[32];
	unsigned char sk[32];
	// sk is a random string. Then, compute the matching public key
	randombytes(sk, 32);
	crypto_ed25519_public_key(pk, sk);
	lua_pushlstring (L, pk, 32); 
	lua_pushlstring (L, sk, 32); 
	return 2;
}//ln_ed25519_keypair()

static int ln_ed25519_public_key(lua_State *L) {
	// return the public key associated to an ed25519 secret key
	// lua api:  ed25519_public_key(sk) return pk
	// sk: a secret key (can be any random value)
	// pk: the matching public key
	size_t skln;
	unsigned char pk[32];
	const char *sk = luaL_checklstring(L,1,&skln); // secret key
	if (skln != 32) LERR("bad sk size");
	crypto_ed25519_public_key(pk, sk);
	lua_pushlstring (L, pk, 32); 
	return 1;
}//ln_ed25519_public_key()

static int ln_ed25519_sign(lua_State *L) {
	// sign a text with a secret key
	// Lua API: ed25519_sign(sk, m) return sig
	//  sk: key string (32 bytes)
	//	m: message to sign (string)
	//  return signature (a 64-byte string)
	size_t mln, skln;
	const char *sk = luaL_checklstring(L,1,&skln);
	const char *m = luaL_checklstring(L,3,&mln);	
	if (skln != 32) LERR("bad key size");
	unsigned char sig[64];
	crypto_ed25519_sign(sig, sk, m, mln);
	lua_pushlstring (L, sig, 64); 
	return 1;
} // ln_ed25519_sign()

static int ln_ed25519_check(lua_State *L) {
	// check a text signature with a public key
	// Lua API: ed25519_check(sig, pk, m) return boolean
	//  sig: signature string (64 bytes)
	//  pk: public key string (32 bytes)
	//	m: message to verify (string)
	//  return true if the signature match, or false
	int r;
	size_t mln, pkln, sigln;
	const char *sig = luaL_checklstring(L,1,&sigln);
	const char *pk = luaL_checklstring(L,1,&pkln);
	const char *m = luaL_checklstring(L,3,&mln);	
	if (sigln != 64) LERR("bad signature size");
	if (pkln != 32) LERR("bad key size");
	r = crypto_ed25519_check(sig, pk, m, mln);
	// r == 0 if the signature matches
	lua_pushboolean (L, (r == 0)); 
	return 1;
} // ln_ed25519_check()

//------------------------------------------------------------
// argon2i password derivation
//

static int ln_argon2i(lua_State *L) {
	// Lua API: argon2i(pw, salt, nkb, niters) => k
	// pw: the password string
	// salt: some entropy as a string (typically 16 bytes)
	// nkb:  number of kilobytes used in RAM (as large as possible)
	// niters: number of iterations (as large as possible, >= 10)
	//  return k, a key string (32 bytes)
	size_t pwln, saltln, kln, mln;
	const char *pw = luaL_checklstring(L,1,&pwln);
	const char *salt = luaL_checklstring(L,2,&saltln);	
	int nkb = luaL_checkinteger(L,3);	
	int niters = luaL_checkinteger(L,4);	
	unsigned char k[32];
	size_t worksize = nkb * 1024;
	unsigned char *work= malloc(worksize);
	crypto_argon2i(	k, 32, pw, pwln, salt, saltln, 
					"", 0, "", 0,  	// optional key and additional data
					work, nkb, niters);

	lua_pushlstring (L, k, 32); 
	free(work);
	return 1;
} // ln_argon2i()

//------------------------------------------------------------
// lua library declaration
//
static const struct luaL_Reg luanachalib[] = {
	{"randombytes", ln_randombytes},
	//
	{"ae_lock", ln_ae_lock},
	{"ae_unlock", ln_ae_unlock},
	//
	{"x25519_keypair", ln_x25519_keypair},
	{"x25519_public_key", ln_x25519_public_key},
	{"lock_key", ln_lock_key},
	//
	{"blake2b", ln_blake2b},
	{"blake2b_init", ln_blake2b_init},
	{"blake2b_update", ln_blake2b_update},
	{"blake2b_final", ln_blake2b_final},
	//
	{"ed25519_keypair", ln_ed25519_keypair},
	{"ed25519_public_key", ln_ed25519_public_key},	
	{"ed25519_sign", ln_ed25519_sign},	
	{"ed25519_check", ln_ed25519_check},	
	//
	{"argon2i", ln_argon2i},	
	//
	{NULL, NULL},
};

int luaopen_luanacha(lua_State *L) {
	luaL_register (L, "luanacha", luanachalib);
    // 
    lua_pushliteral (L, "VERSION");
	lua_pushliteral (L, LUANACHA_VERSION); 
	lua_settable (L, -3);
	return 1;
}

