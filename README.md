# 

# LuaNacha

LuaNacha (*NaCl with Chacha20*) is a small Lua cryptographic library wrapping the excellent Monocypher cryptographic C library by Loup Vaillant (http://loup-vaillant.fr/projects/monocypher/).

The Monocypher library is included here.

Monocypher is itself heavily based on the NaCl crypto library by Daniel Bernstein and Tanja Lange.

Major features and differences with the NaCl library:

* Authenticated encryption with Chacha20 stream encryption (more precisely Xchacha20, ie. Chacha with a 24-byte nonce) and Poly1305 MAC, 
* Curve25519-based key exchange and public key encryption,
* Blake2b hash function,
* Ed25519-based signature function using Blake2b hash instead of sha512,
* Argon2i, a modern key derivation function based on Blake2b.

The complete documentation of the Monocypher library is available at http://loup-vaillant.fr/projects/monocypher/manual

Note:  As of February 2017, Monocypher is not deemed to be production-ready yet. (See section "Current state" at http://loup-vaillant.fr/projects/monocypher/)

## The Lua wrapper

It includes an interface to an OS random generator (for the moment only based on /dev/urandom and tested on Linux)

The functions keep as much as possible the same name as in  Monocypher 
(without the "crypto_" prefix)

LuanachaAPI summary:

```
randombytes(n)
	return a string containing n random bytes
	
--- Authenticated encryption

ae_lock(key, nonce, plain, [prefix]) => crypted
	authenticated encryption
	with an optional prefix prepended to the encrypted text

ae_unlock(key, nonce, crypted, [offset]) => plain
	authenticated decryption
	with an optional offset for the start of the encrypted text

--- Curve25519-based key exchange

x25519_keypair()
	generates a pair of curve25519 keys (secret key, public key)

x25519_public_key(secretkey) => publickey
	return the public key associated to a secret key

lock_key(alice_secretkey, bob_publickey) => session key
	DH key exchange. Return a session key

--- Blake2b cryptographic hash

blake2b_init([digest_size]) => ctx
	initialize and return a blake2b context object

blake2b_update(ctx, text_fragment)
	update the hash with a new text fragment

blake2b_final(ctx) => digest
	return the final value of the hash

blake2b(text) => digest
	compute the hash of a string (convenience function)


--- Ed25519 signature

ed25519_keypair()
	generates a pair of ed25519 signature keys (secret key, public key)

ed25519_public_key(secretkey) => publickey
	return the public key associated to a secret key

ed25519_sign(secretkey, text) => signature
	sign a text with a secret key

ed25519_check(secretkey, text, signature) => boolean
	check a text signature with a public key
```


## License

The original Monocypher source code is licensed under the following terms (from the monocypher README file):

```
For everything *but* Blake2b:

  Copying and distribution of the code, with or without modification,
  are permitted in any medium without royalty.  This code is offered
  as-is, without any warranty.

---

For the Blake2b code:

  Copyright (c) 2015 IETF Trust and the persons identified as authors
  of the code. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  - Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  - Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

  - Neither the name of Internet Society, IETF or IETF Trust, nor the
    names of specific contributors, may be used to endorse or promote
    products derived from this software without specific prior written
    permission.

  - THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS âAS ISâ AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
    THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
```
The LuaNacha wrapper code is MIT-licensed.



