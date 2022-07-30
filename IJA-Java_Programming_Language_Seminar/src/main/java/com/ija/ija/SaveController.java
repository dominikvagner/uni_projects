package com.ija.ija;

import javafx.stage.DirectoryChooser;
import javafx.stage.Stage;

import java.io.File;

public class SaveController {

    public static File display() {
        DirectoryChooser chooseDirectoryDialog = new DirectoryChooser();
        chooseDirectoryDialog.setTitle("Choose folder into which the diagram JSON will be saved");
        File dirPath = chooseDirectoryDialog.showDialog(new Stage());
        return dirPath;
    }
}
