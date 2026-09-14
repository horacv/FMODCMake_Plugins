studio.plugins.registerPluginDescription("ANS LPF 1P", {
    companyName: "Above Noise Studios",
    productName: "ANS - LPF 1P",
    parameters: {
        "Cutoff": { displayName: "Cutoff" },
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
                color: "#e3bb18",
                binding: "Cutoff",
            },
        ]
    }
});