Polymer({
    is: "tabbed-page",
    properties: {
        selected: {
            type: String,
            value: 0
        },
        tabnames: {
            type: Array,
            value: []
        },
        tabids: {
            type: Array,
            value: []
        }
    }
});
