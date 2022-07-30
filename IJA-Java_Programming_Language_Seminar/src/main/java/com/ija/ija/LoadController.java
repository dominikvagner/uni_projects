package com.ija.ija;

import javafx.stage.FileChooser;
import javafx.stage.Stage;
import java.io.File;

public class LoadController {

    public static File display() {
        FileChooser chooseFileDialog = new FileChooser();
        chooseFileDialog.setTitle("Select JSON with diagram");
        FileChooser.ExtensionFilter jsonFilter = new FileChooser.ExtensionFilter("JSON files (*.json)", "*.json");
        chooseFileDialog.getExtensionFilters().add(jsonFilter);
        File jsonPath = chooseFileDialog.showOpenDialog(new Stage());
        return jsonPath;
    }
}
