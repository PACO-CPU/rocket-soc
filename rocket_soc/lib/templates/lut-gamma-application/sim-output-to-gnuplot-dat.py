import sys
import re

e=re.compile(".*\x1b\\[3.;1m([0-9a-fA-F]+) ([0-9a-fA-F]+).*")
with open("lut0.fpga.dat","w") as f:
  for ln in sys.stdin:
    m=e.match(ln.strip())
    if not m: continue
    print(m.groups())
    f.write("%s\t%s\n"%(int(m.groups()[0],16),int(m.groups()[1],16)))
  
