#!/usr/bin/env python3
import sys
import time
import struct
import os.path
import serial
import threading
import re
import subprocess
from paco import util
from PIL import Image


port="/dev/ttyUSB0"
baud=115200
fRunRocket=False
fRunPython=False
filenames=[]

def print_help(f):
  f.write(
    "filter-image.py [options] file1 [file2 ...]\n"
    "applies gamma correction to a specified image\n"
    "options:\n"
    "  --rocket\n"
    "    run the filter on the rocket core.\n"
    "  --python\n"
    "    run the filter in python (quality reference)\n"
    "  -p|--port <port>\n"
    "    specify the port to be used for UART communication with the rocket\n"
    "    SoC. Default: {port}\n"
    "  -b|--baud <baudrate>\n"
    "    specify the baud rate used by the UART interface. default: {baud}\n"
    "".format(port=port,baud=baud)
    )

try:
  s=None
  for arg in sys.argv[1:]:
    if s==None: 
      if arg[:1]=="-":
        if arg in {"--rocket"}: fRunRocket=True
        elif arg in {"--python"}: fRunPython=True
        elif arg in {"-p","--port"}: s="--port"
        elif arg in {"-b","--baud"}: s="--baud"
        elif arg in {"-h","--help"}:
          print_help(sys.stdout)
          sys.exit(0)
        else:
          raise Exception("unrecognized switch: %s"%arg)
      else:
        filenames.append(arg)
    elif s=="--baud":
      baud=int(arg)
      s=None
    elif s=="--port":
      port=arg
      s=None
  
  if len(filenames)<1:
    raise Exception("no filename specified")
  
  if (fRunRocket+fRunPython)<1:
    raise Exception("no target selected to run on")

except Exception as e:
  sys.stderr.write("\x1b[31;1mERROR\x1b[30;0m: %s\n"%e)
  print_help(sys.stderr)
  raise
  sys.exit(1)
      
          

CMD_PUT_BLOCK=0x12
CMD_GET_BLOCK=0x32
CMD_EXECUTE=0x44
CMD_INFO=0x01

block_size=100000

EXPONENT=1.99

class ReadBuffer(threading.Thread):
  def __init__(s,iface):
    s._iface=iface
    s._mutex=threading.Lock()
    s._cond=threading.Condition(s._mutex)
    s._buffer=[]
    threading.Thread.__init__(s)

  def run(s):
    s._running=True
    while s._running:
      buf=s._iface.read(1)
      with s._mutex:
        s._buffer.append(buf[0])
        s._cond.notifyAll()
  
  def terminate(s):
    s._running=False

  def write(s,data):
    s._iface.write(data)

  def read(s,cb):
    with s._mutex:
      while len(s._buffer)<cb:
        s._cond.wait()
      res=bytes(s._buffer[:cb])
      s._buffer=s._buffer[cb:]
      return res

if fRunRocket:
  with open("mode.h","w") as f:
    f.write("#define MODE MODE_STATIC")

if fRunRocket and False:
  rocket=ReadBuffer(serial.Serial(port=port,baudrate=baud))
  rocket.start()

  for i in range(120):
    rocket.write(struct.pack("<B",CMD_INFO))
    (block_size,)=struct.unpack("<I",rocket.read(4))
  print("rocket connected. block size: %i"%(block_size))

def rocket_process_block(buf,res1,res2):
  with open("image.h","w") as f:
    f.write("uint32_t image[] = {%s};\n"%",".join(["0x%xuL"%(v<<16) for v in buf]))
  p=subprocess.Popen("CMP_FIXMATH=1 make run",shell=True,stdout=subprocess.PIPE)
  e=re.compile(".*\x1b\\[3.;1m([0-9a-fA-F]+) ([0-9a-fA-F]+).*")
  n_read1=0
  n_read2=0
  state=0
  for ln in p.stdout:
    ln=ln.decode()
    if ln.find("computing image..")!=-1:
      state=1
      sys.stdout.write(ln)
      continue
    elif ln.find("computing image (fixmath)..")!=-1:
      state=2
      sys.stdout.write(ln)
      continue
    elif state==0:
      sys.stdout.write(ln)
      continue

    m=e.match(ln.strip())
    if not m: 
      sys.stdout.write(ln)
      continue
    
    # perform conversion to signed integer
    v=int(m.groups()[1],16)%0xffffffff
    if v&0x80000000:
      v-=0x100000000

    # only use top 8 bits
    v>>=16

    if state==1:
      res1.append(v)
      n_read1+=1
    elif state==2:
      res2.append(v)
      n_read2+=1
  print(
    "%i -> %i,%i (total: %i,%i)"%(len(buf),n_read1,n_read2,len(res1),len(res2)))
  return res1,res2
  

def rocket_process(buf,pb):
  res1=[]
  res2=[]

  for i in range(0,len(buf),block_size):
    rocket_process_block(buf[i:i+block_size],res1,res2)
  return res1,res2

def python_process(buf,pb):
  res=list(buf)

  for i,v in enumerate(buf):
    res[i]= min(2**8-1,max(0,int(255.0/(255.0**EXPONENT)*v**EXPONENT)))
    pb.increment()
  return res

for fn in filenames:
  print("loading file %s"%fn)
  img=Image.open(fn)
  in_data=img.getdata()
  print("done. size: %s x %s"%img.size)
  
  in_comp=[
    [ pix[k] for pix in in_data ]
    for k in range(3) ]
  rocket_comp_lut=[
    [ 0 for v in strm]
    for strm in in_comp]
  rocket_comp_fix=[
    [ 0 for v in strm]
    for strm in in_comp]
  python_comp=[
    strm
    for strm in in_comp]
  
  if fRunRocket:
    sys.stdout.write("  computing on rocket.. ")
    sys.stdout.flush()
    rocket_comp_lut=[]
    rocket_comp_fix=[]
    with util.ProgressBar(0,len(in_data)*len(in_comp)) as pb:
      for strm in in_comp:
        (lut,fix)=rocket_process(strm,pb)
        rocket_comp_lut.append(lut)
        rocket_comp_fix.append(fix)

  if fRunPython:

    sys.stdout.write("  computing on host.. ")
    sys.stdout.flush()
    with util.ProgressBar(0,len(in_data)*len(in_comp)) as pb:
      python_comp=[
        python_process(strm,pb)
        for strm in in_comp]
  
  
  pix_out=b""
  print("  outputting..") 
  (w,h)=img.size
  for y in reversed(range(h)):
    for comps in [ in_comp,rocket_comp_lut,rocket_comp_fix,python_comp ]:
      pix_out+= (
        b"".join(
          [ 
            struct.pack(
              "BBB",
              *[min(255,max(0,comps[k][y*w+x])) for k in range(len(in_comp))]) 
            for x in range(w) ]))

  img_out=Image.frombuffer("RGB",(w*4,h),pix_out)
  
  (fc,fe)=os.path.splitext(fn)
  img_out.save("%s-out%s"%(fc,fe))

if fRunRocket and False:
  rocket.terminate()

