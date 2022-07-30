package com.ija.ija;

import com.ija.ija.uml.ClassDiagram;
import com.ija.ija.uml.SequenceDiagram;
import com.ija.ija.uml.UMLClass;
import com.ija.ija.uml.UMLSequenceClass;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.concurrent.atomic.AtomicBoolean;

public class AddSDClassController {
    public static boolean display(ClassDiagram classDiagram, SequenceDiagram sequenceDiagram) {
        AtomicBoolean result = new AtomicBoolean(false);
        Stage stage = new Stage();
        stage.setTitle("Add class to sequence diagram.");
        Label label = new Label("Class:");
        label.setPadding(new Insets(10));

        ChoiceBox<String> choiceBox = new ChoiceBox<>();
        for (UMLClass umlClass : classDiagram.getClasses()) {
            boolean existsAlready = false;
            for (UMLSequenceClass sequenceClass : sequenceDiagram.getClasses()) {
                if (sequenceClass.getName().equals(umlClass.getName())) {
                    existsAlready = true;
                    break;
                }
            }
            if (!existsAlready) {
                choiceBox.getItems().add(umlClass.getName());
            }
        }

        Button button = new Button("Add");
        button.setOnAction(actionEvent -> {
            if (choiceBox.getSelectionModel().getSelectedItem() == null) {
                Alert alertError = new Alert(Alert.AlertType.ERROR);
                alertError.setContentText("No class selected!");
                alertError.showAndWait();
            } else {
                sequenceDiagram.addClass(choiceBox.getSelectionModel().getSelectedItem(), false);
                result.set(true);
                stage.close();
            }
        });

        HBox inputLayout = new HBox();
        inputLayout.getChildren().addAll(label, choiceBox);
        inputLayout.setAlignment(Pos.CENTER);
        inputLayout.setPadding(new Insets(20));
        inputLayout.setSpacing(20);

        VBox layout = new VBox();
        layout.getChildren().addAll(inputLayout, button);
        layout.setAlignment(Pos.CENTER);
        layout.setPadding(new Insets(20));
        Scene scene = new Scene(layout);
        stage.setScene(scene);
        stage.showAndWait();

        return result.get();
    }
}
