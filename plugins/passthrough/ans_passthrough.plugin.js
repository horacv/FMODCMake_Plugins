studio.plugins.registerPluginDescription("ANS Passthrough", {
    companyName: "Above Noise Studios",
    productName: "ANS - Passthrough",
    parameters: {
        "Bool Param": { displayName: "Bool" },
        "Float Param": { displayName: "Float" },
        "Int Param": { displayName: "Int" },
        "Int Enum Param": { displayName: "Option" },
        "User Data": { displayName: "Data" },
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
                spacing: 10,
                isFramed: false,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Pixmap, filePath: __dirname + "/resources/ans_logo.png" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.VBoxLayout,
                        spacing: 8,
                        contentsMargins: { left: 2, top: 0, right: 2, bottom: 0 },
                        items: [
                            { deckWidgetType: studio.ui.deckWidgetType.DataDrop, binding: "User Data", fileNameFilters: "*.wav" },
                        ],
                    },
                ],
            },
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.GridLayout,
                spacing: 8,
                isFramed: false,
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#DF171A", row: 0, column: 0, binding: "Float Param", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dial, color: "#47cb1b", row: 0, column: 1, binding: "Int Param", },
                    { deckWidgetType: studio.ui.deckWidgetType.Dropdown, row: 1, column: 0, binding: "Int Enum Param", minimumWidth: 52, },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Button,
                        text: "Active",
                        row: 1, column: 1,
                        buttonWidth: 52,
                        binding: "Bool Param",
                    },
                ],
            },
            { deckWidgetType: studio.ui.deckWidgetType.OutputMeter },
        ]
    }
});