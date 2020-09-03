#!/usr/bin/env python

import json
import pprint
from lxml import etree
import sys

if len(sys.argv) != 2:
    print "Invalid usage!"
    print "  Script must have one argument, path to JSON-format system description file."
    sys.exit(1)
else:
    jsonFilePath = sys.argv[1]

with open(jsonFilePath) as hwjson_data:
    hwjson = json.load(hwjson_data)

# pprint.pprint(hwjson)

jSystem = hwjson['SYSTEM']

# print jSystem['NAME']

# Create system
xSystem = etree.Element('system', id=jSystem['NAME'])

etree.SubElement(xSystem,'creator').text = jSystem['CREATOR']

# Convert crates first
xCrates = etree.SubElement(xSystem, 'crates')
for jCrate in jSystem['CRATES']:
    xCrate = etree.SubElement(xCrates, 'crate', id=jCrate['NAME'])
    etree.SubElement(xCrate,'description').text = jCrate['DESCRIPTION']
    etree.SubElement(xCrate,'location').text = jCrate['LOCATION']

# Then Processor
xProcessors = etree.SubElement(xSystem, 'processors')
for jProcessor in jSystem['PROCESSORS']:
    xProcessor = etree.SubElement(xProcessors,'processor', id=jProcessor['NAME'])
    etree.SubElement(xProcessor,'creator').text = jProcessor['CREATOR']
    etree.SubElement(xProcessor,'hw-type').text = jProcessor['HARDWARE TYPE']
    etree.SubElement(xProcessor,'role').text = jProcessor['ROLE']
    etree.SubElement(xProcessor,'uri').text = jProcessor['URI']
    etree.SubElement(xProcessor,'address-table').text = jProcessor['ADDRESS TABLE']
    etree.SubElement(xProcessor,'crate').text = jProcessor['CRATE NAME']
    etree.SubElement(xProcessor,'slot').text = str(jProcessor['CRATE SLOT'])

    for jRxPort in jProcessor['RX PORTS']:
        etree.SubElement(xProcessor, 'rx-port', name=jRxPort['NAME'], pid=jRxPort['PID'])
    for jTxPort in jProcessor['TX PORTS']:
        etree.SubElement(xProcessor, 'tx-port', name=jTxPort['NAME'], pid=jTxPort['PID'])

# Followed by Daqttcs
#
#  u'DAQTTCS': [{u'ADDRESS TABLE T1': u'file:///opt/cactus/etc/amc13/AMC13XG_T1.xml',
#                u'ADDRESS TABLE T2': u'file:///opt/cactus/etc/amc13/AMC13XG_T2.xml',
#                u'CRATE NAME': u'S2D11-29',
#                u'CRATE SLOT': 13,
#                u'CREATOR': u'swatch::amc13::AMC13Manager',
#                u'FED ID': 1360,
#                u'NAME': u'AMC13',
#                u'ROLE': u'daqttc',
#                u'URI T1': u'chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t1:50001',
#                u'URI T2': u'chtcp-2.0://ctrl-calol2:10203?target=amc-s2d11-29-13-t2:50001'}],
xDaqTtcMgrs = etree.SubElement(xSystem, 'daqttc-mgrs')
for jDadTTC in jSystem['DAQTTCS']:
    xDadTTC = etree.SubElement(xDaqTtcMgrs,'daqttc-mgr', id=jDadTTC['NAME'])
    etree.SubElement(xDadTTC,'creator').text = jDadTTC['CREATOR']
    etree.SubElement(xDadTTC,'role').text = jDadTTC['ROLE']
    etree.SubElement(xDadTTC,'crate').text = jDadTTC['CRATE NAME']
    etree.SubElement(xDadTTC,'slot').text = str(jDadTTC['CRATE SLOT'])
    etree.SubElement(xDadTTC,'uri', id='t1').text = jDadTTC['URI T1']
    etree.SubElement(xDadTTC,'uri', id='t2').text = jDadTTC['URI T2']
    etree.SubElement(xDadTTC,'address-table', id='t1').text = jDadTTC['ADDRESS TABLE T1']
    etree.SubElement(xDadTTC,'address-table', id='t2').text = jDadTTC['ADDRESS TABLE T2']
    etree.SubElement(xDadTTC,'fed-id').text = str(jDadTTC['FED ID'])

# Links
#
#  u'LINKS': [{u'NAME': u'MP0_Tx_[60:66]',
#              u'RX': u'DEMUX',
#              u'RX PORT': u'Rx[00:36:6]',
#              u'TX': u'MP0',
#              u'TX PORT': u'Tx[60:66]'},
xLinks = etree.SubElement(xSystem, 'links')
for jLink in jSystem['LINKS']:
    xLink = etree.SubElement(xLinks,'link', id=jLink['NAME'])
    etree.SubElement(xLink,'from').text = jLink['TX']
    etree.SubElement(xLink,'tx-port').text = jLink['TX PORT']
    etree.SubElement(xLink,'to').text = jLink['RX']
    etree.SubElement(xLink,'rx-port').text = jLink['RX PORT']


# {u'CONNECTED FEDS': [{u'FED ID': 1354,
#                                   u'RX PORTS': [u'MP0.inputPorts.Rx[10:34]',
#                                                 u'MP1.inputPorts.Rx[10:34]',
#                                                 u'MP2.inputPorts.Rx[10:34]',
#                                                 u'MP3.inputPorts.Rx[10:34]',
#                                                 u'MP4.inputPorts.Rx[10:34]',
#                                                 u'MP5.inputPorts.Rx[10:34]',
#                                                 u'MP6.inputPorts.Rx[10:34]',
#                                                 u'MP7.inputPorts.Rx[10:34]',
#                                                 u'MP8.inputPorts.Rx[10:34]']},
xConnFeds = etree.SubElement(xSystem, 'connected-feds')
for jCFed in jSystem['CONNECTED FEDS']:
    if len(jCFed['RX PORTS']) != 0:
        xCFed = etree.SubElement(xConnFeds,'connected-fed', id=str(jCFed['FED ID']))
        for jPort in jCFed['RX PORTS']:
            etree.SubElement(xCFed, 'port', id=jPort)

xExclBoards = etree.SubElement(xSystem, 'excluded-boards')
for jExcl in jSystem['EXCLUDED BOARDS']:
    etree.SubElement(xExclBoards, 'exclude', id=jExcl)

doc = etree.ElementTree(xSystem)


print etree.tostring(doc, pretty_print=True)

# doc.write(sys.stdout)
