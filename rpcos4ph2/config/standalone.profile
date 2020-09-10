<?xml version='1.0'?>
<!-- Order of specification will determine the sequence of installation. all modules are loaded prior instantiation of plugins -->
<xp:Profile xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xp="http://xdaq.web.cern.ch/xdaq/xsd/2005/XMLProfile-11">
	<!-- Compulsory  Plugins -->
	<xp:Application class="executive::Application" id="0" group="profile" service="executive" network="local">
		<properties xmlns="urn:xdaq-application:Executive" xsi:type="soapenc:Struct">
			<logUrl xsi:type="xsd:string">console</logUrl>
                	<logLevel xsi:type="xsd:string">INFO</logLevel>
                </properties>
	</xp:Application>
	<xp:Module>${XDAQ_ROOT}/lib/libi2outils.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libb2innub.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libexecutive.so</xp:Module>

	<xp:Application class="pt::http::PeerTransportHTTP" id="1" group="profile" network="local" service="pthttp">
		 <properties xmlns="urn:xdaq-application:pt::http::PeerTransportHTTP" xsi:type="soapenc:Struct">
		 	<documentRoot xsi:type="xsd:string">${XDAQ_DOCUMENT_ROOT}</documentRoot>
            <aliasName xsi:type="xsd:string">/tmp</aliasName>
            <aliasPath xsi:type="xsd:string">/tmp</aliasPath>
            <expiresByType xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[6]">
			    <item xsi:type="soapenc:Struct" soapenc:position="[0]">
			        <name xsi:type="xsd:string">image/png</name>
			        <value xsi:type="xsd:string">PT4300H</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[1]">
			        <name xsi:type="xsd:string">image/jpg</name>
			        <value xsi:type="xsd:string">PT4300H</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[2]">
			        <name xsi:type="xsd:string">image/gif</name>
			        <value xsi:type="xsd:string">PT4300H</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[3]">
			        <name xsi:type="xsd:string">application/x-shockwave-flash</name>
			        <value xsi:type="xsd:string">PT120H</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[4]">
			        <name xsi:type="xsd:string">application/font-woff</name>
			        <value xsi:type="xsd:string">PT8600H</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[5]">
			        <name xsi:type="xsd:string">text/css</name>
			        <value xsi:type="xsd:string">PT4300H</value>
			    </item>
		   </expiresByType>
		   <httpHeaderFields xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[3]">
		   		<item xsi:type="soapenc:Struct" soapenc:position="[0]">
		        	<name xsi:type="xsd:string">Access-Control-Allow-Origin</name>
		        	<value xsi:type="xsd:string">*</value>
		    	</item>
		    	<item xsi:type="soapenc:Struct" soapenc:position="[1]">
			        <name xsi:type="xsd:string">Access-Control-Allow-Methods</name>
			        <value xsi:type="xsd:string">POST, GET, OPTIONS</value>
			    </item>
			    <item xsi:type="soapenc:Struct" soapenc:position="[2]">
			        <name xsi:type="xsd:string">Access-Control-Allow-Headers</name>
			        <value xsi:type="xsd:string">x-requested-with</value>
		    	</item>
		   </httpHeaderFields>
    	</properties>
	</xp:Application>
	<xp:Module>${XDAQ_ROOT}/lib/libpthttp.so</xp:Module>

	<xp:Application class="pt::fifo::PeerTransportFifo" id="8" group="profile" network="local" service="ptfifo"/>
	<xp:Module>${XDAQ_ROOT}/lib/libptfifo.so</xp:Module>

	<!-- XRelay -->
	<xp:Application class="xrelay::Application" id="4"  service="xrelay" group="profile" network="local"/>
	<xp:Module>${XDAQ_ROOT}/lib/libxrelay.so</xp:Module>

	<!-- HyperDAQ -->
	<xp:Application class="hyperdaq::Application" id="3" group="profile" service="hyperdaq" network="local"/>
	<xp:Module>${XDAQ_ROOT}/lib/libhyperdaq.so</xp:Module>
</xp:Profile>