Polymer({
    is: 'state-machine-panel',
    behaviors:[throwsToast],
    properties: {
      deviceArray: {
        type: Array,
        value: function () { return []; }
      },
      stateMachineArray: {
        type: Array,
        value: function () { return []; }
      },
      selectedDevice: {
        type: Number,
        value: 0
      },
      selectedStateMachine:{
        type: Number,
        value: 0
      },
      stateMachineInfo: {
        type: Object,
        value: function () { return {}; }
      },
      transitionOutput: {
        type: Object,
        value: function () { return {}; }
      },
      selectedTransition:  {
        type: Number,
        value: 0
      },
      lastRunTransition: {
        type: String,
        value: ""
      },
      transitionStatus: {
        type: Object,
        value: function () { return {}; }
      }
    },
    sendSelectedDevice: function (event) {
      //Resetting some variables in case the device is changed while a transition is running
      this.$.transitionStatusRequester.set("noRefresh", true);
      this.set("transitionStatus", {});

      var selectedDevice = event.detail.item.attributes["device-id"].value;
      var stateMachineArrayRequester = this.$.stateMachineArrayRequester;
      stateMachineArrayRequester.set("selecteddevice", selectedDevice);
      stateMachineArrayRequester.generateRequest();
      var transitionButton = this.$$("#runTransitionButton");
      if (transitionButton) transitionButton.set("disabled", false);
      return;
    },
    sendSelectedStateMachine: function (event) {
      this.set("transitionOutput", {});
      var selectedStateMachine = "";
      //Checking for event because this function can be called without firing it
      if (event) selectedStateMachine = event.detail.item.attributes["state-machine-id"].value;
      else  selectedStateMachine = this.stateMachineArray[this.selectedStateMachine];
      var stateMachineInfoRequester = this.$.stateMachineInfoRequester;
      stateMachineInfoRequester.set("selectedstatemachine", selectedStateMachine);
      stateMachineInfoRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      stateMachineInfoRequester.generateRequest();
      this.$$("#stateMachineSection").set("elevation", 2);
      var transitionButton = this.$$("#runTransitionButton");
      if (transitionButton) transitionButton.set("disabled", false);
      return;
    },
    resetStateMachineSelection: function (event) {
      var stateMachineMenu = this.$$("#stateMachineMenu");
      if (stateMachineMenu) {
        stateMachineMenu.set("selected", 0);
        this.sendSelectedStateMachine();
      }
    },
    resetDeviceSelection: function () {
      var deviceMenu = this.$$("#deviceMenu");
      if (deviceMenu) {
        deviceMenu.set("selected", 0);
      }
    },
    isStringEmpty: function (str) {
      return str == "";
    },
    isNotEngaged: function (state) {
      return state == "Not engaged";
    },
    canEngage: function (state) {
      return (this.stateMachineInfo.gkStatus == "Available") && (state == "Not engaged");
    },
    engageButtonClicked: function () {
      var stateMachineActionRequester = this.$.stateMachineActionRequester;
      stateMachineActionRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
      stateMachineActionRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      stateMachineActionRequester.set("action", "engage");
      stateMachineActionRequester.set("istransition", false);
      stateMachineActionRequester.generateRequest();
      return;
    },
    stateMachineActed: function () {
      //If the SM is engaged I have to look for the initial state

      var data = this.$.stateMachineActionRequester.data;
      var action = this.$.stateMachineActionRequester.action;
      var istransition = this.$.stateMachineActionRequester.istransition;

      if(data.status == "Success"){

        if (!istransition) {

          if(action == "engage") {
            var currentState = "";
            for(var x = 0; x < this.stateMachineInfo.structure.length; x++ ){
              var state = this.stateMachineInfo.structure[x];
              if (state.type == "Make") currentState = state.id;
            }

            this.set("deviceArray." + this.selectedDevice + ".isengaged", true);
            this.set("deviceArray." + this.selectedDevice + ".statemachineid", this.stateMachineInfo.stateMachineName);
            this.set("deviceArray." + this.selectedDevice + ".state", currentState);
          }

          if(action == "disengage") {

            this.set("deviceArray." + this.selectedDevice + ".isengaged", false);
            this.set("deviceArray." + this.selectedDevice + ".statemachineid", "");
            this.set("deviceArray." + this.selectedDevice + ".state", "");

          }
        } else {
          //If it was a transition I start updating the status
          var transitionStatusRequester = this.$.transitionStatusRequester;
          transitionStatusRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
          transitionStatusRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
          transitionStatusRequester.set("noRefresh", false);
        }

        this.$.stateMachineInfoRequester.generateRequest();
      }
      
      if(data.status == "Failed") {
        document.querySelector("ag-toaster").set("duration", 100000);
        this.throwToast({
          'type': 'error',
          'message': data.detail,
          'options': ['Ok']
        });
        document.querySelector("ag-toaster").set("duration", 3000);
      }

      return;
    },
    resetButtonClicked: function () {
      var stateMachineActionRequester = this.$.stateMachineActionRequester;
      stateMachineActionRequester.set("action", "reset");
      stateMachineActionRequester.set("istransition", false);
      stateMachineActionRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
      stateMachineActionRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      stateMachineActionRequester.generateRequest();
      return;
    },
    disengageButtonClicked: function () {
      var stateMachineActionRequester = this.$.stateMachineActionRequester;
      stateMachineActionRequester.set("action", "disengage");
      stateMachineActionRequester.set("istransition", false);
      stateMachineActionRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
      stateMachineActionRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      stateMachineActionRequester.generateRequest();
      return;
    },
    isGkAvailable: function (gkStatus) {
      return gkStatus == "Available";
    },
    resetTransitionSelection: function () {
      var transitionMenu = this.$$("#transitionMenu");
      if (transitionMenu) {
        transitionMenu.set("selected", 0);
      }
      return;
    },
    isCurrentState: function (state) {
      return state.id == this.stateMachineInfo.state;
      return;
    },
    runTransitionClicked: function () {
      this.set("transitionStatus", {});
      this.set("transitionOutput", {});
      var selectedTransition = this.$$("#transitionMenu").items[this.selectedTransition].attributes["transition-id"].value;
      var stateMachineActionRequester = this.$.stateMachineActionRequester;
      stateMachineActionRequester.set("action", selectedTransition);
      stateMachineActionRequester.set("istransition", true);
      stateMachineActionRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
      stateMachineActionRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      this.set("lastRunTransition", selectedTransition);
      stateMachineActionRequester.generateRequest();
      this.$$("#runTransitionButton").set("disabled", true);
      return;
    },
    isMissingParameters: function (status){
    return status == "MissingParameters";
  },
  transitionStatusReceived: function () {
    var tsRequester = this.$.transitionStatusRequester;
    var data = tsRequester.data;

    if(data.notFound) {
      tsRequester.set("noRefresh", true);
      return;
    }

    if(data.done) {
      tsRequester.set("noRefresh", true);
      this.$$("#runTransitionButton").set("disabled", false);
      this.$.stateMachineInfoRequester.generateRequest();
    } else {
      tsRequester.set("selectedstatemachine", this.stateMachineArray[this.selectedStateMachine]);
      tsRequester.set("selecteddevice", this.deviceArray[this.selectedDevice].id);
      if (tsRequester.noRefresh) tsRequester.set("noRefresh", false);
      this.$$("#runTransitionButton").set("disabled", true);
    }
    return;
  },
  isBooleanFalse: function (flag) {
    return flag === false;
  },
  checkCurrentState: function (state, curState) {
    return (state == curState)? "CurrentState" : "";
  }
});
