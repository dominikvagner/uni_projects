package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicBoolean;

public class AddCDClassController {
    public static boolean display(ClassDiagram classDiagram) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Create/Add new class to class diagram.");

        // Inputs
        Label labelName = new Label("Class name:");
        labelName.setPadding(new Insets(10));
        TextField textField = new TextField();
        Label labelAbstract = new Label("Is class abstract:");
        labelAbstract.setPadding(new Insets(10));
        CheckBox checkBox = new CheckBox("Abstract");

        // Button
        Button button = new Button("Create");
        button.setOnAction(actionEvent -> {
            if (textField.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't create a class with no name.");
                alertError.showAndWait();
            } else {
                classDiagram.createClass(textField.getText(), checkBox.isSelected());
                result.set(true);
                stage.close();
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(labelName, textField);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);

        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(labelAbstract, checkBox);
        inputLayout2.setAlignment(Pos.CENTER_LEFT);
        inputLayout2.setPadding(new Insets(20));
        inputLayout2.setSpacing(20);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout1, inputLayout2, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
