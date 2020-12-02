
from bitcoin.rpc import RawProxy

p = RawProxy()

txid = str(raw_input("Enter transaction ID: "))

# decoding transaction by getting raw transaction
decoded_tx = p.decoderawtransaction(p.getrawtransaction(txid))

# counting total output value
total_out = 0
for output in decoded_tx['vout']:
    total_out += output['value']

# counting total input value
total_in = 0
for input in decoded_tx['vin']:
    x = p.decoderawtransaction(p.getrawtransaction(input['txid']))
    for output in x['vout']:
        total_in += output['value']

print("Total input: %f" % total_in)
print("Total output: %f" % total_out)

fee = total_in - total_out
print("Fee: %f" % fee)
