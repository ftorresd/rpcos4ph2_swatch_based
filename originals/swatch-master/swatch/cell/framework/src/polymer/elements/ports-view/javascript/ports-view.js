Polymer({
  is: "ports-view",
  properties: {
    processors: Object,
    openedSubmonitorables: {
      type: Array,
      value: function () { return [];}
    }
  },
  showPortMetric: function(event) {
    var rootTarget = Polymer.dom(event).rootTarget;
		var portTableID = event.detail.processorId;
		if (rootTarget) {
			switch (event.detail.portType)
			{
				case "input":
					portTableID += "_" + "inputPorts";
				break;
				case "output":
					portTableID += "_" + "outputPorts";
				break;
			}
		}

		Polymer.dom(this.root).querySelector('#' + portTableID).setNoAnimation(true);
		Polymer.dom(this.root).querySelector('#' + portTableID).isopened = "true";
		Polymer.dom(this.root).querySelector('#' + portTableID).setNoAnimation(false);
		Polymer.dom(this.root).querySelector('#' + portTableID).getPortTD(event.detail.portId).scrollIntoView();


	},
	getID: function(procID, portcollectionID) {
		return procID + "_" + portcollectionID;
	},
  attached: function() {
    Polymer.dom(document).querySelector("#monitoringtitle").set("subtitle", "Ports Summary");
  }  
});
