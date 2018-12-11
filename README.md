# LuaNacha

LuaNacha (*NaCl with Chacha20*) is a small Lua cryptographic library wrapping the excellent Monocypher cryptographic C library by Loup Vaillant -  http://loup-vaillant.fr/projects/monocypher/
and https://github.com/LoupVaillant/Monocypher 

The Monocypher library is included here (currently version 2.0.5)

Monocypher is itself heavily based on the NaCl crypto library by Daniel Bernstein and Tanja Lange.

Major features and differences with the NaCl library:

* Authenticated encryption with Chacha20 stream encryption (more precisely Xchacha20, ie. Chacha with a 24-byte nonce) and Poly1305 MAC, 
* Curve25519-based key exchange and public key encryption,
* Blake2b hash function,
* Ed25519-based signature function using Blake2b hash instead of sha512,
* Argon2i, a modern key derivation function based on Blake2b. Like 
scrypt, it is designed to be expensive in both CPU and memory.

The complete documentation of the Monocypher library is available at http://loup-vaillant.fr/projects/monocypher/manual

Note:  As of August 2017, Monocypher is deemed to be production-ready. (See section "Current state" at http://loup-vaillant.fr/projects/monocypher/) 

## The Lua wrapper

It includes an interface to an OS random generator (for the moment only based on /dev/urandom and tested on Linux)

The functions keep as much as possible the same name as in  Monocypher 
(without the "crypto_" prefix) and the same parameter order.

LuaNacha API summary:

```
randombytes(n)
	return a string containing n random bytes


--- Authenticated encryption

lock(key, nonce, plain [, prefix]) => crypted
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


unlock(key, nonce, crypted [, offset]) => plain
	authenticated decryption - verification of the Poly1305 MAC
	and decryption with Xcahcha20.
	key must be a 32-byte string
	nonce must be a 24-byte string
	crypted is the text to decrypt as a string
	offset is an optional integer. It is the length of the prefix used 
	by lock() if any. It defaults to 0.
	Return the decrypted text as a string or nil if the MAC 
	verification fails.
	
	Note: the responsibility of using matching prefix and offset belongs 
	to the application.
	

--- Curve25519-based key exchange

public_key(sk) => pk
	return the public key associated to a curve25519 secret key
	sk is the secret key as a 32-byte string
	pk is the associated public key as a 32-byte string

keypair() => pk, sk
	generates a pair of curve25519 keys (public key, secret key)
	pk is the public key as a 32-byte string
	sk is the secret key as a 32-byte string
	
	Note: This is a convenience function:
		pk, sk = keypair()  --is equivalent to
		sk = randombytes(32); pk = public_key(sk)

key_exchange(sk, pk) => k
	DH key exchange. Return a session key k used to encrypt 
	or decrypt a text.
	sk is the secret key of the party invoking the function 
	("our secret key"). 
	pk is the public key of the other party 
	("their public key").
	sk, pk and k are 32-byte strings


--- Blake2b cryptographic hash

blake2b_init([digest_size [, key]]) => ctx
	initialize and return a blake2b context object
	digest_size is the optional length of the expected digest. If provided,
	it must be an integer between 1 and 64. It defaults to 64.
	key is an optional key allowing to use blake2b as a MAC function.
	If provided, key is a string with a length that must be between 
	1 and 64. The default is no key.
	ctx is a pointer to the blake2b context as a light userdata.

blake2b_update(ctx, text_fragment)
	update the hash with a new text fragment
	ctx is a pointer to a blake2b context as a light userdata.

blake2b_final(ctx) => digest
	return the final value of the hash
	ctx is a pointer to a blake2b context as a light userdata.
	The digest is returned as a string. The length of the digest
	has been defined at the context creation (see blake2b_init()).
	It defaults to 64.

blake2b(text) => digest
	compute the hash of a string. 
	Returns a 64-byte digest.
	This is a convenience function which combines the init(), 
	update() and final() functions above.


--- Ed25519 signature

sign_public_key(sk) => pk
	return the public key associated to a secret key
	sk is the secret key as a 32-byte string
	pk is the associated public key as a 32-byte string

sign_keypair() => pk, sk
	generates a pair of ed25519 signature keys (public key, secret key)
	pk is the public signature key as a 32-byte string
	sk is the secret signature key as a 32-byte string

	Note: This is a convenience function:
		pk, sk = sign_keypair()  	--is equivalent to
		sk = randombytes(32); pk = sign_public_key(sk)

sign(sk, text) => sig
	sign a text with a secret key
	sk is the secret key as a 32-byte string
	text is the text to sign as a string
	Return the text signature as a 64-byte string.

check(sig, pk, text) => is_valid
	check a text signature with a public key
	sig is the signature to verify, as a 64-byte string
	pk is the public key as a 32-byte string
	text is the signed text
	Return a boolean indicating if the signature is valid or not.
	
	Note: curve25519 key pairs (generated with keypair())
	cannot be used for ed25519 signature. The signature key pairs 
	must be generated with sign_keypair().


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

## Building 

Adjust the Makefile according to your Lua installation.

Targets:
```
	make          -- build luanacha.so
	make test     -- build luanacha.so if needed, 
	                 then run test/test_luanacha.lua
	make clean
	
	make LUA=/path/to/lua LUAINC=/path/to/lua_include_dir test
```

Yes, a rockspec is due :-)

## License

The original Monocypher source code is licensed under the following terms (from the monocypher README file):

```
Monocypher is dual-licensed.  Choose whichever you want.

The first licence is a regular 2-clause BSD licence.  The second licence
is the CC-0 from Creative Commons. It is intended to release Monocypher
to the public domain.  The BSD licence serves as a fallback.


Licence 1 (2-clause BSD)
------------------------

Copyright (c) 2017, Loup Vaillant
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Licence 2 (CC-0)
----------------

> CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
> LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
> ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
> INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
> REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
> PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
> THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
> HEREUNDER.

### Statement of Purpose

The laws of most jurisdictions throughout the world automatically confer
exclusive Copyright and Related Rights (defined below) upon the creator
and subsequent owner(s) (each and all, an "owner") of an original work
of authorship and/or a database (each, a "Work").

Certain owners wish to permanently relinquish those rights to a Work for
the purpose of contributing to a commons of creative, cultural and
scientific works ("Commons") that the public can reliably and without
fear of later claims of infringement build upon, modify, incorporate in
other works, reuse and redistribute as freely as possible in any form
whatsoever and for any purposes, including without limitation commercial
purposes. These owners may contribute to the Commons to promote the
ideal of a free culture and the further production of creative, cultural
and scientific works, or to gain reputation or greater distribution for
their Work in part through the use and efforts of others.

For these and/or other purposes and motivations, and without any
expectation of additional consideration or compensation, the person
associating CC0 with a Work (the "Affirmer"), to the extent that he or
she is an owner of Copyright and Related Rights in the Work, voluntarily
elects to apply CC0 to the Work and publicly distribute the Work under
its terms, with knowledge of his or her Copyright and Related Rights in
the Work and the meaning and intended legal effect of CC0 on those
rights.

1. **Copyright and Related Rights.** A Work made available under CC0 may
   be protected by copyright and related or neighboring rights
   ("Copyright and Related Rights"). Copyright and Related Rights
   include, but are not limited to, the following:

   - the right to reproduce, adapt, distribute, perform, display,
     communicate, and translate a Work;
   - moral rights retained by the original author(s) and/or
     performer(s); publicity and privacy rights pertaining to a person's
     image or likeness depicted in a Work;
   - rights protecting against unfair competition in regards to a Work,
     subject to the limitations in paragraph 4(a), below;
   - rights protecting the extraction, dissemination, use and reuse of
     data in a Work;
   - database rights (such as those arising under Directive 96/9/EC of
     the European Parliament and of the Council of 11 March 1996 on the
     legal protection of databases, and under any national
     implementation thereof, including any amended or successor version
     of such directive); and
   - other similar, equivalent or corresponding rights throughout the
     world based on applicable law or treaty, and any national
     implementations thereof.

2. **Waiver.** To the greatest extent permitted by, but not in
   contravention of, applicable law, Affirmer hereby overtly, fully,
   permanently, irrevocably and unconditionally waives, abandons, and
   surrenders all of Affirmer's Copyright and Related Rights and
   associated claims and causes of action, whether now known or unknown
   (including existing as well as future claims and causes of action),
   in the Work (i) in all territories worldwide, (ii) for the maximum
   duration provided by applicable law or treaty (including future time
   extensions), (iii) in any current or future medium and for any number
   of copies, and (iv) for any purpose whatsoever, including without
   limitation commercial, advertising or promotional purposes (the
   "Waiver"). Affirmer makes the Waiver for the benefit of each member
   of the public at large and to the detriment of Affirmer's heirs and
   successors, fully intending that such Waiver shall not be subject to
   revocation, rescission, cancellation, termination, or any other legal
   or equitable action to disrupt the quiet enjoyment of the Work by the
   public as contemplated by Affirmer's express Statement of Purpose.

3. **Public License Fallback.** Should any part of the Waiver for any
   reason be judged legally invalid or ineffective under applicable law,
   then the Waiver shall be preserved to the maximum extent permitted
   taking into account Affirmer's express Statement of Purpose. In
   addition, to the extent the Waiver is so judged Affirmer hereby
   grants to each affected person a royalty-free, non transferable, non
   sublicensable, non exclusive, irrevocable and unconditional license
   to exercise Affirmer's Copyright and Related Rights in the Work (i)
   in all territories worldwide, (ii) for the maximum duration provided
   by applicable law or treaty (including future time extensions), (iii)
   in any current or future medium and for any number of copies, and
   (iv) for any purpose whatsoever, including without limitation
   commercial, advertising or promotional purposes (the "License"). The
   License shall be deemed effective as of the date CC0 was applied by
   Affirmer to the Work. Should any part of the License for any reason
   be judged legally invalid or ineffective under applicable law, such
   partial invalidity or ineffectiveness shall not invalidate the
   remainder of the License, and in such case Affirmer hereby affirms
   that he or she will not (i) exercise any of his or her remaining
   Copyright and Related Rights in the Work or (ii) assert any
   associated claims and causes of action with respect to the Work, in
   either case contrary to Affirmer's express Statement of Purpose.

4. **Limitations and Disclaimers.**

   - No trademark or patent rights held by Affirmer are waived,
     abandoned, surrendered, licensed or otherwise affected by this
     document.
   - Affirmer offers the Work as-is and makes no representations or
     warranties of any kind concerning the Work, express, implied,
     statutory or otherwise, including without limitation warranties of
     title, merchantability, fitness for a particular purpose, non
     infringement, or the absence of latent or other defects, accuracy,
     or the present or absence of errors, whether or not discoverable,
     all to the greatest extent permissible under applicable law.
   - Affirmer disclaims responsibility for clearing rights of other
     persons that may apply to the Work or any use thereof, including
     without limitation any person's Copyright and Related Rights in the
     Work. Further, Affirmer disclaims responsibility for obtaining any
     necessary consents, permissions or other rights required for any
     use of the Work.
   - Affirmer understands and acknowledges that Creative Commons is not
     a party to this document and has no duty or obligation with respect
     to this CC0 or use of the Work.

```


The LuaNacha wrapper library is MIT-licensed.



