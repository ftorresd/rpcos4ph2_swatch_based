Polymer({
    is: 'port-summary',

    properties: {
      /**
      * Fired when you click a port.
      *
      *  @event port-click
      *  @param {String} portId
      *  @param {String} processorId
      *  @param {String} portType Can be input, output or amc
       */

      /**
       * Title displayed in the summary, tipically the processor name
       */
      header: {
        type: String,
        value: "",
      },
      /**
      *  Contains the input ports data.
      *  Every element of the array must be an object of this form: {id: String, monitoringstatus: String, status: String}.
      */
      inputPorts: {
        type: Array,
        value: function () { return [];  }
      },
      /**
      *  Contains the output ports data.
      *  Every element of the array must be an object of this form: {id: String, monitoringstatus: String, status: String}.
      */
      outputPorts: {
        type: Array,
        value: function () { return [];  }
      },
      /**
      *  Contains the amc ports data.
      *  Every element of the array must be an object of this form: {id: String, monitoringstatus: String, status: String}.
      */
      amcPorts: {
        type: Array,
        value: function () { return [];  }
      },
      /**
      *  Contains the processor ID
      */
      processorId: {
        type: String,
        value: ""
      }
    },
    _isNonCritical: function (monitoringStatus) {
      return (monitoringStatus == "Non critical");
    },
    _mouseIn: function (event) {
      var text = document.createTextNode(event.detail.id)
      Polymer.dom(this.root).querySelector("#portName").appendChild(text);
    },
    _mouseOut: function (event) {
      Polymer.dom(this.root).querySelector("#portName").innerHTML = "";
    },
    _firePortClick: function (event) {
      var rootTarget = Polymer.dom(event).rootTarget;
      this.fire("port-click", {
        portId: event.detail.id,
        processorId: this.processorId,
        portType: rootTarget.attributes['port-type'].value
      });
    }
});
