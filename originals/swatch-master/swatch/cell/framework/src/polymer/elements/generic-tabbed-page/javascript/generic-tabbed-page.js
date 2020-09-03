Polymer({
    is: "generic-tabbed-page",
    properties: {
        pageid: {
            type: String,
            value: 0
        },
        selected: {
            type: String,
            value: 0
        },
        tabnames: {
            type: Array,
            value: []
        }
    },
    attached: function() {
      this.$.ajax.url = window.location.origin + window.location.pathname;
    },
    change: function (event, detail, sender) {
        console.log("tabbed-page change view");
        var parameters = {};
        parameters._eventType_ = "OnSubmit";
        parameters._id_ = "tabbed-page-change-"+this.pageid;
        parameters.selected = this.selected;
        this.$.ajax.params = parameters;
        this.$.ajax.generateRequest();
    },
    tabResponse: function(event, detail, sender) {
        console.log("tabResponse");
        Polymer.dom(this.$.response).innerHTML = detail.response;
    },
    tabError: function(event, detail, sender) {
        console.error("tabbed-page submit error: ", event, detail, sender);
    }
});
