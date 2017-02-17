# luamonocypher

This is intended to be a Lua wrapper for the Monocypher cryptographic library by Loup Vaillant (http://loup-vaillant.fr/projects/monocypher/).

Monocypher is heavily based on the NaCl crypto library by Daniel Bernstein and Tanja Lange.

Major features and differences with the NaCl library:

* Authenticated encryption with Chacha20 stream encryption and Poly1305 MAC,
* Curve25519-based key exchange and public key encryption,
* Blake2b hash function,
* Ed25519-based signature function using Blake2b hash instead of sha512,
* Argon2i, a modern key derivation function based on Blake2b.

The complete documentation of the Monocypher library is available at http://loup-vaillant.fr/projects/monocypher/manual

Note:  As of February 2017, Monocypher is not deemed to be production-ready yet. (See section "Current state" at http://loup-vaillant.fr/projects/monocypher/)


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
This Lua wrapper code is MIT-licensed.


