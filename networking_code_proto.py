from requests import get
from io import BytesIO, StringIO
from string import ascii_lowercase, ascii_uppercase, punctuation, digits
from math import floor, log2

index = ascii_uppercase + digits
#use this for base


ip = get('https://api.ipify.org').text
#ip = "255.255.255.255"
print("IP ", ip)

ipbytes = tuple(int(x) for x in ip.split('.'))
ipcode = 0
for i in range(4): #TODO: should this be reverse?
    ipcode |= ipbytes[i] << (i * 8)
print("IPC", bin(ipcode), ipcode)
portcode = 300
print("PRT", bin(portcode), portcode)

gameinfo = (10, 3, 2) #len, itembox, player
gamecode = gameinfo[0] | (gameinfo[1] << 4) | (gameinfo[2] << 6) 
print("GME", bin(gamecode), gamecode)

def getbit(i, n):
    return (i & (1 << n)) >> n

pattern = "AABAABC"
pout = 0b0


ipc = 0; pc = 0; gc = 0

for i in range(len(pattern) * 8): #pattern length
    #we can check what's at the pattern and build/read from it
    #we can set this up similarly in C++ with a const char*
    print(pout, i)
    c = pattern[i % (len(pattern))]
    if c == "A":
        pout |= (getbit(ipcode,  ipc) << i)
        ipc += 1
    elif c == 'B':
        pout |= (getbit(portcode, pc) << i)
        pc += 1
    else:
        pout |= (getbit(gamecode, gc) << i)
        gc += 1
print(bin(pout))

#calculate code
out = []
iter = pout
i = 0

base = len(index)

while True:
    if iter < (base ** (i + 1)): break
    i += 1
while iter:
    out.append(iter // (base ** i))
    iter %= base ** i
    i -= 1
code = ''.join(index[x] for x in out)
print(code, len(code))
#BEGIN REVERSE
#get int from code
past = out
out = 0
i = 0
for x in reversed(code):
    out += index.index(x) * base**i
    i += 1 
print(bin(out))
#discect bits
#python OFFERS a bit length function, but since we're moving this to c++, im fucking killing you
ipc = 0; pc = 0; gc = 0
ipcode = 0; portcode = 0; gamecode = 0
for i in range(floor(log2(out)) + 1): 
    bit = getbit(out, i)
    c = pattern[i % len(pattern)] #the pattern is reversed cause of how we write
    if c == "A":
        ipcode   |= bit << ipc
        ipc += 1
    elif c == "B":
        portcode |= bit << pc
        pc += 1
    else:
        gamecode |= bit << gc
        gc += 1
ipbytes = [0, 0, 0, 0]
for i in range(4):
    ipbytes[i] = (ipcode >> (i * 8)) & 0xFF
ip = f"{ipbytes[0]}.{ipbytes[1]}.{ipbytes[2]}.{ipbytes[3]}"
print("IP ", ip)
print("IPC", bin(ipcode), ipcode)
print("PRT", bin(portcode), portcode)
print("GME", bin(gamecode), gamecode)