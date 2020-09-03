Polymer({
    is: 'log-table',
    properties: {
      //Contains the messages
      log: {
        type: Array,
        value: function () { return [];}
      }
    },
    attached: function() {
      this.$.table.beforeRowBind = function(data, row) {        
        
        var logLevel = this.log[data.index].logLevel;
        row.toggleClass('RowError-bkg', logLevel == "Error");
        row.toggleClass('RowWarning-bkg', logLevel == "Warning");
        
      }.bind(this); //costed me 1 h -- fuck
    },
    _isStringEmpty: function (str) {
      return str == "";
    },
    _isInfo: function (logLevel) {
      return logLevel == "Info";
    },
    _isError: function (logLevel) {
      return logLevel == "Error";
    },
    _isWarning: function (logLevel) {
      return logLevel == "Warning";
    },
    _isUnknown: function (logLevel) {
      return logLevel == "Unknown";
    }
});
