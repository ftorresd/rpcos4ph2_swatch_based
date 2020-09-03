#!/usr/bin/python

import argparse
import logging
from random import randint
from socket import gethostname
import subprocess
import sys
import time
import xdaqclient


def measureCellMemoryUsage():
    tokens = [line for line in subprocess.check_output(['pidstat', '-l', '-r', '-C', 'xdaq', '-p', 'ALL']).split('\n') if '3333' in line][0].split()
    vsz = int(tokens[5]) / 1000.0
    rss = int(tokens[6]) / 1000.0
    return (vsz, rss)


def executeTransition(operation, transitionName, params):
    logging.debug('Running transition "{}"'.format(transitionName))
    origVSZ, origRSS = measureCellMemoryUsage()

    start = time.time()
    getattr(operation, transitionName)(params=params)
    end = time.time()

    newVSZ, newRSS = measureCellMemoryUsage()
    logging.info(('Executed transition {0:<' + str(max([len(x) for x in ALL_TRANSITIONS])) + '} ({1:.2f}s).  VSZ={2:.3f}MB (diff={3:+.3f}),  RSS={4:.3f}MB (diff={5:+.3f})').format(transitionName, end-start, newVSZ, newVSZ-origVSZ, newRSS, newRSS-origRSS))


logging.basicConfig(format="%(asctime)s : %(levelname)8s - %(message)s", level=logging.INFO)

PARAMS = {'Configuration Key': 'file://'}
ALL_TRANSITIONS = ['engage', 'setup', 'configure', 'align', 'start', 'pause', 'resume', 'stop', 'reset']
DEFAULT_TRANSITIONS = 'engage,setup,configure,align,start,stop,align,start,pause,resume,stop,reset'



if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Execute transitions of 'run control' operation")
    parser.add_argument('-i', '--iterations', type=int, default=10, help='Number of iterations')
    parser.add_argument('-t', '--transitions', default=DEFAULT_TRANSITIONS, help='List of transitions to execute (comma-separated)') # default='engage,reset',

    args = parser.parse_args()
    transitions = args.transitions.split(',')

    invalidTransitions = set(transitions) - set(ALL_TRANSITIONS)
    if len(invalidTransitions) > 0:
        print " ERROR : Invalid transitions were specified: {}".format(invalidTransitions)
        sys.exit(1)

    cell = xdaqclient.Cell(url = 'http://' + gethostname() + ':3333',
                           urn = 'urn:xdaq-application:lid=13')
    op = cell.Operation('Run Control')

    for i in range(args.iterations):
        for transition in transitions:
            executeTransition(op, transition, PARAMS)
