-- quick and dirty test of the major luanacha functions

local na = require "luanacha"

-- some local definitions

local strf = string.format
local byte, char = string.byte, string.char
local spack, sunpack = string.pack, string.unpack

local app, concat = table.insert, table.concat

local function stohex(s, ln, sep)
	-- stohex(s [, ln [, sep]])
	-- return the hex encoding of string s
	-- ln: (optional) a newline is inserted after 'ln' bytes 
	--	ie. after 2*ln hex digits. Defaults to no newlines.
	-- sep: (optional) separator between bytes in the encoded string
	--	defaults to nothing (if ln is nil, sep is ignored)
	-- example: 
	--	stohex('abcdef', 4, ":") => '61:62:63:64\n65:66'
	--	stohex('abcdef') => '616263646566'
	--
	if #s == 0 then return "" end
	if not ln then -- no newline, no separator: do it the fast way!
		return (s:gsub('.', 
			function(c) return strf('%02x', byte(c)) end
			))
	end
	sep = sep or "" -- optional separator between each byte
	local t = {}
	for i = 1, #s - 1 do
		t[#t + 1] = strf("%02x%s", s:byte(i),
				(i % ln == 0) and '\n' or sep) 
	end
	-- last byte, without any sep appended
	t[#t + 1] = strf("%02x", s:byte(#s))
	return concat(t)	
end --stohex()

local function hextos(hs, unsafe)
	-- decode an hex encoded string. return the decoded string
	-- if optional parameter unsafe is defined, assume the hex
	-- string is well formed (no checks, no whitespace removal).
	-- Default is to remove white spaces (incl newlines)
	-- and check that the hex string is well formed
	local tonumber = tonumber
	if not unsafe then
		hs = string.gsub(hs, "%s+", "") -- remove whitespaces
		if string.find(hs, '[^0-9A-Za-z]') or #hs % 2 ~= 0 then
			error("invalid hex string")
		end
	end
	return (hs:gsub(	'(%x%x)', 
		function(c) return char(tonumber(c, 16)) end
		))
end -- hextos

local function px(s) print'--'; print(stohex(s, 8, " ")) end

print("------------------------------------------------------------")
print(_VERSION)

-- xchacha test from
-- https://raw.githubusercontent.com/DaGenix/rust-crypto/master/src/chacha20.rs

k = "\x1b\x27\x55\x64\x73\xe9\x85\xd4\x62\xcd\x51\x19\x7a\x9a\x46\xc7"
 .. "\x60\x09\x54\x9e\xac\x64\x74\xf2\x06\xc4\xee\x08\x44\xf6\x83\x89"
n = "\x69\x69\x6e\xe9\x55\xb6\x2b\x73\xcd\x62\xbd\xa8\x75\xfc\x73\xd6"
 .. "\x82\x19\xe0\x03\x6b\x7a\x0b\x37"
m = string.rep('\0', 139)
e = "\x4f\xeb\xf2\xfe\x4b\x35\x9c\x50" ..
	"\x8d\xc5\xe8\xb5\x98\x0c\x88\xe3" ..
	"\x89\x46\xd8\xf1\x8f\x31\x34\x65" ..
	"\xc8\x62\xa0\x87\x82\x64\x82\x48" ..
	"\x01\x8d\xac\xdc\xb9\x04\x17\x88" ..
	"\x53\xa4\x6d\xca\x3a\x0e\xaa\xee" ..
	"\x74\x7c\xba\x97\x43\x4e\xaf\xfa" ..
	"\xd5\x8f\xea\x82\x22\x04\x7e\x0d" ..
	"\xe6\xc3\xa6\x77\x51\x06\xe0\x33" ..
	"\x1a\xd7\x14\xd2\xf2\x7a\x55\x64" ..
	"\x13\x40\xa1\xf1\xdd\x9f\x94\x53" ..
	"\x2e\x68\xcb\x24\x1c\xbd\xd1\x50" ..
	"\x97\x0d\x14\xe0\x5c\x5b\x17\x31" ..
	"\x93\xfb\x14\xf5\x1c\x41\xf3\x93" ..
	"\x83\x5b\xf7\xf4\x16\xa7\xe0\xbb" ..
	"\xa8\x1f\xfb\x8b\x13\xaf\x0e\x21" ..
	"\x69\x1d\x7e\xce\xc9\x3b\x75\xe6" ..
	"\xe4\x18\x3a"

c = na.ae_lock(k, n, m)
print(stohex(c, 8, " "))
print("\nLibsodium")
-- from libsodium 
-- https://github.com/jedisct1/libsodium/blob/master/test/default/xchacha20.c
k = hextos"79c99798ac67300bbb2704c95c341e3245f3dcb21761b98e52ff45b24f304fc4"
n = hextos"b33ffd3096479bcfbc9aee49417688a0a2554f8d95389419"
e = hextos"c6e9758160083ac604ef90e712ce6e75d7797590744e0cf060f013739c"
m = string.rep('\0', #e)
c = na.ae_lock(k, n, m)
print(stohex(c, 8, " "))
px(e)
px(n)
print'==================================='
k=hextos"eadc0e27f77113b5241f8ca9d6f9a5e7f09eee68d8a5cf30700563bf01060b4e" 
n=hextos"a171a4ef3fde7c4794c5b86170dc5a099b478f1b852f7b64"
e=hextos "23839f61795c3cdbcee2c749a92543baeeea3cbb721402aa42e6cae140447575f2916c5d71108e3b13357eaf86f060cb" 
m = string.rep('\0', #e)
c = na.ae_lock(k, n, m)
px(c); px(e)



print("luatweetnacl  ok")
print("------------------------------------------------------------")


--[[

    // There aren't any convenient test vectors for XChaCha/20,
        // so, a simple test case was generated using Andrew Moon's
        // chacha-opt library, with the key/nonce from test_salsa20_cryptopp().
        let key =
            [0x1b, 0x27, 0x55, 0x64, 0x73, 0xe9, 0x85, 0xd4,
             0x62, 0xcd, 0x51, 0x19, 0x7a, 0x9a, 0x46, 0xc7,
             0x60, 0x09, 0x54, 0x9e, 0xac, 0x64, 0x74, 0xf2,
             0x06, 0xc4, 0xee, 0x08, 0x44, 0xf6, 0x83, 0x89];
        let nonce =
            [0x69, 0x69, 0x6e, 0xe9, 0x55, 0xb6, 0x2b, 0x73,
             0xcd, 0x62, 0xbd, 0xa8, 0x75, 0xfc, 0x73, 0xd6,
             0x82, 0x19, 0xe0, 0x03, 0x6b, 0x7a, 0x0b, 0x37];
        let input = [0u8; 139];
        let mut stream = [0u8; 139];
        let result =
            [0x4f, 0xeb, 0xf2, 0xfe, 0x4b, 0x35, 0x9c, 0x50,
             0x8d, 0xc5, 0xe8, 0xb5, 0x98, 0x0c, 0x88, 0xe3,
             0x89, 0x46, 0xd8, 0xf1, 0x8f, 0x31, 0x34, 0x65,
             0xc8, 0x62, 0xa0, 0x87, 0x82, 0x64, 0x82, 0x48,
             0x01, 0x8d, 0xac, 0xdc, 0xb9, 0x04, 0x17, 0x88,
             0x53, 0xa4, 0x6d, 0xca, 0x3a, 0x0e, 0xaa, 0xee,
             0x74, 0x7c, 0xba, 0x97, 0x43, 0x4e, 0xaf, 0xfa,
             0xd5, 0x8f, 0xea, 0x82, 0x22, 0x04, 0x7e, 0x0d,
             0xe6, 0xc3, 0xa6, 0x77, 0x51, 0x06, 0xe0, 0x33,
             0x1a, 0xd7, 0x14, 0xd2, 0xf2, 0x7a, 0x55, 0x64,
             0x13, 0x40, 0xa1, 0xf1, 0xdd, 0x9f, 0x94, 0x53,
             0x2e, 0x68, 0xcb, 0x24, 0x1c, 0xbd, 0xd1, 0x50,
             0x97, 0x0d, 0x14, 0xe0, 0x5c, 0x5b, 0x17, 0x31,
             0x93, 0xfb, 0x14, 0xf5, 0x1c, 0x41, 0xf3, 0x93,
             0x83, 0x5b, 0xf7, 0xf4, 0x16, 0xa7, 0xe0, 0xbb,
             0xa8, 0x1f, 0xfb, 0x8b, 0x13, 0xaf, 0x0e, 0x21,
             0x69, 0x1d, 0x7e, 0xce, 0xc9, 0x3b, 0x75, 0xe6,
             0xe4, 0x18, 0x3a];

]]