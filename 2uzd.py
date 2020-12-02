from bitcoin.rpc import RawProxy
import hashlib


def convert_string(x):
    a = bytearray.fromhex(x)
    a.reverse()
    return ''.join(format(x, '02x') for x in a)


def convert_int(x):
    return '{:02x}'.format(x)


p = RawProxy()

blockid = raw_input("Enter block ID: ")

header = p.getblockheader(blockid)

version = convert_string(header['versionHex'])
previousBlock = convert_string(header['previousblockhash'])
merkleRoot = convert_string(header['merkleroot'])
time = convert_string(convert_int(header['time']))
bits = convert_string(header['bits'])
nonce = convert_string(convert_int(header['nonce']))

header_hex = version + previousBlock + merkleRoot + time + bits + nonce
header_bin = header_hex.decode('hex')
hash = hashlib.sha256(hashlib.sha256(header_bin).digest()).digest()

final_hash = convert_string(hash.encode('hex_codec'))

print(final_hash)

if (blockid == final_hash):
    print("Block is valid")
else:
    print("Block is not valid!")
