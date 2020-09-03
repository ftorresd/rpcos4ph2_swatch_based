Polymer({
  is: 'command-sequences-panel',
  properties: {
    objectTypes: {
      type: Array,
      value: []
    },
    devices: {
      type: Array,
      value: []
    },
    commandSequences: {
      type: Array,
      value: []
    },
    commandsList: {
      type: Array,
      value: []
    },
    parameters: {
      type: Array,
      value: []
    },
    selectedObjectType: {
      type: String,
      value: "0"
    },
    selectedDevices: {
      type: Array,
      value: []
    },
    selectedCommandSequence: {
      type: String,
      value: "0"
    },
    //Prevents the command from being run accidentally at load time
    run: {
      type: Boolean,
      value: false
    },
    isGateKeeperReady: {
      type: Boolean,
      value: false
    },
    //Contains the return status of the run command sequence (if it was successful or it failed for some reason)
    runCommandSequenceResult: {
      type: Object,
      value: {}
    },
    commandSequenceStatus: {
      type: Array,
      value: []
    }
  },
  sendObjectType: function(event, detail, sender) {

    //Deletes the old information since they do not apply to the new objectType
    if (this.parameters != null) {
      this.set("parameters", []);
      this.set("devices", []);
      this.set("commandSequences", []);
    }

    //Default choice at the first element when changing objectType
    this.set("selectedDevices", []);
    this.set("selectedCommandSequence", "0");

    //Sets up the call
    this.$.deviceArrayRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandSequenceArrayRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.deviceArrayRequester.generateRequest();
    this.$.deviceArrayRequester.set("noRefresh", false);
    this.$.commandSequenceArrayRequester.generateRequest();

    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }


    this.$.commandSequenceStatusUpdater.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.$.commandSequenceStatusUpdater.generateRequest();

    return;
  },
  sendCommandSequence: function(event, detail, sender) {
    this.$.commandsListRequester.set("objecttype", this.objectTypes[this.selectedObjectType]);
    if (this.commandSequences[this.selectedCommandSequence] === undefined) return;
    this.$.commandsListRequester.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.$.commandsListRequester.generateRequest();

    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    this.$.commandSequenceStatusUpdater.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.$.commandSequenceStatusUpdater.generateRequest();
  },
  runCommandSequence: function(event, detail, sender) {

    for (var x = 0; x < this.selectedDevices.length; x++) {
      if (this.devices[this.selectedDevices[x]] === undefined) return;
    }

    if (this.commandSequences[this.selectedCommandSequence] === undefined) return;

    var parametersNames=["commandsequence", "objecttype", "devices", "run"];
    this.$.commandSequenceStarter.set("parameters", parametersNames);

    this.$.commandSequenceStarter.set("objecttype", this.objectTypes[this.selectedObjectType]);

    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    this.$.commandSequenceStarter.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStarter.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.run = true;
    this.$.commandSequenceStarter.set("run", true);

    this.$.commandSequenceStarter.generateRequest();

    this.$.commandSequenceStatusUpdater.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandSequenceStatusUpdater.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.set("noRefresh", false);
    this.$.commandSequenceStatusUpdater.generateRequest();
    this.$.deviceArrayRequester.generateRequest();

    //this.set("selectedDevices", []);

  },
  toBoolean: function (item) {
    if (item === undefined) return false;
    if (item == "true") return true;
    //false by default
    return false;
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
  isBusy: function (status) {
    return status == "Busy";
  },
  isCommandSequenceStatusBusy: function (status) {
    if (status == "Busy") {
      //If I find a busy device while the CommandSequence was going to be run this means that the list is not updated
      this.$.deviceArrayRequester.generateRequest();
      return true;
    }
    return false;
  },
  isCommandSequenceStatusMissingParameter: function (status) {
    return status == "MissingParameter";
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

    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.generateRequest();
    this.checkRunButtonAndAutoUpdate();
  },
  selectNone: function(event, detail, sender) {
    var listbox = Polymer.dom(this.root).querySelector("#deviceMenu");
    listbox.set("selectedValues", []);

    var selectedDevicesIDs = [];

    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.generateRequest();
    this.checkRunButtonAndAutoUpdate();

  },
  getCommandStatusClass: function (status) {
    return 'metricstatus '+status+'-bkg';
  },
  //check if the run button should be disabled and if the auto-update should auto refresh or not
  checkRunButtonAndAutoUpdate: function () {

    //Checking if GK is ready

    if (!this.isGateKeeperReady) {
      this.$.commandSequenceStatusUpdater.set("noRefresh", true);
      if (Polymer.dom(this.root).querySelector("#runCommandSequenceButton") != null)
        Polymer.dom(this.root).querySelector("#runCommandSequenceButton").set("disabled", true);
      return;
    }

    //Checking if at least a device is selected

    if (this.selectedDevices.length == 0) {
      Polymer.dom(this.root).querySelector("#runCommandSequenceButton").set("disabled", true);
      this.$.commandSequenceStatusUpdater.set("noRefresh", true);
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

    //If I don't spam requests anymore, I do a last one in order to get the final status of the cmd sequence
    if((!this.$.commandSequenceStatusUpdater.noRefresh) && (newNoRefresh)) this.$.commandSequenceStatusUpdater.generateRequest();
    this.$.commandSequenceStatusUpdater.set("noRefresh", newNoRefresh);
    Polymer.dom(this.root).querySelector("#runCommandSequenceButton").set("disabled", newDisabledRun);

    return;
  },
  getProgressID: function (deviceID) {
    return deviceID + "Progress";
  },
  onCommandSequenceStatusChanged: function (newVal, oldVal) {
    this.commandSequenceStatus = this.$.commandSequenceStatusUpdater.data;
    this.checkRunButtonAndAutoUpdate();
  },
  sendDevices: function() {
    var selectedDevicesIDs = [];

    for (var x = 0; x < this.selectedDevices.length; x++) {
      selectedDevicesIDs.push(this.devices[this.selectedDevices[x]].id);
    }

    this.$.commandSequenceStatusUpdater.set("objecttype", this.objectTypes[this.selectedObjectType]);
    this.$.commandSequenceStatusUpdater.set("commandsequence", this.commandSequences[this.selectedCommandSequence]);
    this.$.commandSequenceStatusUpdater.set("devices", JSON.stringify(selectedDevicesIDs));
    this.$.commandSequenceStatusUpdater.set("noRefresh", false);
    this.$.commandSequenceStatusUpdater.generateRequest();
    this.checkRunButtonAndAutoUpdate();
  },
  deviceArrayChanged: function (newVal, oldVal) {
    this.devices = this.$.deviceArrayRequester.data;
    this.checkRunButtonAndAutoUpdate();
  },
  getButtonCollapseID: function (deviceID) {
    return deviceID + "Button";
  },
  getIronCollapseID: function (deviceID) {
    return deviceID + "IronCollapse";
  },
  toggleIronCollapse: function (event, detail) {
    console.log (event.model.cmdSeq.device);
    Polymer.dom(this.root).querySelector("#" + event.model.cmdSeq.device + "IronCollapse").toggle();
  },
  isRunning: function (cmd) {
    return cmd.running == "true";
  },
  toggleCommandsListCollapse: function(){
    Polymer.dom(this.root).querySelector("#commandsListCollapse").toggle();
  }

});
