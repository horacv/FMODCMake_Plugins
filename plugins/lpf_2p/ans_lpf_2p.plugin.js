studio.plugins.registerPluginDescription("ANS LPF 2P", {
    companyName: "Above Noise Studios",
    productName: "ANS - LPF 2P",
    parameters: {
        "Cutoff": { displayName: "Cutoff" },
        "Q": { displayName: "Q" },
    },
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        spacing: 18,
        isFramed: false,
        items: [
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                spacing: 18,
                isFramed: false,
                items: [
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Pixmap,
                        filePath: __dirname + "/resources/ans_logo.png"
                    },
                ]
            },
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                spacing: 18,
                isFramed: false,
                items: [
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Dial,
                        color: "#e3bb18",
                        binding: "Cutoff",
                    },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Dial,
                        color: "#df7b17",
                        binding: "Q",
                    },
                ]
            },
        ]
    }
});