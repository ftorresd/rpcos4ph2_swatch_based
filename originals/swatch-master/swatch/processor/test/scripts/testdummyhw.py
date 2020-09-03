#!/usr/bin/env python

import threading
import subprocess
import time
import uhal
import sys, getopt

#argument parser
def parser(argv):
   testmode = ''
   try:
      opts, args = getopt.getopt(argv,"hm:",["mode="])
   except getopt.GetoptError:
      print 'usage: python testdummyhw.py (default: build a full processor)'
      print ' '
      print 'or'
      print ' '
      print 'usage: python testdummyhw.py -m <testmode>'
      print 'use -h option for more info about test modes'
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'usage: python testdummyhw.py (default: build a full processor)'
         print ' '
         print 'or'
         print ' '
         print 'usage: python testdummyhw.py -m <testmode>'
         print 'available modes:'
         print '  - ctrl_build          : build ctrl block'
         print '  - ttc_build           : build ttc block'
         print '  - chnctrl_build       : build channel ctrl block'
         print '  - chnbuff_build       : build channel buffer block'
         print '  - channel_build       : build channel (ctrl+buffer) block'
         print '  - info_build          : build info block'
         print '  - connection          : test connection object'
         sys.exit()
      elif opt in ("-m", "--mode"):
         testmode = arg
   print 'tested block is ', testmode

uhal.setLogLevelTo(uhal.LogLevel.ERROR)

class DummyHWThread( threading.Thread ):

    def __init__( self, name, port, addr, values={} ):
        threading.Thread.__init__(self)
        self.name = name
        self.port = port
        self.addr = addr
        self._values = values
        self._terminate = False
        self._kill = False
        self._code = None
        self._subp = None
        self.hw = None

    def __del__(self):
        # print self._subp.poll();
        if self._subp and self._subp.poll() == None:
            self._subp.kill()


    def run(self):

        cmd = ['/opt/cactus/bin/uhal/tests/DummyHardwareUdp.exe','-p'+str(self.port), '-v2']
        self._subp = subprocess.Popen(cmd, cwd='/tmp')
        dummyhw = self._subp

        print "Let it run for 1 sec"
        time.sleep(1.)

#        if self._values:
        hw = uhal.getDevice('dummy_'+self.name,'ipbusudp-2.0://localhost:'+str(self.port), self.addr)

        for k,v in self._values.iteritems():
            hw.getNode(k).write(v)

        hw.dispatch()
        
        self.hw = hw

        retcode = None
        while( not retcode ):
            retcode = dummyhw.poll()

            if retcode is not None:
                self.code = retcode
                break
            
            if self._kill:
                try:
                    print self.name,'Game over!'
                    dummyhw.kill()
                except OSError:
                    pass
                break
            else:
                time.sleep(0.1)

    def kill(self):
        self._kill = True

    def poll(self):
        if not self._subp:
            return None
        return self._subp.poll()


class PuppetMaster:
    def __init__(self):
        self._dummies = {}

    def add(self,d):
        if d.name in self._dummies:
            raise RuntimeError(d.name+" exists")
          
        self._dummies[d.name] = d
        
    def get(self, name):
#        if name not in self._dummies:
#            raise KeyError("dummy "+name+" not found")
        return self._dummy[name]
            

    def start(self):
        for d in self._dummies.itervalues():
            print 'Starting',d.name
            d.start()
            print d.name,'started'

    def kill(self):
        for d in self._dummies.itervalues():
            print 'Killing',d.name
            d.kill()
        print 'Wait for '
        while threading.activeCount() > 1:
            time.sleep(.1)

    def monitor(self):

        while threading.activeCount() > 1:

            for d in self._dummies.itervalues():
                r = d.poll()
                if r is not None:
                    print d.name,"terminated with code",r

            time.sleep(1)

if __name__ == "__main__":
    parser(sys.argv[1:])

    addr="file:///${SWATCH_ROOT}/processor/test/etc/dummy.xml"

    common = {
      'ctrl.id.magic' : 0xc0de9,
      'ctrl.id.fwrev' : 0x11223344,
      'ctrl.id.infos.n_rx': 32,
      'ctrl.id.infos.n_tx': 18,
      'buffer.stat.size': 1024,
    }


    regs = {'ctrl.id.magic':0xc0de1234}
    regs.update(common)

    pm = PuppetMaster()

    pm.add( DummyHWThread( 'dummy1', 50001, addr,  regs ) )
#    pm.add( DummyHWThread( 'dummy2', 50011, addr, {'ctrl.id.magic':0xc0de4567} ) )

    import time
    
    try:
        # start the puppets
        pm.start()
        time.sleep(1)
        if (len(sys.argv) < 3):
            subprocess.call(['ProcessorTest.exe','processor_build'])
        else:
            subprocess.call(['ProcessorTest.exe',sys.argv[2]])
        pm.kill()  
        # wait for the puppets
        pm.monitor()
    except KeyboardInterrupt:
        pm.kill()
