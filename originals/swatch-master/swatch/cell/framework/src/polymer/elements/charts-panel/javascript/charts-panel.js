Polymer({
    is: "charts-panel",
    properties: {
      metricsList: {
        type: Array,
        value: function () {return [];}
      },
      metricSelectorOpened: {
        type: Boolean,
        value: true
      },
      metricsSnapshot: {
        type: Array,
        value: function () {return [];}
      },
      query: String,
      //keeps the list of the tracked metrics
      selectedMetricsList: {
        type: Array,
        value: function () {return [];}
      },
      maxSeriesLength: {
        type: Number,
        value: 1000
      },
      maxNumberOfSeries: {
        type: Number,
        value: 20
      },
      chartConfig: {
        type: Object,
        value: function() {
          return {
            showLegend : true,
            margin: {left: 70, top: 50, right: 50},
            showYAxis: true,
            showXAxis: true,
            useInteractiveGuideline: true,
          };
        }
      },
      chartJSConfig: {
        type: Function,
        value: function () {
          return function () {
            this._chart.xAxis
            .tickFormat(
              function (d)  {
                return d3.time.format('%X')(new Date(d));
              }
            );

            this._chart.x2Axis
            .axisLabel("Time")
            .tickFormat(
              function (d)  {
                return d3.time.format('%X')(new Date(d));
              }
            );

            this._chart.yAxis
            .axisLabel("Metric Value");

            this._chart.interactiveLayer.tooltip.headerFormatter(
              function (obj) {
                return d3.time.format('%X')(new Date(Number(obj)));
              }
            );

            this._chart.legend.margin({bottom: 15});
          }
        }// End of chartJSConfig.value
      },//end of chartJSConfig
      selectedTab: {
        type: Number,
        value: 0
      }
    },//End of properties
    report: function(event, detail, sender) {
      if (event.keyCode === 13) { // enter pressed!
        var buttonClicked = Polymer.dom(event).localTarget;
        this.query = buttonClicked.value;
        this.set("metricsList", []);
        this.$.metricsListLoader.generateRequest();
        this.deselectAllMetrics();
      }
    },
    attached: function(){
      this.$.chart.$$("paper-material").remove();
    },
    addMetric: function (event) {
      //this.selectedMetricsList.push(this.$.metricSelector.selectedItem.innerText);
      var metricSelector = Polymer.dom(this.root).querySelector("#metricSelector");
      var selectedValues = metricSelector.selectedValues;
      var metricsToPlot = [];
      var metricsList = [];

      for(var x = 0; x < selectedValues.length; x++){
        var metric = this.metricsList[selectedValues[x]];
        var alreadySelected = false;
        for (var i = 0; i < this.selectedMetricsList.length; i++) {
          if (this.selectedMetricsList[i].id == metric.id) alreadySelected = true;
        }

        if(!alreadySelected) {
          if (metric.plottable) metricsToPlot.push(metric);
          this.push("selectedMetricsList", metric);
        }
      }

      this.addSeries(metricsToPlot);

      //I start the auto-refresh, or force a new one
      var metricsList = [];

      for (var i = 0; i < this.selectedMetricsList.length; i++) {
        metricsList.push(this.selectedMetricsList[i].id);
      }

      this.$.autoupdatemetrics.set("metricsList", JSON.stringify(metricsList));
      if (this.$.autoupdatemetrics.noRefresh) {
        this.$.autoupdatemetrics.set("noRefresh", false);
      } else {
        this.$.autoupdatemetrics.manualRefresh();
      }

      this.deselectAllMetrics();
    },
    //Receives an array of [key, plottable]
    addSeries: function (selMetric) {
      if (selMetric == "") return;
      var chart = this.$$("#chart");

      for (var y = 0; (y < selMetric.length) && (chart.data.length < this.maxNumberOfSeries); y++){
        if(!selMetric[y].plottable) continue;
        //Creating the data series
        var newSeries = {
          values: [],
          //key: this.$.metricSelector.selectedItem.innerText
          key: selMetric[y].id
        };

        chart.push("data", newSeries);

      }

    },
    addPoints: function () {
      if (this.$.autoupdatemetrics.data == null) return;
      var chart = this.$$("#chart");
      var metricsValues = chart.data;

      //Flags if this function is adding the fisrt set of points in the chart
      //Used to implement a proper x scale at the beginning
      var isFirstPoint = null;

      var chartIndex = 0;

      for (var x = 0; (x < this.$.autoupdatemetrics.data.length) && (x < this.maxNumberOfSeries); x++){

        if (this.$.autoupdatemetrics.data[x].plottable){
          var lastUpdated = this.$.autoupdatemetrics.data[x].unixtimestamp;
          var seriesLength = metricsValues[chartIndex].values.length;
          var newValue = (this.$.autoupdatemetrics.data[x].value);

          //If it is the first point of the series I don't have to worry about the previous point's timestamp
          if (seriesLength == 0) {
            //If isFirstPoint is not defined we put it to true, since we have a firstPoint, if we get a second point in another series it will be overwritten
            if (isFirstPoint == null) isFirstPoint = true;

            var newY = Number(this.$.autoupdatemetrics.data[x].value);
            var isNewYNaN = isNaN(this.$.autoupdatemetrics.data[x].value);

            if (isNewYNaN) {
              if (newValue == "true") newY = 1;
              else if (newValue == "false") newY = 0;
              else {
                console.error("Received NaN value, not plotting it.");
                newY = null;
              }
            }

            var newPoint = {
              y: newY,
              //BE CAREFUL: If you put a string the sort get screwed up!!!
              x: Number(lastUpdated)
            };

            chart.push("data." + chartIndex + ".values", newPoint);
          } else {


            var firstPoint = metricsValues[chartIndex].values[0];
            var firstPointTimestamp = firstPoint.x;

            var lastPointTimestamp = metricsValues[chartIndex].values[seriesLength - 1].x;

            if(lastPointTimestamp != lastUpdated){

              isFirstPoint = false;

              var newY = Number(this.$.autoupdatemetrics.data[x].value)
              var isNewYNaN = isNaN(this.$.autoupdatemetrics.data[x].value);

              if (isNewYNaN) {
                if (newValue == "true") newY = 1;
                else if (newValue == "false") newY = 0;
                else {
                  console.error("Received NaN value, not plotting it.");
                  newY = null;
                }
              }

              var newPoint = {
                y: newY,
                //BE CAREFUL: If you put a string the sort get screwed up!!!
                x: Number(lastUpdated)
              };


              chart.push("data." + chartIndex + ".values", newPoint);

              if(seriesLength >= this.maxSeriesLength){
                var removedPoint = chart.shift("data." + chartIndex + ".values");
              }
            }
          }
          chartIndex++;
        }
      }

      //If we have a first point we put a custom range, otherwise we use the autorange
      //isFirstPoint == null if we don't get any new point and the range should stay as it is
      if (isFirstPoint!= null) {
        if(isFirstPoint == true) {
          chart._chart.xDomain([chart.data[0].values[0].x - 10000, chart.data[0].values[0].x + 10000]);
        } else {
          //If this is not the first set of point, I use the auto range:
          chart._chart.xDomain(null);
          //console.log("Not first point, seting autoscale, ", chart._chart.xDomain() );
        }
      }

      chart._dataChanged();
    },
    toggleMetricSelectorCollapse: function () {
      //this.$.metricSelectorCollapse.set("noAnimation", true);
      this.$.metricSelectorCollapse.toggle();
      //this.$.metricSelectorCollapse.set("noAnimation", false);
    },
    setNOfPoints: function (event) {
      if (event.keyCode === 13) { // enter pressed!
        if (isNaN(Polymer.dom(event).localTarget.value)) {
          console.error("Inserted number of points is NaN");
          return;
        }

        var inputValue = Number(Polymer.dom(event).localTarget.value);

        if (inputValue <= 0) console.error("Inserted number of points <= 0");

        if(this.maxSeriesLength > inputValue) {
          var chart = this.$$("#chart");
          var metricsValues = chart.data;

          for (var x = 0; x < metricsValues.length; x++) {
            if (metricsValues[x].values.length > inputValue)
            chart.splice("data." + x + ".values", 0, metricsValues[x].values.length - inputValue);
          }

          var chart = this.$$("#chart");
          chart._dataChanged();
        }

        this.set("maxSeriesLength", inputValue);

      }
      return;
    },
    saveAsJson: function () {
      this.$$("#chart").saveJson();
    },
    saveSettingsAsJson: function () {
      var blob = new Blob([JSON.stringify(this.selectedMetricsList)], {type : 'application/json'});
      var fileName = window.prompt("Please choose a file name", "metrics.conf")
      saveAs(blob, fileName);
    },
    saveAsSvg: function () {
      this.$$("#chart").saveSvg();
    },
    saveAsPng: function () {
      this.$$("#chart").savePng();
    },
    saveAsHtml: function () {
      this.$$("#chart").saveHtml();
    },
    loadSettingsFromJson: function (event) {
      var fileReader = new FileReader();

      //We define what should happen when the file is loaded
      fileReader.onloadend = function (fileEvent) {
        if(fileEvent.target.readyState == FileReader.DONE) {

          var readyFileReader = fileEvent.target;

          var parsedFile;

          console.log("File has been loaded ", this);

          try {
            parsedFile = JSON.parse(readyFileReader.result);
            this.set("selectedMetricsList", parsedFile);
          } catch (syntaxErrorException) {
            console.error("Invalid JSON file format!");
            return;
          }

          //Before loading the file, we clean up the plot deleting everything
          this.$$("#chart").set("data", []);

          this.addSeries(this.selectedMetricsList);
          //I start the auto-refresh, or force a new one
          var metricsList = [];

          for (var i = 0; i < this.selectedMetricsList.length; i++) {
            metricsList.push(this.selectedMetricsList[i].id);
          }

          this.$.autoupdatemetrics.set("metricsList", JSON.stringify(metricsList));
          if (this.$.autoupdatemetrics.noRefresh) {
            this.$.autoupdatemetrics.set("noRefresh", false);
          } else {
            this.$.autoupdatemetrics.manualRefresh();
          }

        } else {
          console.error("Something went wrong while loading the file");
        }
      }.bind(this);

      fileReader.readAsText(Polymer.dom(event).localTarget.files[0]);

    },
    clearChart: function () {
      var chart = this.$$("#chart");
      var metricsValues = chart.data;
      for(var x = 0; x < metricsValues.length; x++){
        this.$$("#chart").set("data." + x +".values", []);
      }
      chart._dataChanged();
    },
    popDeleteSeriesDialog: function () {
      this.$.deleteSeriesDialog.open();
    },
    deleteSeries: function () {
      var selectedValues = this.$.deleteSeriesMenu.selectedValues;
      console.log(selectedValues);
      var chart = this.$$("#chart");
      var metricsValues = chart.data;

      //In order to not have problem with shifting indexes we remove the item starting from the hightest index
      //For this reason we use a reverse sort on the selectedValues

      selectedValues.sort(function(a, b){return b-a});

      for (var x = 0; x < selectedValues.length; x++) {
        var selectedIndex = selectedValues[x];
        var removed = this.selectedMetricsList[selectedIndex];
        this.splice("selectedMetricsList", selectedIndex, 1);
        this.splice("metricsSnapshot", selectedIndex, 1);
        var indexToRemove = -1;
        for(var y = 0; y < metricsValues.length; y++){
          if (metricsValues[y].key == removed.id) indexToRemove = y;
        }

        if (indexToRemove >= 0) chart.splice("data", indexToRemove, 1);
      }

      var metricsList = [];

      for (var i = 0; i < this.selectedMetricsList.length; i++) {
        metricsList.push(this.selectedMetricsList[i].id);
      }

      this.$.autoupdatemetrics.set("metricsList", JSON.stringify(metricsList));
      this.$.deleteSeriesMenu.set("selectedValues", []);
      this.$.deleteSeriesDialog.close();
      chart._dataChanged();
    },
    selectAllMetrics: function () {
      var listbox = Polymer.dom(this.root).querySelector("#metricSelector");
      var sa = [];
      for (var x = 0; x < listbox.items.length; x++) if (!listbox.items[x].disabled) sa.push(x);
      listbox.set("selectedValues", sa);
    },
    deselectAllMetrics: function () {
      var listbox = Polymer.dom(this.root).querySelector("#metricSelector");
      listbox.set("selectedValues", []);
    },
    selectAllMetricsRemove: function () {
      var listbox = Polymer.dom(this.root).querySelector("#deleteSeriesMenu");
      var sa = [];
      for (var x = 0; x < listbox.items.length; x++) if (!listbox.items[x].disabled) sa.push(x);
      listbox.set("selectedValues", sa);
    },
    deselectAllMetricsRemove: function () {
      var listbox = Polymer.dom(this.root).querySelector("#deleteSeriesMenu");
      listbox.set("selectedValues", []);
    },
    refreshChart: function () {
      var chart = this.$$("#chart");
      chart._dataChanged();
    },
    setYScale: function () {
      var isChecked = this.$$("#autoscaleYCheckbox").checked;
      var chart = this.$$("#chart");
      if (isChecked) {
        chart._chart.yDomain(null);
      } else {
        var yDomainMin = Number(this.$$("#minimumYInput").value);
        var yDomainMax = Number(this.$$("#maximumYInput").value);
        var yDomain = [yDomainMin, yDomainMax];
        chart._chart.yDomain(yDomain);
      }
      chart._dataChanged();
      return;
    },
    setLogYScale: function () {
      var isChecked = this.$$("#logYCheckbox").checked;
      var chart = this.$$("#chart");
      if (isChecked) {
        chart._chart.yScale(d3.scale.log());
      } else {
        chart._chart.yScale(d3.scale.linear());
      }
      chart._dataChanged();
      return;
    }
});
