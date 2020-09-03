Polymer({
    is: "monitoring-thread-setup-view",
    properties: {
      monitoringThreadData:{
        type: Object,
        value: function () {
          return {
            status: "Unknown",
            period: "Unknown"
          };
        }
      },
      newPeriod: {
        type: String,
        value: ""
      }
    },
  setPeriod: function () {
    this.$.monitoringThreadCommandSender.set("command", "setperiod");
    this.$.monitoringThreadCommandSender.generateRequest();
    return;
  },
  startMonitoringThread: function () {
    this.$.monitoringThreadCommandSender.set("command", "start");
    this.$.monitoringThreadCommandSender.generateRequest();
    return;
  },
  stopMonitoringThread: function () {
    this.$.monitoringThreadCommandSender.set("command", "stop");
    this.$.monitoringThreadCommandSender.generateRequest();
    return;
  },
  executeOnce: function () {
    this.$.monitoringThreadCommandSender.set("command", "execute");
    this.$.monitoringThreadCommandSender.generateRequest();
    return;
  },
  setInitialButtons: function () {

    this.set("newPeriod", this.$.monitoringThreadStatusRequester.data.period);

    if (this.$.monitoringThreadStatusRequester.data.status == "Running") {
      this.$.startButton.set("disabled", true);
      this.$.stopButton.set("disabled", false);
    }

    if (this.$.monitoringThreadStatusRequester.data.status == "Stopped") {
      this.$.startButton.set("disabled", false);
      this.$.stopButton.set("disabled", true);
    }

    return;
  },
  setButtons: function () {

    if (this.$.monitoringThreadCommandSender.data.status == "Running") {
      this.$.startButton.set("disabled", true);
      this.$.stopButton.set("disabled", false);
    }

    if (this.$.monitoringThreadCommandSender.data.status == "Stopped") {
      this.$.startButton.set("disabled", false);
      this.$.stopButton.set("disabled", true);
    }

    return;
  }
});
