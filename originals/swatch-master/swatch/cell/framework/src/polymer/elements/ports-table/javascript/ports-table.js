Polymer({
  is: 'ports-table',
  properties: {
    portcollection: {
      type: Object,
      value: function() { return {};}
    },
    name: String,
    isopened: String
  },
  toggle: function(event) {
    event.preventDefault()
    this.$.link_table.toggle();
    this.fire(
      'toggled',
      {
        sender: this.portcollection.id,
        opened: this.$.link_table.opened
      }
    );
  },
  open: function () {
    this.$.link_table.set("opened", true);
    this.fire(
      'toggled',
      {
        sender: this.portcollection.id,
        opened: this.$.link_table.opened
      }
    );
  },
  close: function () {
    this.$.link_table.set("opened", false);
    this.fire(
      'toggled',
      {
        sender: this.portcollection.id,
        opened: this.$.link_table.opened
      }
    );
  },
  getMetricIdArray: function(metrics){
    return Object.keys(metrics);
  },
  getMetricStatus: function(port, metricId) {
    if (port.metrics[metricId] === undefined)
      return "Unknown";
    else
      return port.metrics[metricId].status;
  },
  getMetricValue: function(port, metricId) { 
    if (port.metrics[metricId] === undefined)
      return "";
    else
      return port.metrics[metricId].value;
  },
  getMergedMetricIdArray: function (portcollections) {
    // is it cached?
    if (this.header !== undefined) return this.header;

    var idArrayCollection = [];
    // Preparing an array of array of ids
    portcollections.forEach(function (portcollection) {
      var idArray = this.getMetricIdArray(portcollection[0].metrics);
      idArrayCollection.push(idArray);
    }.bind(this));
    // Finding common metrics
    var intersection = idArrayCollection[0];
    idArrayCollection.forEach(function (idArray) {
      intersection = intersection.filter(function(value) { return -1 !== idArray.indexOf(value)});
    }.bind(this));
    //Remove common elements from original ids and adding it to the intersection
    idArrayCollection.forEach(function (idArray) {
      idArray = idArray.filter( function (el) { return !intersection.includes( el )} );
      intersection = intersection.concat(idArray);
    }.bind(this));
    //cache it
    this.set("header", intersection);
    return intersection;
  },
  getSortedMetricIdArray: function(portcollections, port){
    //find max length for table column
    var maxLength = 0;
    portcollections.forEach(function (ports) {
      if (this.getMetricIdArray(ports[0].metrics).length > maxLength) maxLength = this.getMetricIdArray(ports[0].metrics).length;
    }.bind(this));
    // taking mega header
    var header = this.getMergedMetricIdArray(portcollections);
    // removing metrics non belonging to that port type
    header = header.filter(function (el) { return this.getMetricIdArray(port.metrics).includes( el )}.bind(this) );
    //adding missing headers to have a consistent number of rows
    for (var x = header.size; x < maxLength; x++) {
      header.push("");
    }
    return header;
  },
  getHeadingClass: function(status) {
    return 'heading '+status+'-faded-bkg';
  },
  getPortTD: function(portID) {
    return Polymer.dom(this.root).querySelector("#"+portID);
  },
  setNoAnimation: function (noanim) {
    Polymer.dom(this.root).querySelector("#link_table").noAnimation = noanim;
    return;
  },
  getCapitalBoolean: function (flag){
    switch(flag){
      case true: return "True"; break;
      case false: return "False"; break;
    }
    return "Invalid flag";
  },
  isNotEmpty: function(status) {
    return !(status === undefined);
  },
  getMetricStatusClass: function(status) {
    return 'metricstatus '+status+'-bkg';
  },
  isMonitored: function (monitoringStatus) {
    return monitoringStatus == "Enabled";
  },
  isDisabled: function (monitoringStatus) {
    return monitoringStatus == "Disabled";
  },
  isNonCritical: function (monitoringStatus) {
    return monitoringStatus == "Non critical";
  },
  isWarning: function (metric) {
    return (this.isMonitored(metric.monitoringstatus)) && (metric.status == "Warning");
  },
  isError: function (metric) {
    return (this.isMonitored(metric.monitoringstatus)) && (metric.status == "Error");
  }
});