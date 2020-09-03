Polymer({
  is: 'metric-table',
  properties: {
    object: {
      type: Object,
      value: function () { return {}; },
      observer: 'refreshItems'
    },
    name: String,
    recursive: Boolean,
    sortAlphabetically: {
      type: Boolean,
      value: false
    },
    openedSubmonitorables: {
      type: Array,
      value: function () { return []; }
    },
    isOpened: {
      type: Boolean,
      value: false
    },
    columns: {
      type: Array,
      value: function () {
        return [
          {name: "Metric ID", renderer: this.valueColumnRenderer.bind(this), sortable: true},
          {name:"Value"},
          {name: "Last updated (UTC)"},
          {name: "Monitoring"},
          {name: "Error Condition"},
          {name: "Warning Condition"}
        ];
      }
    },
    oldMetricsNumber: {
      type: Number,
      value: -1
    }
  },
  toggle: function(event, detail, sender) {
    event.preventDefault();
    
    var collapseopened = this.$.metric_table.opened;
    if (!collapseopened) {
      this.open();
    } else {
      this.close();
    }
  },
  getHeadingClass: function(status) {    
    return 'heading '+status+'-faded-bkg'
  },
  isNotEmpty: function(status) {
    return !(status === undefined);
  },
  getMetricStatusClass: function(status) {
    return 'metricstatus '+status+'-bkg';
  },
  getCapitalBoolean: function (flag){
    switch(flag){
      case true: return "True"; break;
      case false: return "False"; break;
    }
    return "Invalid flag";
  },
  //Tracks the opened children
  updateOpenedSubmonitorables: function (event){

    var senderObject = {
      id: event.detail.sender,
    }

    if (event.detail.opened) {
      senderObject.openedSubmonitorables = event.detail.openedSubmonitorables;
      this.openedSubmonitorables.push(senderObject);
    }
    else {

      var found = false;
      var indexToRemove = -1;
      //Looking for the element to remove
      for(var x = 0; (x < this.openedSubmonitorables.length) && (!found) ; x++){
        if (this.openedSubmonitorables[x].id == senderObject.id) {
          found = true;
          indexToRemove = x;
        }
      }

      if (indexToRemove >= 0) this.openedSubmonitorables.splice(indexToRemove, 1);
    }

    //toggle must be handled only by the direct parent
    event.stopPropagation();

    this.fire(
      'children-changed',
      {
        sender: this.object.id,
        opened: event.detail.opened
      }
    );

  },
  open: function () {
    if (!this.$.metric_table.opened){
      this.$.metric_table.toggle();      
      this.fire(
        'toggled',
        {
          sender: this.object.id,
          openedSubmonitorables: this.openedSubmonitorables,
          opened: this.$.metric_table.opened
        }
      );
    }
  },
  close: function () {
    if (this.$.metric_table.opened){
      this.$.metric_table.toggle();
      //If it is closed it must be removed from the array, so I don't need the openedSubmonitorables because I'm going to look for the ID and to remove the element
      this.fire(
        'toggled',
        {
          sender: this.object.id,
          opened: this.$.metric_table.opened
        }
      );
      //Also all the sub metric-table will be closed.
      var metrictables = Polymer.dom(this.root).querySelectorAll("metric-table");
      for(var x = 0; x < metrictables.length; x++) metrictables[x].close();
      this.openedSubmonitorables = [];
    }
  },
  rowify: function (metrics) {
    //Prevents crash when the table is created without metrics
    if (!metrics) return;
    var rows = [];
    for(var x = 0; x < metrics.length; x++) {
      var row = {
        "Device Path": metrics[x].path,
        "Metric ID": {
          status: metrics[x].status,
          id: metrics[x].id
        },
        "Value": metrics[x].value,
        "Last updated (UTC)": metrics[x].timestamp,
        "Monitoring": metrics[x].monitoringstatus,
        "Error Condition": metrics[x].errorCondition,
        "Warning Condition": metrics[x].warningCondition
      };
      rows.push(row);
    }
    return rows;
  },
  valueColumnRenderer: function (cell) {
    cell.element.innerHTML = "";
    //<div title="Metric Status: {{metric.status}}" class$="{{getMetricStatusClass(metric.status)}}"></div>{{metric.id}}
    var status = document.createElement("monitorable-status");
    status.setAttribute("status", cell.data.status);
    var id = document.createTextNode(cell.data.id);
    cell.element.appendChild(status);
    cell.element.appendChild(id);
  },
  sortTable: function (event) {

    if(!this.object.metrics) return;
    if(!this.object.metrics.length) return;

    //We have 2 sorts, by metric and by path
    var table = this.$$("#table");
    var columnName = table.columns[table.sortOrder[0].column].name;
    var direction = table.sortOrder[0].direction == 'asc' ? -1 : 1;
    var sort = function(row1, row2) {
      var row1Data;
      var row2Data;
      row1Data = row1[columnName].id;
      row2Data = row2[columnName].id;
      return (row1Data < row2Data) ? direction : -direction;
    }
    table.items.sort(sort);

    return;
  },
  //Refreshing the table if a sort is selected
  refreshItems: function (newValue, oldValue) {

    var table = this.$$("#table");
    var containerDiv = this.$$("#container");

    //If the number of rows changes for whatever reason I rescale the div resetting the height to auto
    if((newValue) && (newValue.metrics)) {
      if (this.oldMetricsNumber != newValue.metrics.length){
        containerDiv.style.height = "auto";
        this.oldMetricsNumber = newValue.metrics.length;
        //Then I wait for the updateHeight event to do its job
      }
    }

    //I have to check if the table exists, because I have a dom-if
    //Updating sorting in case of an auto-update
    if (table) {
      if((table.sortOrder)&&(table.sortOrder.length)){
        this.sortTable();
        table.refreshItems();
      }
    }
  },
  //When refreshing a table, it gets momentarily empty, causing the div to have 0px height
  //To fix this I can let the div auto scale until it has content, then fix the height to this value
  updateHeight: function () {
    var containerDiv = this.$$("#container");
    var computedHeight = window.getComputedStyle(containerDiv).height;
    //At the beginning the height is 0, so I don't have to save it
    if ((computedHeight != "0px") && (containerDiv.style.height == "auto")){
      containerDiv.style.height = window.getComputedStyle(containerDiv).height;
    }
  },
  sortByMetricID: function (rowA, rowB) {
    return (rowA.id < rowB.id) ? -1 : +1;
  },
  sortByMetricIDAndErrorWarningFirst: function (rowA, rowB) {
    //If I am not being monitored I will be sorted alphabetically after the monitored ones
    //Error before everything
    if ((rowA.monitoringstatus == "Enabled") && (rowB.monitoringstatus == "Enabled")) {
      if ((rowA.status == "Error") && (rowB.status != "Error")) return -1;
      if ((rowA.status != "Error") && (rowB.status == "Error")) return +1;
      //Warning before everything, but after error
      if ((rowA.status == "Error") && (rowB.status == "Warning")) return -1;
      if ((rowA.status == "Warning") && (rowB.status == "Error")) return +1;
      if ((rowA.status == "Warning") && (rowB.status != "Warning")) return -1;
      if ((rowA.status != "Warning") && (rowB.status == "Warning")) return +1;
      //If we get here, then we have the same status, we sort alphabetically, then.
      return (rowA.id < rowB.id) ? -1 : +1;
    }
    if ((rowB.monitoringstatus != "Enabled") && (rowA.monitoringstatus == "Enabled")) return -1;
    if ((rowA.monitoringstatus != "Enabled") && (rowB.monitoringstatus == "Enabled")) return +1;
    // If we get here we have two non-enabled metrics, we sort alphabetically, then
    return (rowA.id < rowB.id) ? -1 : +1;
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
  },
  sortFunction: function (rowA, rowB) {
    if (this.sortAlphabetically) {
      return this.sortByMetricID(rowA, rowB);
    } else {
      return this.sortByMetricIDAndErrorWarningFirst(rowA, rowB);
    }
  },
  updateSorting: function(event){
    this.$$("#metricRepeater").render();
    return;
  }
});
