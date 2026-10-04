studio.plugins.registerPluginDescription("ANS Tone Generator", {
    companyName: "Above Noise Studios",
    productName: "ANS - Tone Generator",
    parameters: {
        "Freq.": { displayName: "Freq." },
        "Gain": { displayName: "Gain" },
        "Osc Type": { displayName: "Type" },
    },
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        contentsMargins: { left: 0, top: 0, right: 0, bottom: 0 },
        spacing: 20,
        isFramed: false,
        items: [
            { deckWidgetType: studio.ui.deckWidgetType.InputMeter },
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                alignment: studio.ui.alignment.Center,
                contentsMargins: { left: 4 },
                spacing: 12,
                isFramed: false,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Pixmap, filePath: __dirname + "/resources/ans_logo.png" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.VBoxLayout,
                        spacing: 8,
                        items: [
                            { deckWidgetType: studio.ui.deckWidgetType.Dropdown, binding: "Osc Type", minimumWidth: 48, },
                        ],
                    },
                ],
            },
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                contentsMargins: { left: 0, top: 8, right: 0, bottom: 0 },
                spacing: 10,
                isFramed: false,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 0, column: 0, binding: "Freq.", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 1, binding: "Gain", },
                ],
            },
            { deckWidgetType: studio.ui.deckWidgetType.OutputMeter },
        ]
    }
});