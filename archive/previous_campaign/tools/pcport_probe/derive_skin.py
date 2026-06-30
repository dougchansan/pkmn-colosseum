#!/usr/bin/env python3
"""Derive per-bone skinMtx from the FIFO: skinMtx = view^-1 * loadedPNMTX.

The game loads PNMTX = view * skinMtx for each bone. The static room geometry is
world-space, so its PNMTX ~= view. We capture the view from an early non-skinned
draw, then for the skinned character draws compute view^-1 * M[slot] = skinMtx
and report each bone's translation + whether it's ~identity (verts are
model-space for that bone) or a real transform (bone-local).
"""
import struct, sys

d = open(sys.argv[1] if len(sys.argv) > 1 else "fifo1.dff", "rb").read()
fifo = d[0x104aa0:0x104aa0 + 0x2131b]; n = len(fifo)

def mat4_from_rows(rows3):
    # rows3: 3 lists of 4 -> 4x4 with [0,0,0,1]
    return [list(rows3[0]), list(rows3[1]), list(rows3[2]), [0.0, 0.0, 0.0, 1.0]]

def mat_mul(A, B):
    return [[sum(A[i][k]*B[k][j] for k in range(4)) for j in range(4)] for i in range(4)]

def mat_inv(M):
    # general 4x4 inverse via Gauss-Jordan
    import copy
    a = [row[:] + [1.0 if i == j else 0.0 for j in range(4)] for i, row in enumerate(copy.deepcopy(M))]
    for col in range(4):
        piv = max(range(col, 4), key=lambda r: abs(a[r][col]))
        if abs(a[piv][col]) < 1e-12: return None
        a[col], a[piv] = a[piv], a[col]
        d0 = a[col][col]
        a[col] = [x / d0 for x in a[col]]
        for r in range(4):
            if r != col:
                f = a[r][col]
                a[r] = [a[r][k] - f * a[col][k] for k in range(8)]
    return [row[4:] for row in a]

# --- walk FIFO, track XF matrices + draws (reuse minimal sizing) ---
vcd_lo = vcd_hi = 0; vat = [[0,0,0] for _ in range(8)]
xfmem = [0.0]*0x100; loaded = {}
def vsz(v):
    a = vat[v][0]; s = 0
    if vcd_lo & 1: s += 1
    for i in range(8):
        if (vcd_lo>>(1+i))&1: s += 1
    pos=(vcd_lo>>9)&3
    if pos: s += (pos-1) if pos>=2 else ({0:1,1:1,2:2,3:2,4:4}[(a>>1)&7]*(3 if a&1 else 2))
    nrm=(vcd_lo>>11)&3
    if nrm: s += (nrm-1) if nrm>=2 else 12
    for sh in (13,15):
        cl=(vcd_lo>>sh)&3
        if cl: s += (cl-1) if cl>=2 else 4
    for t in range(8):
        tx=(vcd_hi>>(t*2))&3
        if tx: s += (tx-1) if tx>=2 else 8
    return s

view = None
p = 0; draws = []
while p < n:
    op = fifo[p]; p += 1
    if op == 0: continue
    if op == 0x08:
        a=fifo[p]; v,=struct.unpack_from(">I",fifo,p+1); p+=5
        if a==0x50: vcd_lo=v
        elif a==0x60: vcd_hi=v
        elif 0x70<=a<=0x77: vat[a&7][0]=v
    elif op==0x10:
        h,=struct.unpack_from(">I",fifo,p);p+=4;cnt=((h>>16)&0xF)+1;addr=h&0xFFFF
        vals=struct.unpack_from(">%df"%cnt,fifo,p);p+=cnt*4
        if addr+cnt<=0x100:
            for i in range(cnt): xfmem[addr+i]=vals[i]
            loaded[(addr//12)*12]=True
    elif op==0x61: p+=4
    elif op in (0x20,0x28,0x30,0x38): p+=4
    elif 0x80<=op<=0xBF:
        cnt,=struct.unpack_from(">H",fifo,p);p+=2;s=vsz(op&7)
        if not s or not cnt: break
        mats={sl:[xfmem[sl:sl+4],xfmem[sl+4:sl+8],xfmem[sl+8:sl+12]] for sl in sorted(loaded)}
        draws.append((cnt,mats)); loaded={}; p+=cnt*s
    else: break

# view = first draw with a SINGLE matrix whose Z translate is a real camera
# distance (|z|>500 => a world-space 3D object PNMTX = view*identity = view).
print("=== single-matrix draw candidates (z translate) ===")
for cnt,mats in draws[:60]:
    if len(mats)==1:
        sl=list(mats)[0]; m=mats[sl]
        print(f"  verts={cnt} slot{sl//12} t=({m[0][3]:.1f},{m[1][3]:.1f},{m[2][3]:.1f})")
for cnt,mats in draws:
    if len(mats)==1:
        sl=list(mats)[0]; m=mats[sl]
        if abs(m[2][3])>500:
            view=mat4_from_rows(m)
            print(f"\nview = {cnt}-vert single-mat draw, t=({m[0][3]:.1f},{m[1][3]:.1f},{m[2][3]:.1f})")
            break
if view is None:
    print("no view candidate"); sys.exit()
vinv = mat_inv(view)

print("\n=== per-bone skinMtx = view^-1 * M[slot] for the multi-bone (character) draws ===")
shown=0
for cnt,mats in draws:
    if len(mats) >= 6:   # skinned char PObj (many bones)
        print(f"\ndraw verts={cnt} bones={len(mats)}:")
        for sl,m in list(mats.items())[:10]:
            M=mat4_from_rows(m)
            sk=mat_mul(vinv,M)
            t=(sk[0][3],sk[1][3],sk[2][3])
            # rotation identity-ness: diag near 1, off near 0
            diag=abs(sk[0][0]-1)+abs(sk[1][1]-1)+abs(sk[2][2]-1)
            ident = diag<0.05 and abs(t[0])+abs(t[1])+abs(t[2])<0.5
            print(f"  slot {sl//12}: skin translate=({t[0]:.2f},{t[1]:.2f},{t[2]:.2f}) diag~1err={diag:.2f} {'<= ~IDENTITY (model-space verts)' if ident else ''}")
        shown+=1
        if shown>=4: break
