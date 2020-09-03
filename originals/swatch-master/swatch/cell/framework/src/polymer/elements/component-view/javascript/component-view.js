Polymer({
    is: "component-view",
    properties: {
        components: {
            type: Array,
            value: []
        },
        selected: {
            type: String,
            value: 0
        },
        component: {
            type: Object,
            notify: true
        },
        selectedLabel: {
            type: String,
            value: ""
        },
        openedSubmonitorables: {
          type: Array,
          value: function () { return [];}
        }
    },
    report: function(event, detail, sender) {
        //console.log("component-view report");
        this.openedSubmonitorables[0] = {};
        this.openedSubmonitorables[0].id = this.selectedLabel;
        var metrictables = Polymer.dom(this.root).querySelectorAll("metric-table");
        this.openedSubmonitorables[0].openedSubmonitorables = [];
        for(var x = 0; x < metrictables.length; x++) metrictables[x].close();
        Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
        console.log(JSON.stringify(this.openedSubmonitorables));
        Polymer.dom(this.root).querySelector("#autoupdateprocessor").manualRefresh();
    },
    getLabel: function(status) {
    if (status == "") return 'label statusspan NoLimit-bkg';
    switch (status)
    {
      case "Good":
      case "Warning":
      case "Error":
      case "NoLimit":
      case "Enabled":
      case "Disabled":
        return 'label statusspan '+status+'-bkg';
      break;
      case "Non critical":
        return 'label statusspan NonCritical-bkg';

    }
        return 'label statusspan '+status+'-bkg';
    },
    getSubCompClass: function(status) {
        return 'cmp '+status+'-bkg';
    },
    showPortMetric: function(event) {
      var rootTarget = Polymer.dom(event).rootTarget;
      if (rootTarget) {
        var metricTableRoot = {};
        switch (event.detail.portType)
        {
          case "input":
            metricTableRoot = Polymer.dom(Polymer.dom(this.root).querySelector('#inputPorts').root);
            metricTableRoot.querySelector("#metric_table").set("noAnimation", true);
            Polymer.dom(this.root).querySelector('#inputPorts').open();
            metricTableRoot.querySelector("#metric_table").set("noAnimation", false);
          break;
          case "output":
            metricTableRoot = Polymer.dom(Polymer.dom(this.root).querySelector('#outputPorts').root);
            metricTableRoot.querySelector("#metric_table").set("noAnimation", true);
            Polymer.dom(this.root).querySelector('#outputPorts').open();
            metricTableRoot.querySelector("#metric_table").set("noAnimation", false);
          break;
          case "amc":
            metricTableRoot = Polymer.dom(Polymer.dom(this.root).querySelector('#amcports').root);
            metricTableRoot.querySelector("#metric_table").set("noAnimation", true);
            Polymer.dom(this.root).querySelector('#amcports').open();
            metricTableRoot.querySelector("#metric_table").set("noAnimation", false);
          break;
        }
      //I have to enter in the dom root and open the metric-table
        metricTableRoot.querySelector('#' + event.detail.portId).open();
        metricTableRoot.querySelector('#' + event.detail.portId).scrollIntoView();
    }
  },
  isProcessor: function( item ) {
    if (item == null ) return false;
    return item.type !== "daqttc";
  },
  isDaqTTC: function( item ) {
    if (item == null ) return false;
    return item.type == "daqttc";
  },
  isInputPorts: function( item ) {
    if (item == null ) return false;
    return item.type == "inputPorts";
  },
  isOutputPorts: function( item ) {
    if (item == null ) return false;
    return item.type == "outputPorts";
  },
  isAMCPorts: function( item ) {
    if (item == null ) return false;
    return item.type == "amcPorts";
  },
  showComponent: function(e){
    var component = e.target.attributes["hover-data"].value;
    //Polymer.dom(this.root).querySelector('#' + component).isopened = "true";
    Polymer.dom(this.root).querySelector("#" + component).open();
    Polymer.dom(this.root).querySelector('#' + component).scrollIntoView();
  },
  attached: function() {
    Polymer.dom(document).querySelector("#monitoringtitle").set("subtitle", "Component view");
  },
  updateOpenedSubmonitorables: function (event){

    var senderObject = {
      id: event.detail.sender,
    }

    if (event.detail.opened) {
      senderObject.openedSubmonitorables = event.detail.openedSubmonitorables;
      this.openedSubmonitorables[0].openedSubmonitorables.push(senderObject);
      Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
      //console.log ( JSON.stringify(this.openedSubmonitorables));
      //If I add a component I need a new request
      Polymer.dom(this.root).querySelector("#autoupdateprocessor").manualRefresh();
    }
    else {

      var found = false;
      var indexToRemove = -1;
      //Looking for the element to remove
      for(var x = 0; (x <  this.openedSubmonitorables[0].openedSubmonitorables.length) && (!found) ; x++){
        if (this.openedSubmonitorables[0].openedSubmonitorables[x].id == senderObject.id) {
          found = true;
          indexToRemove = x;
        }
      }

      if (indexToRemove >= 0) this.openedSubmonitorables[0].openedSubmonitorables.splice(indexToRemove, 1);

      Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
      //console.log ( JSON.stringify(this.openedSubmonitorables));
    }



  },
  updateJSON: function (event) {

    Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
    //console.log ( JSON.stringify(this.openedSubmonitorables));
    if(event.detail.opened) Polymer.dom(this.root).querySelector("#autoupdateprocessor").manualRefresh();

  }
});
