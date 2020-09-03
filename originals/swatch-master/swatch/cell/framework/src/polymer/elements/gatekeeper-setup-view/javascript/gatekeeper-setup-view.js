Polymer({
    is: "gatekeeper-setup-view",
    properties: {
      xmlFilePath: {
        type: String,
        value: ""
      },
      xmlKey: {
        type: String,
        value: ""
      },
      loadStatus: {
        type: Object,
        value: function () { return {};}
      },
      subsystemId: String,
      configurationKey: String,
      runSettingsKey: String,
      showGateKeeper: {
        type: Boolean,
        value: false
      }
    },
    onLoadedGateKeeperFromXML: function () {
      Polymer.dom(this.root).querySelector("#loadingSpinner").set("active", false);
      return;
    },
    onLoadedGateKeeperFromDB: function () {
      Polymer.dom(this.root).querySelector("#loadingSpinner").set("active", false);
      return;
    },
    loadGateKeeperFromXML: function () {
      this.$.gateKeeperXMLPathSender.set("load", "true");
      this.$.gateKeeperXMLPathSender.generateRequest();
      Polymer.dom(this.root).querySelector("#loadingSpinner").set("active", true);
      return;
    },
    loadGateKeeperFromDB: function () {
      this.$.gateKeeperDBKeySender.generateRequest();
      Polymer.dom(this.root).querySelector("#loadingSpinner").set("active", true);
      return;
    },
    toggleGateKeeperContent: function () {
      this.set("showGateKeeper", true);
      if (this.loadStatus.output == "") {
        Polymer.dom(this.root).querySelector("#loadingSpinner").set("active", true);
        this.$.gateKeeperXMLPathSender.set("load", "false");
        this.$.gateKeeperXMLPathSender.generateRequest();
      }
      return;
    },
    isLoaded: function (status) {
      return status == "Loaded";
    },
    isUndefined: function (status) {
      return status == "Undefined";
    }
});
