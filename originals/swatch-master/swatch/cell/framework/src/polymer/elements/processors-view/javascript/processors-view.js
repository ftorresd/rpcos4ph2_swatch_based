Polymer({
    is: "processors-view",
    properties: {
        data: Object,
        openedSubmonitorables: {
          type: Array,
          value: function () { return [];}
        }
    },
    isTTC: function(item) {
        return item.id == "ttc";
    },
    isAlgo: function(item) {
        return item.id == "algo";
    },
    isReadout: function(item) {
        return item.id == "readout";
    },
    isInputPorts: function(item) {
        return item.id == "inputPorts";
    },
    isOutputPorts: function(item) {
        return item.id == "outputPorts";
    },
    attached: function() {
      Polymer.dom(document).querySelector("#monitoringtitle").set("subtitle", "Processors summary");
    },
    updateOpenedSubmonitorables: function (event){

      var senderObject = {
        id: event.detail.sender
      }

      if (event.detail.opened) {
        if(event.detail.openedSubmonitorables !== undefined){
          senderObject.openedSubmonitorables = event.detail.openedSubmonitorables;
        }
        //let's look for what processor this submonitorable belongs to
        var processorIndex = -1;
        for(var x = 0; ((x < this.openedSubmonitorables.length)&&(processorIndex < 0)); x++) {
          //If the id of the object is the same of the name of metric-table we found the processor
          if (this.openedSubmonitorables[x].id == Polymer.dom(event).localTarget.name){
            this.openedSubmonitorables[x].openedSubmonitorables.push(senderObject);
            processorIndex = x;
          }
        }

        //If I don't find the processor I create the entry
        if(processorIndex < 0) {
          var newProcessor = {
            id: Polymer.dom(event).localTarget.name,
            openedSubmonitorables: [senderObject]
          }
          this.openedSubmonitorables.push(newProcessor);
        }

        Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
        console.log ( JSON.stringify(this.openedSubmonitorables));
        //If I add a component I need a new request
        Polymer.dom(this.root).querySelector("#autoupdateprocessor").manualRefresh();
      }
      else {

        var found = false;
        var indexToRemove = -1;
        //Looking for the element to remove
        var processorIndex = -1;
        //First we look 4 the processor
        for(var y = 0; ((y < this.openedSubmonitorables.length)&&(processorIndex < 0)); y++) {
          if (this.openedSubmonitorables[y].id == Polymer.dom(event).localTarget.name){
            processorIndex = y;
            //Then for the submonitorable
            for(var x = 0; (x <  this.openedSubmonitorables[y].openedSubmonitorables.length) && (!found) ; x++){
              if (this.openedSubmonitorables[y].openedSubmonitorables[x].id == senderObject.id) {
                this.openedSubmonitorables[y].openedSubmonitorables.splice(x, 1);
                found = true;
              }
            }
          }
        }

        Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
        console.log ( JSON.stringify(this.openedSubmonitorables));
      }

    },
    updateJSON: function (event) {

      Polymer.dom(this.root).querySelector("#autoupdateprocessor").set("monitorables", JSON.stringify(this.openedSubmonitorables));
      console.log ( JSON.stringify(this.openedSubmonitorables));
      if(event.detail.opened) Polymer.dom(this.root).querySelector("#autoupdateprocessor").manualRefresh();

    }
});
