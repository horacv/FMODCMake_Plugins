studio.plugins.registerPluginDescription("ANS EQ 4B", {
    companyName: "Above Noise Studios",
    productName: "ANS - EQ 4B",
    parameters: {
        // LOW BAND
        "Freq. L": { displayName: "Freq. (L)" },
        "Type L": { displayName: "Type (L)" },
        "Gain L": { displayName: "Gain (L)" },
        "Q L": { displayName: "Q (L)" },

        // LOW MID BAND
        "Freq. LM": { displayName: "Freq. (LM)" },
        "Gain LM": { displayName: "Gain (LM)" },
        "Q LM": { displayName: "Q (LM)" },

        // HIGH MID BAND
        "Freq. HM": { displayName: "Freq. (HM)" },
        "Gain HM": { displayName: "Gain (HM)" },
        "Q HM": { displayName: "Q (HM)" },

        // HIGH BAND
        "Freq. H": { displayName: "Freq. (H)" },
        "Type H": { displayName: "Type (H)" },
        "Gain H": { displayName: "Gain (H)" },
        "Q H": { displayName: "Q (H)" },
    },
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        contentsMargins: { top: 6, left: 4, right: 4, },
        spacing: 8,
        isFramed: false,
        items: [
            {
                // LOW BAND
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                contentsMargins: { left: 4, right: 4, },
                spacing: 4,
                isFramed: true,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 0, column: 0, binding: "Freq. L", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dropdown, row: 1, column: 0, binding: "Type L", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 1, binding: "Gain L", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#df7b17", row: 1, column: 1, binding: "Q L", },
                ],
            },
            {
                // LOW MID BAND
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                contentsMargins: { left: 4, right: 4, },
                spacing: 4,
                isFramed: true,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 0, column: 0, binding: "Freq. LM", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 1, binding: "Gain LM", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#df7b17", row: 1, column: 1, binding: "Q LM", },
                ],
            },
            {
                // LOGO
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: false,
                items: [
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Pixmap,
                        filePath: __dirname + "/resources/ans_logo.png"
                    },
                ]
            },
            {
                // HIGH MID BAND
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                contentsMargins: { left: 4, right: 4, },
                spacing: 4,
                isFramed: true,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 0, binding: "Gain HM", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#df7b17", row: 1, column: 0, binding: "Q HM", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 0, column: 1, binding: "Freq. HM", },
                ],
            },
            {
                // HIGH BAND
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                contentsMargins: { left: 4, right: 4, },
                spacing: 4,
                isFramed: true,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#ff3537", row: 0, column: 0, binding: "Gain H", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#df7b17", row: 1, column: 0, binding: "Q H", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#e3bb18", row: 0, column: 1, binding: "Freq. H", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dropdown, row: 1, column: 1, binding: "Type H", },
                ],
            },

        ]
    }
});