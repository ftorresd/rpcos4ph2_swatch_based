Polymer({
    is: 'explore-panel',

    properties: {
      exploreTree: {
        type: Object,
        value: function () { return{}}
      },
      selectedItem: {
        type: String,
        value: ""
      }
    },
    treeClicked: function (event) {
      this.set("selectedItem", event.detail.nodeId);
      console.log(this.selectedItem);
    }
});
