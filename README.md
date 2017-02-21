# *(work in progress!!)*

# LuaNacha

LuaNacha (*NaCl with Chacha20*) is a small Lua cryptographic library wrapping the excellent Monocypher cryptographic C library by Loup Vaillant (http://loup-vaillant.fr/projects/monocypher/).

The Monocypher library is included here.

Monocypher is itself heavily based on the NaCl crypto library by Daniel Bernstein and Tanja Lange.

Major features and differences with the NaCl library:

* Authenticated encryption with Chacha20 stream encryption (more precisely Xchacha20, ie. Chacha with a 24-byte nonce) and Poly1305 MAC, 
* Curve25519-based key exchange and public key encryption,
* Blake2b hash function,
* Ed25519-based signature function using Blake2b hash instead of sha512,
* Argon2i, a modern key derivation function based on Blake2b. Like 
scrypt, it is designed to be expensive in both CPU and memory.

The complete documentation of the Monocypher library is available at http://loup-vaillant.fr/projects/monocypher/manual

Note:  As of February 2017, Monocypher is not deemed to be production-ready yet. (See section "Current state" at http://loup-vaillant.fr/projects/monocypher/)

## The Lua wrapper

It includes an interface to an OS random generator (for the moment only based on /dev/urandom and tested on Linux)

The functions keep as much as possible the same name as in  Monocypher 
(without the "crypto_" prefix) and the same parameter order.

Luanacha API summary:

```
randombytes(n)
	return a string containing n random bytes
	
--- Authenticated encryption

ae_lock(key, nonce, plain [, prefix]) => crypted
	authenticated encryption using Xchacha20 and a Poly1305 MAC
	key must be a 32-byte string
	nonce must be a 24-byte string
	plain is the text to encrypt as a string
	prefix is an optional string. If it is provided, it is prepended 
	to the encrypted text. The prefix can be use for example to 
	store the nonce, and avoid extra string allocation and copying in 
	Lua applications. The prefix defaults to the empty string.
	Return the encrypted text as a string. The encrypted text includes 
	the 16-byte MAC. So #crypted == #plain + 16 + #prefix
	
	Note: the prefix is not an "additional data" in the AEAD sense.
	The MAC is computed over only the encrypted text. It does not include 
	the prefix.


ae_unlock(key, nonce, crypted [, offset]) => plain
	authenticated decryption - verification of the Poly1305 MAC
	and decryption with Xcahcha20.
	key must be a 32-byte string
	nonce must be a 24-byte string
	crypted is the text to decrypt as a string
	offset is an optional integer. It is the length of the prefix used 
	by ae_lock() if any. It defaults to 0.
	Return the decrypted text as a string or nil if the MAC 
	verification fails.
	
	Note: the responsibility of using matching prefix and offset belongs 
	to the application.
	

--- Curve25519-based key exchange

x25519_public_key(sk) => pk
	return the public key associated to a secret key
	sk is the secret key as a 32-byte string
	pk is the associated public key as a 32-byte string

x25519_keypair() => pk, sk
	generates a pair of curve25519 keys (public key, secret key)
	pk is the public key as a 32-byte string
	sk is the secret key as a 32-byte string
	
	Note: This is a convenience function:
		pk, sk = x25519_keypair()
		--is equivalent to
		sk = randombytes(32); pk = x25519_public_key(sk)

lock_key(sk, pk) => k
	DH key exchange. Return a session key k used to encrypt 
	or decrypt a text.
	sk is the secret key of the party invoking the function 
	("our secret key"). 
	pk is the public key of the other party 
	("their public key").
	sk, pk and k are 32-byte strings

--- Blake2b cryptographic hash

blake2b_init([digest_size]) => ctx
	initialize and return a blake2b context object
	digest_size is the optional length of the expected digest. 
	If provided, it must be an integer between 1 and 64.
	It defaults to 64.
	ctx is a pointer to the blake2b context as a light userdata

blake2b_update(ctx, text_fragment)
	update the hash with a new text fragment
	ctx is a pointer to a blake2b context as a light userdata

blake2b_final(ctx) => digest
	return the final value of the hash
	ctx is a pointer to a blake2b context as a light userdata
	The digest is returned as a string. The length of the digest
	has been defined at the context creation (see blake2b_init()).
	It defaults to 64.

blake2b(text) => digest
	compute the hash of a string. 
	Returns a 64-byte digest.
	This is a convenience function which combines the init(), 
	update() and final() functions above.


--- Ed25519 signature

ed25519_public_key(sk) => pk
	return the public key associated to a secret key
	sk is the secret key as a 32-byte string
	pk is the associated public key as a 32-byte string

ed25519_keypair() => pk, sk
	generates a pair of ed25519 signature keys (public key, secret key)
	pk is the public signature key as a 32-byte string
	sk is the secret signature key as a 32-byte string

	Note: This is a convenience function:
		pk, sk = ed25519_keypair()
		--is equivalent to
		sk = randombytes(32); pk = ed25519_public_key(sk)

ed25519_sign(sk, text) => sig
	sign a text with a secret key
	sk is the secret key as a 32-byte string
	text is the text to sign as a string
	Return the text signature as a 64-byte string.

ed25519_check(sig, pk, text) => is_valid
	check a text signature with a public key
	sig is the signature to verify, as a 64-byte string
	pk is the public key as a 32-byte string
	text is the signed text
	Return a boolean indicating if the signature is valid or not.
	
	Note: curve25519 key pairs (generated with x25519_keypair())
	cannot be used for ed25519 signature. The signature key pairs 
	must be generated with ed25519_keypair().

--- Argon2i password derivation 

argon2i(pw, salt, nkb, niter) => k
	compute a key given a password and some salt
	This is a password key derivation function similar to scrypt.
	It is intended to make derivation expensive in both CPU and memory.
	pw: the password string
	salt: some entropy as a string (typically 16 bytes)
	nkb:  number of kilobytes used in RAM (as large as possible)
	niter: number of iterations (as large as possible, >= 10)
	Return k, a key string (32 bytes).

	For example: on a CPU i5 M430 @ 2.27 GHz laptop,
	with nkb=100000 (100MB) and niter=10, the derivation takes ~ 1.8 sec
	
	Note: this implementation has no threading support, so no parallel 
	execution.
	
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



