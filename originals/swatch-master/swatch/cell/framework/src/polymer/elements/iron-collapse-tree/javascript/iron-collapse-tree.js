Polymer({
    is: 'iron-collapse-tree',
    properties: {
      /**
       * Opens the collapse 
       */
      opened: {
        type: Boolean,
        value: false
      },
      /**
       * Stores the tree JSON
       */
      tree: {
       type: Object,
       value: function () { return {}; }
      }
    },
    toggleCollapse: function () {
      if (!this._firstOpen) this.set("_firstOpen", true);
      this.$.collapse.toggle();
    },
    /**
     * Fired when a node is clicked or toggled
     *
     * @event node-clicked
     * @param {nodeId} node name
     */
    _fireClickEvent: function(event) {
      this.fire('node-clicked', {nodeId: this.tree.name});
      return;
    },
    /**
     * Updated the iron collapse height after rendering
     */
    _updateCollapse: function() {
      console.log("Called update collapse");
      this.$.collapse.updateSize("auto", true);
    },
    //Used to update the collapse height when the element is attached
    attached: function () {
      this.fire("attached", {});
    }
});
