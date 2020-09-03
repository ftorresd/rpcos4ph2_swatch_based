Polymer({
    is: "system-setup-view",
    properties: {
      currentSystem: {
        type: String,
        value: "empty"
      },
      filePath: {
        type: String,
        value: ""
      },
      loadStatus: {
        type: Object,
        value: ""
      },
      systemTree: {
        type: Array,
        value: []
      },
      selectedItem: {
        type: String,
        value: ""
      },
      itemDetails: {
        type: Array,
        value: []
      },
      loaded: {
        type: Boolean,
        value: false
      },
      subsystemId:{
        type: String,
        value: ""
      },
      configurationKey:{
        type: String,
        value: ""
      }
    },
    loadSystemFromFile: function () {
      this.$.systemFromFileLoader.generateRequest();
      this.set("loadStatus.errorMessage", "");
      this.loaded = false;
    },
    loadSystemFromDB: function () {
      this.$.systemFromDBLoader.generateRequest();
      this.set("loadStatus.errorMessage", "");
      this.loaded = false;
    },
    isStatusError: function (status){
      return (status == "error");
    },
    onLoadedSystemFromFile: function () {
      if(this.$.systemFromFileLoader.data.status == "success"){
        this.set("currentSystem", this.$.systemFromFileLoader.data.systemID);
        //Loads the tree only if Itialize is pressed
        if(!this.loaded){
          this.$.systemTreeRequester.generateRequest();
          this.loaded = true;
        }
      }
//      if(this.$.systemFromFileLoader.data.status == "error"){
//        Polymer.dom(this.root).querySelector('#ErrorBox').innerHTML = this.$.systemFromFileLoader.data.errorMessage;
//      }
    },
    onLoadedSystemFromDB: function () {
      if(this.$.systemFromDBLoader.data.status == "success"){
        this.set("currentSystem", this.$.systemFromDBLoader.data.systemID);
        //Loads the tree only if Itialize is pressed
        if(!this.loaded){
          this.$.systemTreeRequester.generateRequest();
          this.loaded = true;
        }
      }
//      if(this.$.systemFromDBLoader.data.status == "error"){
//        Polymer.dom(this.root).querySelector('#ErrorBox').innerHTML = this.$.systemFromDBLoader.data.errorMessage;
//      }
    },
    onReplySystemTree: function () {
      this.set("itemDetails", []);
      this.set("selectedItem", "");
      if (this.$.systemTreeRequester == null) return;
      this.set("currentSystem", this.$.systemTreeRequester.data[0].name);
    },
    getObjectDetails: function () {
      //I need the type of objectname in the systemTree
      var selectedItemType = this.getItemType(this.$.tree.selected, this.systemTree[0]);
      if (selectedItemType == "undefined") return;
      this.set("itemDetails", []);
      this.$.objectDetailsRequester.set("objectname", this.$.tree.selected);
      this.$.objectDetailsRequester.set("type", selectedItemType);
      this.$.objectDetailsRequester.generateRequest();
      return;
    },
    getItemType: function (itemName, treeNode) {
      if(treeNode.name == itemName) return treeNode.type;
      if(treeNode.tree === undefined) return "undefined";
      if(treeNode.tree == null) return "undefined";
      var type = "undefined";
      for(var x = 0; x < treeNode.tree.length; x++) {
        type = this.getItemType(itemName, treeNode.tree[x]);
        if(type != "undefined") x = treeNode.tree.length;
      }
      return type;
    },
    getOnClickObjectDetails: function (e, detail) {
      //console.log(Polymer.dom(e));
      if (Polymer.dom(e).rootTarget.lastChild == null) return;
      var selectedItemType = this.getItemType(Polymer.dom(e).rootTarget.lastChild.textContent, this.systemTree[0]);
      if (selectedItemType == "undefined") return;
      this.set("itemDetails", []);
      this.$.objectDetailsRequester.set("objectname", Polymer.dom(e).rootTarget.lastChild.textContent);
      this.$.objectDetailsRequester.set("type", selectedItemType);
      this.$.objectDetailsRequester.generateRequest();
      return;
    }
});
