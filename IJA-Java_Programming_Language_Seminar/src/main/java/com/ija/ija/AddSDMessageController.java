package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.SequenceDiagram;
import com.ija.ija.uml.UMLClass;
import com.ija.ija.uml.UMLSequenceClass;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import java.util.concurrent.atomic.AtomicBoolean;

public class AddSDMessageController {
    public static boolean display(ClassDiagram classDiagram, SequenceDiagram sequenceDiagram, UMLSequenceClass classFrom) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Add a message from this class.");

        // Inputs
        Label labelType = new Label("Type of message:");
        labelType.setPadding(new Insets(10));
        ChoiceBox<String> choiceBoxType = new ChoiceBox<>();
        for (String messageType : sequenceDiagram.getConnectionTypes()) {
            choiceBoxType.getItems().add(messageType);
        }
        choiceBoxType.setMaxWidth(Double.MAX_VALUE);

        Label label = new Label("Message text:");
        label.setPadding(new Insets(10));
        TextField textField = new TextField();

        Label labelTo = new Label("Which class to go to:");
        labelTo.setPadding(new Insets(10));
        ChoiceBox<String> choiceBoxTo = new ChoiceBox<>();
        for (UMLSequenceClass sequenceClass: sequenceDiagram.getClasses()) {
            choiceBoxTo.getItems().add(sequenceClass.getName());
        }
        choiceBoxTo.setMaxWidth(Double.MAX_VALUE);

        Label create = new Label("If creating new class, choose it here:");
        create.setPadding(new Insets(10));
        ChoiceBox<String> choiceBoxCreate = new ChoiceBox<>();
        for (UMLClass umlClass : classDiagram.getClasses()) {
            boolean existsAlready = false;
            for (UMLSequenceClass sequenceClass : sequenceDiagram.getClasses()) {
                if (sequenceClass.getName().equals(umlClass.getName())) {
                    existsAlready = true;
                    break;
                }
            }
            if (!existsAlready) {
                choiceBoxCreate.getItems().add(umlClass.getName());
            }
        }
        choiceBoxCreate.setMaxWidth(Double.MAX_VALUE);

        // Button
        Button button = new Button("Add");
        button.setOnAction(actionEvent -> {
            if (textField.getText().length() == 0) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("Please enter a message!");
                alertError.showAndWait();
            } else if (choiceBoxType.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No message type selected!");
                alertError.showAndWait();
            } else if (choiceBoxType.getSelectionModel().getSelectedItem().equals("Creation")) {
                if (choiceBoxCreate.getSelectionModel().getSelectedItem() == null) {
                    Alert alertError = new Alert(Alert.AlertType.ERROR);
                    alertError.setContentText("No class to create selected!");
                    alertError.showAndWait();
                } else {
                    sequenceDiagram.addClass(choiceBoxCreate.getSelectionModel().getSelectedItem(), true);
                    sequenceDiagram.addConnection(
                            choiceBoxType.getSelectionModel().getSelectedItem(),
                            textField.getText(),
                            classFrom.getName(),
                            choiceBoxCreate.getSelectionModel().getSelectedItem()
                    );
                    stage.close();
                    result.set(true);
                }
            } else {
                if (choiceBoxTo.getSelectionModel().getSelectedItem() == null) {
                    Alert alertError = new Alert(Alert.AlertType.ERROR);
                    alertError.setContentText("No class to which the message is supposed to go chosen!");
                    alertError.showAndWait();
                } else {
                    sequenceDiagram.addConnection(
                            choiceBoxType.getSelectionModel().getSelectedItem(),
                            textField.getText(),
                            classFrom.getName(),
                            choiceBoxTo.getSelectionModel().getSelectedItem()
                    );
                    stage.close();
                    result.set(true);
                }
            }
        });

        // Layout
        HBox inputLayout1 = new HBox();
        inputLayout1.getChildren().addAll(labelType, choiceBoxType);
        inputLayout1.setAlignment(Pos.CENTER_LEFT);
        inputLayout1.setPadding(new Insets(20));
        inputLayout1.setSpacing(20);
        HBox.setHgrow(choiceBoxType, Priority.ALWAYS);

        HBox inputLayout2 = new HBox();
        inputLayout2.getChildren().addAll(label, textField);
        inputLayout2.setAlignment(Pos.CENTER_LEFT);
        inputLayout2.setPadding(new Insets(20));
        inputLayout2.setSpacing(20);
        HBox.setHgrow(textField, Priority.ALWAYS);

        HBox inputLayout3 = new HBox();
        inputLayout3.getChildren().addAll(labelTo, choiceBoxTo);
        inputLayout3.setAlignment(Pos.CENTER_LEFT);
        inputLayout3.setPadding(new Insets(20));
        inputLayout3.setSpacing(20);
        HBox.setHgrow(choiceBoxTo, Priority.ALWAYS);

        HBox inputLayout4 = new HBox();
        inputLayout4.getChildren().addAll(create, choiceBoxCreate);
        inputLayout4.setAlignment(Pos.CENTER_LEFT);
        inputLayout4.setPadding(new Insets(20));
        inputLayout4.setSpacing(20);
        HBox.setHgrow(choiceBoxCreate, Priority.ALWAYS);

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
