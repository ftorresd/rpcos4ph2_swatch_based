Polymer({
    is: 'masked-panel',
    behaviors: [throwsToast],
    properties: {
      maskedObjects: {
        type: Object,
        value: function () { return {}; }
      },
      selectedMaskedFilter: {
        type: String,
        value: ""
      },
      selectedDisabledFilter: {
        type: String,
        value: ""
      },
      fedMaps: {
        type: String,
        value: ""
      },
      isGatekeeperReady: {
        type: Boolean,
        value: false
      }
    },
    filterMaskedItems: function (event) {
      this.set("selectedMaskedFilter", event.detail.item.attributes["filter-tag"].value);
      return;
    },
    filterDisabledItems: function (event) {
      this.set("selectedDisabledFilter", event.detail.item.attributes["filter-tag"].value);
      return;
    },
    isAll: function (filterTag) {
      return filterTag == "all";
    },
    isDTM: function (filterTag) {
      return ((this.isAll(filterTag)) || (filterTag == "dtm"));
    },
    isAMC: function (filterTag) {
      return ((this.isAll(filterTag)) || (filterTag == "amc"));
    },
    isAMCPort: function (filterTag) {
      return ((this.isAll(filterTag)) || (filterTag == "amcport"));
    },
    isLink: function (filterTag) {
      return ((this.isAll(filterTag)) || (filterTag == "link"));
    },
    isInput: function (filterTag) {
      return ((this.isAll(filterTag)) || (filterTag == "input"));
    },
    applyDynamicMasks: function () {
      this.$.applyDynamicMasksAjax.generateRequest();
      return;
    },
    applyStaticMasks: function () {
      this.$.applyStaticMasksAjax.generateRequest();
      return;
    },
    clearMasks: function () {
      this.$.clearMasksAjax.generateRequest();
      return;
    },
    checkStatus: function(event) { 
      
      var data = event.detail;      
      
      if (data.status == "Success") {
        this.throwToast({
          'type': 'info',
          'message': data.message
        });
      }
      
      if (data.status == "Error") {
        this.throwToast({
          'type': 'error',
          'message': data.message,
          'blocking': true,
          'options': ['Ok']
        });
      }
      
      this.$.maskedObjectsRequester.generateRequest();
      
      return;
    }
});
