studio.plugins.registerPluginDescription("ANS Gain", {
    companyName: "Above Noise Studios",
    productName: "ANS - Gain",
    parameters: {
        "Gain": { displayName: "Gain" },
    },
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.VBoxLayout,
        spacing: 18,
        isFramed: false,
        items: [
            {
                deckWidgetType: studio.ui.deckWidgetType.Pixmap,
                filePath: __dirname + "/resources/ans_logo.png"
            },
            {
                deckWidgetType: studio.ui.deckWidgetType.Dial,
                color: "#ff3537",
                binding: "Gain",
            },
        ]
    }
});