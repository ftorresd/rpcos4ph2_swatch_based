Polymer({
  is: "metrics-view",
  properties: {
      metrics: {
          type: Array,
          notify: true,
          observer: 'refreshItems'
      },
      query: String,
      columns: {
        type: Array,
        value: function () {
          return [
            {name: "Device Path", sortable: true},
            {name: "Metric ID", renderer: this.valueColumnRenderer.bind(this), sortable: true},
            {name:"Value"},
            {name: "Last updated (UTC)"},
            {name: "Monitoring"},
            {name: "Error Condition"},
            {name: "Warning Condition"}
          ];
        }
      }
  },
  isProcessor: function( item ) {
    return item.type !== "daqttc";
  },
  isDaqTTC: function( item ) {
    return item.type == "daqttc";
  },
  isMetric: function(obj) {
    return (obj.type == "metric");
  },
  isNotMetric: function(obj) {
    return !(obj.type == "metric");
  },
  getPath: function(string1, string2, string3) {
  var path = string1 + "." + string2;
  if (string3) path += "." + string3;
  return path;
  },
  getMetricStatusClass: function(status) {
    return 'metricstatus '+status+'-bkg';
  },
  rowify: function (metrics) {
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
  attached: function() {
    this.$.table.addEventListener('sort-order-changed', this.sortTable.bind(this));
  },
  sortTable: function () {

    if(this.metrics.length == 0) return;
    var table = this.$.table;
    //We have 2 sorts, by metric and by path


    var secondarySort = function() {return 0};
    //If there is a secondary sort
    if (table.sortOrder[1]) {
      var columnNameSecondary = table.columns[table.sortOrder[1].column].name;
      var directionSecondary = table.sortOrder[1].direction == 'asc' ? -1 : 1;
      secondarySort = function(row1, row2) {
        var row1Data;
        var row2Data;
        if (columnNameSecondary == "Metric ID"){
          row1Data = row1[columnNameSecondary].id;
          row2Data = row2[columnNameSecondary].id;
        } else if (columnNameSecondary == "Device Path") {
          row1Data = row1[columnNameSecondary];
          row2Data = row2[columnNameSecondary];
        } else {
          console.error("Sorting column " + columnNameSecondary + " not found!");
        }
        return (row1Data < row2Data) ? directionSecondary : -directionSecondary;
      }
    }
    // primary sort
    var columnName = table.columns[table.sortOrder[0].column].name;
    var direction = table.sortOrder[0].direction == 'asc' ? -1 : 1;
    var sort = function(row1, row2) {
      var row1Data;
      var row2Data;
      if (columnName == "Metric ID"){
        row1Data = row1[columnName].id;
        row2Data = row2[columnName].id;
      } else if (columnName == "Device Path") {
        row1Data = row1[columnName];
        row2Data = row2[columnName];
      } else {
        console.error("Sorting column " + columnName + " not found!");
      }
      var result = (row1Data < row2Data) ? direction : -direction;
      if (row1Data == row2Data) {
        result = secondarySort(row1, row2);
      }
      return result;
    }

    table.items.sort(sort);

    return;
  },
  //Refreshing the table if a sort is selected
  refreshItems: function () {
    var table = this.$.table;
    if((table.sortOrder) && (table.sortOrder.length)) {
      this.sortTable();
      table.refreshItems();
    }
    return;

  }
});
