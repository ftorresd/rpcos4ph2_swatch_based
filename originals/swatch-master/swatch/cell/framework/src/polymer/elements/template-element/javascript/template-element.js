Polymer({
    is: 'template-element',

    behaviors: [
      // Polymer.PaperInputBehavior
    ],

    properties: {
      /**
       * Fired when you make a dinosaur
       *
       * @event made-a-dinosaur
       */

      /**
       * The message the element will show
       */
      someproperty: {
        type: String,
        value: "Hello, World!",
        //alternatively, this can be a computed property, based on other properties
        // computed: 'computeFullName(first, last)'
        //someproperty-changed event will be fired when property changes (required for data-binding to parent)
        notify: true,
        //element attribute will be updated when property changes
        reflectToAttribute: true,
        //function to execute if property changes
        observer: '_disabledChanged',
        //if true, cannot be updated except with _setSomeproperty(value)
        readOnly: false
      }
    },
    observers: [
      // 'dosomething(someproperty, someotherproperty)'
    ],

    /**
     * This will do something nice
     *
     * @param {Egg} egg The dinosaur egg.
     * @return {Dinosaur}
     */
    makeDinosaur: function(egg) {
      alert('you clicked the button!');
      if (!egg) {egg = new Egg('velociraptor');}

      // using this, developers can use your event to fire a function of their own
      // <element-template on-made-a-dinosaur="customfunction"></element-template>
      // the second argument is optional
      this.fire('made-a-dinosaur', {fromEgg: egg});
      return new Dinosaur(egg);
    },

    /**
     * This is a private function, do not use
     */
    _destroyHumanity: function() {
      // if you have a function you don't want others to use outside your element
      // prefix the function with '_'
      dinosaurs = new Array();
      for (var i = 0; i < 100000000; i++) {
        dinosaurs[i] = new Dinosaur();
        dinosaurs[i]._killAllHumans();
      }
    },

    // Fires when an instance of the element is created
    // you have no data binding and the element does not contain html code yet
    created: function() {},

    // Fires when the local DOM has been fully prepared
    // data binding works and the template html is ready
    ready: function() {},

    // Fires when the element was inserted into the document
    attached: function() {},

    // Fires when the element was removed from the document
    detached: function() {},

    // Fires when an attribute was added, removed, or updated
    attributeChanged: function(name, type) {}
});
