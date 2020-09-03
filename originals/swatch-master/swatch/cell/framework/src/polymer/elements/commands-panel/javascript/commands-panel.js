Polymer({
  is: 'commands-panel',
  properties: {
    objectTypes: {
      type: Array,
      value: function () { return [];  }
    },
    devices: {
      type: Array,
      value: function () { return [];  }
    },
    commands: {
      type: Array,
      value: function () { return [];  }
    },
    parameters: {
      type: Array,
      value: function () { return [];  }
    },
    commandStatus: {
      type: Array,
      value: function () { return [];  },
      observer: 'checkRunButtonAndAutoUpdate'
    },
    selectedObjectType: {
      type: Number,
      value: 0
    },
    selectedDevices: {
      type: Array,
      value: function () { return []; },
      observer: 'checkRunButtonAndAutoUpdate'
    },
    selectedCommand: {
      type: Number,
      value: 0
    },
    //Prevents the command from being run accidentally at load time
    run: {
      type: Boolean,
      value: false
    },
    isGateKeeperReady: {
      type: Boolean,
      value: false
    }
  },
  sendObjectType: function(event, detail, sender) {

    //Deletes the old information since they do not apply to the new objectType
    if (this.parameters != null) {
      this.set("parameters", []);
      this.set("devices", []);
      this.set("commands", []);
    }

    //Default choice at the first element when changing objectType
    this.selectNone();
    this.set("selectedCommand", "0");

    //Sets up the call
    this.$.deviceArrayRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandArrayRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.deviceArrayRequester.generateRequest();
    this.$.deviceArrayRequester.set("noRefresh", false);
    this.$.commandArrayRequester.generateRequest();
    return;
  },
  sendDevicesAndCommand: function(event, detail, sender) {
    this.$.parameterArrayRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    if (this.devices == null) return;
    if (this.commands == null) return;
    if (this.commands[this.selectedCommand] === undefined) return;

    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    var loadFromGateKeeperButton = Polymer.dom(this.root).querySelector('#loadFromGateKeeperButton');
    var loadDefaultsButton = Polymer.dom(this.root).querySelector('#loadDefaultsButton');
    var namespaceInput = Polymer.dom(this.root).querySelector('#namespace');

    //I check if the buttons exist and if there is more than one device selected I don't allow the load from GK feature
    if ((this.isGateKeeperReady) && (loadDefaultsButton) && (loadFromGateKeeperButton)){
      if (this.selectedDevices.length > 1) {
        loadFromGateKeeperButton.set("disabled", true);
        loadDefaultsButton.set("disabled", true);
        namespaceInput.set("disabled", true);
      }
      if (this.selectedDevices.length == 1) {
        loadFromGateKeeperButton.set("disabled", false);
        loadDefaultsButton.set("disabled", false);
        namespaceInput.set("disabled", false);
      }
    }

    this.$.parameterArrayRequester.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.parameterArrayRequester.set("command", this.commands[this.selectedCommand]);
    this.$.parameterArrayRequester.set("loadfromgk", "false");
    if (!(Polymer.dom(this.root).querySelector("#namespace") == null)) Polymer.dom(this.root).querySelector("#namespace").set("value","");
    this.$.parameterArrayRequester.generateRequest();
    this.$.commandStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandStatusUpdater.set("command", this.commands[this.selectedCommand]);
    this.$.commandStatusUpdater.generateRequest();
  },
  runCommand: function(event, detail, sender) {

    var deviceMenu = Polymer.dom(this.root).querySelector('#deviceMenu');
    if (!deviceMenu.selectedValues.length) return;
    if (this.commands[this.selectedCommand] === undefined) return;

    var parametersNames = ["command", "objecttype", "devices", "run"];
    for (var x = 0; x < this.parameters.length; x++){
      parametersNames.push(this.parameters[x].name);
    }

    var selectedDevicesIDs = [];
    for (var x = 0; x < deviceMenu.selectedValues.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    this.$.commandStarter.set("parameters", parametersNames);
    this.$.commandStarter.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandStarter.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandStarter.set("command", this.commands[this.selectedCommand]);
    this.set("run",  true);
    this.$.commandStarter.set("run", true);

    for (var x = 0; x < this.parameters.length; x++){
      this.$.commandStarter.set(this.parameters[x].name,
        Polymer.dom(this.root).querySelector("#" + this.sanitize(this.parameters[x].name)).value);
    }

    this.$.commandStarter.generateRequest();
    this.$.commandStatusUpdater.set("devices",  JSON.stringify(selectedDevicesIDs));
    this.$.commandStatusUpdater.set("command", this.commands[this.selectedCommand]);
    this.$.commandStatusUpdater.set("noRefresh", false);
    this.$.commandStatusUpdater.generateRequest();
    this.$.deviceArrayRequester.generateRequest();

  },
  toBoolean: function (item) {
    if (item === undefined) return false;
    if (item.toLowerCase() == "true") return true;
    //false by default
      else return false;
  },
  setProgressClass: function (item, deviceID) {

    var statusBarID = CSS.escape(this.getProgressID(deviceID));

    if (Polymer.dom(this.root).querySelector("#" + statusBarID) == null) return item;
    switch (item){
      case "Done":
        Polymer.dom(this.root).querySelector("#" + statusBarID).customStyle['--paper-progress-active-color'] = '#66bb6a';
        Polymer.dom(this.root).querySelector("#" + statusBarID).updateStyles();
      break;
      case "Error":
        Polymer.dom(this.root).querySelector("#" + statusBarID).customStyle['--paper-progress-active-color'] = '#E44E50';
        Polymer.dom(this.root).querySelector("#" + statusBarID).updateStyles();
      break;
      case "Warning":
        Polymer.dom(this.root).querySelector("#" + statusBarID).customStyle['--paper-progress-active-color'] = 'orange';
        Polymer.dom(this.root).querySelector("#" + statusBarID).updateStyles();
      break;
      case "Running":
        Polymer.dom(this.root).querySelector("#" + statusBarID).customStyle['--paper-progress-active-color'] = '#2196f3';
        Polymer.dom(this.root).querySelector("#" + statusBarID).updateStyles();
      break;
      case "Scheduled":
        Polymer.dom(this.root).querySelector("#" + statusBarID).customStyle['--paper-progress-active-color'] = '#bdbdbd';
        Polymer.dom(this.root).querySelector("#" + statusBarID).updateStyles();
      break;
    }
    return item;
  },
  loadFromGateKeeper: function () {
    this.$.parameterArrayRequester.set("namespace", Polymer.dom(this.root).querySelector("#namespace").value);
    this.$.parameterArrayRequester.set("loadfromgk", "true");
    this.$.parameterArrayRequester.generateRequest();
  },
  loadDefaults: function () {
    this.$.parameterArrayRequester.set("loadfromgk", "false");
    this.$.parameterArrayRequester.generateRequest();
  },
  checkRunButtonAndAutoUpdate: function (){

    if (this.devices == null) return;

    if (Polymer.dom(this.root).querySelector("#runCommandButton") == null) return;

      //Checking if at least a device is selected

      if (this.selectedDevices.length == 0) {
        Polymer.dom(this.root).querySelector("#runCommandButton").set("disabled", true);
        this.$.commandStatusUpdater.set("noRefresh", true);
        return;
      }

      //Checking if one of the selected device is busy

      var newDisabledRun = false;
      var newNoRefresh = true;

      for (var x = 0; x < this.selectedDevices.length; x++) {
        if (this.devices[this.selectedDevices[x]].status == "Busy") {
          newDisabledRun = true;
          newNoRefresh = false;
        }
      }

      //If I don't spam requests anymore, I do a last one in order to get the final status of the cmd
      if((!this.$.commandStatusUpdater.noRefresh) && (newNoRefresh)) this.$.commandStatusUpdater.generateRequest();
      this.$.commandStatusUpdater.set("noRefresh", newNoRefresh);
      Polymer.dom(this.root).querySelector("#runCommandButton").set("disabled", newDisabledRun);

  },
  isCommandExisting: function (selCmd, firstCmd) {
    if (this.commands[this.selectedCommand] === undefined) return false;
    return true;
  },
  sanitize: function (value) {
    return value.toLowerCase().replace(/[^a-zA-Z0-9]+/g, "-");
  },
  onCommandStatusChanged: function () {
    this.commandStatus = this.$.commandStatusUpdater.data;
    this.checkRunButtonAndAutoUpdate();
  },
  deviceArrayChanged: function (newVal, oldVal) {
    this.devices = this.$.deviceArrayRequester.data;
    this.checkRunButtonAndAutoUpdate();
  },
  selectAll: function(event, detail, sender) {
    var listbox = Polymer.dom(this.root).querySelector("#deviceMenu");
    var sa = [];
    for (var x = 0; x < listbox.items.length; x++)
      if (!listbox.items[x].disabled) sa.push(x);
    listbox.set("selectedValues", sa);

    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    this.$.commandStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandStatusUpdater.generateRequest();
    this.checkRunButtonAndAutoUpdate();
  },
  selectNone: function(event, detail, sender) {
    var listbox = Polymer.dom(this.root).querySelector("#deviceMenu");
    listbox.set("selectedValues", []);

    var selectedDevicesIDs = [];

    this.$.commandStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandStatusUpdater.generateRequest();
    this.checkRunButtonAndAutoUpdate();

  },
  getProgressID: function (deviceID) {
    return deviceID + "Progress";
  },
  getButtonCollapseID: function (deviceID) {
    return deviceID + "Button";
  },
  getIronCollapseID: function (deviceID) {
    return deviceID + "IronCollapse";
  },
  toggleIronCollapse: function (event, detail) {
    Polymer.dom(this.root).querySelector("#" + event.model.cmd.device + "IronCollapse").toggle();
  },
});
