package com.ija.ija;

import com.ija.ija.uml.*;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicReference;

public class AddCDAttributeController {
    public static UMLAttribute display(ClassDiagram classDiagram, UMLClass umlClass, boolean operation) {
        AtomicReference<UMLAttribute> result = new AtomicReference<>(
                new UMLAttribute()
        );
        Stage stage = new Stage();
        stage.setTitle("Add an attribute to this class.");

        // Inputs
        Label labelMod = new Label("Choose attributes access modifier:");
        labelMod.setPadding(new Insets(10));
        ChoiceBox<String> choiceBoxMod = new ChoiceBox<>();
        for (String mod : classDiagram.getAccessMods()) {
            choiceBoxMod.getItems().add(mod);
        }
        choiceBoxMod.setMaxWidth(Double.MAX_VALUE);
        if (operation)
            choiceBoxMod.getSelectionModel().select("+");

        Label labelName = new Label("New attribute name:");
        labelName.setPadding(new Insets(5, 10, 5, 10));
        TextField textFieldName = new TextField();

        Label labelType = new Label("Choose the type of the attribute:");
        labelType.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxType = new ChoiceBox<>();
        for (String mod : classDiagram.printAllClassifiers()) {
            choiceBoxType.getItems().add(mod);
        }
        choiceBoxType.setMaxWidth(Double.MAX_VALUE);

        Label labelList = new Label("The attribute is a list of the selected type:");
        labelList.setPadding(new Insets(10));
        CheckBox checkBox = new CheckBox("Is list?");

        // Buttons
        Button button = new Button("Add");
        button.setOnAction(actionEvent -> {
            if (choiceBoxMod.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No access modifier chosen!");
                alertError.showAndWait();
            } else if (textFieldName.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Can't create attribute with no name!");
                alertError.showAndWait();
            } else if (choiceBoxType.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No attribute type chosen!");
                alertError.showAndWait();
            } else {
                UMLAttribute newAttr = new UMLAttribute(
                        textFieldName.getText(),
                        classDiagram.classifierForName(choiceBoxType.getSelectionModel().getSelectedItem()),
                        choiceBoxMod.getSelectionModel().getSelectedItem(),
                        checkBox.isSelected()
                );
                if (umlClass.getAttributes().stream().anyMatch(a -> a.getName().equals(newAttr.getName())) && !operation) {
                    Alert alertError = new Alert(Alert.AlertType.ERROR);
                    alertError.setContentText("Attribute with this name already created, choose a different name!");
                    alertError.showAndWait();
                } else {
                    result.set(newAttr);
                    stage.close();
                }
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(labelMod, choiceBoxMod);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);
        HBox.setHgrow(choiceBoxMod, Priority.ALWAYS);


        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(labelName, textFieldName);
        inputLayout2.setAlignment(Pos.CENTER_LEFT);
        inputLayout2.setPadding(new Insets(20));
        inputLayout2.setSpacing(20);

        HBox inputLayout3 = new HBox();
        inputLayout3.getChildren().addAll(labelType, choiceBoxType);
        inputLayout3.setAlignment(Pos.CENTER_LEFT);
        inputLayout3.setPadding(new Insets(20));
        inputLayout3.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        HBox inputLayout4 = new HBox();
        inputLayout4.getChildren().addAll(labelList, checkBox);
        inputLayout4.setAlignment(Pos.CENTER_LEFT);
        inputLayout4.setPadding(new Insets(20));
        inputLayout4.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        VBox layout = new VBox();
        if (operation)
            layout.getChildren().addAll(inputLayout2, inputLayout3, inputLayout4, button);
        else
            layout.getChildren().addAll(inputLayout1, inputLayout2, inputLayout3, inputLayout4, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
