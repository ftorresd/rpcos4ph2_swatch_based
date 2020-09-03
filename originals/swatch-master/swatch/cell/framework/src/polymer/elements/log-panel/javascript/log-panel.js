Polymer({
    is: 'log-panel',
    properties: {
      selectedLogger: {
        type: Number,
        value: 0
      },
      loggers: {
        type: Array,
        value: 0
      },
      log: {
        type: Object,
        value: function () { return {};}
      }
    },
    getLog: function(event) {
      var logLoader = this.$.logLoader;
      var logger = this.$.loggerSelector.items[this.selectedLogger].attributes["data-tab"].value;
      logLoader.set("logger", logger);
      logLoader.set("command", "get");
      logLoader.generateRequest();
    },
    clearLog: function() {
      var logLoader = this.$.logLoader;
      var logger = this.$.loggerSelector.items[this.selectedLogger].attributes["data-tab"].value;
      logLoader.set("logger", logger);
      logLoader.set("command", "clear");
      logLoader.generateRequest();
    }
});
