package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.UMLClass;
import com.ija.ija.uml.UMLOperation;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicBoolean;

public class AddCDConnectionController {
    public static boolean display(ClassDiagram classDiagram, UMLClass umlClass) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Add a connection from this class.");

        // Inputs
        Label labelType = new Label("Choose the type of connection:");
        labelType.setPadding(new Insets(10));
        ChoiceBox<String> choiceBoxType = new ChoiceBox<>();
        for (String connType : classDiagram.getConnTypes()) {
            choiceBoxType.getItems().add(connType);
        }
        choiceBoxType.setMaxWidth(Double.MAX_VALUE);

        Label labelClassTo = new Label("Choose the target class:");
        labelClassTo.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxClassTo = new ChoiceBox<>();
        for (UMLClass cls : classDiagram.getClasses()) {
            if (!cls.getName().equals(umlClass.getName()))
                choiceBoxClassTo.getItems().add(cls.getName());
        }
        choiceBoxClassTo.setMaxWidth(Double.MAX_VALUE);

        Label labelCardinalityFrom = new Label("Choose the connection cardinality from the starting class:");
        labelCardinalityFrom.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxCardinalityFrom = new ChoiceBox<>();
        for (String cardinality : classDiagram.getConnCardinalities()) {
            choiceBoxCardinalityFrom.getItems().add(cardinality);
        }
        choiceBoxCardinalityFrom.setMaxWidth(Double.MAX_VALUE);

        Label labelCardinalityTo = new Label("Choose the connection cardinality to the target class:");
        labelCardinalityTo.setPadding(new Insets(5, 10, 5, 10));
        ChoiceBox<String> choiceBoxCardinalityTo = new ChoiceBox<>();
        for (String cardinality : classDiagram.getConnCardinalities()) {
            choiceBoxCardinalityTo.getItems().add(cardinality);
        }
        choiceBoxCardinalityTo.setMaxWidth(Double.MAX_VALUE);

        // Button
        Button button = new Button("Add");
        button.setOnAction(actionEvent -> {
            if (choiceBoxType.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose a type of connection to add!");
                alertError.showAndWait();
            } else if (choiceBoxClassTo.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose the target class!");
                alertError.showAndWait();
            } else if (choiceBoxCardinalityFrom.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose a connection cardinality of the initial class!");
                alertError.showAndWait();
            } else if (choiceBoxCardinalityTo.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please choose a connection cardinality of the target class!");
                alertError.showAndWait();
            } else {
                classDiagram.addConnection(
                        choiceBoxType.getSelectionModel().getSelectedItem(),
                        umlClass.getName(),
                        choiceBoxClassTo.getSelectionModel().getSelectedItem(),
                        choiceBoxCardinalityFrom.getSelectionModel().getSelectedItem(),
                        choiceBoxCardinalityTo.getSelectionModel().getSelectedItem()
                );
                stage.close();
                result.set(true);
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(labelType, choiceBoxType);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20, 20, 10, 20));
        inputLayout1.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(labelClassTo, choiceBoxClassTo);
        inputLayout2.setAlignment(Pos.CENTER_LEFT);
        inputLayout2.setPadding(new Insets(10, 20, 10, 20));
        inputLayout2.setSpacing(20);
        HBox.setHgrow(choiceBoxClassTo, Priority.ALWAYS);

        HBox inputLayout3 = new HBox();
        inputLayout3.getChildren().addAll(labelCardinalityFrom, choiceBoxCardinalityFrom);
        inputLayout3.setAlignment(Pos.CENTER_LEFT);
        inputLayout3.setPadding(new Insets(10, 20, 10, 20));
        inputLayout3.setSpacing(20);
        HBox.setHgrow(choiceBoxCardinalityFrom, Priority.ALWAYS);

        HBox inputLayout4 = new HBox();
        inputLayout4.getChildren().addAll(labelCardinalityTo, choiceBoxCardinalityTo);
        inputLayout4.setAlignment(Pos.CENTER_LEFT);
        inputLayout4.setPadding(new Insets(10, 20, 10, 20));
        inputLayout4.setSpacing(20);
        HBox.setHgrow(choiceBoxCardinalityTo, Priority.ALWAYS);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout1, inputLayout2, inputLayout3, inputLayout4, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
