studio.plugins.registerPluginDescription("ANS Delay", {
    companyName: "Above Noise Studios",
    productName: "ANS - Delay",
    parameters: {
        "Delay": { displayName: "Delay" },
        "Gain Wet": { displayName: "Wet Level" },
        "Gain Dry": { displayName: "Dry Level" },
        "Feedback": { displayName: "Feedback" },
    },
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        contentsMargins: { top: 6, left: 4, right: 4, },
        isFramed: false,
        items: [
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                alignment: studio.ui.alignment.Center,
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
                // DIALS
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                contentsMargins: { left: 4, right: 4, },
                spacing: 9,
                isFramed: false,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#47cb1b", row: 0, column: 0, binding: "Delay", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 1, binding: "Gain Wet", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 1, column: 1, binding: "Gain Dry", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 1, column: 0, binding: "Feedback", },
                ],
            },
        ]
    }
});